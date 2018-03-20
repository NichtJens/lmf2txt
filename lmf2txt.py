#!/usr/bin/env python

NUM_CHANNELS = 80
NUM_IONS = 200

import argparse

parser = argparse.ArgumentParser()
parser.add_argument("filename", help="LMF data file")
parser.add_argument("-f", "--full", help="if omitted only the header and the first 100 events are read",
                    action='store_true')
parser.add_argument("-n", "--ns", help="convert values to nanoseconds", action='store_true')
clargs = parser.parse_args()

ifname = clargs.filename
only_100_events_flag = not clargs.full
time_conversion_flag = clargs.ns

from time import ctime
import numpy as np

import lmf4py

LMF = lmf4py.LMF_IO(NUM_CHANNELS, NUM_IONS)
if not LMF.OpenInputLMF(ifname):
    raise SystemExit("Open failed")

print "File name =", LMF.FilePathName
print "Versionstring =", LMF.Versionstring
print "Comment =", LMF.Comment

print "Headersize =", LMF.Headersize

print "Number of channels =", LMF.GetNumberOfChannels()
print "Number of hits =", LMF.GetMaxNumberOfHits()
print "Number of Coordinates =", LMF.Numberofcoordinates

print "Timestamp info =", LMF.timestamp_format
print "Common start" if LMF.common_mode == 0 else "Common stop"
print "Number of events =", LMF.uint64_Numberofevents
print "Data format =", LMF.data_format_in_userheader
print "DAQ_ID =", hex(LMF.DAQ_ID)

print "TDC resolution = {:.6f} ns".format(LMF.tdcresolution)
if LMF.DAQ_ID == 0x000008 or LMF.DAQ_ID == 0x000010:
    #    print "TDC8HP Header Version", LMF.TDC8HP.UserHeaderVersion
    print "Trigger channel = {} (counting from 1)".format(LMF.TDC8HP.TriggerChannel_p64)
    print "Trigger dead time = {:f} ns".format(LMF.TDC8HP.TriggerDeadTime_p68)
    print "Group range start = {:f} ns".format(LMF.TDC8HP.GroupRangeStart_p69)
    print "Group range end = {:f} ns".format(LMF.TDC8HP.GroupRangeEnd_p70)

print "Starttime:", ctime(LMF.Starttime)
print "Stoptime: ", ctime(LMF.Stoptime)
print

# raise SystemExit


number_of_hits = np.zeros((NUM_CHANNELS,), dtype=np.uint32)
iTDC = np.zeros((NUM_CHANNELS, NUM_IONS), dtype=np.int32)
dTDC = np.zeros((NUM_CHANNELS, NUM_IONS), dtype=np.double)

first_timestamp = last_timestamp = 0

while LMF.ReadNextEvent():
    print "------- #{} -------".format(LMF.GetEventNumber())

    new_timestamp = LMF.GetDoubleTimeStamp()
    if first_timestamp == 0:
        first_timestamp = new_timestamp

    new_timestamp -= first_timestamp

    if LMF.timestamp_format != 0:
        print "T  = {:.3f} ns = \t{:f} s".format(new_timestamp * 1.e9, new_timestamp)
        print "dT = {:.3f} ns = \t{:f} s".format((new_timestamp - last_timestamp) * 1.e9,
                                                 new_timestamp - last_timestamp)

    last_timestamp = new_timestamp

    LMF.GetNumberOfHitsArray(number_of_hits)

    if LMF.errorflag:
        raise SystemExit(LMF.GetErrorText(LMF.errorflag))

    if LMF.data_format_in_userheader == 5:
        LMF.GetTDCDataArray(dTDC)
    else:
        LMF.GetTDCDataArray(iTDC)

    if LMF.errorflag:
        raise SystemExit(LMF.GetErrorText(LMF.errorflag))

    for i in xrange(LMF.GetNumberOfChannels()):
        print "chan {:5d} {:5d}".format(i + 1, number_of_hits[i]),

        for j in xrange(number_of_hits[i]):
            if LMF.data_format_in_userheader == 5:
                print "{:f}".format(dTDC[i][j]),
            else:
                if time_conversion_flag:
                    print "{:.3f}".format(iTDC[i][j] * LMF.tdcresolution),
                else:
                    print "{:5d}".format(iTDC[i][j]),

        print

    print "Levelinfo:", LMF.GetLastLevelInfo()
    print

    if only_100_events_flag and LMF.GetEventNumber() > 100:
        break
