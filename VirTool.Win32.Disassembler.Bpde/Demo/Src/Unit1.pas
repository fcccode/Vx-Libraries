unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls;

type
  TForm1 = class(TForm)
    ListBox1: TListBox;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;
PCode = ^_Code;
_Code = packed record
FieldPresents:word;
Prefix:word;
OpCodeExt:byte;
OpCodeWord:word;
OpCode:byte;
OpCode2:byte;
ModRM:byte;
SIB:byte;
Immediate:longint;
ImmediateEx:longint;
Displacement:longint;
Segment:word;
RelOffset:longint;
a:longint;
InstructionString:array[0..49]of char;
Group:byte;
Flags:longint;
Length:longint;
end;

D = function(x:PChar;y:PCode):integer;stdcall;
var
  Form1: TForm1;
  y:pointer;
  m:integer;
  x:PCode;
  z:D;
implementation

{$R *.DFM}

procedure TForm1.Button1Click(Sender: TObject);
var
   Length:integer;
   OpCode:byte;
   i:integer;
begin
     ListBox1.Clear;
     x:=VirtualAlloc(nil,1000,MEM_COMMIT or MEM_RESERVE,PAGE_READWRITE);
     y:=GetProcAddress(LoadLibrary('kernel32.dll'),'CloseHandle');
     z:=GetProcAddress(LoadLibrary('bpdedll.dll'),'Disasm');
     for i:=1 to 400 do
     begin
          FillMemory(x,1000,0);
          z(y,x);
          ListBox1.Items.Add(x^.InstructionString);
          y:= pointer(integer(y) + x^.Length);
     end;
     VirtualFree(x,1000,MEM_RELEASE);
end;

procedure TForm1.Button2Click(Sender: TObject);
var
   Length:integer;
   OpCode:byte;
   i:integer;
begin
     ListBox1.Clear;
     x:=VirtualAlloc(nil,1000,MEM_COMMIT or MEM_RESERVE,PAGE_READWRITE);
     y:=GetProcAddress(LoadLibrary('kernel32.dll'),'ExitProcess');
     z:=GetProcAddress(LoadLibrary('bpdedll.dll'),'Disasm');
     for i:=1 to 400 do
     begin
          FillMemory(x,1000,0);
          z(y,x);
          ListBox1.Items.Add(x^.InstructionString);
          y:= pointer(integer(y) + x^.Length);
     end;
     VirtualFree(x,1000,MEM_RELEASE);
end;

procedure TForm1.Button3Click(Sender: TObject);
var
   Length:integer;
   OpCode:byte;
   i:integer;
begin
     ListBox1.Clear;
     x:=VirtualAlloc(nil,1000,MEM_COMMIT or MEM_RESERVE,PAGE_READWRITE);
     y:=GetProcAddress(LoadLibrary('ntdll.dll'),'NtClose');
     z:=GetProcAddress(LoadLibrary('bpdedll.dll'),'Disasm');
     for i:=1 to 400 do
     begin
          FillMemory(x,1000,0);
          z(y,x);
          ListBox1.Items.Add(x^.InstructionString);
          y:= pointer(integer(y) + x^.Length);
     end;
     VirtualFree(x,1000,MEM_RELEASE);
end;

end.
