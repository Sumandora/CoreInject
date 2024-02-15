#include "CoreInject.hpp"
#include "CoreInject/NeedsRoot.hpp"

#include <iostream>

#include "argparse/argparse.hpp"

int main(int argc, char** argv)
{
	if (CoreInject::needsRoot()) {
		std::cerr << "ERROR: This tool requires root privileges, because ptrace_scope is bigger than 0" << std::endl;
		exit(1);
	}

	argparse::ArgumentParser program("CoreCLI");
	program.add_argument("-p", "--pid")
		.help("The process id of the target process")
		.required()
		.scan<'d', std::size_t>();

	program.add_argument("-m", "--module")
		.help("If enabled, then modules that are already loaded by the target process will be ignored\n"
			  "Note: This doesn't work when the already injected module has been renamed, for better accuracy use OVERWRITE as existsStrategy when applicable")
		.append()
		.required();

	program.add_argument("--preventDoubleInjection")
		.help("If enabled, then modules wont be checked if they are already injected")
		.flag();

	program.add_argument("--waitBetweenModules")
		.help("If multiple modules are to be injected a self-imposed delay can be added by setting this variable over 0")
		.default_value(0L)
		.implicit_value(0L)
		.scan<'d', long>();

	program.add_argument("--workaroundSandboxes")
		.help("Before injection, move the library to the current working directory of the target process")
		.flag();

	program.add_argument("--existsStrategy")
		.help("If the target file already exists, [then it will be overwritten/then the target will silently be renamed]")
		.choices("OVERWRITE", "RENAME")
		.default_value("OVERWRITE")
		.implicit_value("OVERWRITE");

	program.add_argument("--deleteAfterInjection")
		.help("Deletes the module after injection. In case of a sandbox workaround, this affects the relocated module, not the original")
		.flag();

	try {
		program.parse_args(argc, argv);
	} catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return 1;
	}

	auto pid = program.get<std::size_t>("--pid");
	auto modules = program.get<std::vector<std::string>>("--module");
	CoreInject::Settings settings{
		.preventDoubleInjection = program.get<bool>("--preventDoubleInjection"),
		.waitBetweenModules = program.get<long>("--waitBetweenModules"),
		.workaroundSandboxes = program.get<bool>("--workaroundSandboxes"),
		.existsStrategy = program.get<std::string>("--existsStrategy") == "OVERWRITE" ? CoreInject::Settings::ExistsStrategy::OVERWRITE : CoreInject::Settings::ExistsStrategy::RENAME,
		.deleteAfterInjection = program.get<bool>("--deleteAfterInjection")
	};

	std::cout << "Injecting " << modules.size() << " modules into " << pid << std::endl;

	CoreInject::CoreInject core(pid, settings);
	core.run(std::vector<std::filesystem::path>(modules.begin(), modules.end()));
}