program Clayworks;
{$mode ObjFPC}{$H+}
{$i clay.inc}

uses
   {FPC}
   {$IFDEF UNIX}cthreads, {$ENDIF}
   SysUtils, Classes, Keyboard,

   {Clayworks}
   ClayTypes, ClayGlobal, ClayLog,
   ClayWindow,
   {$IFDEF Windows} ClayWindowWin32, ClayRenderGDI, {$ENDIF}

   {Original Units}
   tmaths, diskop, Sincos, colour, stdpal, pcx256,
   msmouse, chardef, strings, vectfont,
   gbasics, ggraph, Basic3d,
   tdb, tdeditb,
   views, twinb, tdwin, twindraw,
   gadgets, cdialog, tmenust,

   {Note : temporary, old main program unit but lots of stuff in it}
   claycp;

type
   { TClayworks }

   TClayworks = class
      Window : TClayWindow;
      constructor Create(); virtual;
      destructor Destroy(); override;
      procedure Execute(); virtual;
   private
      FProgramPath : String;
      FTimeStamp : UInt32;
      FFPS : Float32;
      procedure DoKeyDown(AWindow : TClayWindow; AKey : UInt32);
      procedure DoKeyUp(AWindow : TClayWindow; AKey : UInt32);
      procedure DoMouseDown(AWindow : TClayWindow; X, Y : SInt32; AButton : SInt32);
      procedure DoMouseMove(AWindow : TClayWindow; X, Y : SInt32);
      procedure DoMouseUp(AWindow : TClayWindow; X, Y : SInt32; AButton : SInt32);
      procedure DoWindowClear(AWindow : TClayWindow);
   end;

var
   Clay : TClayworks;

{ TClayworks }

constructor TClayworks.Create();

begin
   LogStatus('TClayWorks','V'+VersionStr+', (C) T.lewis 1995.$'+chr(13));


   {Create Window}
   LogStatus('ClayWorks','Creating Window');

   {$IFDEF Windows}
   Window := TClayWindowWin32.Create('Clayworks', 1024, 768);
   {Assign Input Callbacks}
   Window.OnMouseMove := @DoMouseMove;
   Window.OnMouseDown := @DoMouseDown;
   Window.OnMouseUp := @DoMouseUp;
   Window.OnKeyDown  := @DoKeyDown;
   Window.OnKeyUp  := @DoKeyUp;

   {Assign General Callbacks}
   Window.OnClear  := @DoWindowClear;

   {Temp hack - store pointer to canvas in old style ClayRenderGDI.pas driver}
   ClayRenderGDI.GDICanvas := TClayCanvasGDI(Window.Canvas);

   {Set Global Message Pump Pointer}
   PumpOSMessages := @TClayWindowWin32(Window).PumpOSMessages;
   {$ENDIF}

   Window.SetCaption('Clayworks '+VersionStr);

   {Reset Host Input Vars}
   HostMessageLock := false;
   HostMouseX := 0;
   HostMouseY := 0;
   HostMouseZ := 0;

   {Reset msmouse.pas vars}
   xm := 0;
   ym := 0;
   zm := 0;
   ms_pressed := false;
   ms_released := false;
   ms_doubleclicked := false;
   ms_gridmoved := false;

   {Setup Perspective Var}
   PerspectiveINT:=PersStart;

   {Extract Program Path}
   FProgramPath := IncludeTrailingBackslash(ExtractFileDir(ParamStr(0)));
   {Setup Global Clayworks Paths}
   clay_dir := FProgramPath;
   font_path := clay_dir+'SANSF16B.FNT';

   {Initialize Graphics}
   LogStatus('ClayWorks','Initializing Graphics');
   initgraph(VideoModeGDI1024x768);
   //AResult := initgraph(VideoModeLazarusFullscreen);

   {Setup Palette}
   usecolours(0,16);

   {Load Font List}
   if fexist(font_path) then loadfontlist(font_path, tcharset) else
   begin
      {TODO : Debug log}
      //writeln('Font file not found, please place the file sans16b.fnt in the program file''s directory');
      exit;
   end;

   {Setup Character Set}
   //move(altletters,mem[seg(tcharset):ofs(tcharset)+(254 shl 4)],32);
   //move(ctrlletters,mem[seg(tcharset):ofs(tcharset)+(252 shl 4)],32);

   {Manual Width Tweaks?}
   tcharset.widths[252]:=8;
   tcharset.widths[253]:=9;
   tcharset.widths[254]:=8;
   tcharset.widths[255]:=9;
   tcharset.attrib:=1;

   {Load Font}
   LogStatus('ClayWorks','Loading Font');
   if fexist(clay_dir+defaultfont) then
      Vfontin := vfont.load(defaultfont)
   else
      Vfontin := false;

   {Load Info}
   if fexist(clay_dir+'info.txt') then
      textfile := loadtext(clay_dir+'info.txt');
   getmem(objectinfo,512);
   gridpic:=nil;

   {Setup Draw Object}
   new(draw_object,create(4900,4200,4200));

   {Initialize Graphics Mode}
   setpal(@pal16,0,16);

   {Initialize 3D renderer}
   init3d();

   {Setup Light}
   lightvec.setlight(0,0,0,500,50,10,SC.ColourDepth shr 1);

   {Create Clayworks Application}
   LogStatus('ClayWorks','Creating Application');
   with SC.screenport do
      application := new(claycp.clay,create(0,0,x2,y2));

   InitTWIN(application);

   {Load warning PCX}
  if fexist(clay_dir+'warn.pcx') then
    warningsign:=readpcx(clay_dir+'warn.pcx',PAcolcheck)
  else
    warningsign:=nil;

  mouseon;
  root:=application;
  {
  {Show register file}
  registerfile:=loadtext(clay_dir+'register.txt');
  desktop^.add_and_draw(new(infodialog,create(90,80,590,470,registerfile,true)),VPcentre,PIDundefined);
  }

  {Set Root UI Object to initially be the application}
  Views.root := application;

  application^.draw;
