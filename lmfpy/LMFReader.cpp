#include "LMFReader.h"


using namespace lmfpy;


Event::Event(uint64_t event, double timestamp, vector<uint32_t> nhits, vector<double> hits)
        : event(event), timestamp(timestamp), nhits(nhits), hits(hits) {
}

uint64_t LMFReader::at() const {
    return uint64_number_of_read_events;
}

void LMFReader::at(uint64_t event) {  // todo: support to seek other events!
    lock_guard<mutex> locker(mut);
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

LMFReader::LMFReader(string filename, uint32_t nchannelrooms, uint32_t nhitrooms)
        : LMF_IO(nchannelrooms, nhitrooms), nchannelrooms(nchannelrooms), nhitrooms(nhitrooms) {
    auto b = OpenInputLMF(move(filename));
    if (not b or not InputFileIsOpen) {  // expected error codes: 3, 4, 6, 7, 8
        if (errorflag != 0) throw runtime_error(error_text[errorflag]);
        throw runtime_error("Unknown error!");
    }
}

chrono::system_clock::time_point LMFReader::time_fr() const {
    return chrono::system_clock::from_time_t(Starttime);
}

chrono::system_clock::time_point LMFReader::time_to() const {
    return chrono::system_clock::from_time_t(Stoptime);
}

LMFIterator LMFReader::begin(uint64_t event) {
    at(event);
    return LMFIterator(*this);
}

uint64_t LMFReader::end() const {
    return uint64_Numberofevents;
}

Event LMFReader::operator[](int64_t event) {
    uint64_t mod = event % end();
    return *begin(mod);
}

LMFIterator::LMFIterator(LMFReader &reader)
        : reader(&reader), nchannelrooms(reader.nchannelrooms), nhitrooms(reader.nhitrooms),
          nchannels(static_cast<uint8_t>(reader.GetNumberOfChannels())),
          type((reader.data_format_in_userheader == 5) ? float_ : int_), ihits(0), fhits(0) {
    if (type == int_) ihits.resize(nchannelrooms * nhitrooms);
    if (type == float_) fhits.resize(nchannelrooms * nhitrooms);
}

bool LMFIterator::operator==(uint64_t i) const {
    return reader->at() == i;
}

bool LMFIterator::operator!=(uint64_t i) const {
    return reader->at() != i;
}

const LMFIterator &LMFIterator::operator++() const {
    return *this;
}

Event LMFIterator::operator*() {
    vector<uint32_t> nhits(nchannelrooms);
    vector<double> flatten;
    flatten.reserve(nchannels * nhitrooms);

    if (reader == nullptr) return {};
    lock_guard<mutex> locker(reader->mut);
    auto b = reader->ReadNextEvent();
    if (not b) {  // expected error codes: 1, 2, 9, 14, 18
        if (reader->errorflag != 0) throw runtime_error(reader->error_text[reader->errorflag]);
        if (reader->input_lmf->error != 0) throw runtime_error("Something wrong on the reading file!");
        throw runtime_error("Unknown error!");
    }

    reader->GetNumberOfHitsArray(nhits.data());
    nhits.erase(nhits.begin() + nchannels, nhits.begin() + nchannelrooms);
    if (type == int_) reader->GetTDCDataArray(ihits.data());
    if (type == float_) reader->GetTDCDataArray(fhits.data());
    if (type == int_) {
        for (auto i = 0; i < nchannels; ++i) {
            for (auto j = 0; j < nhits[i]; ++j) {
                flatten.push_back(static_cast<double>(ihits[i * nhitrooms + j]));
            }
        }
    }
    if (type == float_) {
        for (auto i = 0; i < nchannels; ++i) {
            for (auto j = 0; j < nhits[i]; ++j) {
                flatten.push_back(static_cast<double>(fhits[i * nhitrooms + j]));
            }
        }
    }
    return {
            .event = reader->at() - 1,
            .timestamp = reader->GetDoubleTimeStamp(),
            .nhits = move(nhits),
            .hits = move(flatten)
    };
}

uint64_t LMFIterator::end() const {
    return reader->end();
}
