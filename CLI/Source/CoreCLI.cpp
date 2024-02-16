#include "CoreInject.hpp"
#include "CoreInject/NeedsRoot.hpp"

#include <iostream>

#include "argparse/argparse.hpp"

#include "CoreCLI.hpp"

std::string CoreInject::CoreCLI::flagNameToParamName(const std::string& flagName) {
	return "--" + flagName;
}

using CoreInject::CoreCLI::flagNameToParamName;

int main(int argc, char** argv)
{
	if (CoreInject::needsRoot()) {
		std::cerr << "ERROR: Your system's ptrace_scope setting is currently set above 0. In order to run this application, you must run it with root privileges. Please note that lowering ptrace_scope may compromise your system's security." << std::endl;
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

	CoreInject::Settings settings;

	settings.forEachSetting([&program](const CoreInject::Flag& flag) {
		program.add_argument(flagNameToParamName(flag.name))
			.help(flag.description)
			.flag();
	});

	try {
		program.parse_args(argc, argv);
	} catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return 1;
	}

	auto pid = program.get<std::size_t>("--pid");
	auto modules = program.get<std::vector<std::string>>("--module");

	settings.forEachSetting([&program](CoreInject::Flag& flag) {
		std::string paramName = flagNameToParamName(flag.name);
		if(flag.dependant != nullptr)
			if(program.is_used(paramName) && !program.is_used(flagNameToParamName(flag.dependant->name)))
				std::cerr << flag.name + " depends on " + flag.dependant->name << ". Flipping " + flag.name + " will have no effect." << std::endl;
		flag.state = program.get<bool>(paramName);
	});


	CoreInject::CoreInject core(pid, settings);
	try {
		std::size_t injected = core.run(std::vector<std::filesystem::path>(modules.begin(), modules.end()));
		std::cout << "Injected " << injected << " modules into " << pid << std::endl;
	} catch (const std::runtime_error& e) {
		std::cerr << "Injection failed\n" << e.what() << std::endl;
	}
}