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
            .def_property_readonly("recorded_at", &LMFReader::recorded_at)
            .def_property_readonly("version", &LMFReader::version)
            .def_property_readonly("comment", &LMFReader::comment)
            .def_property_readonly("nchannels", &LMFReader::nchannels)
            .def_property_readonly("max_nhits", &LMFReader::max_nhits)
            .def_property_readonly("ncoordinates", &LMFReader::ncoordinates)
            .def_property_readonly("to_nanosec", &LMFReader::to_nanosec)
            .def_property_readonly("time_fr", &LMFReader::time_fr)
            .def_property_readonly("time_to", &LMFReader::time_to)
            .def("__len__", &LMFReader::end)
            .def("__iter__", [](LMFReader &self) {
                return py::make_iterator<py::return_value_policy::move>(self.begin(), self.end(),
                                                                        py::call_guard<py::gil_scoped_release>());
            }, py::keep_alive<0, 1>())
            .def("__getitem__", [](LMFReader &self, int64_t event) -> Event { return self[event]; },
                 py::return_value_policy::move, py::call_guard<py::gil_scoped_release>());


    py::class_<Event>(m, "Event")
            .def_readonly("event", &Event::event)
            .def_readonly("timestamp", &Event::timestamp)
            .def_readonly("nhits", &Event::nhits)  // todo: return numpy array
            .def_readonly("hits", &Event::hits);
}
