unit ClayTypes;
{$mode ObjFPC}{$H+}
{$i clay.inc}

interface

type
   {unsigned 8-bit integer}
   UInt8 = Byte;
   PUInt8 = ^UInt8;
   PPUInt8 = ^PUInt8;
   {signed 8-bit integer}
   SInt8 = ShortInt;
   PSInt8 = ^SInt8;
   PPSInt8 = ^PSInt8;
   {unsigned 16-bit integer}
   UInt16 = Word;
   PUInt16 = ^UInt16;
   PPUInt16 = ^PUInt16;
   {signed 16-bit integer}
   SInt16 = SmallInt;
   PSInt16 = ^SInt16;
   PPSInt16 = ^PSInt16;
   {unsigned 32-bit integer}
   UInt32 = LongWord;
   PUInt32 = ^UInt32;
   PPUInt32 = ^PUInt32;
   {signed 32-bit integer}
   SInt32 = LongInt;
   PSInt32 = ^SInt32;
   PPSInt32 = ^PSInt32;
   {unsigned 64-bit integer}
   {'UInt64' already defined in FPC}
   PUInt64 = ^UInt64;
   PPUInt64 = ^PUInt64;
   {signed 64-bit integer}
   SInt64 = Int64;
   PSInt64 = ^SInt64;
   PPSInt64 = ^PSInt64;

   {32-bit float}
   Float32 = Single;
   PFloat32 = ^Float32;
   PPFloat32 = ^PFloat32;

   {64-bit float}
   Float64 = Double;
   PFloat64 = ^Float64;
   PPFloat64 = ^PFloat64;

   {16-bit Helper}
   T16 = packed record
      case UInt8 of
      0 : (Bytes : array[0..1] of UInt8);
      1 : (Chars : array[0..1] of Char);
      {$IFDEF ENDIAN_LITTLE}
      2 : (Low, High : UInt8);
      {$ENDIF}
      {$IFDEF ENDIAN_BIG}
      2 : (High, Low : UInt8);
      {$ENDIF}
      4 : (U16 : UInt16);
      5 : (S16 : SInt16);
      6 : (Bits : bitpacked array [0..15] of 0..1);
   end;
   P16 = ^T16;
   PP16 = ^P16;

   {32-bit Helper}
   T32 = packed record
      case UInt8 of
      0 : (Bytes : array[0..3] of UInt8);
      1 : (Chars : array[0..3] of Char);
      {$IFDEF ENDIAN_LITTLE}
      2 : (Low, High : UInt16);
      {$ENDIF}
      {$IFDEF ENDIAN_BIG}
      2 : (High, Low : UInt16);
      {$ENDIF}
      4 : (U32 : UInt32);
      5 : (S32 : SInt32);
      6 : (Bits : bitpacked array [0..31] of 0..1);
   end;
   P32 = ^T32;
   PP32 = ^P32;

   {3 Channel 8-bit}
   TColour3 = packed record
   case UInt8 of
      1:(R,G,B: UInt8);
      2:(RGB : packed array [0..2] of UInt8);
   end;
   PColour3 = ^TColour3;
   PPColour3 = ^PColour3;
   {4 Channel 8-bit}
   TColour4 = packed record
   case UInt8 of
      1:(RGB: TColour3);
      2:(R,G,B,A: UInt8);
      3:(RGBA: packed array [0..3] of UInt8);
      4:(RGBA32 : UInt32);
   end;
   PColour4 = ^TColour4;
   PPColour4 = ^PColour4;





   {-----Old Types------}

const
   stringsize=20;

