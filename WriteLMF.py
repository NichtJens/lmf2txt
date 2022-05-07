#!/usr/bin/env python

NUM_CHANNELS = 80
NUM_IONS     = 32


import numpy as np
import lmf4py


LMF = lmf4py.LMF_IO(NUM_CHANNELS, NUM_IONS)


if not LMF.OpenInputLMF("data/Cobold2011R5-2_Header-Template_1TDC8HP.lmf"):
    raise SystemExit("Opening input failed")

LMF.CloseInputLMF()


LMF.number_of_channels_output = 16
LMF.max_number_of_hits_output = 10
LMF.frequency = 1.
LMF.tdcresolution_output = 0.016
LMF.TDC8HP.GroupingEnable_p66_output = 0
LMF.TDC8HP.GroupRangeStart_p69 = -100.
LMF.TDC8HP.GroupRangeEnd_p70 = 1000.


if not LMF.OpenOutputLMF("testpy.lmf"):
    raise SystemExit("Opening output failed")



dTimeStamp = 0
igoodlines = 0
with open("data/input_file.dat") as f:
    for i, line in enumerate(f):
        line = line.split()
        if not line:
            continue

        ch = igoodlines % LMF.number_of_channels_output
        if ch == 0:
            dTimeStamp += 1
            number_of_hits = np.zeros(NUM_CHANNELS, dtype=np.uint32)
            TDC = np.zeros((NUM_CHANNELS, NUM_IONS), dtype=float)

        igoodlines += 1

        num = line[0]
        num = int(num)
        assert num + 1 == len(line)
#        if num == 0:
#            continue

        line = line[1:]
        times = map(float, line)

        number_of_hits[ch] = num
        TDC[ch][:num] = times

#        print ch
        if ch == LMF.number_of_channels_output - 1:
            TDC = TDC / LMF.tdcresolution_output
            LMF.WriteTDCData(dTimeStamp, number_of_hits, TDC)
            print i, dTimeStamp

LMF.CloseOutputLMF()
del LMF