end;

destructor TClayworks.Destroy();
begin
   LogStatus('ClayWorks','Shutting Down');

   {Clayworks Flag}
   fin := True;


   mouseoff;
   if vfontin then
   vfont.done;

   {Destroy Draw}
   dispose(draw_object, done);
   close3d;

   {Destroy ClayWorks Application}
   dispose(application, done);

   freemem(textfile,strlen(textfile));
   {freemem(objectinfo,512);}

   {if warningsign<>nil then
   destroybitmap(warningsign);}

   closegraph;

   {Free Objects Memory}
   Window.Free;

   LogStatus('TClayWorks','That was ClayWorks V'+VersionStr+', (C) T.lewis 1995.$'+chr(13));
end;

procedure TClayworks.Execute();
begin
   while (not ClayFlagQuit) do
   begin
      {Poll the Window for OS Messages}
      Window.PumpOSMessages();

      {Poll Clayworks Mouse Events}
      MPos();
      EventMPos();
      {Poll Clayworks Keyboard Events}
      eventkey();
      {Force Render of UI Application}
      //application^.draw();
   end;
end;

procedure TClayworks.DoMouseMove(AWindow : TClayWindow; X, Y : SInt32);
begin
   HostMouseX := X;
   HostMouseY := Y;
end;

procedure TClayworks.DoMouseDown(AWindow : TClayWindow; X, Y : SInt32; AButton : SInt32);
begin
   HostMouseX := X;
   HostMouseY := Y;
   HostMouseZ := 1;
end;

procedure TClayworks.DoMouseUp(AWindow : TClayWindow; X, Y : SInt32; AButton : SInt32);
begin
   HostMouseX := X;
   HostMouseY := Y;
   HostMouseZ := 0;
end;

procedure TClayworks.DoWindowClear(AWindow : TClayWindow);
begin
   {Just re-render the entire application for now}
   application^.draw();
end;

procedure TClayworks.DoKeyDown(AWindow : TClayWindow; AKey : UInt32);
begin
   HostKey := AKey;
   //HostScan := AScan;
end;

procedure TClayworks.DoKeyUp(AWindow : TClayWindow; AKey : UInt32);
begin

end;

{Include Resource Files - IE Application Icon}
{$R *.res}

begin
   Clay := TClayworks.Create();
   Clay.Execute();
   Clay.Free();
end.
