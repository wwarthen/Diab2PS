%!PS

%%%%%%%%%%
%
% Prologue for Diab2PS - Diablo 630 to PostScript Translator
% Copyright (C) 2017, Wayne Warthen, GPLv3
% Version 0.9, February 14, 2017
%
%%%%%%%%%%

%   This program is free software: you can redistribute it and/or modify
%   it under the terms of the GNU General Public License as published by
%   the Free Software Foundation, either version 3 of the License, or
%   (at your option) any later version.
%
%   This program is distributed in the hope that it will be useful,
%   but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%   GNU General Public License for more details.

%   You should have received a copy of the GNU General Public License
%   along with this program.  If not, see <http://www.gnu.org/licenses/>.


/BeginPage
{
  Landscape
  {
    << /PageSize [yPage xPage] /Orientation 3 >> setpagedevice
    90 rotate 0 yPage neg translate
  }
  {
    << /PageSize [xPage yPage] /Orientation 0 >> setpagedevice
  } ifelse

  0 Top VMI sub moveto
  
  % gsave
  % 0 yPage VMI sub moveto (123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890) MyShow
  % 0 yPage 2 div moveto (123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890) MyShow
  % 0 0 moveto (123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890) MyShow
  % grestore

  % gsave
  % Bottom Left exch moveto
  % Top Left exch lineto
  % Top Right exch lineto
  % Bottom Right exch lineto
  % closepath
  % stroke
  % grestore
} def

/EndPage {showpage} def

/BeginLine {currentpoint exch pop Left exch moveto Normal ClrJustify SetForward ClrUnderline ClrSuppress} def
/EndLine {} def

/ResetHMI {/HMI DefHMI def} def
/ResetVMI {/VMI DefVMI def} def

/ClrTop {/Top yPage def} def
/ClrBottom {/Bottom 0 def} def
/ClrLeft {/Left 0 def} def
/ClrRight {/Right xPage def} def

/PriColor {0 0 0 setrgbcolor} def % Black
/SecColor {1 0 0 setrgbcolor} def % Red

% Move cursor horizontally to col
% Stack: col, Returns nothing
% NOTE: First column is 1
/AbsHT {1 sub HMI mul currentpoint exch pop moveto} def

% Move cursor vertically to line
% Stack: line, Returns nothing
% NOTE: First row is 1
/AbsVT {VMI mul yPage exch sub currentpoint pop exch moveto} def

% Backspace cursor by one character position
/Backspace
{
  Backward
  {HMI 0 rmoveto}
  {HMI neg 0 rmoveto} ifelse
} def

% Backspace by 1/120"
/MicroBS {rmove 72 120 neg div 0} def

% Set left margin based on current cursor position
% Stack: nothing, Returns nothing
/SetLeft {currentpoint pop /Left exch def} def

% Set right margin based on current cursor position
% Stack: nothing, Returns nothing
/SetRight {currentpoint pop HMI add /Right exch def} def

% Set top margin based on current cursor position
% Stack: nothing, Returns nothing
/SetTop {currentpoint exch pop VMI add /Top exch def} def

% Set bottom margin based on current cursor position
% Stack: nothing, Returns nothing
/SetBottom {currentpoint exch pop /Bottom exch def} def

/SetVMI {72 mul 48 div /VMI exch def} def
/SetHMI{72 mul 120 div /HMI exch def} def

/SetUnderline {/Underline true def} def
/ClrUnderline {/Underline false def} def

/SetBackward {/Backward true def} def
/SetForward {/Backward false def} def

/SetSuppress {/Suppress true def} def
/ClrSuppress {/Suppress false def} def

% Position cursor to center print a string of len chars centered between margins
% Stack: len, Returns nothing
/Center
{
  Left Right add 2 div currentpoint exch pop moveto % move to center of line
  HMI mul 2 div neg 0 rmoveto  
} def

/Justify % CharCount BlankCount
{
  exch HMI mul Right currentpoint pop sub exch sub exch div /SpacePad exch def
} def

/ClrJustify {/SpacePad 0 def} def

/LineFeed
{
  0 VMI neg rmoveto
  currentpoint exch pop
  Bottom exch gt
  {
    FormFeed
  } if
} def

/RevLineFeed {0 VMI rmoveto} def

/HalfLineFeed
{
  0 VMI 2 div neg rmoveto
  currentpoint exch pop
  Bottom exch gt
  {
    FormFeed
  } if
} def

/RevHalfLineFeed {0 VMI 2 div rmoveto} def

/IsNull {type /nulltype eq} def

