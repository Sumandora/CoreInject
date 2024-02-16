#ifndef COREINJECT_CORECLI_HPP
#define COREINJECT_CORECLI_HPP

#include <string>

namespace CoreInject::CoreCLI {
	const std::string privilegeError =
		"ERROR: Your system's ptrace_scope setting is currently set above 0."
		"In order to run this application, you must run it with root privileges."
		"Please note that lowering ptrace_scope may compromise your system's security.";

	struct Parameter {
		const std::string shorthand;
		const std::string longhand;
		const std::string description;
	};
	const Parameter pid{ "-p", "--pid", "The process id of the target process" };
	const Parameter module{ "-m", "--module", "The modules which are to be injected" };

	constexpr std::string flagNameToParamName(const std::string& flagName) {
		return "--" + flagName;
	}

	constexpr std::string version = "1.1";
}

#endif
