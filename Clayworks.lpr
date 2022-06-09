program Clayworks;
{$mode ObjFPC}{$H+}
{$i clay.inc}

uses
   {FPC}
   {$IFDEF UNIX}cthreads, {$ENDIF}
   Classes,

   {Clayworks}
   ClayTypes, ClayGlobal, ClayLog, ClayWindow, ClayWindowWin32;

var
   Window : TClayWindowWin32;

begin
   {Create Window}
   Window := TClayWindowWin32.Create('Clayworks', 1024, 768);

   LogError('Main','Not enough cheese');
   LogStatus('Main','Yes There is - I made it');

   while (not ClayFlagQuit) do
   begin
      {Poll the Window for OS Messages}
      Window.PumpOSMessages();


   end;


   {Free Objects Memory}
   Window.Free;
end.

