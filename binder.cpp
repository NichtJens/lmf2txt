//
// Created by Daehyun You on 3/22/18.
//

#include "binder.h"


using namespace lmfpy;


LMFReader::LMFReader(const string &filename, uint8_t nchannelrooms, uint8_t nhitrooms)
        : LMF_IO(nchannelrooms, nhitrooms), nchannelrooms(nchannelrooms), nhitrooms(nhitrooms) {
    auto b = OpenInputLMF(filename);
    if (not b or not InputFileIsOpen) {  // expected error codes: 3, 4, 6, 7, 8
        if (errorflag == 3) throw runtime_error("This file is already open!");
        if (errorflag == 4) throw runtime_error("Could not open this file!");
        if (errorflag == 6) throw runtime_error("Fail to read the header!");
        if (errorflag == 7) throw runtime_error(
                    "This DAQ is not supported! Supported DAQs: 'TDC8', '2TDC8', 'TDC8HP', 'TDC8HPRAW', 'HM1', "
                    "'CAMAC', 'HM1_ABM', 'TCPIP', 'FADC8', 'FADC4'"
            );
        if (errorflag == 8) throw runtime_error(
                    "This format is not supported! Supported formats: 'USERDEF', 'SHORT', 'SLONG', 'DOUBLE', 'CAMAC'"
            );
        throw runtime_error("Unknown error!");
    }
}

LMFReader::~LMFReader() {
    CloseInputLMF();
}

int32_t LMFReader::version() const {
    return GetVersionNumber();
}

chrono::system_clock::time_point LMFReader::time_fr() const {
    return chrono::system_clock::from_time_t(Starttime);
}

chrono::system_clock::time_point LMFReader::time_to() const {
    return chrono::system_clock::from_time_t(Stoptime);
}

uint64_t LMFReader::len() const {
    return uint64_Numberofevents;
}

uint64_t LMFReader::at() const {
    return uint64_number_of_read_events;
}

void LMFReader::at(uint64_t event) {  // todo: support to seek other events!
    if (event == 0) {
        auto ret = fseek(input_lmf->file, Headersize + User_header_size, SEEK_SET);
        if (ret) throw runtime_error("File to seek an event: " + to_string(ret) + "!");
        input_lmf->error = 0;
        input_lmf->eof = false;
        uint64_number_of_read_events = 0;
        return;
    }
    throw runtime_error("Not supported yet!");
}

py::dict LMFReader::getitem(int64_t event) {
    uint64_t mod = event % len();
    auto &it = iter(mod);
    return it.next();
}

LMFIterator &LMFReader::iter(uint64_t event) {
    at(event);
    if (not __iter) __iter = make_unique<LMFIterator>(*this);  // todo: fix it using friend's permission
    return *__iter.value();
}

LMFIterator::LMFIterator(LMFReader &reader)
        : reader(reader), nchannelrooms(reader.nchannelrooms), nhitrooms(reader.nhitrooms),
          nchannels(static_cast<uint8_t>(reader.GetNumberOfChannels())), nhits(nhitrooms) {
    if (reader.data_format_in_userheader == 5) dfloat = vector<double>(nchannelrooms * nhitrooms);
    else dint = vector<int32_t>(nchannelrooms * nhitrooms);
}

py::dict LMFIterator::next() {
    lock_guard<mutex> locker(mut);
    auto b = reader.ReadNextEvent();
    if (not b) {  // expected error codes: 1, 2, 9, 14, 18
        // fine
        if (reader.errorflag == 18 and reader.at() == reader.len()) {
            reader.errorflag = 0;
            reader.at(0);
            throw py::stop_iteration();
        }
        // fatal errors!
        if (reader.errorflag == 1) throw runtime_error("Broken file!");
        if (reader.errorflag == 2) throw runtime_error("Wrong event length!");
        if (reader.errorflag == 9) throw runtime_error("Missing the reading file!");
        if (reader.errorflag == 14) throw runtime_error("Invalid arguments of DAQ 'CAMAC' reader!");
        if (reader.errorflag == 18) throw runtime_error("Broken file!");
        if (not reader.input_lmf->error == 0) throw runtime_error("Something wrong on the reading file!");
        throw runtime_error("Unknown error!");
    }
    reader.GetNumberOfHitsArray(nhits.data());
    if (reader.data_format_in_userheader == 5) reader.GetTDCDataArray(dfloat->data());
    else reader.GetTDCDataArray(dint->data());

    py::dict ret;
    ret["event"] = reader.at() - 1;
    ret["timestamp"] = reader.GetDoubleTimeStamp();
    for (auto i = 0; i < nchannels; ++i) {  // todo: return numpy array
        auto key = fmt::format("ch{:02d}", i);
        auto n = nhits[i];
        if (reader.data_format_in_userheader == 5) {
            py::list hits(n);
            for (auto j = 0; j < n; ++j) hits[j] = (*dfloat)[i * nhitrooms + j];
            ret[key.c_str()] = hits;
        } else {
            py::list hits(n);
            for (auto j = 0; j < n; ++j) hits[j] = (*dint)[i * nhitrooms +j];
            ret[key.c_str()] = hits;
        }
    }
    return ret;
}

PYBIND11_MODULE(lmfpy, m) {  // todo: add other values
    py::class_<LMFReader>(m, "LMFReader")
            .def(py::init<const string &, int32_t, int32_t>(),
                 "filename"_a, "nchannelrooms"_a = 80, "nhitrooms"_a = 100)

            .def_readonly("recorded_at", &LMFReader::FilePathName)
            .def_property_readonly("version", &LMFReader::version)
            .def_readonly("comment", &LMFReader::Comment)
                    // &LMFReader::Headersize
            .def_property_readonly("nchannels", &LMFReader::GetNumberOfChannels)
            .def_property_readonly("max_nhits", &LMFReader::GetMaxNumberOfHits)
                    // &LMFReader::Numberofcoordinates
                    // &LMFReader::timestamp_format
                    // &LMFReader::common_mode
                    // &LMFReader::data_format_in_userheader
                    // &LMFReader::DAQ_ID
            .def_readonly("to_nanosec", &LMFReader::tdcresolution)
            .def_property_readonly("time_fr", &LMFReader::time_fr)
            .def_property_readonly("time_to", &LMFReader::time_to)
                    // &LMFReader::TDC8HP

            .def("__len__", &LMFReader::len)
            .def("__iter__", [](LMFReader &self) -> LMFIterator & { return self.iter(); },
                 py::return_value_policy::reference)
            .def("__getitem__", [](LMFReader &self, int64_t event) { return self.getitem(event); },
                 py::return_value_policy::take_ownership, py::call_guard<py::gil_scoped_release>());

    py::class_<LMFIterator, shared_ptr<LMFIterator>>(m, "LMFIterator")
            .def("__iter__", [](py::object &self) { return self; })
            .def("__next__", &LMFIterator::next,
                 py::return_value_policy::take_ownership, py::call_guard<py::gil_scoped_release>());
}
