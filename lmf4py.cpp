#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/ndarrayobject.h>

#include "LMF_IO.h"


//aliases for overloaded methods
bool (LMF_IO::*OpenInputLMF)(__int8*) = &LMF_IO::OpenInputLMF;
bool (LMF_IO::*OpenOutputLMF)(__int8*) = &LMF_IO::OpenOutputLMF;
const char* (LMF_IO::*GetErrorText)(__int32) = &LMF_IO::GetErrorText;

//void (LMF_IO::*GetTDCDataArray_int)(__int32 *tdc) = &LMF_IO::GetTDCDataArray;
//void (LMF_IO::*GetTDCDataArray_double)(double *tdc) = &LMF_IO::GetTDCDataArray;


//wrap functions that expect array pointers to accept numpy arrays

void NP_GetNumberOfHitsArray(LMF_IO* lmfio, PyObject* obj) {

    // is the object an array?
    if (! PyArray_Check(obj)) {
        return;
    }

    PyArrayObject * arr = (PyArrayObject*) obj;

    // is the dtype integer?
    if (! PyArray_ISUNSIGNED(arr)) {
        return;
    }

    unsigned __int32* data = (unsigned __int32*) PyArray_DATA(arr);
    lmfio->GetNumberOfHitsArray(data);
}


void NP_GetTDCDataArray(LMF_IO* lmfio, PyObject* obj) {

    if (! PyArray_Check(obj)) {
        return;
    }

    PyArrayObject * arr = (PyArrayObject*) obj;
    int type = PyArray_TYPE(arr);

    switch(type) {
        case NPY_INT32: {
            __int32* data = (__int32*) PyArray_DATA(arr);
            lmfio->GetTDCDataArray(data);
            break;
        }
        case NPY_DOUBLE: {
            double* data = (double*) PyArray_DATA(arr);
            lmfio->GetTDCDataArray(data);
            break;
        }
        case NPY_UINT16: {
            unsigned __int16* data = (unsigned __int16*) PyArray_DATA(arr);
            lmfio->GetTDCDataArray(data);
            break;
        }
        default:
            return;
    }
}


void NP_WriteTDCData(LMF_IO* lmfio, PyObject* timestamp, PyObject* cnt, PyObject* tdc) {
    namespace py = boost::python;

    py::extract<unsigned __int64> timestamp_is_uint64(timestamp);
    py::extract<double> timestamp_is_double(timestamp);

    if ((! timestamp_is_uint64.check()) && (! timestamp_is_double.check())) {
        std::cout << "timestamp " << PyString_AsString(PyObject_Repr(timestamp)) << " is neither uint64 nor double" << std::endl;
        return;
    }

    if (! PyArray_Check(cnt)) {
        std::cout << "cnt is not a numpy array" << std::endl;
        return;
    }

    if (! PyArray_Check(tdc)) {
        std::cout << "tdc is not a numpy array" << std::endl;
        return;
    }

    PyArrayObject * cnt_arr = (PyArrayObject*) cnt;
    int cnt_type = PyArray_TYPE(cnt_arr);

    PyArrayObject * tdc_arr = (PyArrayObject*) tdc;
    int tdc_type = PyArray_TYPE(tdc_arr);


    if (cnt_type != NPY_UINT32) {
        std::cout << "cnt is not uint32" << std::endl;
        return;
    }

    unsigned __int32* cnt_data = (unsigned __int32*) PyArray_DATA(cnt_arr);

    switch(tdc_type) {
        case NPY_INT32: {
            __int32* tdc_data = (__int32*) PyArray_DATA(tdc_arr);
            if (timestamp_is_double.check()) {
                lmfio->WriteTDCData(timestamp_is_double(), cnt_data, tdc_data);
            } else {
                lmfio->WriteTDCData(timestamp_is_uint64(), cnt_data, tdc_data);
            }
            break;
        }
        case NPY_DOUBLE: {
            double* tdc_data = (double*) PyArray_DATA(tdc_arr);
            if (timestamp_is_double.check()) {
                lmfio->WriteTDCData(timestamp_is_double(), cnt_data, tdc_data);
            } else {
                lmfio->WriteTDCData(timestamp_is_uint64(), cnt_data, tdc_data);
            }
            break;
        }
        case NPY_UINT16: {
            unsigned __int16* tdc_data = (unsigned __int16*) PyArray_DATA(tdc_arr);
            if (timestamp_is_double.check()) {
                lmfio->WriteTDCData(timestamp_is_double(), cnt_data, tdc_data);
            } else {
                lmfio->WriteTDCData(timestamp_is_uint64(), cnt_data, tdc_data);
            }
            break;
        }
        default:
            std::cout << "tdc is none of (int32, double, uint16)" << std::endl;
            return;
    }

}



