unit ClayRenderGDI;
{$mode ObjFPC}{$H+}
{$i clay.inc}

interface

uses
   {Windows}
   windows,
   {FPC}
   Types, SysUtils,
   {ClayWorks}
   ClayTypes, ClayGlobal, ClayWindowWin32,
   Gbasics, chardef, tmaths, stdpal, colour;

function set_mode(mode:byte):boolean;
Procedure bar(x1,y1,x2,y2:integer);
Procedure GTriangle(xa,ya,xb,yb,xc,yc,ia,ib,ic:integer);
Procedure Triangle(xa,ya,xb,yb,xc,yc:integer);
Procedure Line(x1,y1,x2,y2:integer);
Procedure Hline(x1,x2,y:Integer);
Procedure Vline(x,y1,y2:Integer);
Procedure putpixel(x1,y1:Integer);
Function  getpixel(x1,y1:Integer):byte;
Procedure outtextxy(x1,y1:integer;txt:string);
Procedure outtextxy_length(x1,y1:integer;txt:pchar;length:byte);
Procedure Drawbytes(x1,Y1:Integer;pic:bytearray;nbytes:byte);
Procedure Drawbytesxy(x1,y1:Integer;pic:bytearray;xbytes,ybytes:byte);
Procedure Screencopy(x1,y1,x2,y2,xd,yd:integer;page1,page2:byte);
procedure putbitmap(x1,y1:integer;bitmap:bitmaptypeptr);
function getbitmap(x1,y1,x2,y2:integer):bitmaptypeptr;
Procedure cleardevice;
procedure savescreenregs;
procedure restorescreenregs;

{quick generic interface to screen -good for file loading}
Procedure QSetXY(x,y:integer);
Procedure Qwrite(count:word);
function  Qget:byte;

var
   GDICanvas : TClayCanvasGDI;

implementation

uses
   Views;

var
   ColourPen : TColour3;
   ColourBrush : TColour3;


procedure SetBrushColour();
begin
   ColourBrush.R := ColourMap^[T_fillcol].R * 4;
   ColourBrush.G := ColourMap^[T_fillcol].G * 4;
   ColourBrush.B := ColourMap^[T_fillcol].B * 4;
   GDICanvas.SetColour(ColourBrush.R,ColourBrush.G,ColourBrush.B);
   //SetDCBrushColor(Fwin32GDIBitmapDC, RGB(ColourBrush.R,ColourBrush.G,ColourBrush.B));
 end;

procedure SetPenColour();
begin
   ColourPen.R := ColourMap^[T_col].R * 4;
   ColourPen.G := ColourMap^[T_col].G * 4;
   ColourPen.B := ColourMap^[T_col].B * 4;
   GDICanvas.SetColour(ColourPen.R,ColourPen.G,ColourPen.B);

   {Set Xor Write Mode}
{   if (t_writemode = xorput) then
      LazCanvas.Pen.Mode := pmXor else
      LazCanvas.Pen.Mode := pmCopy;}
end;

function set_mode(mode : byte) : boolean;
begin
   {case mode of
      100 : begin LazForm.Width := 640; LazForm.Height := 480; end;
      101 : begin LazForm.Width := 800; LazForm.Height := 600; end;
      102 : begin LazForm.Width := 1024; LazForm.Height := 768; end;
      103 : begin {Fullscreen Mode}
         LazForm.WindowState := wsMaximized;
         LazForm.Width := Screen.Width;
         LazForm.Height := Screen.Height;
         LazForm.BorderStyle := bsNone;
         LazImage.Width := Screen.Width;
         LazImage.Height := Screen.Height;
         LazImage.Stretch := false;
      end;
   end;
   {Center Window On Desktop}
   LazForm.Position := poDesktopCenter;

   {Resize Framebuffer Bitmap}
   LazImage.Picture.Bitmap.SetSize(LazForm.Width, LazForm.Height);}
end;

