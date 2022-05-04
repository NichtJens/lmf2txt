
______________________________________________________
Cobold2011R5-2_Header-Template_1TDC8HP.lmf
This is a template file for a typical LMF file.
This file must exist in this folder.

______________________________________________________
input_file.dat
This file is an example file that contains signals (in nanoseconds).
WriteLMF.exe will read this file and convert it into a LMF file.

______________________________________________________
LMF_IO.cpp  and LMF_IO.h
These files are used by the converter. Please ignore them.

______________________________________________________
lmf2txt.exe -name_of_LMF [-ns]
A converter that converts LMF back into an ASCII-file.
You can use it to check the written LMF files.

(Use the argument -ns to write nanoseconds.)

______________________________________________________


WriteLMF reads input_file.dat and concerts it into an LMF.
But this can of course be changed.

Achim Czach <czasch@roentdek.com> , 2022