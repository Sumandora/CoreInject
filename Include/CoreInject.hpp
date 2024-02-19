#ifndef COREINJECT_HPP
#define COREINJECT_HPP

#include <any>
#include <filesystem>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "CoreInject/Settings.hpp"

namespace CoreInject {
	using Module = std::filesystem::path;

	class CoreInject {
		struct Process* process;
		Settings settings;

		void checkELFHeaders(const std::vector<Module>& modules) const;
		void removeAlreadyInjected(std::vector<Module>& modules) const;
		void relocateModules(std::vector<Module>& modules) const;

	public:
		explicit CoreInject(std::size_t targetProcess, Settings settings);

		[[nodiscard]] std::size_t run(std::vector<Module> modules) const;
	};
}

#endif