procedure bar(x1, y1, x2, y2 : integer);
var
   OffsetX, OffsetY : Integer;
   ARect : Types.TRect;
begin
   ARect.Left := SC.Viewport.x1;
   ARect.Right := SC.Viewport.x2+1;
   ARect.Top := SC.Viewport.y1;
   ARect.Bottom := SC.Viewport.y2+1;

   GDICanvas.SetClipRect(ARect.Left,ARect.Top,ARect.Right,ARect.Bottom);
   //LazCanvas.ClipRect := ARect;

   SetBrushColour();

   //annoying filled beige rectangle filling the screen
   //while I'm trying to work - skip all beige.
   //if T_fillcol=15  then exit;

   OffsetX := Root^.x1; //SC.ScreenPort.X1 + SC.Viewport.X1 + Root^.x1;
   OffsetY := Root^.y1; //SC.ScreenPort.Y1 + SC.Viewport.Y1 + Root^.y1;

   GDICanvas.Rect(OffsetX + x1,OffsetY + y1, OffsetX + x2 + 1, OffsetY + y2 + 1);
end;

procedure GTriangle(xa, ya, xb, yb, xc, yc, ia, ib, ic : integer);
begin

end;

procedure Triangle(xa, ya, xb, yb, xc, yc : integer);
begin

end;

procedure Line(x1, y1, x2, y2 : integer);
var
   OffsetX, OffsetY : Integer;
   ARect : Types.TRect;
begin
   ARect.Left := SC.Viewport.x1;
   ARect.Right := SC.Viewport.x2+1;
   ARect.Top := SC.Viewport.y1;
   ARect.Bottom := SC.Viewport.y2+1;

   GDICanvas.SetClipRect(ARect.Left,ARect.Top,ARect.Right,ARect.Bottom);
   //LazCanvas.ClipRect := ARect;

   SetPenColour();

   OffsetX := Root^.x1;
   OffsetY := Root^.y1;

   GDICanvas.Line(OffsetX + x1, OffsetY + y1, OffsetX + x2, OffsetY + y2);
end;

procedure Hline(x1, x2, y : Integer);
var
   OffsetX, OffsetY : Integer;
   ARect : Types.TRect;
begin
   ARect.Left := SC.Viewport.x1;
   ARect.Right := SC.Viewport.x2+1;
   ARect.Top := SC.Viewport.y1;
   ARect.Bottom := SC.Viewport.y2+1;


   GDICanvas.SetClipRect(ARect.Left,ARect.Top,ARect.Right,ARect.Bottom);


   SetPenColour();

   OffsetX := Root^.x1;
   OffsetY := Root^.y1;
   //OffsetX := SC.ScreenPort.x1 + SC.Viewport.x1 + Root^.x1;
   //OffsetY := SC.ScreenPort.y1 + SC.Viewport.y1 + Root^.y1;

   {if x1<0 then X1 := LazForm.Width + X1;
   if x2<0 then X2 := LazForm.Width + X2;
   if y<0 then Y := LazForm.Height + Y;}

   GDICanvas.Line(OffsetX + x1, OffsetY + y, OffsetX + x2  + 1, OffsetY + y);
end;

procedure Vline(x, y1, y2 : Integer);
var
   OffsetX, OffsetY : Integer;
   ARect : Types.TRect;
begin
   ARect.Left := SC.Viewport.x1;
   ARect.Right := SC.Viewport.x2+1;
   ARect.Top := SC.Viewport.y1;
   ARect.Bottom := SC.Viewport.y2+1;

   GDICanvas.SetClipRect(ARect.Left,ARect.Top,ARect.Right,ARect.Bottom);

   SetPenColour();

   OffsetX := Root^.x1;
   OffsetY := Root^.y1;

   {if x<0 then X := LazForm.Width + X;
   if y1<0 then Y1 := LazForm.Height + Y1;
   if y2<0 then Y2 := LazForm.Height + Y2;}

   GDICanvas.Line(OffsetX + x, OffsetY + y1, OffsetX + x, OffsetY+ y2 + 1);
