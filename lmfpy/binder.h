//
// Created by Daehyun You on 3/22/18.
//

#ifndef LMFPY_BINDER_H
#define LMFPY_BINDER_H


#include <mutex>
#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>

#include "LMF_IO.h"


using namespace std;
using namespace pybind11::literals;
namespace py = pybind11;


namespace lmfpy {
    class LMFIterator;


    class LMFReader : public LMF_IO {
    public:
        const uint8_t nchannelrooms, nhitrooms;

        LMFReader(const string &filename, uint8_t nchannelrooms, uint8_t nhitrooms);

        ~LMFReader() override;

        int32_t version() const;

        chrono::system_clock::time_point time_fr() const;

        chrono::system_clock::time_point time_to() const;

        uint64_t len() const;

        uint64_t at() const;

        void at(uint64_t event);

        py::dict getitem(int64_t event);

    private:
        unique_ptr<LMFIterator> __iter;

    public:
        LMFIterator &iter(uint64_t event = 0);
    };


    class LMFIterator : public enable_shared_from_this<LMFIterator> {
    private:
        friend class LMFReader;
        LMFReader &reader;
        const uint8_t &nchannelrooms, nhitrooms, nchannels;
        vector<uint32_t> nhits;
        unique_ptr<vector<int32_t>> hits_int;
        unique_ptr<vector<double>> hits_float;
        mutex mut;

    public:
        explicit LMFIterator(LMFReader &reader);

        py::dict next();
    };
}

#endif //LMFPY_BINDER_H
