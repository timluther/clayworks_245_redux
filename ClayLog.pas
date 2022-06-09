unit ClayLog;
{$mode ObjFPC}{$H+}
{$i clay.inc}

interface

uses
   {FPC}
   SysUtils, crt;

procedure LogStatus(ASender : String; AStatus : String);
procedure LogError(ASender : String; AError : String);

implementation

procedure LogStatus(ASender : String; AStatus : String);
begin
   TextColor(LightGreen);
   Write(ASender);
   TextColor(White);
   Write(' - ');
   TextColor(LightGray);
   WriteLn(AStatus);
end;

procedure LogError(ASender : String; AError : String);
begin
   TextColor(LightRed);
   Write(ASender);
   TextColor(White);
   Write(' - ');
   TextColor(LightGray);
   WriteLn(AError);
end;

end.

