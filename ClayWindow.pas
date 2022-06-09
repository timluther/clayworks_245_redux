unit ClayWindow;
{$mode ObjFPC}{$H+}
{$i clay.inc}

interface

uses
   {FPC}
   SysUtils,
   {Clayworks}
   ClayTypes, ClayGlobal;
type
   {Forward Declerations}
   TClayCanvas = class;

   { TClayWindow }

   TClayWindow = class
   protected
      FCaption : String;
      FWidth : SInt32;
      FHeight : SInt32;
   public
      Canvas : TClayCanvas;

      {Input}
      OnMouseMove : procedure(AWindow : TClayWindow; X,Y : SInt32) of object;
      OnMouseDown : procedure(AWindow : TClayWindow; X,Y : SInt32; AButton : SInt32) of object;
      OnMouseUp : procedure(AWindow : TClayWindow; X,Y : SInt32; AButton : SInt32) of object;
      OnKeyDown : procedure(AWindow : TClayWindow; AKey : SInt32) of object;
      OnKeyUp : procedure(AWindow : TClayWindow; AKey : SInt32) of object;

      {General}
      OnClear : procedure(AWindow : TClayWindow) of object;

      constructor Create(ACaption : String; AWidth, AHeight : SInt32); virtual;
      destructor Destroy(); override;

      {Poll the OS for Window Messages}
      procedure PumpOSMessages(); virtual; abstract;

      procedure SetCaption(ACaption : string); virtual;
   end;

   { TClayCanvas }

   TClayCanvas = class
   protected
      FWindow : TClayWindow;
   public
      constructor Create(AWindow : TClayWindow); virtual;
      destructor Destroy(); override;

      procedure Line(X0,Y0,X1,Y1 : SInt32); virtual; abstract;

      procedure Rect(X0,Y0,X1,Y1 : SInt32); virtual; abstract;

      procedure Pixel(X0,Y0 : SInt32; AColour : TColour3); virtual; abstract; overload;
      procedure Pixel(X0,Y0 : SInt32; Red, Green, Blue : UInt8); overload; inline;

      procedure SetColour(ARed, AGreen, ABlue : UInt8); virtual; abstract;
      procedure SetClipRect(X0,Y0,X1,Y1 : SInt32); virtual; abstract;
   end;

implementation

{ TClayWindow }

constructor TClayWindow.Create(ACaption : String; AWidth, AHeight : SInt32);
begin
   FCaption := ACaption;
   FWidth := AWidth;
   FHeight := AHeight;

   Canvas := nil;

   OnMouseMove := nil;
   OnMouseDown := nil;
   OnMouseUp := nil;
   OnKeyDown := nil;
   OnKeyUp := nil;

   OnClear := nil;
end;

destructor TClayWindow.Destroy();
begin
   if Canvas<>nil then FreeAndNil(Canvas);
   inherited Destroy();
end;

procedure TClayWindow.SetCaption(ACaption : string);
begin
   FCaption := ACaption;
end;

{ TClayCanvas }

constructor TClayCanvas.Create(AWindow : TClayWindow);
begin
   FWindow := AWindow;
end;

destructor TClayCanvas.Destroy();
begin
   inherited Destroy();
end;

procedure TClayCanvas.Pixel(X0, Y0 : SInt32; Red, Green, Blue : UInt8); inline;
begin
   Pixel(X0,Y0,Colour3(Red,Green,Blue));
end;

end.
