#ifndef COREINJECT_GDBPROCESS_HPP
#define COREINJECT_GDBPROCESS_HPP

#include "CoreInject.hpp"

namespace CoreInject {

	struct GDBProcess {
		int inPipe;
		int outPipe;

		bool attached = false;

		GDBProcess();
		GDBProcess(const GDBProcess&) = delete;
		GDBProcess(GDBProcess&&) = delete;
		~GDBProcess(); // Closes both pipes

		void attach(std::size_t pid) const;

		void waitUntilReady() const;
		void write(const std::string& command) const;
		[[nodiscard]] std::string nextLine() const;
		[[nodiscard]] std::pair<std::string, std::string> awaitOneOf(std::initializer_list<std::string> keywords) const;

		static std::string parseKeyword(const std::string& line);
	};
}

#endif
