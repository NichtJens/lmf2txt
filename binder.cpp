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
//    if (len() <= event) throw out_of_range("The arg has to be less than: " + to_string(len()) + "!");
//    uint64_t eventsize = 4 * number_of_channels +
//                         ((data_format_in_userheader == 5) ? 8 : 4) * number_of_channels * max_number_of_hits;
//    auto ret = fseek(input_lmf->file,
//                     static_cast<uint32_t>(eventsize * event + Headersize + User_header_size),
//                     SEEK_SET);
//    if (ret) throw runtime_error("File to seek an event: " + to_string(ret) + "!");
//    uint64_number_of_read_events = event;
}

py::dict LMFReader::getitem(int64_t event) {
    uint64_t mod = event % len();
    auto it = iter(mod);
    return it.next();
}

LMFReader::LMFIterator &LMFReader::iter(uint64_t event) {
    at(event);
    if (not __iter) __iter = make_unique<LMFIterator>(*this);
    return *__iter.value();
}

LMFReader::LMFIterator::LMFIterator(LMFReader &reader)
        : reader(reader), nchannelrooms(reader.nchannelrooms), nhitrooms(reader.nhitrooms), nhits(nhitrooms) {
    if (reader.data_format_in_userheader == 5)
        ptr_float = make_shared<vector<double>>(nchannelrooms * nhitrooms);
    else ptr_int = make_shared<vector<int32_t>>(nchannelrooms * nhitrooms);
}

py::dict LMFReader::LMFIterator::next() {
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
    if (reader.data_format_in_userheader == 5) reader.GetTDCDataArray(ptr_float.get()->data());
    else reader.GetTDCDataArray(ptr_int.get()->data());
    py::dict ret;
    ret["event"] = reader.at() - 1;
    ret["timestamp"] = reader.GetDoubleTimeStamp();
//    for (auto i = 0; i < nchannels; ++i) {
//        string key = "ch" + to_string(i);
//        ret[key] = nhits[i];
//    }
    return ret;
//    map<string, uint32_t> m;
//    for (auto i = 0; i < nchannels; ++i) {
//        m["ch" + to_string(i)] = nhits[i];
//    }
//    return py::dict(  // todo: return other values
//            "event"_a = reader.at() - 1,
//            "timestamp"_a = reader.GetDoubleTimeStamp()  // seconds
//    );
}

PYBIND11_MODULE(lmfpy, m) {
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
            .def("__iter__", [](LMFReader &self) { return self.iter(); })
            .def("__getitem__", [](LMFReader &self, int64_t event) { return self.getitem(event); },
                 py::return_value_policy::take_ownership, py::call_guard<py::gil_scoped_release>());

    py::class_<LMFReader::LMFIterator, shared_ptr<LMFReader::LMFIterator>>(m, "LMFIterator")
            .def("__iter__", [](py::object &self) { return self; })
            .def("__next__", &LMFReader::LMFIterator::next,
                 py::return_value_policy::take_ownership, py::call_guard<py::gil_scoped_release>())
            ;
}
