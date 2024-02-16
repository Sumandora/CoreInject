#ifndef COREINJECT_HPP
#define COREINJECT_HPP

#include <any>
#include <filesystem>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace CoreInject {
	struct Flag {
		std::string name;
		std::string description;
		Flag* dependant = nullptr;
		bool state = false;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
		operator bool() const { return state; }
#pragma clang diagnostic pop
	};

	struct Settings {
		Flag allowDoubleInjection{
			"allowDoubleInjection",
			"If enabled, then modules that are already loaded by the target process wont be filtered\n"
			"Note: Turning this off might not filter accurately because the library might be renamed when 'workaroundSandboxes' is used without 'overwriteRelocationTarget'"
		};

		Flag workaroundSandboxes{
			"workaroundSandboxes",
			"Before injection, move the library to the current working directory of the target process"
		};

		Flag overwriteRelocationTarget{
			"overwriteRelocationTarget",
			"If the target file already exists, then it will be overwritten",
			&workaroundSandboxes
		};

		Flag deleteAfterInjection{
			"deleteAfterInjection",
			"Deletes the module after injection. In case of a sandbox workaround, this affects the relocated module, not the original"
		};

		static constexpr auto allSettings = std::tuple(
			&Settings::allowDoubleInjection,
			&Settings::workaroundSandboxes,
			&Settings::overwriteRelocationTarget,
			&Settings::deleteAfterInjection);

		template <typename F, std::size_t Idx = std::tuple_size_v<decltype(allSettings)> - 1>
		constexpr void forEachSetting(F f)
		{
			if constexpr (Idx > 0)
				forEachSetting<F, Idx - 1>(f);

			f(*this.*(std::get<Idx>(allSettings)));
		}
	};

	using Module = std::filesystem::path;

	class CoreInject {
		struct Process* process;
		Settings settings;

		void removeAlreadyInjected(std::vector<Module>& modules) const;
		void relocateModules(std::vector<Module>& modules) const;

	public:
		explicit CoreInject(std::size_t targetProcess,
			const Settings& settings);

		[[nodiscard]] std::size_t run(std::vector<Module> modules) const;
	};
};

#endif
