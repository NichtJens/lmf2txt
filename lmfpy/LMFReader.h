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
        uint64_t event;
        double timestamp;
        vector<uint32_t> nhits;
        vector<double> hits;
        Event(uint64_t event, double timestamp, vector<uint32_t> nhits, vector<double> hits);
        Event() = default;
    };


    /**
     * \class LMFReader
     * \brief A LMF file reader
     * \example a loop on C++11
     * auto afile = make_shared<LMFReader>("afile.lmf");
     * for (auto iter = afile->begin(); iter != afile->end();) {
     *     auto i = *iter;  // i is a instance Event
     *     // ...
     * }
     * \example a loop on C++14
     * auto afile = make_unique<LMFReader>("afile.lmf");
     * for (auto iter = afile->begin(); iter != afile->end();) {
     *     auto i = *iter;  // i is a instance Event
     *     // ...
     * }
     * \example a loop on C++17
     * auto afile = make_unique<LMFReader>("afile.lmf");
     * for (auto i : afile) {  // i is a instance Event
     *     // ...
     * }
     */
    class LMFReader : private LMF_IO {
        friend class LMFIterator;

        mutex mut;
        const uint32_t nchannelrooms, nhitrooms;

        uint64_t at() const;

        void at(uint64_t event) noexcept(false);

    public:
        explicit LMFReader(string filename,
                           uint32_t nchannelrooms = 32,
                           uint32_t nhitrooms = 64) noexcept(false);

        string recorded_at() const;

        string version() const;

        string comment() const;

        uint32_t nchannels() const;

        uint32_t max_nhits() const;

        uint32_t ncoordinates() const;

        double to_nanosec() const;

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
        uint64_t end() const;

        bool operator==(uint64_t i) const;

        bool operator!=(uint64_t i) const;

        const LMFIterator &operator++() const;

        Event operator*() noexcept(false);
    };
}


#endif //LMFPY_LMFREADER_H
