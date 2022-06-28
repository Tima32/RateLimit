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
const uint32_t reg_size{ 2 };
const uint32_t rate_offset{ 1 * reg_size };
const uint32_t struct_size{ 10 };
const uint32_t clk{ 250000 };

void print_help()
{
	cout << 
	"Usage: rate_limit [command] {args}\n"
	"Commands:\n"
	"enable  [port number] Enables traffic limiting.\n"
	"disable [port number] Disables traffic limiting.\n"
	"stats   [port number] Displays the port setting.\n"
	"rate    [port number] [rate] Set rate limit.\n"
	"Rate is specified in bit\\s. If rate is not a multiple of 250,000, then it is rounded up to a lower multiple.\n"
	;
}
void print_stat()
{
	//open files
	fstream cr_base_f{cr_base_path, ios::in};
	if (!cr_base_f.is_open())
	{
		cerr << "Error open cr_base" << endl;
		exit(1);
	}
	fstream cr_cnt_f{cr_cnt_path, ios::in};
	if (!cr_cnt_f.is_open())
	{
		cerr << "Error open cr_base" << endl;
		exit(1);
	}
	FILE* etn_f = fopen("/dev/etn", "r+b");
	if (etn_f == nullptr)
	{
		perror("fopen");
		cout << "Error open etn " << strerror(errno) << endl;
		exit(1);
	}	

	//read etn position
	string cr_base;
	cr_base_f >> cr_base;
	size_t offset{ 0 };
	{
		stringstream ss;
		ss << cr_base;
		ss >> offset;
	}
	offset *= reg_size; //Multiply by register size.

	uint16_t reg_enable_0{false};
	uint16_t reg_rate_limit_0{0};
	uint16_t reg_enable_1{false};
	uint16_t reg_rate_limit_1{0};

	fseek(etn_f, offset, SEEK_SET);
	fread(&reg_enable_0, reg_size, 1, etn_f);
	fread(&reg_rate_limit_0, reg_size, 1, etn_f);

	offset += struct_size;
	fseek(etn_f, offset, SEEK_SET);
	fread(&reg_enable_1, reg_size, 1, etn_f);
	fread(&reg_rate_limit_1, reg_size, 1, etn_f);

	fclose(etn_f);

	cout << "Statistics:" << endl;

	cout << "----[Port 0]----" << endl;
	cout << "Status: " << (reg_enable_0 ? "enable" : "disable") << endl;
	cout << "Rate: " << reg_rate_limit_0 * clk << endl;

	cout << "----[Port 1]----" << endl;
	cout << "Status: " << (reg_enable_1 ? "enable" : "disable") << endl;
	cout << "Rate: " << reg_rate_limit_1 * clk << endl;
}
void enable(uint32_t port, bool enable_b)
{
	//open files
	fstream cr_base_f{cr_base_path, ios::in};
	if (!cr_base_f.is_open())
	{
		cerr << "Error open cr_base" << endl;
		exit(1);
	}
	fstream cr_cnt_f{cr_cnt_path, ios::in};
	if (!cr_cnt_f.is_open())
	{
		cerr << "Error open cr_base" << endl;
		exit(1);
	}
	FILE* etn_f = fopen("/dev/etn", "r+b");
	if (etn_f == nullptr)
	{
		perror("fopen");
		cout << "Error open etn " << strerror(errno) << endl;
		exit(1);
	}	

	//read etn position
	string cr_base;
	cr_base_f >> cr_base;

	size_t offset{ 0 };
	{
		stringstream ss;
		ss << cr_base;
		ss >> offset;
	}
	offset *= reg_size; //Multiply by register size.
	offset += (struct_size * port);
	fseek(etn_f, offset, SEEK_SET);

	uint16_t reg{enable_b};
	fwrite(&reg, reg_size, 1, etn_f);
	fclose(etn_f);
}
void rate(uint32_t port, uint16_t rate)
{
	//open files
	fstream cr_base_f{cr_base_path, ios::in};
	if (!cr_base_f.is_open())
	{
		cerr << "Error open cr_base" << endl;
		exit(1);
	}
	fstream cr_cnt_f{cr_cnt_path, ios::in};
	if (!cr_cnt_f.is_open())
	{
		cerr << "Error open cr_base" << endl;
		exit(1);
	}
	FILE* etn_f = fopen("/dev/etn", "r+b");
	if (etn_f == nullptr)
	{
		perror("fopen");
		cout << "Error open etn " << strerror(errno) << endl;
		exit(1);
	}	

	//read etn position
	string cr_base;
	cr_base_f >> cr_base;

	size_t offset{ 0 };
	{
		stringstream ss;
		ss << cr_base;
		ss >> offset;
	}
	offset *= reg_size; //Multiply by register size.
	offset += (struct_size * port) + rate_offset;
	fseek(etn_f, offset, SEEK_SET);

	uint16_t reg{rate};
	fwrite(&reg, reg_size, 1, etn_f);
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

		//rate
		if (command == "rate")
		{
			uint32_t port_n{};
			uint32_t rate_n{};

			auto pos = ap.find(command);
			auto port_s = ap[pos + 1];
			auto rate_s = ap[pos + 2];

			std::stringstream ss;
			ss << port_s << " " << rate_s;
			ss >> port_n >> rate_n;
			rate(port_n, rate_n / clk);
		}
	}
	catch(const std::exception& ex)
	{
		cout << ex.what() << endl;
		return 1;
	}

	return 0;
}
