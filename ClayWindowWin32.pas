unit ClayWindowWin32;
{$mode ObjFPC}{$H+}
{$i clay.inc}

interface

uses
   {Win32}
   Windows,
   {FPC}
   Classes, SysUtils,
   {Clayworks}
   ClayTypes, ClayGlobal, ClayLog, ClayWindow;

type

   { TClayWindowWin32 }

   TClayWindowWin32 = class(TClayWindow)
   protected
      {Process}
      FWin32Instance : HINST;
      {Window Object}
      FWin32Class : TWNDCLASSA;
      FWin32ClassName : String;
      FWin32Handle : HWND;
      FWin32HDC : HDC;
      {Window Properties}
      FWin32Style : DWORD;
      FWin32ExStyle : LONG_PTR;
      {Cursor}
      FWin32CursorCurrent : HCURSOR;

      {Win32 Message Handler}
      function ProcessOSMessage(Win32Msg:UINT; Win32wParam:wPARAM; Win32lParam:LPARAM):LRESULT;
   public
      constructor Create(ACaption : String; AWidth, AHeight : SInt32); override;
      destructor Destroy(); override;
      procedure PumpOSMessages(); override;
      procedure SetCaption(ACaption : string); override;
   end;

   { TClayCanvasGDI }

   TClayCanvasGDI = class(TClayCanvas)
   private
      FWin32HDC : HDC; {Target HDC}
      FGDIPen : HPEN;
      FGDIPenOld : HPEN;
      FGDIBrush : HBRUSH;
      FGDIBrushOld : HBRUSH;
      FGDIBrushTransparent : HBRUSH;
      FGDIPenTransparent : HPEN;

      {Clipping Region}
      FGDIRegion : HRGN;
      FGDIRegionOld : HRGN;
   public
      constructor Create(AWindow : TClayWindow); override;
      destructor Destroy(); override;

      procedure Line(X0,Y0,X1,Y1 : SInt32); override;
      procedure Rect(X0,Y0,X1,Y1 : SInt32); override;
      procedure Pixel(X0,Y0 : SInt32; AColour : TColour3); override;

      procedure SetColour(ARed, AGreen, ABlue : UInt8); override;
      procedure SetClipRect(X0,Y0,X1,Y1 : SInt32); override;
   end;


implementation

const
   DC_BRUSH = 18; {Win32 DC_BRUSH}
   DC_PEN = 19; {Win32 DC_PEN}
   GRADIENT_FILL_RECT_H = $00;
   GRADIENT_FILL_RECT_V = $01;
   GRADIENT_FILL_TRIANGLE = $02;
   GRADIENT_FILL_OP_FLAG = $ff;

{Flat Win32 Callback - Win32 Message Pipe}
function Win32MsgProc(hWnd:HWND; Msg:UINT; wParam:wPARAM; lParam:LPARAM) : LRESULT; stdcall;
var
   Win32Window : TClayWindowWin32;
begin
   Win32Window := nil;
   if(Msg = WM_NCCREATE) then
   begin
      {Get JWindow from lparam of WM_NCCREATE msg}
      Win32Window := TClayWindowWin32(LPCREATESTRUCT(lparam)^.lpCreateParams);
      {Store JWindow reference in UserData of Win32 Window Class}
      SetWindowLong(hWnd, GWL_USERDATA, ptrint(Win32Window));
      Win32Window.FWin32Handle := hwnd;
   end;
   {Try to get Window Instance}
   Win32Window := TClayWindowWin32(PtrInt(GetWindowLong(hWnd, GWL_USERDATA)));
   if Win32Window<>nil then
   begin {Message is for TJWindowWin32 instance}
      Result := Win32Window.ProcessOSMessage(Msg, wParam, lParam);
   end else
   begin {Not Our Message. Pass it to windows}
      Result := DefWindowProc(hWnd,Msg,wParam,lParam);
   end;
end;

{ TClayWindowWin32 }

constructor TClayWindowWin32.Create(ACaption : String; AWidth, AHeight : SInt32);
var
   APosX,APosY :SInt32;
   RectAdjusted : TRect;
