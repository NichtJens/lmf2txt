# lmf2txt

Modified version of lmf2txt as originally found under:

http://www.atom.uni-frankfurt.de/czasch/default_files/software/LMF_tools/lmf2txt/lmf2txt.zip

Can be compiled under Linux via a simple `make`.

The Visual Studio project for Windows compilation should also still work fine. It hasn't been tested, though...

---

# lmf4py

[Boost.Python](https://wiki.python.org/moin/boost.python)-based wrapper of LMF_IO.

Compile the library via `make python`. Needs a `make clean-objects` before if lmf2txt has been compiled to ensure the shared object can be generated.

So far, only classes/structs used in the original C lmf2txt are wrapped.

A [Python replica](../master/lmf2txt.py) of the C lmf2txt is included. Note that the code is still very C-like. The library will be made more pythonic in the future.
