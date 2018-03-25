#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>

#include "LMFReader.h"


using namespace pybind11::literals;
namespace py = pybind11;
using namespace lmfpy;


PYBIND11_MODULE(lmfpy, m) {
    py::class_<LMFReader>(m, "LMFReader")
            .def(py::init<string, int32_t, int32_t>(),
                 "filename"_a, "nchannelrooms"_a = 32, "nhitrooms"_a = 64)
            .def_readonly("recorded_at", &LMFReader::FilePathName)
            .def_readonly("version", &LMFReader::Versionstring)
            .def_readonly("comment", &LMFReader::Comment)
            .def_property_readonly("nchannels", &LMFReader::GetNumberOfChannels)
            .def_property_readonly("max_nhits", &LMFReader::GetMaxNumberOfHits)
            .def_readonly("ncoordinates", &LMFReader::Numberofcoordinates)
            .def_readonly("to_nanosec", &LMFReader::tdcresolution)
            .def_property_readonly("time_fr", &LMFReader::time_fr)
            .def_property_readonly("time_to", &LMFReader::time_to)
            .def("__len__", &LMFReader::end)
            .def("__iter__", [](LMFReader &self) { return py::make_iterator(self.begin(), self.end()); },
                 py::keep_alive<0, 1>())  // todo: return with type annotation
            .def("__getitem__", [](LMFReader &self, int64_t event) { return self[event]; },
                 py::return_value_policy::move, py::call_guard<py::gil_scoped_release>());


    py::class_<Event>(m, "Event")
            .def_readonly("event", &Event::event)
            .def_readonly("timestamp", &Event::timestamp)
            .def_readonly("nhits", &Event::nhits)
            .def_readonly("hits", &Event::hits);
}
