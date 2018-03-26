from argparse import ArgumentParser
from lmfpy import LMFReader


parser = ArgumentParser()
parser.add_argument("filename", help="LMF data file")
parser.add_argument("-f", "--full",
                    help="if omitted only the header and the first 100 events are read",
                    action='store_true')
parser.add_argument("-n", "--ns",
                    help="convert values to nanoseconds",
                    action='store_true')
args = parser.parse_args()


afile = LMFReader(args.filename)
print("Filename: {}".format(afile.recorded_at))
print("Version: {}".format(afile.version))
print("Comment: {}".format(afile.comment))
print("Number of Channels: {}".format(afile.nchannels))
print("Maximum Number of Hits: {}".format(afile.max_nhits))
print("Number of Coordinates: {}".format(afile.ncoordinates))
print("Number of Events: {}".format(len(afile)))
print("TDC Resolution: {:.6f} ns".format(afile.to_nanosec))
print("Start Time: {}".format(afile.time_fr))
print("Stop Time: {}".format(afile.time_to))

limit = 100 if not args.full else len(afile)
k = 1 if not args.ns else afile.to_nanosec
for i, event in enumerate(afile):
    if not i < limit:
        break
    print("------- #{} -------".format(event.event))
    print("At: {:.6f} s".format(event.timestamp))
    for ch, n in enumerate(event.nhits):
        fr = sum(event.nhits[:ch])
        hits = ", ".join("{: 9.3f}".format(k*f) for f in event.hits[fr:fr+n])
        print("Channel #{:03d} Total {:3d} Hit(s): {}".format(ch, n, hits))
