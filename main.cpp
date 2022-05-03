#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include "ArgumentParser.hpp"

using namespace std;


void print_help()
{
	cout << 
	"Usage: rate_limit [command] {args}\n"
	"Commands:\n"
	"enable  [port number] Enables traffic limiting.\n"
	"desable [port number] Disables traffic limiting.\n"
	"stat    [port number] Displays the port setting.\n"
	;
}
void print_stat()
{
	cout << "Stat:" << endl;
}
void enable(uint32_t port, bool enable_b)
{
	
}
int main(int argc, const char **argv)
{
	ArgumentParser ap(argc, argv);

	//help
	if (argc == 1 || ap.find("--help") != ArgumentParser::not_found)
	{
		print_help();
		return 0;
	}

	//enable / disable / stat
	const string command{ argv[1] };
	if (command == "stats")
	{
		print_stat();
	}
	try
	{
		if (command == "enable")
		{
			auto port = ap.get<uint32_t>("enable");

		}
		if (command == "disable")
		{
			auto port = ap.get<uint32_t>("disable");
		}
	}
	catch(const std::exception& ex)
	{
		cout << ex.what() << endl;	
	}

	return 0;
}