#ifndef LMFPY_LMFREADER_H
#define LMFPY_LMFREADER_H


#include <mutex>
#include <memory>
#include <vector>
#include <numeric>

#include "LMF_IO.h"


using namespace std;


namespace lmfpy {
    class LMFIterator;

    struct Event {
        uint64_t event{};
        double timestamp{};
        vector<uint32_t> nhits;
        vector<double> hits;
    };


    /**
     * \class LMFReader
     * \brief A LMF file reader
     * \example On C++14
     * auto reader = make_unique<LMFReader>("afile.lmf");
     * for (auto iter = reader->begin(); iter != reader->end(); ++iter) {
     *     auto i = *iter;
     *     cout << "------- #" << i.event << " -------" << endl;
     *     cout << "time: " << i.timestamp << " s" << endl;
     *     // ...
     * }
     * \example On C++17
     * auto reader = make_unique<LMFReader>("afile.lmf");
     * for (auto i : reader) {
     *     cout << "------- #" << i.event << " -------" << endl;
     *     cout << "time: " << i.timestamp << " s" << endl;
     *     // ...
     * }
     */
    class LMFReader : public LMF_IO {
        friend class LMFIterator;

        mutex mut;
        const uint32_t nchannelrooms, nhitrooms;

        uint64_t at() const;

        void at(uint64_t event);

    public:
        explicit LMFReader(string filename, uint32_t nchannelrooms = 32, uint32_t nhitrooms = 64);

        chrono::system_clock::time_point time_fr() const;

        chrono::system_clock::time_point time_to() const;

        LMFIterator begin(uint64_t event = 0);

        uint64_t end() const;

        Event operator[](int64_t event);
    };


    class LMFIterator {
        friend class LMFReader;

        LMFReader *const reader;
        const uint32_t nchannelrooms, nhitrooms, nchannels;
        enum HitType { int_, float_ } type;
        vector<int32_t> ihits;
        vector<double> fhits;

        explicit LMFIterator(LMFReader &reader);

    public:
        bool operator==(uint64_t i) const;

        bool operator!=(uint64_t i) const;

        const LMFIterator &operator++() const;

        Event operator*();
    };
}


#endif //LMFPY_LMFREADER_H
