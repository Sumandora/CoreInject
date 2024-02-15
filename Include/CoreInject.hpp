#ifndef COREINJECT_HPP
#define COREINJECT_HPP

#include <filesystem>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace CoreInject {
	using Module = std::filesystem::path;

	struct Settings {
		// If enabled, then modules that are already loaded by the target process will be ignored
		// Note: This doesn't work when the already injected module has been renamed, for better accuracy use OVERWRITE as existsStrategy when applicable
		bool preventDoubleInjection = true;

		// If multiple modules are to be injected a self-imposed delay can be added by setting this variable over 0
		time_t waitBetweenModules = 0;

		// Before injection, move the library to the current working directory of the target process
		bool workaroundSandboxes = false;
		enum class ExistsStrategy { // If the target file already exists, ...
			OVERWRITE, // then it will be overwritten
			RENAME // then the target will silently be renamed
		};
		ExistsStrategy existsStrategy = ExistsStrategy::OVERWRITE;

		// Deletes the module after injection. In case of a sandbox workaround, this affects the relocated module, not the original
		bool deleteAfterInjection = false;
	};

	class CoreInject {
		struct Process* process;
		Settings settings;

		void removeAlreadyInjected(std::vector<Module>& modules) const;
		void relocateModules(std::vector<Module>& modules) const;

	public:
		explicit CoreInject(std::size_t targetProcess,
			const Settings& settings);

		void run(std::vector<Module> modules) const;
	};
};

#endif
