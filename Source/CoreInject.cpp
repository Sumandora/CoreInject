#include "CoreInject.hpp"
#include "GDBProcess.hpp"
#include "Process.hpp"

#include <algorithm>
#include <format>
#include <iostream>
#include <thread>

CoreInject::CoreInject::CoreInject(std::size_t targetProcess, const Settings& settings)
	: process(new Process(targetProcess))
	, settings(settings)
{
}

void CoreInject::CoreInject::removeAlreadyInjected(std::vector<Module>& modules) const
{
	if (settings.allowDoubleInjection)
		return;

	std::erase_if(modules, [this](const Module& module) {
		return process->hasModule(module.filename());
	});
}

void CoreInject::CoreInject::relocateModules(std::vector<Module>& modules) const
{
	if (!settings.workaroundSandboxes || !process->isSandboxed())
		return;
	std::ranges::transform(modules, modules.begin(), [&](const Module& module) {
		Module newModule = process->intoRoot(process->getCwd()) / module.filename();

		if (std::filesystem::exists(newModule)) {
			if (settings.overwriteRelocationTarget) {
				std::error_code err;
				if (!std::filesystem::remove(newModule, err))
					throw std::runtime_error{ err.message() };
			} else {
				auto basePath = newModule.parent_path();
				auto name = newModule.stem().string();
				auto extension = newModule.extension().string();
				std::size_t c = 0;
				while (std::filesystem::exists(newModule)) {
					auto newPath = basePath / std::string(name).append("_").append(std::to_string(c)).append(extension);
					newModule = newPath;
					c++;
				}
			}
		}

		std::filesystem::copy(module, newModule);
		return newModule;
	});
}

std::size_t CoreInject::CoreInject::run(std::vector<Module> modules) const
{
	if (std::ranges::any_of(modules, [arch = process->getExecutable().getArchitecture()](const Module& module) {
			ElfFile elf(module);
			return elf.getArchitecture() != arch;
		}))
		throw std::runtime_error("Architecture mismatch");

	removeAlreadyInjected(modules);
	if (modules.empty()) {
		return modules.size(); // How unpredictable
	}

	relocateModules(modules);

	GDBProcess gdb;
	gdb.attach(process->getId());

	auto pair = gdb.awaitOneOf({ "done", "error" });
	if (pair.first == "error")
		throw std::runtime_error("Failed to attach to process: " + pair.second);

	for (const Module& module : modules) {
		// insane syntax design:
		static std::string dlopen = R"a(-data-evaluate-expression "((void*(*)(const char*, int))dlopen)(\"{}\", 2)")a";
		gdb.write(std::vformat(dlopen, std::make_format_args("./" + std::filesystem::relative(module, process->getCwd()).string())));
		pair = gdb.awaitOneOf({ "done", "error", "stopped" });
		if (pair.first == "error")
			throw std::runtime_error("Failed to inject " + module.string() + ": " + pair.second);
		else if (pair.first == "stopped") {
			throw std::runtime_error(module.filename().string() + " caused a fault");
		}

		if (settings.deleteAfterInjection)
			std::filesystem::remove(module);
	}

	return modules.size();
}