BOOST_PYTHON_MODULE(lmf4py)
{
    import_array();
    using namespace boost::python;


    class_<LMF_IO>("LMF_IO", init<__int32, __int32>())

        //regular methods
        .def("CloseInputLMF", &LMF_IO::CloseInputLMF)
        .def("CloseOutputLMF", &LMF_IO::CloseOutputLMF)
        .def("GetNumberOfChannels", &LMF_IO::GetNumberOfChannels)
        .def("GetMaxNumberOfHits", &LMF_IO::GetMaxNumberOfHits)
        .def("ReadNextEvent", &LMF_IO::ReadNextEvent)
        .def("GetEventNumber", &LMF_IO::GetEventNumber)
        .def("GetDoubleTimeStamp", &LMF_IO::GetDoubleTimeStamp)
        .def("GetLastLevelInfo", &LMF_IO::GetLastLevelInfo)

        //static methods
        .def("GetVersionNumber", &LMF_IO::GetVersionNumber)
        .staticmethod("GetVersionNumber")

        //overloaded methods
        .def("OpenInputLMF", OpenInputLMF)
        .def("OpenOutputLMF", OpenOutputLMF)
        .def("GetErrorText", GetErrorText)

        //methods with array arguments
        .def("GetNumberOfHitsArray", NP_GetNumberOfHitsArray)
        .def("GetTDCDataArray", NP_GetTDCDataArray)
        .def("WriteTDCData", NP_WriteTDCData)

        //member variables
        .def_readonly("FilePathName", &LMF_IO::FilePathName)
        .def_readonly("Versionstring", &LMF_IO::Versionstring)
        .def_readonly("Comment", &LMF_IO::Comment)
        .def_readonly("Headersize", &LMF_IO::Headersize)
        .def_readonly("Numberofcoordinates", &LMF_IO::Numberofcoordinates)
        .def_readonly("timestamp_format", &LMF_IO::timestamp_format)
        .def_readonly("common_mode", &LMF_IO::common_mode)
        .def_readonly("uint64_Numberofevents", &LMF_IO::uint64_Numberofevents)
        .def_readonly("data_format_in_userheader", &LMF_IO::data_format_in_userheader)
        .def_readonly("DAQ_ID", &LMF_IO::DAQ_ID)
        .def_readonly("tdcresolution", &LMF_IO::tdcresolution)
        .def_readonly("Starttime", &LMF_IO::Starttime)
        .def_readonly("Stoptime", &LMF_IO::Stoptime)
        .def_readonly("errorflag", &LMF_IO::errorflag)

        .def_readwrite("number_of_channels_output", &LMF_IO::number_of_channels_output)
        .def_readwrite("max_number_of_hits_output", &LMF_IO::max_number_of_hits_output)
        .def_readwrite("frequency", &LMF_IO::frequency)
        .def_readwrite("tdcresolution_output", &LMF_IO::tdcresolution_output)

        //custom member variables
        .def_readonly("TDC8HP", &LMF_IO::TDC8HP)
    ;


    class_<TDC8HP_struct>("TDC8HP_struct")

        //member variables
        .def_readonly("UserHeaderVersion", &TDC8HP_struct::UserHeaderVersion)
        .def_readonly("TriggerChannel_p64", &TDC8HP_struct::TriggerChannel_p64)
        .def_readonly("TriggerDeadTime_p68", &TDC8HP_struct::TriggerDeadTime_p68)
        .def_readonly("GroupRangeStart_p69", &TDC8HP_struct::GroupRangeStart_p69)
        .def_readonly("GroupRangeEnd_p70", &TDC8HP_struct::GroupRangeEnd_p70)

        .def_readwrite("GroupingEnable_p66_output", &TDC8HP_struct::GroupingEnable_p66_output)
        .def_readwrite("GroupRangeStart_p69", &TDC8HP_struct::GroupRangeStart_p69)
        .def_readwrite("GroupRangeEnd_p70", &TDC8HP_struct::GroupRangeEnd_p70)
    ;
}



