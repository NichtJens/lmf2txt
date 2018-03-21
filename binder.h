//
// Created by Daehyun You on 3/22/18.
//

#ifndef LMFPY_BINDER_H
#define LMFPY_BINDER_H


#include <memory>
#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>

#include "LMF_IO.h"


using namespace std;
using namespace pybind11::literals;
namespace py = pybind11;


namespace lmfpy {
    class LMFReader : public LMF_IO {
    public:
        const uint8_t nchannelrooms, nhitrooms;

        LMFReader(const string &filename, uint8_t nchannelrooms, uint8_t nhitrooms);

        ~LMFReader() override;

        void check_error();

        int32_t version() const;

        chrono::system_clock::time_point time_fr() const;

        chrono::system_clock::time_point time_to() const;

        uint64_t len() const;

        uint64_t at() const;

        void at(uint64_t event);

        py::dict getitem(int64_t event);
    };


    class LMFIterator {
    private:
        LMFReader &reader;
        const uint8_t &nchannelrooms, nhitrooms;
        const uint8_t nchannels;
        vector<uint32_t> nhits;
        shared_ptr<vector<int32_t>> ptr_int;
        shared_ptr<vector<double>> ptr_float;

    public:
        explicit LMFIterator(LMFReader &reader, uint64_t event = 0);

        ~LMFIterator() = default;

        py::dict next();
    };
}


#endif //LMFPY_BINDER_H
