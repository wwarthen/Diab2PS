# Diab2PS

Diablo 630 to PostScript Translator

## Description

The **diab2ps** program converts Diablo 630 encoded print files into PostScript encoded print files.
The resulting PostScript output files can be easily converted to PDF using common tools such as ps2pdf.

The primary use of this program is to allow the output of older word processors and text processors
to be printed on a modern printer or displayed in a modern PDF reader with very high quality.

The vast majority of Diablo 630 printer control codes are fully supported.  Additionally, you can
specify the page size, character pitch, and page orientation at startup.  Many advanced Diablo
630 print controls are supported including: auto-justification, auto-center, underline, bold,
shadow, horizontal and vertical tabs, margins, microspacing, alternate color, and backward printing.

The program is a 32-bit Windows command line application that should run on all versions of Microsoft
Windows XP and later.  The source code is straightforward C and can probably be adapted to other
operating systems with minimal modification.

## Usage

diab2ps [-fFPL] [-s {LET|A4}] [-t \<pitch\>] [-p \<prologue_file\>] [-o \<output_file\>] \<input_file\>

The \<input_file\> is a file containing Diablo 630 print output with imbedded printer control codes.  By default,
the program will produce a PostScript encoded output file with the same name as \<input_file\>, but with an
extension of ".ps".

The program includes a PostScript prologue file that is prepended to the output file.  This prologue file is required for the output file to be valid.  The default prologue file is called diab2ps.pro and is distributed with the program.  By default the program will expect to find the prologue file called diab2ps.pro in the same directory as the application.  The name and location of the prologue file can be overridden using the -p option.  Advanced users may create modified versions of the prologue file which would allow modifying things like font faces.

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

## Limitations

* Proportional space printing is not supported.  However, auto-justification and centering is fully supported.
* Auto carriage return is not supported.
* Reverse Printing is not supported.  However, backward printing is supported.
* Graphics and HyPlot modes are not supported.
* Extended character sets are not supported.

When the program encounters unsupported printer control codes, it will silently ignore them.  In most cases, this
results in reasonable output.

## Licensing

Copyright (C) 2017, Wayne Warthen, GPLv3

wwarthen@wwarthen.com