end;

procedure putpixel(x1, y1 : Integer);
var
   OffsetX, OffsetY : Integer;
   ARect : Types.TRect;
begin
   ARect.Left := SC.Viewport.x1;
   ARect.Right := SC.Viewport.x2+1;
   ARect.Top := SC.Viewport.y1;
   ARect.Bottom := SC.Viewport.y2+1;

   GDICanvas.SetClipRect(ARect.Left,ARect.Top,ARect.Right,ARect.Bottom);

   SetPenColour();

   OffsetX := Root^.x1;
   OffsetY := Root^.y1;

   GDICanvas.Pixel(OffsetX + x1, OffsetY + y1, ColourPen);
end;

function getpixel(x1, y1 : Integer) : byte;
begin
   Result := 0;
end;

procedure outtextxy(x1, y1 : integer; txt : string);
var
   I : Integer;
   OffsetX, OffsetY : Integer;
   ARect : Types.TRect;
begin

   if tcharset.attrib<>CTproportional then
   for I:=1 to length(txt) do begin
     drawbytes(x1,y1,@tcharset.typeface[byte(txt[I])],fontheight);
     inc(x1,8);
   end else
   for I:=1 to length(txt) do begin
     drawbytes(x1,y1,@tcharset.typeface[byte(txt[I])],fontheight);
     inc(x1,tcharset.widths[byte(txt[I])]);
   end;

   {
   ARect.Left := SC.Viewport.x1;
   ARect.Right := SC.Viewport.x2+1;
   ARect.Top := SC.Viewport.y1;
   ARect.Bottom := SC.Viewport.y2+1;

   LazCanvas.ClipRect := ARect;

   ColourPen := FPColor(
   (pal16[T_col].R * 4) * 255,
   (pal16[T_col].G * 4)* 255,
   (pal16[T_col].B * 4)* 255,
   255 * 255);


   if (txt = 'Front') then
   begin
      OffsetX := 34;
   end;

   LazCanvas.Font.FPColor := ColourPen;

   OffsetX := Root^.x1;
   OffsetY := Root^.y1;

   LazCanvas.Brush.Style := bsClear;
   LazCanvas.TextOut(OffsetX + x1,OffsetY + y1,txt);
   LazCanvas.Brush.Style := bsSolid;}
end;

procedure outtextxy_length(x1, y1 : integer; txt : pchar; length : byte);
var
   I : Integer;
   OffsetX, OffsetY : Integer;
   AString : String;
   ARect : Types.TRect;
begin
   if tcharset.attrib<>CTproportional then
   for I:=1 to length do begin
     drawbytes(x1,y1,@tcharset.typeface[byte(txt^)],fontheight);
     inc(x1,8);
     txt+=1;
   end else
   for I:=1 to length do begin
     drawbytes(x1,y1,@tcharset.typeface[byte(txt^)],fontheight);
     inc(x1,tcharset.widths[byte(txt^)]);
     txt+=1;
   end;
   {
   ARect.Left := SC.Viewport.x1;
   ARect.Right := SC.Viewport.x2+1;
   ARect.Top := SC.Viewport.y1;
   ARect.Bottom := SC.Viewport.y2+1;

   LazCanvas.ClipRect := ARect;

   AString := copy(txt,0,length);

   ColourPen := FPColor(
   (pal16[T_col].R * 4) * 255,
   (pal16[T_col].G * 4)* 255,
   (pal16[T_col].B * 4)* 255,
   255 * 255);

   LazCanvas.Font.FPColor := ColourPen;

   OffsetX := Root^.x1;
   OffsetY := Root^.y1;

   LazCanvas.Brush.Style := bsClear;
   LazCanvas.TextOut(OffsetX + x1,OffsetY + y1,txt);
   LazCanvas.Brush.Style := bsSolid; }
end;

