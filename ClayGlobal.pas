unit ClayGlobal;
{$mode ObjFPC}{$H+}
{$i clay.inc}

interface

uses
   ClayTypes;

var
   {Global Flags}
   ClayFlagQuit : boolean = false;

   PumpOSMessages : procedure() of object;

   {Host Input Note: depreciated}
   HostMessageLock : Boolean;
   HostMouseX,HostMouseY,HostMouseZ : SInt32;
   HostKey : UInt16;
   HostScan : UInt16;

   {Clayworks Keyboard input (DOS Scancode style)}
   scancode:byte;
   charcode:char;

   {Global Paths}
   font_path,clay_dir:string[80];

const
   backspacekey=chr(8);
   escapekey=chr(27);
   returnkey=chr(13);
   tabkey=#9;
   whitespace=' ';
   rightkey=77;
   rightkeyctrl=116;
   leftkey=75;
   leftkeyctrl=115;
   upkey=72;
   downkey=80;
   pageupkey=73;
   pagedownkey=81;
   homekey=71;
   delkey=83;
   endkey=79;

   smLEFTSHIFT=2;
   smLEFTCTRL=4;
   smLEFTALT=8;
   smRIGHTSHIFT=1;
   smSCANMASK=15;

implementation

end.

