#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include "ArgumentParser.hpp"

using namespace std;

const string cr_base_path("/sys/class/fpga/fpga0/features/RX_RATE_LIMIT/cr_base");
const string cr_cnt_path("/sys/class/fpga/fpga0/features/RX_RATE_LIMIT/cr_cnt");
const string etn_path("/dev/etn");

void print_help()
{
	cout << 
	"Usage: rate_limit [command] {args}\n"
	"Commands:\n"
	"enable  [port number] Enables traffic limiting.\n"
	"disable [port number] Disables traffic limiting.\n"
	"stat    [port number] Displays the port setting.\n"
	;
}
void print_stat()
{
	//open files
	fstream cr_base_f{cr_base_path, ios::in};
	if (!cr_base_f.is_open())
	{
		cerr << "Error open cr_base" << endl;
		exit(0);
	}
	fstream cr_cnt_f{cr_cnt_path, ios::in};
	if (!cr_cnt_f.is_open())
	{
		cerr << "Error open cr_base" << endl;
		exit(0);
	}
	FILE* etn_f = fopen("/dev/etn", "r+b");
	if (etn_f == nullptr)
	{
		perror("fopen");
		cout << "Error open eth " << strerror(errno) << endl;
		exit(0);
	}	

	//read etn position
	string cr_base;
	cr_base_f >> cr_base;
	cout << "cr_base: " << cr_base << endl;

	size_t offset{ 0 };
	{
		stringstream ss;
		ss << cr_base;
		ss >> offset;
	}
	offset *= 2; //Multiply by register size.
	//offset += (10 * port);

	uint16_t reg_enable_0{false};
	uint16_t reg_rate_limit_0{0};
	uint16_t reg_enable_1{false};
	uint16_t reg_rate_limit_1{0};

	fseek(etn_f, offset, SEEK_SET);
	fread(&reg_enable_0, 2, 1, etn_f);
	fread(&reg_rate_limit_0, 2, 1, etn_f);

	offset += 10;
	fseek(etn_f, offset, SEEK_SET);
	fread(&reg_enable_1, 2, 1, etn_f);
	fread(&reg_rate_limit_1, 2, 1, etn_f);

	fclose(etn_f);

	cout << "Statistics:" << endl;

	cout << "----[Port 0]----" << endl;
	cout << "Status: " << (reg_enable_0 ? "enable" : "disable") << endl;
	cout << "Rate: " << reg_rate_limit_0 << endl;

	cout << "----[Port 1]----" << endl;
	cout << "Status: " << (reg_enable_1 ? "enable" : "disable") << endl;
	cout << "Rate: " << reg_rate_limit_1 << endl;
}
void enable(uint32_t port, bool enable_b)
{
	//open files
	fstream cr_base_f{cr_base_path, ios::in};
	if (!cr_base_f.is_open())
	{
		cerr << "Error open cr_base" << endl;
		exit(0);
	}
	fstream cr_cnt_f{cr_cnt_path, ios::in};
	if (!cr_cnt_f.is_open())
	{
		cerr << "Error open cr_base" << endl;
		exit(0);
	}
	FILE* etn_f = fopen("/dev/etn", "r+b");
	if (etn_f == nullptr)
	{
		perror("fopen");
		cout << "Error open eth " << strerror(errno) << endl;
		exit(0);
	}	

	//read etn position
	string cr_base;
	cr_base_f >> cr_base;
	cout << "cr_base: " << cr_base << endl;

	// string cr_cnt;
	// cr_cnt_f >> cr_cnt;
	// cout << "cr_cnt: " << cr_cnt << endl;

	size_t offset{ 0 };
	{
		stringstream ss;
		ss << cr_base;
		ss >> offset;
	}
	offset *= 2; //Multiply by register size.
	offset += (10 * port);
	fseek(etn_f, offset, SEEK_SET);

	uint16_t reg{enable_b};
	fwrite(&reg, 2, 1, etn_f);
	fclose(etn_f);
}
int main(int argc, const char **argv)
{
	ArgumentParser ap(argc, argv);

	//help
	if (argc == 1 || ap.find("--help") != ArgumentParser::not_found ||
		ap.find("help") != ArgumentParser::not_found)
	{
		print_help();
		return 0;
	}

	//enable / disable / stat
	const string command{ argv[1] };
	if (command == "stats")
	{
		print_stat();
		return 0;
	}
	try
	{
		if (command == "enable")
		{
			auto port = ap.get<uint32_t>("enable");
			enable(port, true);
			return 0;
		}
		if (command == "disable")
		{
			auto port = ap.get<uint32_t>("disable");
			enable(port, false);
			return 0;
		}
	}
	catch(const std::exception& ex)
	{
		cout << ex.what() << endl;	
	}

	return 0;
}