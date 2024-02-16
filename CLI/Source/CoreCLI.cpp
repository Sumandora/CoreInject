#include "CoreInject.hpp"
#include "CoreInject/NeedsRoot.hpp"

#include <iostream>

#include "argparse/argparse.hpp"

#include "CoreCLI.hpp"

using namespace CoreInject::CoreCLI;

int main(int argc, char** argv)
{
	if (CoreInject::needsRoot()) {
		std::cerr << privilegeError << std::endl;
		exit(1);
	}

	argparse::ArgumentParser program("CoreCLI");
	program.add_argument(pid.shorthand, pid.longhand)
		.help(pid.description)
		.required()
		.scan<'d', std::size_t>();

	program.add_argument(module.shorthand, module.longhand)
		.help(module.description)
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
	auto pid = program.get<std::size_t>(CoreInject::CoreCLI::pid.longhand);
#pragma clang diagnostic pop
	auto modules = program.get<std::vector<std::string>>(module.longhand);

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