/*
  Diab2PS - Diablo 630 to Postscript Translator
	Copyright (C) 2017, Wayne Warthen, GPLv3
	Version 0.9.1, March 24, 2017
*/

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/		

/*
		For Windows:
			Build with Open Watcom C/C++: "cl diab2ps.cpp"
			Place prologue file (diab2ps.pro) in same directory as executable
		
		For Unix/Linux:
			Build with GCC: "g++ diab2ps.cpp"
			Place prologue file (diab2ps.pro) in /usr/share/diab2ps/
*/
		
/*
    Things not supported:
		- Auto Backward Printing (not relevant)
		- Auto Carriage Return
		- Reverse Printing
		- Graphics Mode
		- HyPlot
		- Proportional Space Printing
		- Carriage Settling Time (not relevant)
		- Program Mode
		- Feeder Control Commands
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>

#define NUL 0x00
#define BS 0x08
#define HT 0x09
#define LF 0x0A
#define VT 0x0B
#define VT 0x0B
#define FF 0x0C
#define CR 0x0D
#define SO 0x0E
#define DC1 0x11
#define SYN 0x16
#define CAN 0x18
#define EM 0x19
#define SUB 0x1A
#define ESC 0x1B
#define GS 0x1D
#define RS 0x1E
#define US 0x1F
#define DEL 0x7F

#define ST_IDLE 0
#define ST_PRINT 1
#define ST_ESC1 2
#define ST_ESC2 3

const char szEsc1[] = {
	'T', 'L', 'C', '9', '0', '1', '8', '-', '2', 'S',
	'/', '\\', '?', '!', '<', '>', '6', '5', 'U', 'D',
	LF, '3', '4', 'A', 'B', '7', 'Y', 'Z', 'P', 'Q',
	'E', 'R', 'O', 'W', '&', '%', 'N', 'M', '=', BS,
	'X', 0
};

const char szEsc2[] = {
	FF, US, HT, RS, VT, DC1, SO, CR, SO, CAN, SYN, GS, 0
};

bool bEOF = false;
FILE *fIn, *fOut;
char sLineBuf[256] = "";
int nLineBuf = 0;
int nLineIdx = 0;
bool bCenter = false;
bool bJustify = false;
int nState = ST_PRINT;
int cSav;
bool bLeading = true;
bool bFirstFF = true;
bool bSupFirstFF = false;
bool bSupFinalFF = false;
bool bLandscape = false;
int nPitch = 12;

void Usage()
{
  printf("Usage: diab2ps [-fFPL] [-s {LET|A4}] [-t <pitch>] [-p <prologuefile>] [-o <outputfile>] <inputfile>\n");
}

int Error(const char * szErrDesc, ...)
{
	va_list arglist;
	
	va_start(arglist, szErrDesc);
	printf("\nERROR: ");
	vprintf(szErrDesc, arglist);
	printf("\n");
	va_end(arglist);
	
	return 8;
}

int DoEsc2(int c)
{
  switch (cSav)
  {
    case HT: // Initiate absolute horizontal tab to print position (n)
      fprintf(fOut, " %i AbsHT", c);
      break;
      
    case VT: // Initiate absolute vertical tab to line (n)
      fprintf(fOut, " %i AbsVT", c);
      break;
			
		case US: // Set Horizontal Motion Index (HMI) to (n-1)
			fprintf(fOut, " %i SetHMI", c - 1);
			break;
			
		case RS: // Set Vertical Motion Index (VMI) to (n-l)
			fprintf(fOut, " %i SetVMI", c - 1);
			break;
			
		case FF: // Set lines per page to (n)
			fprintf(fOut, " ClrTop ClrBottom");
			break;
			
		case SUB: // Remote diagnostics...
			if (c == 'I')
  			fprintf(fOut, "\r\n\r\nReset\r\n\r\n");
			break;
			
		case CR: // First byte of remote reset
			if (c == 'P')
  			fprintf(fOut, "\r\n\r\nReset\r\n\r\n");
			break;
  }
	
  return ST_IDLE;
}

int CountChars()
{
  int i;
  int n = 0;
  int c;
  
  for (i = nLineIdx; i < nLineBuf; i++)
  {
    c = sLineBuf[i];

    if (c >= ' ')
    {
      n++;
      continue;
    }

    if (c == BS)
      n--;

    if (c == ESC)
    {
			i++;
      c = sLineBuf[i];
			if (strchr(szEsc1, c))
			{
				switch (c)
				{
					case 'X':
						return 0;	// Cancel WP
						
					case 'Y':
					case 'Z':
					  n++;	// Count special chars
						break;
				}
			}
			else if (strchr(szEsc2, c))
				i++;	// Handle additonal parm char
		}
  }
  
  return n;
}

int CountBlanks()
{
  int i;
  int n = 0;
  int c;
  
  for (i = nLineIdx; i < nLineBuf; i++)
  {
    c = sLineBuf[i];

    if (c == ' ')
    {
      n++;
      continue;
    }

    if (c == ESC)
    {
			i++;
      c = sLineBuf[i];
			if (strchr(szEsc1, c))
			{
				if (c == 'X')
					return 0;
			}
			else if (strchr(szEsc2, c))
				i++;	// Handle additonal parm char
		}
	}
  
  return n;
}

int DoEsc1(int c)
{
  if (strchr(szEsc2, c))
  {
    cSav = c;
    return ST_ESC2;
  }

  switch (c)
  {
		case 'T': // Set top page margin at current paper position
			fprintf(fOut, " SetTop");
			break;

		case 'L': // Set bottom page margin at current paper position
			fprintf(fOut, " SetBottom");
			break;
			
		case 'C': // Clear top and bottom margins
			fprintf(fOut, " ClrTop ClrBottom");
			break;

    case '9': // Set left margin at current paper position
      fprintf(fOut, " SetLeft");	
      break;
      
    case '0': // Set right margin at current paper position
      fprintf(fOut, " SetRight");	
      break;
			
		case 'U': // Perform half-line feed
      fprintf(fOut, " HalfLineFeed");	
      break;
      
		case 'D': // Perform negative half-line feed
      fprintf(fOut, " RevHalfLineFeed");	
      break;
      
		case LF: // Perform negative line feed
      fprintf(fOut, " RevLineFeed");
      break;
			
		case 'A': // Print in secondary color (red)
      fprintf(fOut, " SecColor");
      break;
			
		case 'B': // Print in primary color (black)
      fprintf(fOut, " PriColor");
      break;
			
    case 'O': // Bold print on
      fprintf(fOut, " Bold");
      break;
      
    case 'W': // Shadow print on
      fprintf(fOut, " Shadow");
      break;
      
    case 'X': // Cancel WP modes except proportional
      fprintf(fOut, " Normal ClrUnderline");
      bCenter = false;
      bJustify = false;
      break;
      
    case '&': // Cancel Bold and Shadow printing
      fprintf(fOut, " Normal");
      break;
      
    case '=': // Auto center
      bCenter = true;
			bLeading = true;
      break;
      
    case 'M': // Begin autojustify
      bJustify = true;
			bLeading = true;
      break;
      
    case 'E': // Begin auto underscore
      fprintf(fOut, " SetUnderline");
      break;
    
    case 'R': // End auto underscore
      fprintf(fOut, " ClrUnderline");
      break;
			
		case '5': // Enable Forward Printing mode
      fprintf(fOut, " SetForward");
      break;
			
		case '6': // Enable Backward Printing mode
      fprintf(fOut, " SetBackward");
      break;
		
		case '1': // Set horizontal tab stop at current carriage (print) position
      fprintf(fOut, " HTabSet");
      break;
		
		case '8': // Clear horizontal tab stop at current carriage (print) position
      fprintf(fOut, " HTabClr");
      break;
		
		case '-': // Set vertical tab stop at current paper position
      fprintf(fOut, " VTabSet");
      break;
		
		case '2': // Clear all horizontal and vertical tab stops
      fprintf(fOut, " HTabInit VTabInit");
      break;
		
		case 'S': // Return HMI control to the SPACING switch
      fprintf(fOut, " ResetHMI");
      break;
			
		case BS: // Backspace 1/120"
      fprintf(fOut, " MicroBS");
			break;
			
		case 'Y': // Print the print wheel character under ASCII code 20 hex (cent)
      fprintf(fOut, " (%c) MyShow", 0xA2);
			break;
			
		case 'Z': // Print the print wheel character under ASCII code 7F hex (quotesingle)
      fprintf(fOut, " (%c) MyShow", 0xA9);
			break;
			
		case '7': // Enable Print Suppression (Cleared by CR)
      fprintf(fOut, " SetSuppress");
			break;
  }

  return ST_IDLE;
}

int DoCtl(int c)
{
  switch(c)
  {
    case FF:
		  if (bSupFirstFF && bFirstFF)
				bFirstFF = false;
			else
				fprintf(fOut, "\r\nFormFeed");
      break;

    case CR:
			bCenter = false;
			bLeading = true;
      fprintf(fOut, "\r\nBeginLine");
      break;

    case LF:
      fprintf(fOut, "\r\nLineFeed");
      break;
      
    case BS:
      fprintf(fOut, " Backspace");
      break;

    case ESC:
      return ST_ESC1;
  }

  return ST_IDLE;
}

void EmitChar(int c)
{
  if ((c == '(') || (c == ')') || (c == '\\'))
    fputc('\\', fOut);
  fputc(c, fOut);
}

int ReadChr(void)
{
	int c;
	
	if (bEOF)
		return EOF;

	do
	{
		c = fgetc(fIn);
	} while ((c == NUL) || (c == DEL));

	bEOF = (c == EOF);

	return c;
}

int NextChr(void)
{
	int c;
	
	if (nLineIdx < nLineBuf)
		return (sLineBuf[nLineIdx++]);

	return ReadChr();
}

void BufferLine(int c)
{
	if (nLineIdx < nLineBuf)
	{
		if (nLineIdx > 0)
		{
			sLineBuf[--nLineIdx] = c;
		}
		else
		{
			memcpy(sLineBuf, sLineBuf + 1, nLineBuf++);
			sLineBuf[0] = c;
		}
		
		return;
	}
	
	nLineBuf = nLineIdx = 0;
	
	do
	{
		sLineBuf[nLineBuf++] = c;

		if (c == ESC)
		{
			// read and store ESC parm char
			if ((c = ReadChr()) == EOF)
				break;

			sLineBuf[nLineBuf++] = c;

			if (strchr(szEsc2, c))
			{
				// 2 parm chars, read and store second char
				if ((c = ReadChr()) == EOF)
					break;

				sLineBuf[nLineBuf++] = c;
			}
		}
		else
		{
			if ((c == CR) || (c == LF) || (c == FF))
				break;
		}
	} while ((c = ReadChr()) != EOF);
}

void DoLine(void)
{
	if (bCenter)
	{
		int n = CountChars();
		if (n)
			fprintf(fOut, " %i Center", n);
	}
	else if (bJustify)
	{
		int nc = CountChars();
		int nb = CountBlanks();

		if (nc && nb)
			fprintf(fOut, " %i %i Justify", nc, nb);
	}

	bLeading = false;
}

void Process (int c)
{
	switch (nState)
	{
		case ST_IDLE:
		  if (c < ' ')
			{
				nState = DoCtl(c);
				break;
			}
			
			if (bLeading && (c != ' '))
			{
				if (bCenter || bJustify)
				{
					BufferLine(c);
					DoLine();
					break;
				}
			}
			
			fprintf(fOut, " (");
			EmitChar(c);
			nState = ST_PRINT;
		  break;
			
		case ST_PRINT:
		  if (c < ' ')
			{
				fprintf(fOut, ") MyShow");
			  nState = DoCtl(c);
			}
			else if (bLeading && (c != ' ') && (bCenter || bJustify))
			{
				fprintf(fOut, ") MyShow");
				nState = ST_IDLE;
				Process(c);
			}
			else
		    EmitChar(c);
			break;

		case ST_ESC1:
		  nState = DoEsc1(c);
		  break;
		
		case ST_ESC2:
		  nState = DoEsc2(c);
		  break;
	}
}

int main(int argc, char *argv[])
{
	char szPrologueFileSpec[PATH_MAX] = "";
	char szInputFileSpec[PATH_MAX] = "";
	char szOutputFileSpec[PATH_MAX] = "";
	
#if __WATCOMC__
	char szDrive[_MAX_DRIVE] = "";
	char szDir[_MAX_DIR] = "";
	char szFileName[_MAX_FNAME] = "";
	char szExt[_MAX_EXT] = "";
#endif

	char szPageSize[20] = "LET";
	int c;
	
	printf("Diablo 630 to PostScript Translator, v0.9.1\n");
	printf("Copyright (C) 2017, Wayne Warthen, GPLv3\n");
	
	while ((c = getopt(argc, argv, ":fFPLs:c:p:o:")) != -1)
	{
		switch (c)
		{
			case 'f':	// Suppress first FF
				bSupFirstFF = true;
				break;
				
			case 'F':	// Suppress final FF
			  bSupFinalFF = true;
				break;
				
			case 'P':	// Portrait
			case 'L':	// Landscape
			  bLandscape = (c == 'L');
				break;
				
			case 'p':	// Prologue file override
				strcpy(szPrologueFileSpec, optarg);
				break;
				
			case 'o':	// Output file override
				strcpy(szOutputFileSpec, optarg);
				break;
				
			case 's':
				strcpy(szPageSize, optarg);
				for (char * p = szPageSize; *p != '\0'; p++)
					*p = toupper(*p);
				if (!(!strcmp(szPageSize, "LET") || !strcmp(szPageSize, "A4")))
					return Error("Invalid page size specified '%s'", szPageSize);
				break;
				
			case 'c':
			  nPitch = atoi(optarg);
				if (nPitch == 0)
					return Error("Invalid character size (pitch) specified '%s'", optarg);
				break;
				
			case ':':
				Usage();
				return 1;

			case '?':
				Usage();
				return 1;
		}
	}
	
  if ((argc - optind) != 1)
  {
    Usage();
    return 1;
  }
	
#ifdef __WATCOMC__
	if (_fullpath(szInputFileSpec, argv[optind], _MAX_PATH) == NULL)
		return Error("Invalid input file specified '%s'", argv[optind]);
	
	if (szOutputFileSpec[0] == 0)
	{
		_splitpath(szInputFileSpec, szDrive, szDir, szFileName, NULL);
		_makepath(szOutputFileSpec, szDrive, szDir, szFileName, "ps");
	}

	if (szPrologueFileSpec[0] == 0)
	{
		_splitpath(argv[0], szDrive, szDir, NULL, NULL);
		_makepath(szPrologueFileSpec, szDrive, szDir, "diab2ps", "pro");
	}
#endif

#ifdef __GNUC__
	strcpy(szInputFileSpec, argv[optind]);
	
	if (szOutputFileSpec[0] == 0)
	{
		strcpy(szOutputFileSpec, szInputFileSpec);
		char * p = strrchr(szOutputFileSpec, '/');
		if (p == NULL)
			p = szOutputFileSpec;
		p = strrchr(p, '.');
		if (p == NULL)
			strcat(szOutputFileSpec, ".ps");
		else
			strcpy(p, ".ps");
	}
	
	if (szPrologueFileSpec[0] == 0)
		strcpy(szPrologueFileSpec, "/usr/share/diab2ps/diab2ps.pro");
#endif
	
  //printf("Prologue File Spec: %s\n", szPrologueFileSpec);
  //printf("Input File Spec: %s\n", szInputFileSpec);
  //printf("Output File Spec: %s\n", szOutputFileSpec);
	
	if (access(szInputFileSpec, F_OK) != 0)
		return Error("Input file does not exist or is not readable '%s'", szInputFileSpec);
	
  if ((fOut = fopen(szOutputFileSpec, "wb")) == NULL)
    return Error("Failed to open output file '%s'", szOutputFileSpec);
  
  if ((fIn = fopen(szPrologueFileSpec, "rb")) == NULL)
    return Error("Failed to open prologue file '%s'", szPrologueFileSpec);
	
	while ((c = fgetc(fIn)) != EOF)
		if (fputc(c, fOut) == EOF)
      return Error("Failed to copy prologue file '%s'", szPrologueFileSpec);
		
	fclose(fIn);

  if ((fIn = fopen(szInputFileSpec, "rb")) == NULL)
    return Error("Failed to open input file '%s'", szInputFileSpec);

  //freopen_s(NULL, "rb", stdin);
  //freopen_s(NULL, "wb", stdout);

#if __WATCOMC__
	_splitpath(szInputFileSpec, NULL, NULL, szFileName, szExt);
	printf("\n%s%s ==> ", szFileName, szExt);
	_splitpath(szOutputFileSpec, NULL, NULL, szFileName, szExt);
	printf("%s%s\n\n", szFileName, szExt);
#else
	printf("\n%s ==> %s\n\n", szInputFileSpec, szOutputFileSpec);
#endif
  
  nLineBuf = nLineIdx = 0;
	nState = ST_IDLE;
	bLeading = true;
  
	fprintf(fOut, "(%s) %s %i Initialize\r\n", szPageSize, bLandscape ? "true" : "false", nPitch);
  
	while ((c = NextChr()) != EOF)
		Process(c);
	
	if (!bSupFinalFF)
		fprintf(fOut, "\r\nEndPage");
  
	fprintf(fOut, "\r\n\r\n");
	
	fclose(fIn);
	fclose(fOut);

  return 0;
}