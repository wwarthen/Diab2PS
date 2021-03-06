# Diab2PS

Diablo 630 to PostScript Translator

## Description

The **diab2ps** program converts Diablo 630 encoded print files into PostScript encoded print files.
The resulting PostScript output files can be easily converted to PDF using common tools such as ps2pdf.
They should also be printable directly to a PostScript printer.

The primary use of this program is to allow the output of older word processors and text processors
to be printed on a modern printer or displayed in a modern PDF reader with very high quality.

The vast majority of Diablo 630 printer control codes are fully supported.  Additionally, you can
specify the initial page size, character pitch, and page orientation at startup.  Many advanced Diablo
630 print controls are supported including: auto-justification, auto-center, auto-underscore, bold,
shadow, horizontal and vertical tabs, margins, microspacing, alternate color, and backward printing.

This is a command line program for either Windows (32-bit or 64-bit) or Unix/Linux.  The source code is
straightforward C and will currently compile under Open Watcom (Windows) and GCC (Unix/Linux).

## Usage

diab2ps [-fFPL] \[-s {LET|A4}\] \[-t \<pitch\>\] \[-p \<prologue_file\>\] \[-o \<output_file\>\] \<input_file\>

The \<input_file\> is a file containing Diablo 630 print output with imbedded printer control codes.  By default,
the program will produce a PostScript encoded output file with the same name as \<input_file\>, but with the
extension replaced with ".ps".

The program includes a PostScript prologue file that is prepended to the output file.  This prologue file is required for the output file to be valid.  The default prologue file is called diab2ps.pro and is distributed with the program.  By default the program will expect to find the prologue file called diab2ps.pro in the same directory as the application (Windows) or in /usr/share/diab2ps/ (Unix/Linux).  The name and location of the prologue file can be overridden using the -p option. Advanced users may create modified versions of the prologue file which would allow modifying things like font faces.

The program has no automated installation.  Simply copy the program file (diab2ps.exe) and the prologue file (diab2ps.pro) to the appropriate directories and execute the application from a command prompt.

### Windows Installation

For Windows, a precompiled application is available.  You can download the zip file, extract the contents, and copy diab2ps.exe to any directory that is in your PATH search list.  If you are not clear what that is, you may simply copy the program to "C:\Program Files (x86)".  You should also copy diab2ps.pro to the same directory since the program will look for the prologue file in the same directory as the application by default.  Alternatively, you can specifiy the location of the prologue file using the -p option on the command line.

### Unix/Linux Installation

For Unix/Linux, you will need to compile the program first.  Download and decompress the source zip file.  The application is a single source file called diab2ps.cpp.  You will need the GCC compiler to build it.  Enter "g++ diab2ps.cpp" on the Unix/Linux command line to build the program.  The result will be called "a.out".  Normally, you would copy the file to /usr/bin/diab2ps.  You may also need to set the execute bit on the file using the command "chmod +x /usr/bin/diab2ps".  Additionally, you need to copy the prologue file (diab2ps.pro) to /usr/share/diab2ps/diab2ps.pro which is where the program will look for it by default.

## Options

| Option | Description |
| --- | --- |
| &#8209;f | Suppress the first formfeed control character in the input file.  In some cases, the input file will have an initial formfeed which results in a blank page at the start of the output file.  This option will prevent this. |
| &#8209;F | Suppress the final formfeed control character in the input file.  In some cases, the input file will have a final formfeed which results in a blank page at the end of the output file.  This option will prevent this. |
| &#8209;P | Specifies portrait orientation for the pages in the output file.  This is the default orientation. |
| &#8209;L | Specifies landscape orientation for the pages in the output file. |
| &#8209;t&nbsp;\<pitch\> | Specifies the character spacing (pitch) of the characters in the output file.  This is an integer value that specifies the number of characters that will be printed horizontally per inch by default.  Diablo 630 printer control codes can override this in the input file. |
| &#8209;s&nbsp;{LET\|A4} | Specifies the page size to be created in the output file.  The program currently supports letter (LET) and A4 page sizes. |
| &#8209;p&nbsp;\<prologue_file\> | Specifies the name of the prologue file to prepend to the output file instead of the default file. |
| &#8209;o&nbsp;\<output_file\> | Overrides the default output file name. |

## Notes

* The bold print mode on a Diablo 630 is implemented by mechanically double striking the character.  This wold have no effect in a PostScript file.  As a result, the bold print mode is implemented as italic font.
* The standard Courier PostScript font is used.  This is a mono-spaced font, so Diablo 630 proportional spacing has no effect, but does not cause any issues in general.  Auto-justification and auto-centering are fully supported.
* Auto carriage return is not supported, but this is generally not used or relevant.
* Reverse Printing is not supported.  However, backward printing is supported.
* Graphics and HyPlot modes are not supported.
* Extended character sets are not supported.
* Output has been verified to work well with ps2pdf for creation of PDF files.  Alternative PostScript to PDF converters (such as Adobe Acrobat Distiller) have not been tested.

## Licensing

Copyright (C) 2017, Wayne Warthen, GPLv3

wwarthen@wwarthen.com
