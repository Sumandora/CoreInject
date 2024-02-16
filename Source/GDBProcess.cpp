#include "GDBProcess.hpp"

#include <csignal>
#include <iostream>
#include <sys/prctl.h>
#include <unistd.h>
#include <algorithm>

using namespace CoreInject;

GDBProcess::GDBProcess()
{
	int outputPipe[2];
	int inputPipe[2];

	pipe(outputPipe);
	pipe(inputPipe);
	int child;
	if (child = fork(); child == 0) {
		dup2(inputPipe[0], STDIN_FILENO);
		dup2(outputPipe[1], STDOUT_FILENO);
		dup2(outputPipe[1], STDERR_FILENO);
		prctl(PR_SET_PDEATHSIG, SIGTERM);
		close(inputPipe[1]);
		close(outputPipe[0]);
		execl("/usr/bin/gdb", "gdb", "-i=mi", nullptr);
		exit(1);
	}
	close(inputPipe[0]);
	close(outputPipe[1]);

	inPipe = inputPipe[1];
	outPipe = outputPipe[0];
}

GDBProcess::~GDBProcess()
{
	if(attached)
		write("-target-detach");
	write("-gdb-exit");
	close(inPipe);
	close(outPipe);
}

void GDBProcess::attach(std::size_t pid) const {
	write("-target-attach " + std::to_string(pid));
}

void GDBProcess::waitUntilReady() const
{
	while(true) {
		std::string line = nextLine();
		if(line.starts_with("(gdb)"))
			break;
	}
}

void GDBProcess::write(const std::string& command) const {
	waitUntilReady();
	::write(inPipe, (command + "\n").c_str(), command.size() + 1);
}

std::string GDBProcess::nextLine() const
{
	std::string line;
	char c = 0;
	while(true) {
		if(read(outPipe, &c, sizeof(c)) == 0)
			break;
		if(c == '\n')
			break;
		line += c;
	}
	return line;
}

std::pair<std::string, std::string> GDBProcess::awaitOneOf(std::initializer_list<std::string> keywords) const
{
	while(true) {
		std::string line = nextLine();
		std::string parsedKeyword = parseKeyword(line);
		if(std::ranges::any_of(keywords, [&parsedKeyword](const std::string& keyword) { return parsedKeyword == keyword; }))
			return { parsedKeyword, line };
	}
}

std::string GDBProcess::parseKeyword(const std::string& line) {
	if(line.starts_with("(gdb)"))
		return "ready";
	std::stringstream ss{line};
	char type;
	ss >> type;
	switch (type) {
	case '~':
		return "stream/console output";
	case '@':
		return "stream/target output";
	case '&':
		return "stream/log output";
	default:
		std::string keyword;
		std::getline(ss, keyword, ',');
		return keyword;
	}
}