begin
   inherited Create(ACaption, AWidth, AHeight);

   {Clear win32 record structures}
   ZeroMemory(@FWin32Class,SizeOf(FWin32Class));

   {Get the Win32 Instance Handle}
   FWin32Instance := HINSTANCE;

   {Setup Win32 Window Class}

   {note : for multiple windows you need to reuse the class or use a different ClassName for each}
   FWin32ClassName := 'ClayWindow';
   FWin32Class.lpszClassName := pchar(FWin32ClassName);
   FWin32Class.hInstance := FWin32Instance;
   FWin32Class.lpfnWndProc := @Win32MsgProc; {Set the Win32 Message Router}

   {Here we Control the Buffering, surface caching for win32 redrawing etc}
   FWin32Class.Style := 0;
   //FWin32Class.Style := CS_OWNDC;
   //FWin32Class.Style := (CS_HREDRAW or CS_VREDRAW or CS_OWNDC);

   FWin32Class.hbrBackground := GetStockObject(BLACK_BRUSH); {Clear to Black}
   FWin32Class.hCursor := LoadCursor(0,MakeIntResource(IDC_ARROW)); {Set Cursor}
   FWin32Class.hIcon := LoadIcon(FWin32Instance,MakeIntResource('MAINICON')); {Set Window Icon from Resource Icon}
   FWin32Class.cbWndExtra := 128;

   {Register Class}
   If (RegisterClassA(FWin32Class)=0) Then
   Begin
      {Note : RegisterClassW = Unicode}
      LogError('TClayWindowWin32.Create','RegisterClass() Failed');
      Exit();
   End;

   {Window Titlebar Style, Border Style etc}
   FWin32Style := (
            WS_CAPTION or WS_SYSMENU or
            WS_MINIMIZEBOX {or WS_MAXIMIZEBOX or WS_SIZEBOX});
   FWin32ExStyle := WS_EX_APPWINDOW;

   {Calculate Screen Centre Window Offset}
   APosX := (GetSystemMetrics(SM_CXSCREEN)-FWidth) Div 2;
   APosY := (GetSystemMetrics(SM_CYSCREEN)-FHeight) Div 2;

   {Calculate Window Size}
   RectAdjusted := Rect(APosX,APosY,APosX+FWidth-1,APosY+FHeight-1);
   {Calculate windowrect plus Windows borders+decorations}
   AdjustWindowRectEx(@RectAdjusted,FWin32Style,false,FWin32ExStyle);

   APosX := RectAdjusted.Left;
   APosY := RectAdjusted.Top;
   AWidth := (RectAdjusted.Right-RectAdjusted.Left)+1;
   AHeight := (RectAdjusted.Bottom-RectAdjusted.Top)+1;

   {Load Cursor Reference for native cursor mode}
   FWin32CursorCurrent := LoadCursor(0, IDC_ARROW);

   {Create Win32 Window}
   FWin32Handle := CreateWindowEx(
      FWin32ExStyle,
      PChar(FWin32ClassName), PChar(FCaption),
      FWin32Style,
      APosX, APosY, AWidth, AHeight,
      0, 0, FWin32Instance, self); {pass pointer to this object for WM_NCCREATE Msg}

   if FWin32Handle=0 then
   begin
      LogError('TClayWindowWin32.Create','CreateWindowEx() Failed');
      exit();
   end;

   {Get HDC for Window Handle}
   FWin32HDC := GetDC(FWin32Handle);

   If FWin32HDC=0 Then
   begin
      LogError('TClayWindowWin32.Create','GetDC() Failed');
      exit();
   end;

   {Sync Desired Window Properties with Actual Window Properties}
   FWin32Style := GetWindowLongPtr(FWin32Handle, GWL_STYLE);
   FWin32ExStyle := GetWindowLongPtr(FWin32Handle, GWL_EXSTYLE);

   {Setup Window State}
   SetForegroundWindow(FWin32Handle);
   SetFocus(FWin32Handle);
   UpdateWindow(FWin32Handle);

   {Create Window Surface Canvas}
   Canvas := TClayCanvasGDI.Create(Self);

   {Show the Window}
   ShowWindow(FWin32Handle, SW_SHOW);
end;

