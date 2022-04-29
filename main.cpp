#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int main()
{
    fstream cr_base_f{"/sys/class/fpga/fpga0/features/RX_RATE_LIMIT/cr_base", ios::in};
    if (!cr_base_f.is_open())
    {
        cerr << "Error open cr_base" << endl;
        exit(0);
    }
    fstream cr_cnt_f{"/sys/class/fpga/fpga0/features/RX_RATE_LIMIT/cr_cnt", ios::in};
    if (!cr_cnt_f.is_open())
    {
        cerr << "Error open cr_base" << endl;
        exit(0);
    }

    string cr_base;
    cr_base_f >> cr_base;
    cout << "cr_base: " << cr_base << endl;

    string cr_cnt;
    cr_cnt_f >> cr_cnt;
    cout << "cr_cnt: " << cr_cnt << endl;


    fstream etn{"/dev/etn", ios::in | ios::out | ios::binary};
    if (!etn.is_open())
    {
        cerr << "Error open etn" << endl;
        exit(0);
    }

    size_t offset{0};
    {
        stringstream ss;
        ss << cr_base;
        ss >> offset;
    }
    offset *= 2;
    offset += 2;
    etn.seekg(offset, ios::beg);
    if (!etn)
    {
        cerr << "Error seekg: " << std::strerror(errno) << endl;
    }
    cout << "offset: " << offset << endl;


    uint16_t reg;
    char buf[10]{0};
    etn.read(buf, 10);
    if (!etn)
    {
        cerr << "Error: " << std::strerror(errno) << endl;
    }
    reg = *(reinterpret_cast<uint16_t*>(buf));
    cout << "Reg: " << reg << endl;
    etn.close();

    FILE* fr = fopen("/dev/etn", "r+b");
    if (fr == nullptr)
    {
        perror("fopen");
        cout << "Error open eth " << strerror(errno) << endl;
        exit(0);
    }

    fseek(fr, offset, SEEK_SET);
    fread(buf, 2, 1, fr);
    reg = *(reinterpret_cast<uint16_t*>(buf));
    cout << "Reg: " << reg << endl;

    buf[0] = !buf[0];
    fseek(fr, offset, SEEK_SET);
    fwrite(buf, 2, 1, fr);

    return 0;
}