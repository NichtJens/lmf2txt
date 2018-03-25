//
// Created by Daehyun You on 3/24/18.
//

#include <iostream>
#include "LMFReader.h"

using namespace std;
using namespace lmfpy;

int main() {
    auto reader = make_unique<LMFReader>("/Users/daehyun/Desktop/N2_429eV_000.lmf");
    for (auto it = reader->begin(); it != reader->end(); ++it) {
        auto i = *it;
        cout << "0 " << i.event << " " << reader->end() << endl;
        if (i.event == 100) break;
    }
    for (auto it = reader->begin(); it != reader->end(); ++it) {
        auto i = *it;
        cout << "1 " << i.event << " " << reader->end() << endl;
        if (i.event == 100) break;
    }
    for (auto it = reader->begin(); it != reader->end(); ++it) {
        auto i = *it;
        cout << "2 " << i.event << " " << reader->end() << endl;
        if (i.event == 100) break;
    }
    return 0;
}
