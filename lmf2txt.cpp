#include <iostream>
#include <iomanip>

#include "lmfpy/LMFReader.h"


using namespace std;
using namespace lmfpy;

int main(int argc, char* argv[]) {
    if (argc == 1) {
        cout << "syntax: lmf2txt filename [--full] [--ns]" << endl;
        cout << "        if --full is omitted then only the header" << endl;
        cout << "        and the first 100 events will be written." << endl;
        cout << "        --ns converts the values to nanoseconds." << endl;
    }
    if (argc > 4) {
        cout << "wrong number of arguments." << endl;
        return 0;
    }

    string filename = argv[1];
    bool is_full = false, is_ns = false;
    if (argc >= 3)	{
        if (strcmp(argv[2], "--full") == 0) is_full = true;
        if (strcmp(argv[2], "--ns") == 0) is_ns = true;
    }
    if (argc == 4)	{
        if (strcmp(argv[3], "--full") == 0) is_full = true;
        if (strcmp(argv[3], "--ns") == 0) is_ns = true;
    }

    auto afile = make_shared<LMFReader>(filename);
    auto time_fr = chrono::system_clock::to_time_t(afile->time_fr());
    auto time_to = chrono::system_clock::to_time_t(afile->time_to());
    cout << "               Filename: " << afile->recorded_at() << endl;
    cout << "                Version: " << afile->version() << endl;
    cout << "                Comment: " << afile->comment() << endl;
    cout << "     Number of Channels: " << afile->nchannels() << endl;
    cout << " Maximum Number of Hits: " << afile->max_nhits() << endl;
    cout << "  Number of Coordinates: " << afile->ncoordinates() << endl;
    cout << "       Number of Events: " << afile->end() << endl;
    cout << "    TDC Resolution (ns): " << afile->to_nanosec() << endl;
    cout << "             Start Time: " << ctime(&time_fr);
    cout << "              Stop Time: " << ctime(&time_to);

    auto limit = is_full ? afile->end() : 100;
    auto i = 0;
    auto iter = afile->begin();
    for (; iter != afile->end(); ++i) {
        if (i >= limit) break;
        auto event = *iter;
        cout << "########################" << endl;
        cout << "# " << event.event << endl;
        cout << "########################" << endl;
        cout << "                 At (s): " << setw(12) << event.timestamp << endl;
        auto ch = 0, k = 0;
        auto n = event.nhits.begin();
        for (; n != event.nhits.end(); ++ch, ++k, ++n) {
            cout << "Channel #" << setw(3) << ch << " " << setw(3) << *n << " Hit(s): ";
            for (auto kk = 0; kk < *n; ++k, ++kk) {
                cout << setw(12) << event.hits[k] << ", ";
            }
            cout << endl;
        }
    }
    return 0;
}
