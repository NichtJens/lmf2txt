#!/usr/bin/env python3

from functools import partial
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
parser.add_argument("-o", "--output",
                    help="output, if not print it out",
                    type=str, default="")
args = parser.parse_args()


def main(reader: LMFReader, limit=None, k=None, out=None):
    if limit is None:
        limit = len(reader)
    if k is None:
        k = 1
    if out is None:
        out = partial(print, end="")

    out("               Filename: {}\n".format(reader.recorded_at))
    out("                Version: {}\n".format(reader.version))
    out("                Comment: {}\n".format(reader.comment))
    out("     Number of Channels: {}\n".format(reader.nchannels))
    out(" Maximum Number of Hits: {}\n".format(reader.max_nhits))
    out("  Number of Coordinates: {}\n".format(reader.ncoordinates))
    out("       Number of Events: {}\n".format(len(reader)))
    out("    TDC Resolution (ns): {:.6f}\n".format(reader.to_nanosec))
    out("             Start Time: {}\n".format(reader.time_fr))
    out("              Stop Time: {}\n".format(reader.time_to))

    for i, event in zip(range(limit), reader):
        if not i < limit:
            break
        out("########################\n")
        out("# {}\n".format(event.event))
        out("########################\n")
        out("                 At (s): {: 12.3f}\n".format(event.timestamp))
        for ch, n in enumerate(event.nhits):
            fr = sum(event.nhits[:ch])
            hits = ", ".join("{: 12.3f}".format(k*f) for f in event.hits[fr:fr+n])
            out("Channel #{:03d} {:3d} Hit(s): {}\n".format(ch, n, hits))


reader = LMFReader(args.filename)
limit = 100 if not args.full else None
k = None if not args.ns else reader.to_nanosec
if args.output != "":
    with open(args.output, 'w') as f:
        main(reader, limit=limit, k=k, out=f.write)
else:
    main(reader, limit=limit, k=k)