type
   stdstring=string[stringsize];

   stringptr=^string;
   realptr=^real;
   integerptr=^integer;
   wordptr=^word;
   byteptr=^byte;

   Stringarray=^Stringarray_;
   Stringarray_=array[0..0] of stdstring;

   bytearray=^bytearray_;
   bytearray_=array[0..0] of byte;

   CharArray=^CharArray_;
   CharArray_=array[0..0] of Char;

   ShortArray=^ShortArray_;
   ShortArray_=array[0..0] of ShortInt;

   Integerarray=^Integerarray_;
   Integerarray_=array[0..0] of Integer;

   Wordarray=^Wordarray_;
   Wordarray_=array[0..0] of word;

   Longintarray=^Longintarray_;
   Longintarray_=array[0..0] of longint;

   realarray=^realarray_;
   realarray_=array[0..0] of real;

   PointerArray=^PointerArray_;
   PointerArray_=array[0..0] of pointer;

   BytePointerArray=^BytePointerArray_;
   BytePointerArray_=array[0..0] of BytePtr;

   BElemPtr=^BElem;
   BElem=object
      {Prev,}Next:Pointer;
   end;

   LinkedList=Object
      Count,size:word;
      first,Last:pointer;
      Function Prev(Elem:BElemPtr):Pointer;
      Procedure Add(Elem:BelemPtr);
      Procedure Insert(By,Elem:BElemPtr);
      Procedure Delete(Elem:BelemPtr);
      function GetByIndex(index:word):Pointer;
      Constructor Create(Size_:word);
      Destructor Done;virtual;
   end;
   linkedlistptr=^linkedlist;

   wordlistelem=object(belem)
      data:word;
   end;
   wordlistelemptr=^wordlistelem;

   integerlistelem=object(belem)
      data:integer;
   end;
   integerlistelemptr=^integerlistelem;
   longintlistelem=object(belem)
      data:longint;
   end;
   longintlistelemptr=^longintlistelem;
   reallistelem=object(belem)
      data:real;
   end;
   reallistelemptr=^reallistelem;

Function CountFrom(elem:belemptr;count:word):Pointer;
function createwordelem(data:word):belemptr;
function createintegerelem(data:integer):belemptr;

{Type Constructors}
function Colour3(Red, Green, Blue : UInt8) : TColour3; inline;
function Colour4(Red, Green, Blue, Alpha : UInt8) : TColour4; inline;

implementation

function Colour3(Red, Green, Blue : UInt8) : TColour3; inline;
begin
   Result.R := Red;
   Result.G := Green;
   Result.B := Blue;
end;

function Colour4(Red, Green, Blue, Alpha : UInt8) : TColour4; inline;
begin
   Result.R := Red;
   Result.G := Green;
   Result.B := Blue;
   Result.A := Alpha;
end;


  function createwordelem(data:word):belemptr;
  var new:belemptr;
  begin
    getmem(new,sizeof(wordlistelem));
    wordlistelemptr(new)^.data:=data;
    createwordelem:=new;
  end;

  function createintegerelem(data:integer):belemptr;
  var new:belemptr;
  begin
    getmem(new,sizeof(integerlistelem));
    integerlistelemptr(new)^.data:=data;
    createintegerelem:=new;
  end;

  Function CountFrom(elem:belemptr;count:word):Pointer;
  var lop:word;
  begin
    For lop:=1 to count-1 do
      elem:=elem^.next;
    CountFrom:=elem;
  end;

  Function LinkedList.Prev(Elem:BelemPtr):Pointer;
  var E:BelemPtr;
  begin
    E:=First;
    While (E<>nil)and(E^.next<>Elem) do
      E:=E^.next;
    Prev:=E;
  end;

  Constructor LinkedList.Create(Size_:word);
  begin
    Count:=0;
    first:=nil;
    Last:=nil;
    Size:=Size_;
  end;

  Destructor LinkedList.Done;
  var next:BelemPtr;
  begin
    while first<>nil do begin
      next:=BelemPtr(first)^.next;
      Freemem(BelemPtr(first),size);
      first:=next;
    end;
    Count:=0;
    last:=nil;
    first:=nil;
  end;

  {assumes Elem^.next = nil}
  Procedure LinkedList.add(Elem:BelemPtr);
  begin
    BelemPtr(Elem)^.next:=nil;
    if First=nil then
      First:=Elem
    else
      BelemPtr(last)^.next:=Elem;
    last:=Elem;
    inc(count);
  end;

  Procedure LinkedList.delete(Elem:BelemPtr);
  var Previous:BelemPtr;
  begin
    if Elem=first then begin
      first:=Elem^.next;
      Previous:=nil;
    end else begin
      Previous:=Prev(elem);
      Previous^.next:=Elem^.next;
    end;
    if elem=last then last:=Previous;
    Freemem(Elem,Size);
    dec(count);
  end;

  Procedure LinkedList.insert(by,elem:BelemPtr);
  var previous:BelemPtr;
  begin
    if by=first then begin
      elem^.next:=by;
      first:=elem;
    end else begin
      Previous:=Prev(by);
      Elem^.next:=Previous^.next;
      Previous^.next:=Elem;
    end;
    inc(count);
  end;

  function LinkedList.GetByIndex(index:word):Pointer;
  var lop:word;
      CurrElem:BelemPtr;
  begin
    if index>count then
      GetByIndex:=Last
    else begin
      Currelem:=First;
      for lop:=1 to index do
        Currelem:=Currelem^.next;
      GetByIndex:=CurrElem;
    end;
  end;

end.

