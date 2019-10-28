-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
=-=-=-=-Bill Prisoner Disassembler Engine-=-=-=-=
-=-=-=-=-=-=-=-=-=-Version 1.1-=-=-=-=-=-=-=-=-=-
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
	(C)oded by 
		Bill Prisoner / TPOC
		     2006
/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

		      Что это?
		      ========
BPDE  - универсальный дизассемблерный движок. Соз-
дан для применения в вирусах, троянах и прочей жи-
вности.  Пока движок представлен в виде DLL. Но в-
скоре  планируется сделать obj файлы для вставки в
проекты Delphi или в C++, а также поддержка базо-
вой независимости.
		      Интерфейс
		      =========
BPDEDLL.DLL - библиотека динамической компоновки.
Библиотека экспортирует единственную функцию -
Disasm. Вот ее прототип:
int Disasm(char* Offset,Code* Instruction)
Offset - указатель на бинарные данные
Instruction - указатель на структуру Code.
Она определена так:
C:
---
typedef struct _Code
{
	short FieldPresents;//For checking presenting fields
	short Prefix;//Prefixes of instruction
	char OpCodeExt;//if first BYTE1 of opcode is 0FH
	short OpCodeWord;//if OpCode is WORD1 - in escape opcodes (or floating point opcodes)
	unsigned char OpCode;//OpCode BYTE
	unsigned char OpCode2;//If OpCode is Two Byte then this field is second byte in the opcode
	unsigned char ModRM;//ModRM BYTE 
	char SIB;//SIB BYTE
	unsigned long Immediate;//Immediate operand 
	unsigned long ImmediateEx;//Second Immediate operand (only ENTER instruction)
	unsigned long Displacement;//Displacement for operand
	unsigned short Segment;//Segment for Pointer operands
	unsigned long RelOffset;//Relative Offset in jump instructions
	long Offset;//Offset for Pointer Operands 
	char InstructionString[50];//Disassembled String
	char Group;//NumberOfGroup(1-16)
	long Flags;//Opcode Flags
	long Length;//Length of instruction
} Code;

Delphi:
-------
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
	a:longint;//Т.к. DLL была написана на Си
	//то компилятор выровнил поля структуры, 
	//а это заполнитель для доступа к последую-
	//щим полям
	InstructionString:array[0..49]of char;
	Group:byte;
	Flags:longint;
	Length:longint;
end;

Для начала из этой структуры нужно только OpCode,
InstructionString и Length.
!!! Перед вызовом функции Disasm обнулите поля ст-
руктуры Code.!!!

 	               Слова
		       =====
Темными ночами зимы 2006 года я хмуро сидел над д-
изасмом и набивал тысячи строк кода, а после пыта-
лся  с особенной настойчивостью их скомпелировать. 
Глаза  за  это время ослепли окончательно и больше 
ничего не оставалось как компелировать вслепую. К-
лавиатура после этого совсем перестала сопротивля-
ться  и кнопки нажимались как будто сами и как  по
маслу. В это время сидя за кампиком 100% бодровст-
вования мою душу оберегала идея власти кампиков н-
ад миром. Именно это давало силы всегда и будет д-
авать дальше и всю оставшуются жизнь. Очень согре-
вает постоянное присутствие в сети, а когда ее нет
становиться одиноко, но это все равно не может по-
мешать кодировать дальше. Настоящая вирусописател-
ьская музыка всегда давалу руку в будующее и не п-
окидала никогда даже в самые трудные минуты. Коне-
чно хочу сказать спасибо уникальной музыкальной г-
руппе - Mettalica. Это группа наряду с Apocalipti-
ca является истинно вирусописательской, потому что
вирописательстно трудный путь, но непременно веду-
щий к победе разума над эмоциями и гениальности н-
ад безумием. Ты юный вирмейкер должен все это зна-
ть, чтобы быть выше этой огромной, никчемной, глу-
пой толпы скотов, живущих ради набивки живота пиш-
ей  и отьеба. Чтож возможно это будет очень трудно.
Но делать тебе ничего самому не придеться. Идея с-
ама придет к тебе, постучит тебя по куполу и спро-
сит:  "Хочешь ли ты быть выше?". Своими действиями
ты ответишь ей. Но отвечая выжми из себя все само-
е лучшее, если надо будь на волоске от смерти,  но
это того стоит, а иначе зачем вообще жить? Проебы-
вать свою жалкую жизненку? Дышать чужим кислородом?
Кислородом людей, которые  достойны  по-настоящему 
жить? Нет, не сделая чего-то, то обречешь себя  на
вечной пребывание в этом гнилом, высыхшем, вонючим
и мерзким мирке скотов, падали и пустоты. Будь вы-
ше, пиши вирусы. Это не просто кодинг - это искус-
ство,  искусство человеческого разума,  творчество
доступное единицами смельчаков, бросихших вызов о-
диночеству во имя борьбы за возвышения над стадом
фальшивый тел. Но если ты просто обыватель, то зн-
ай НАШ КОД СКОРО ПОСЕТИТ ТВОЙ РАЗУМ...
				3:14 по Москве

		      Контакт
		      =======
Для всех вопросов по движку, идеологии и компелир-
ования.
WWW: http://tpoc.h16.ru
E-MAIL: bill_tpoc@mail.ru
ICQ: 271792804