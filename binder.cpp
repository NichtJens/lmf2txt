//
// Created by Daehyun You on 3/22/18.
//

#include "binder.h"


using namespace lmfpy;


LMFReader::LMFReader(const string &filename, uint8_t nchannelrooms, uint8_t nhitrooms)
        : LMF_IO(nchannelrooms, nhitrooms), nchannelrooms(nchannelrooms), nhitrooms(nhitrooms) {
    auto ret = OpenInputLMF(filename);
    if (not ret) check_error();
}

LMFReader::~LMFReader() {
    CloseInputLMF();
    check_error();
}

void LMFReader::check_error() {
    if (errorflag == 0) return;
    if (errorflag == 2) throw runtime_error("Got error code: 2!");
    if (errorflag == 3) throw runtime_error("This file is already open!");
    if (errorflag == 4) throw runtime_error("Could not open the file!");
    if (errorflag == 6) throw runtime_error("Could not read the header!");
    if (errorflag == 7) throw runtime_error("This LMF is not supported!");
    if (errorflag == 8) throw runtime_error("Got error code: 8!");
    if (errorflag == 9) throw runtime_error("Got error code: 9!");
    if (errorflag == 10) throw runtime_error("Got error code: 10!");
    if (errorflag == 11) throw runtime_error("This file is already open!");
    if (errorflag == 12) throw runtime_error("Could not open a file for output!");
    if (errorflag == 14) throw runtime_error("Got error code: 14!");
    if (errorflag == 16) throw runtime_error("Got error code: 16!");
    if (errorflag == 18) throw runtime_error("Got error code: 18!");
    if (errorflag == 19) throw runtime_error("Got error code: 19!");
    if (errorflag == 20) throw runtime_error("Got error code: 20!");
    throw runtime_error("Got unknown error code: " + to_string(errorflag) + "!");
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

void LMFReader::at(uint64_t event) {  // todo: fix it!
    if (event == 0) {
        auto ret = fseek(input_lmf->file, Headersize + User_header_size, SEEK_SET);
        if (ret) throw runtime_error("File to seek an event: " + to_string(ret) + "!");
        uint64_number_of_read_events = 0;
        return;
    }
    if (len() <= event) throw out_of_range("The arg has to be less than: " + to_string(len()) + "!");
    uint64_t eventsize = 4 * number_of_channels +
                         ((data_format_in_userheader == 5) ? 8 : 4) * number_of_channels * max_number_of_hits;
    auto ret = fseek(input_lmf->file,
                     static_cast<uint32_t>(eventsize * event + Headersize + User_header_size),
                     SEEK_SET);
    if (ret) throw runtime_error("File to seek an event: " + to_string(ret) + "!");
    uint64_number_of_read_events = event;
}

py::dict LMFReader::getitem(int64_t event) {
    uint64_t mod = event % len();
    auto it = LMFIterator(*this, mod);
    return it.next();
}

LMFIterator::LMFIterator(LMFReader &reader, uint64_t event)
        : reader(reader), nchannelrooms(reader.nchannelrooms), nhitrooms(reader.nhitrooms),
          nchannels(static_cast<const uint8_t>(reader.GetNumberOfChannels())), nhits(nhitrooms) {
    reader.at(event);
    if (reader.data_format_in_userheader == 5)
        ptr_float = make_shared<vector<double>>(nchannelrooms * nhitrooms);
    else ptr_int = make_shared<vector<int32_t>>(nchannelrooms * nhitrooms);
}

py::dict LMFIterator::next() {
    auto b = reader.ReadNextEvent();
    if (not b) {
        if (reader.errorflag == 18 and reader.at() == reader.len()) {
            reader.errorflag = 0;
            reader.at(0);
            throw py::stop_iteration();
        }
        reader.check_error();
    }
    reader.GetNumberOfHitsArray(nhits.data());
    if (reader.data_format_in_userheader == 5) reader.GetTDCDataArray(ptr_float.get()->data());
    else reader.GetTDCDataArray(ptr_int.get()->data());
    map<string, uint32_t> m;
    for (auto i = 0; i < nchannels; ++i) {
        m["ch" + to_string(i)] = nhits[i];
    }
    return py::dict(  // todo: return other values
            "event"_a = reader.at() - 1,
            "timestamp"_a = reader.GetDoubleTimeStamp()  // seconds
    );
}

PYBIND11_MODULE(lmfpy, m) {
    py::class_<LMFReader>(m, "LMFReader")
            .def(py::init<const string &, int32_t, int32_t>(), "filename"_a, "nchannelrooms"_a = 80,
                 "nhitrooms"_a = 100)

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
            .def("__iter__", [](LMFReader &self) { return LMFIterator(self); })
            .def("__getitem__", [](LMFReader &self, int64_t event) { return self.getitem(event); });

    py::class_<LMFIterator>(m, "LMFIterator")
            .def("__iter__", [](LMFIterator &self) { return self; })
            .def("__next__", &LMFIterator::next);
}
