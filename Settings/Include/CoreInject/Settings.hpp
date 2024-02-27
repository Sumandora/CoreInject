#ifndef COREINJECT_SETTINGS_HPP
#define COREINJECT_SETTINGS_HPP

#include <string>

namespace CoreInject {

	struct Flag {
		std::string name;
		std::string cleanName;
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
			"Allow double injection",
			"If enabled, then modules that are already loaded by the target process wont be filtered\n"
			"Note: Turning this off might not filter accurately because the library might be renamed when 'workaroundSandboxes' is used without 'overwriteRelocationTarget'"
		};

		Flag workaroundSandboxes{
			"workaroundSandboxes",
			"Workaround sandboxes",
			"Before injection, copy the library to the current working directory of the target process"
		};

		Flag overwriteRelocationTarget{
			"overwriteRelocationTarget",
			"Overwrite relocation target",
			"If the target file already exists, then it will be overwritten",
			&workaroundSandboxes
		};

		Flag deleteAfterInjection{
			"deleteAfterInjection",
			"Delete after injection",
			"Deletes the module after injection. In case of a sandbox workaround, this affects the relocated module, not the original"
		};

		Flag ignoreELFHeader{
			"ignoreELFHeader",
			"Ignore ELF header",
			"Allows modules with invalid elf header to be injected (also allows mismatching architectures)"
		};

		static constexpr std::tuple allSettings{
			&Settings::allowDoubleInjection,
			&Settings::workaroundSandboxes,
			&Settings::overwriteRelocationTarget,
			&Settings::deleteAfterInjection,
			&Settings::ignoreELFHeader
		};

		template <typename F, std::size_t Idx = std::tuple_size_v<decltype(allSettings)> - 1>
		constexpr void forEachSetting(F f)
		{
			if constexpr (Idx > 0)
				forEachSetting<F, Idx - 1>(f);

			f(*this.*(std::get<Idx>(allSettings)));
		}
	};
}

#endif