/OrdAryIns
{
  exch /Ary exch def
  /n 0 def
  {
    Ary n get /v exch def % v := value at Ary[n]
    dup v IsNull {pop Ary exch n exch put exit} if % append new value and done
    dup v eq {pop exit} if % ignore if tab position already set
    dup v lt % insert and bubble up
    {
      {
        Ary n get exch Ary exch n exch put % swap tos w/ current array pos value
        dup IsNull {exit} if % exit if end of array
        n 1 add /n exch def % next array position
      } loop
      pop exit
    } if
    n 1 add /n exch def % next array position
  } loop
} def

/OrdAryDel
{
  exch /Ary exch def
  /n 0 def
  {
    Ary n get /v exch def % v := value at Ary[n]
    dup v IsNull {pop exit} if % end of array, done
    dup v eq
    {
      {
        Ary n 1 add get
        dup Ary exch n exch put % array[n] : array[n + 1]
	IsNull {exit} if % if array[n + 1] = null then exit
        n 1 add /n exch def % next array position
      } loop
      pop exit
    } if
    n 1 add /n exch def % next array position
  } loop
} def

/OrdAryNxt
{
  exch /Ary exch def
  /n 0 def
  {
    Ary n get /v exch def % v := value at Ary[n]
    v IsNull {pop null exit} if % end of array, return null
    dup v lt {pop v exit} if % if gt cur val, return new value
    n 1 add /n exch def % next array position
  } loop
} def

/OrdAryPrv
{
  exch /Ary exch def
  /n Ary length 1 sub def
  {
    Ary n get /v exch def % v := value at Ary[n]
    v IsNull not {dup v gt {pop v exit} if} if
    n 1 sub /n exch def % prior array position
    n 0 lt {pop null exit} if
  } loop
} def

/HTabInit {/HTabAry 50 array def} def
/HTabSet {HTabAry currentpoint pop OrdAryIns} def
/HTabClr {HTabAry currentpoint pop OrdAryDel} def
/HTab {HTabAry currentpoint pop OrdAryNxt dup IsNull {pop} {currentpoint exch pop moveto} ifelse} def

/VTabInit {/VTabAry 50 array def} def
/VTabSet {VTabAry currentpoint exch pop OrdAryIns} def
/VTabClr {VTabAry currentpoint exch pop OrdAryDel} def
/VTab {VTabAry currentpoint exch pop OrdAryPrv dup IsNull {pop} {currentpoint pop exch moveto} ifelse} def

/FormFeed {currentpoint pop EndPage BeginPage currentpoint exch pop moveto} def

/ShowChar %stack: code
{
  /CharBuf 1 string def
  CharBuf 0 3 -1 roll put
  Suppress
  {( ) show}
  {CharBuf show} ifelse
} def

/MyShow
{
  currentpoint pop exch
  {
    dup
    gsave ShowChar grestore
    Backward
    {
      HMI neg 0 rmoveto
    }
    {
      HMI 0 rmoveto
    } ifelse
    32 eq {
      SpacePad 0 rmoveto
    } if
  } forall
  Underline
  {
    gsave
    currentfont /FontMatrix get 0 get dup % size
    currentfont /FontInfo get /UnderlineThickness get mul setlinewidth
    currentfont /FontInfo get /UnderlinePosition get mul 0 exch rmoveto
    currentpoint exch pop lineto
    stroke
    grestore
  }
  {pop}
  ifelse
} def

/NextLine {BeginLine LineFeed} def

/Reset
{
  ResetHMI
  ResetVMI
  
  ClrTop
  ClrBottom
  ClrLeft
  ClrRight

  ClrJustify
  ClrUnderline
  ClrSuppress
  SetForward
  
  HTabInit
  VTabInit

  Normal

  BeginPage
  BeginLine
} def

/Initialize % PageSize(string) Landscape(bool) Pitch(int) Initialize -
{
  dup /Pitch exch def

  72 exch div /DefHMI exch def
  /DefVMI 12 def
  
  /Landscape exch def

  dup (LET) eq {/xPage 612 def /yPage 792 def} if % Letter
  dup (A4)  eq {/xPage 595 def /yPage 842 def} if % A4
  pop
  
  Landscape {xPage yPage /xPage exch def /yPage exch def} if
  
  /Scale 120 Pitch div def

  /Normal {/Courier findfont Scale scalefont setfont} def
  /Bold {/Courier-Italic findfont Scale scalefont setfont} def
  /Shadow {/Courier-Bold findfont Scale scalefont setfont} def

  % /Normal {/BeraSansMono-Roman findfont Scale scalefont setfont} def
  % /Bold {/BeraSansMono-Italic findfont Scale scalefont setfont} def
  % /Shadow {/BeraSansMono-Bold findfont Scale scalefont setfont} def

  Reset
} def

% (LET) false 12 Initialize