destructor TClayWindowWin32.Destroy();
begin
   {Release Mouse Capture (if enabled)}
   ReleaseCapture;

   {Release the HDC (we shouldn't really hold onto DCs but its only one)}
   ReleaseDC(FWin32Handle, FWin32HDC);

   {Destroy the Window}
   if not DestroyWindow(FWin32Handle) then
      LogError('TClayWindowWin32.Destroy','DestroyWindow() Failed');

   {Unregister the Window Class}
   if not Windows.UnregisterClass(pchar(FWin32ClassName), FWin32Instance) then
      LogError('TClayWindowWin32.Destroy','UnregisterClass() Failed');

   inherited Destroy();
end;

procedure TClayWindowWin32.PumpOSMessages();
var
   AMessage : TMSG;
begin
   {Process All Messages}
   while (PeekMessage(AMessage,0,0,0,PM_REMOVE)) do
   begin
      {Translate and dispatch message to the window messsage handler}
      TranslateMessage(AMessage);
      DispatchMessage(AMessage);
   end;
end;

procedure TClayWindowWin32.SetCaption(ACaption : string);
begin
   inherited SetCaption(ACaption);
   SetWindowText(FWin32Handle,pchar(FCaption));
end;

function TClayWindowWin32.ProcessOSMessage(Win32Msg : UINT; Win32wParam : wPARAM; Win32lParam : LPARAM) : LRESULT;
begin
   Result := -1;

   Case (Win32Msg) Of
      WM_CLOSE :
      begin
         //PostQuitMessage(0);
         ClayFlagQuit := true;
         Result := 0;
      end;
      WM_KILLFOCUS:
      begin
         {App Lost Focus, Release Keys etc}
      end;
      WM_ERASEBKGND : {The Windows background needs to be redrawn}
      begin
         if OnClear<>nil then
         begin {Callback for OS Window Clear Notification}
            OnClear(Self);
            Result := 1; {Tell Windows that we've handled it ourselves}
         end;
      end;
      WM_SIZING : {The Window is Currently Resizing}
      begin

      end;
      WM_SIZE : {The window has been Resized}
      begin

      end;
      WM_NCHITTEST : {For OS control of Custom UI borders}
      begin

      end;
      WM_MOUSEMOVE:
      begin
         if OnMouseMove<>nil then
            OnMouseMove(Self, P32(@Win32lparam)^.Low, P32(@Win32lparam)^.High);
      end;
      WM_LBUTTONDOWN :
      begin
         if OnMouseDown<>nil then
            OnMouseDown(Self, P32(@Win32lparam)^.Low, P32(@Win32lparam)^.High, 0);
      end;
      WM_MBUTTONDOWN :
      begin

      end;
      WM_RBUTTONDOWN :
      begin

      end;
      WM_LBUTTONUP :
      begin
         if OnMouseUp<>nil then
            OnMouseUp(Self, P32(@Win32lparam)^.Low, P32(@Win32lparam)^.High, 0);
      end;
      WM_MBUTTONUP :
      begin

      end;
      WM_RBUTTONUP :
      begin

      end;
      WM_MOUSEWHEEL:
      begin

      end;
   end;

   if (Result = -1) then {Message not handled. Pass to Windows}
      Result := DefWindowProc(FWin32Handle,Win32Msg,Win32wParam,Win32lParam);
end;

{ TClayCanvasGDI }

constructor TClayCanvasGDI.Create(AWindow : TClayWindow);
begin
   inherited Create(AWindow);
   FGDIRegion := 0;
   {Store Window HDC}
   FWin32HDC := TClayWindowWin32(FWindow).FWin32HDC;
   {Setup Pens and Brushes}
   FGDIPen := GetStockObject(DC_PEN);
   FGDIPenOld := SelectObject(FWin32HDC, FGDIPen);
   FGDIBrush := GetStockObject(DC_BRUSH);
   FGDIBrushOld := SelectObject(FWin32HDC, FGDIBrush);
   FGDIBrushTransparent := GetStockObject(HOLLOW_BRUSH);
   FGDIPenTransparent := GetStockObject(NULL_PEN);
end;

destructor TClayCanvasGDI.Destroy();
begin
   inherited Destroy();
end;

procedure TClayCanvasGDI.Line(X0, Y0, X1, Y1 : SInt32);
begin
   MoveToEx(FWin32HDC, X0, Y0, nil);
   LineTo(FWin32HDC, X1,  Y1);
end;

procedure TClayCanvasGDI.Rect(X0, Y0, X1, Y1 : SInt32);
var
   ARect : TRect;
begin
   ARect.Left := X0;
   ARect.Top := Y0;
   ARect.Right := X1;
   ARect.Bottom := Y1;
   FillRect(FWin32HDC, ARect, FGDIBrush);
end;

procedure TClayCanvasGDI.Pixel(X0, Y0 : SInt32; AColour : TColour3);
begin
   SetPixel(FWin32HDC, X0, Y0, RGB(AColour.R, AColour.G, AColour.B));
end;

procedure TClayCanvasGDI.SetColour(ARed, AGreen, ABlue : UInt8);
var
   C : COLORREF;
begin
   C := RGB(ARed, AGreen, ABlue);
   {Keep the GDI pen and brush the same for convenience}
   SetDCPenColor(FWin32HDC, C);
   SetDCBrushColor(FWin32HDC, C);
end;

procedure TClayCanvasGDI.SetClipRect(X0, Y0, X1, Y1 : SInt32);
begin
   if (FGDIRegion <> 0) then DeleteObject(FGDIRegion);
   FGDIRegion := CreateRectRgn(X0, Y0, X1, Y1);
   SelectClipRgn(FWin32HDC, FGDIRegion);
end;

end.