procedure Drawbytes(x1, Y1 : Integer; pic : bytearray; nbytes : byte);
var
   OffsetX, OffsetY : Integer;
   IWidth,IHeight : Integer;
   PX,PY : Integer;
   IX,IY : Integer;
   AByte : Byte;
   ABit : Byte;
   ARect : Types.TRect;
begin
   ARect.Left := SC.Viewport.x1;
   ARect.Right := SC.Viewport.x2+1;
   ARect.Top := SC.Viewport.y1;
   ARect.Bottom := SC.Viewport.y2+1;

   GDICanvas.SetClipRect(ARect.Left,ARect.Top,ARect.Right,ARect.Bottom);

   SetPenColour();

   OffsetX := Root^.x1;
   OffsetY := Root^.y1;

   {Calculate Image Width and Height}
   IWidth := 8;
   IHeight := nbytes;

   for IY := 0 to IHeight-1 do
   begin
      for IX := 0 to IWidth-1 do
      begin
         {Extract Byte}
         AByte := pic^[(IY * 1) + (IX div 8)];
         {Extract Bit}
         ABit := (AByte shr (IX mod 8)) and 1;

         {Invert X Pixel Order}
         PX := (OffsetX + X1 + IWidth) - (IX+1);
         //PX := OffsetX + X1 + IX;
         PY := OffsetY + Y1 + IY;

         {Render Pixel}
         {$IFDEF Enable_BGRABitmap}
         if ABit=1 then LazCanvas.SetPixelColor(PX, PY, FPColorToTColor(ColourPen));
         {$ELSE}
         if ABit=1 then GDICanvas.Pixel(PX, PY, ColourPen);
         {$ENDIF}
      end;
   end;
end;

procedure Drawbytesxy(x1, y1 : Integer; pic : bytearray; xbytes, ybytes : byte);
var
   OffsetX, OffsetY : Integer;
   IWidth,IHeight : Integer;
   PX,PY : Integer;
   IX,IY : Integer;
   I : Integer;
   AByte : Byte;
   ABit : Byte;
   ARect : Types.TRect;
begin
   ARect.Left := SC.Viewport.x1;
   ARect.Right := SC.Viewport.x2+1;
   ARect.Top := SC.Viewport.y1;
   ARect.Bottom := SC.Viewport.y2+1;

   GDICanvas.SetClipRect(ARect.Left,ARect.Top,ARect.Right,ARect.Bottom);

   SetPenColour();

   OffsetX := Root^.x1;
   OffsetY := Root^.y1;

   IWidth := XBytes*8;
   IHeight := YBytes;

   for IY := 0 to IHeight-1 do
   begin
      for IX := 0 to IWidth-1 do
      begin
         {Extract Byte}
         AByte := pic^[(IY * xbytes) + 1-(IX div 8)];
         {Extract Bit}
         ABit := (AByte shr (IX mod 8)) and 1;

         {Invert X Pixel Order}
         PX := (OffsetX + X1 + IWidth) - (IX+1);
         //PX := OffsetX + X1 + IX;
         PY := OffsetY + Y1 + IY;

         {Render Pixel}
         if ABit=1 then GDICanvas.Pixel(PX, PY, ColourPen);
      end;
   end;
end;

procedure Screencopy(x1, y1, x2, y2, xd, yd : integer; page1, page2 : byte);
var
   X : Integer;
begin
   X := 0;
end;

procedure putbitmap(x1, y1 : integer; bitmap : bitmaptypeptr);
var
   X : Integer;
begin
   X := 0;
end;

function getbitmap(x1, y1, x2, y2 : integer) : bitmaptypeptr;
var
   X : Integer;
begin
   X := 0;
end;

procedure cleardevice;
begin
   SetPenColour();
   SetBrushColour();
   Rect(0,0,1024,768);
end;

procedure savescreenregs;
begin

end;

procedure restorescreenregs;
begin

end;

procedure QSetXY(x, y : integer);
begin

end;

procedure Qwrite(count : word);
begin

end;

function Qget : byte;
begin

end;

end.
