#include "CoreInject.hpp"
#include "GDBProcess.hpp"
#include "Process.hpp"

#include <algorithm>
#include <format>
#include <ranges>
#include <utility>

CoreInject::CoreInject::CoreInject(std::size_t targetProcess, Settings settings)
	: process(new Process(targetProcess))
	, settings(std::move(settings))
{
	if(!std::filesystem::exists(process->getProcFS()))
		throw std::runtime_error("Process doesn't exist");
}

void CoreInject::CoreInject::checkELFHeaders(const std::vector<Module>& modules) const {
	if(settings.ignoreELFHeader)
		return;

	auto elfFiles = modules | std::ranges::views::transform([](const Module& module) {
		return std::pair{ module, ElfFile{ module } };
	});

	using ModuleElfPair = std::pair<Module, ElfFile>;

	auto invalidHeaders = elfFiles | std::ranges::views::filter([](const ModuleElfPair& pair) {
		return !pair.second.isHeaderValid();
	});
	if (!invalidHeaders.empty()) {
		std::string what = "Invalid ELF header(s):\n";
		for(const auto& file : invalidHeaders)
			what += file.first.string() + "\n";
		throw std::runtime_error(what);
	}

	auto wrongArchitectures = elfFiles | std::ranges::views::filter([arch = process->getExecutable().getArchitecture()](const ModuleElfPair& pair) {
		return pair.second.getArchitecture() != arch;
	});
	if (!wrongArchitectures.empty()) {
		std::string what = "Architecture mismatch(es):\n";
		for(const auto& file : wrongArchitectures)
			what += file.first.string() + "\n";
		throw std::runtime_error(what);
	}
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
				std::filesystem::path basePath = newModule.parent_path();
				std::string name = newModule.stem().string();
				std::string extension = newModule.extension().string();
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
	auto nonExistant = modules | std::ranges::views::filter([](const Module& mod) {
		return !std::filesystem::exists(mod);
	});
	if (!nonExistant.empty()) {
		std::string what = "File(s) not found:\n";
		for(const auto& file : nonExistant)
			what += file.string() + "\n";
		throw std::runtime_error(what);
	}

	checkELFHeaders(modules);

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
		static std::format_string<std::string> dlopen = R"a(-data-evaluate-expression "((void*(*)(const char*, int))dlopen)(\"./{}\", 2)")a";
		std::string modulePath = std::filesystem::relative(module, process->getCwd()).string();
		gdb.write(std::vformat(dlopen.get(), std::make_format_args(modulePath)));
		pair = gdb.awaitOneOf({ "done", "error", "stopped" });
		if (pair.first == "error")
			throw std::runtime_error("Failed to inject " + module.string() + ": " + pair.second);
		if (pair.first == "stopped")
			throw std::runtime_error(module.filename().string() + " caused a fault");

		if (settings.deleteAfterInjection)
			std::filesystem::remove(module);
	}

	return modules.size();
}
