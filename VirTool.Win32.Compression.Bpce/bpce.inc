NumberOfLeaf equ 512;Максимальное количество узлов в дереве
;===============================================================================;
;Структура минимальных данных для одного листа дерева Хаффмана
;Этих данных достаточно для декодирования
;===============================================================================;
Leaf struct
	Left dw ?;Предыдущий левый узел
	Right dw ?;Предыдущий правый узел
Leaf ends
;===============================================================================;


;===============================================================================;
;Структура заголовка сжатых данных
;===============================================================================;
Header struct
	Signature dw ?;Сигнатура. Равна "BP"
	HuffmanCodesSize dw ?;Размер таблицы кодов Хаффмана
	CompressSize dd ?;Сжатый размер данных
	DecompressSize dd ?;Распакованный размер
	CRC32 dd ?;Чексумма для (!) Сжатых данных
Header ends
;===============================================================================;

;===============================================================================;
;CRC32
;===============================================================================;
CRC32 proc uses esi edi Offset1:DWORD,Size1:DWORD
	mov esi,Offset1
	mov edi,Size1
	cld
	xor     ecx,ecx
	dec     ecx
	mov     edx,ecx
	push    ebx
NextByteCRC:
	xor     eax,eax
	xor     ebx,ebx
	lodsb
	xor     al,cl
	mov     cl,ch
	mov     ch,dl
	mov     dl,dh
	mov     dh,8
NextBitCRC:
	shr     bx,1
	rcr     ax,1
	jnc     NoCRC
	xor     ax,08320h
	xor     bx,0EDB8h
NoCRC:
        dec     dh
	jnz     NextBitCRC
	xor     ecx,eax
	xor     edx,ebx
        dec     edi
	jnz     NextByteCRC
	pop     ebx
	not     edx
	not     ecx
	mov     eax,edx
	rol     eax,16
	mov     ax,cx
	ret
CRC32 endp
;===============================================================================;


;===============================================================================;
;Структура для дополнительных данных одного листа дерева Хаффмана.
;===============================================================================;
LeafEx struct
	Next dw ?;Следующий узел
	Counter dd ?;Счетчик числа вхождений
LeafEx ends
;===============================================================================;

;===============================================================================;
;Структура для хранения кода Хаффмана
;===============================================================================;
HuffmanCode struct
	Length1 db ?;Длинна кода в битах
	Code db 32 dup (?);Максимальная длина кода Хаффмана - 32 байта	
HuffmanCode ends
;===============================================================================;

;Эти данные будут храниться в рабочем буфере , адрес которого говорит нам юзер. Нам только надо считать значения смещений для Indexes, Tree, LeafEx
;Размер буфера - как минимум 14,25 Кб
TreeOffset equ 0
TreeExOffset equ 2048
IndexesOffset equ 5120
HuffmanCodesOffset equ 6144

;Tree	Leaf	NumberOfLeaf dup <0>;Дерево Хаффмана с минимальными данными для декодировки
;TreeEx	LeafEx	NumberOfLeaf dup <0>;Дерево Хаффмана с дополнительными данными для построения кодов
;Indexes	dw	NumberOfLeaf dup (0);Массив индексов узлов расположенных в порядке возрастания частот
;HuffmanCodes 	HuffmanCode 256 dup <0>

;===============================================================================;
;Заполнение узлов начальными значениями
;===============================================================================;
TreeInit proc uses esi edi WorkingBuffer:DWORD
	push edi

	mov ecx,NumberOfLeaf;Счетчик слов для заполнения
;Заполнение Tree начальными значениями. Заполняем все -1
	mov edi,WorkingBuffer;В edi - адрес Tree
	mov eax,-1; -1 означает, что ссылка на правый или левый отсутствует
	rep stosd
;Заполнение LeafEx.Counter начальными значениями.
	mov ecx,NumberOfLeaf
	mov edi,WorkingBuffer
	lea edi,[edi+TreeExOffset]
	assume edi:ptr LeafEx
NextCounter:
	mov [edi].Counter,0
	add edi,6
	loop NextCounter
;Заполнение LeafEx.Next начальными значениями.
	mov ecx,NumberOfLeaf
	mov edi,WorkingBuffer
	lea edi,[edi+TreeExOffset]
NextNext: 
	mov [edi].Next,-1
	add edi,sizeof LeafEx
	loop NextNext
	pop edi
	ret 
TreeInit endp
;===============================================================================;


;===============================================================================;
;Массив индексов заполняется начальными значениями - -1 для всех
;===============================================================================;
IndexInit proc WorkingBuffer:DWORD
	push edi
	mov ecx,NumberOfLeaf
	mov edi,WorkingBuffer
	lea edi,[edi+IndexesOffset];В EDI - адрес массива индексов 
	xor eax,eax
	dec eax
	rep stosw;
	pop edi
	ret 
IndexInit endp
;===============================================================================;


;===============================================================================;
;Считаются частоты вхождений
;===============================================================================;
CountBytes proc uses esi edi WorkingBuffer:DWORD,Buffer:DWORD,size1:DWORD
	push esi
	push edi

	mov esi,WorkingBuffer
	lea esi,[esi+TreeExOffset];В ESI - адрес LeafEx
	xor eax,eax;Начальное значение байта для подсчета
	mov ecx,256;Количество кодов
NextCode:
	push ecx	
	mov edi,Buffer
	mov ecx,size1
NextByte:
	assume esi:ptr LeafEx
	.if byte ptr [edi]==al
		inc [esi].Counter;Увеличиваем количество вхождений для данного кода 
	.endif
	inc edi
	loop NextByte
	pop ecx
	
	add esi,sizeof LeafEx;Следующий код и его частота
	inc ax
	loop NextCode
	
	pop edi
	pop esi
	ret
CountBytes endp
;===============================================================================;

;===============================================================================;
;Изменение порядка следования бит
;===============================================================================;
BitsSwap proc uses esi edi ebx HCode:DWORD
LOCAL Flag1:DWORD
	mov esi,HCode
	assume esi:ptr HuffmanCode
	xor ecx,ecx
	mov cl,[esi].Length1
	shr ecx,1
	xor edi,edi;Счетчик битов с начала
	xor ebx,ebx
	mov bl,[esi].Length1;Количество бит
	mov Flag1,0;Флаг для указания - бит с краю нулевой или единичный
	inc esi
NextBit:
	push ecx

	xor edx,edx
	mov ecx,8
	mov eax,edi
	xor edx,edx
	div ecx
	mov al,[esi][eax];Получаем нужный байт
	bt eax,edx;Какой очередной бит в байте?
	pushf
	mov ecx,edi
	mov eax,ebx
	sub eax,ecx;Получаем бит с другого краю
	dec eax;Нумерация с нуля
	mov ecx,8
	xor edx,edx
	div ecx
	mov al,[esi][eax]
	bt eax,edx
	jnc IsNull1
	mov Flag1,1
	jmp N1
IsNull1:
	mov Flag1,0
N1:	
	popf
	jnc IsNull;Переход на IsNull если ноль
	;Первый бит единица
	.if Flag1==1;Первый бит единица, последний бит единица
	;Оставляем так же
	.endif
	.if Flag1==0;Первый бит единица, последний бит ноль
		;Меняемся местами
		mov eax,edi;Номер первого
		mov ecx,8
		xor edx,edx
		div ecx
		push eax
		mov al,[esi][eax]
		btr eax,edx;Устанавливаем первый в ноль
		pop ecx
		mov byte ptr [esi][ecx],al

		;Номер последнего
		mov ecx,edi
		mov eax,ebx
		sub eax,ecx
		dec eax
		xor edx,edx
		mov ecx,8
		div ecx
		push eax
		mov al,[esi][eax]	
		bts eax,edx
		pop ecx
		mov byte ptr [esi][ecx],al
	.endif
	jmp N2
IsNull:
	;Первый бит нулевой
	.if Flag1==1;Первый бит нулевой, последний бит единица
		;Меняемся местами
		mov eax,edi;Номер первого
		mov ecx,8
		xor edx,edx
		div ecx
		push eax
		mov al,[esi][eax]
		bts eax,edx;Устанавливаем первый в ноль
		pop ecx
		mov byte ptr [esi][ecx],al

		;Номер последнего
		mov ecx,edi
		mov eax,ebx
		sub eax,ecx
		dec eax
		xor edx,edx
		mov ecx,8
		div ecx
		push eax
		mov al,[esi][eax]	
		btr eax,edx
		pop ecx
		mov byte ptr [esi][ecx],al
	.endif
	.if Flag1==0;Первый бит нулевой, последний бит нулевой
	;Оставляем также
	.endif
N2:
	pop ecx
	inc edi
	dec ecx
	.if ecx>0
		jmp NextBit
	.endif
	ret
BitsSwap endp 
;===============================================================================;

;===============================================================================;
;Сортировка по частотам и заполнение массива индексов нормальными значениями
;===============================================================================;
SetIndexes proc uses edi esi ebx WorkingBuffer:DWORD
	mov edi,WorkingBuffer
	lea esi,[edi+IndexesOffset];В ESI - адрес массива индексов 
	lea edi,[edi+TreeExOffset];В EDI - адрес TreeEx
;Процесс сортировки.
	assume edi:ptr LeafEx
	mov ecx,256
NextElement:;Внешний цикл
	xor bx,bx;Для подсчета своего места
	mov eax,[edi].Counter;Это значение сравниваем со всеми
	mov edx,edi;Значение адреса нужное в случае одинаковых частот
	push edi;Это указатель на текущий просматриваемый член
	push ecx;Счетчик
	mov ecx,256
	mov edi,WorkingBuffer
	lea edi,[edi+TreeExOffset];В EDI - адрес LeafEx
NextCounter:;Внутренний цикл. Сравниваем значение данной частоты со всеми частотами.
	.if eax>[edi].Counter
		inc bx
	.endif
	.if (eax==[edi].Counter)&&(edx>edi);Если частота одинаковая с предыдущими, но им надо присвоить разный индекс.
		inc ebx
	.endif
	add edi,sizeof LeafEx;Следующий элемент массива
	loop NextCounter
	pop ecx
	pop edi
	add edi,sizeof LeafEx
	mov word ptr [esi],bx;Сохраняем индекс для этого элемента
	add esi,2;Следующий элемент массива индексов
	loop NextElement
	ret
SetIndexes endp
;===============================================================================;


;===============================================================================;
;Получаем следующий адрес в массиве индексов для которого можно брать частоту
;Если нет такого элемента возвращаем 0
;===============================================================================;
NextCount proc uses esi edi WorkingBuffer:DWORD,Offset1:DWORD
	mov esi,Offset1
	mov edi,WorkingBuffer
	lea edi,[edi+IndexesOffset+1024];Конец таблицы индексов 
	.while esi<edi;Пока не дошли до конца таблицы
		.if word ptr [esi]!=-1
			mov eax,esi
			ret
		.endif
		add esi,2
	.endw
	xor eax,eax
	ret
NextCount endp
;===============================================================================;


;===============================================================================;
;Сортировка по частотам новой таблицы
;Те частоты, которые не входят в новую таблицу - их индексы при вызове равны -1
;Те частоты, которые входят в новую таблицу - их индексы при вызове равны 0
;Т.о. составляем массив индексов и используем частоты только для тех индексов для которых значение индекса первоночально равно 0
;===============================================================================;
SortIndexes proc uses edi esi ebx WorkingBuffer:DWORD
LOCAL Last:DWORD
LOCAL Count:WORD
	mov esi,WorkingBuffer
	lea edi,[esi+IndexesOffset];Указатель на текущий просматриваемый элемент в таблице индексов
	mov esi,edi
;Процесс сортировки. Простая сортировка здесь подходит, т.к. нам важно не использовать дополнительных буферов. Да и обменов нам никаких не нужно.
NextElement:;Внешний цикл
;Получаем адрес в таблице индексов частота которого может участвовать в сортировке
	push esi
	push WorkingBuffer
	call NextCount
	.if eax==0
		ret
	.endif
	mov esi,eax
;Получаем частоту для которой индекс ей соответствующий не равен -1 начиная с некоторой
	push eax
	push WorkingBuffer
	call GetCounter
;Частоту получили в eax
	mov Count,0;Для подсчета своего места
	mov edx,esi;Значение адреса нужное в случае одинаковых частот
	mov edi,WorkingBuffer
	lea edi,[edi+IndexesOffset]
	mov ebx,eax;В EBX - данная частота
NextCounter:;Внутренний цикл. Сравниваем значение данной частоты со всеми частотами.
	push edi
	push WorkingBuffer
	call NextCount
	.if eax==0
		mov cx,Count
		mov word ptr [esi],cx;Сохраняем индекс для этого элемента
		add esi,2;Следующий элемент массива индексов
		jmp NextElement
	.endif
	mov edi,eax

	push eax
	push WorkingBuffer
	call GetCounter
	.if ebx>eax
		inc Count
	.endif
	.if (ebx==eax)&&(edx>edi);Если частота одинаковая с предыдущими, но расположение у них разное, то им надо присвоить разный индекс.
		inc Count
	.endif
	add edi,2;Следующий элемент массива индексов
	jmp NextCounter
SortIndexes endp
;===============================================================================;


;===============================================================================;
;Получение частоты для данного смещения в таблице индексов
;===============================================================================;
GetCounter proc uses esi edi edx WorkingBuffer:DWORD,Offset1:DWORD
	mov edi,WorkingBuffer
	lea esi,[edi+TreeExOffset];В ESI - адрес LeafEx
	lea edi,[edi+IndexesOffset];В EDI - адрес массива индексов

	mov eax,Offset1
	sub eax,edi;Получаем смещение в таблице индексов
	shr eax,1;Делим на 2 и т.о. получаем номер элемента
	mov ecx,6
	mul ecx;Умножаем на 6 и т.о. получаем смещение в таблице TreeEx
	mov eax,dword ptr [esi+2][eax]
	ret
GetCounter endp
;===============================================================================;


;===============================================================================;
;Получение смещения элемента в массиве LeafEx по его индексу
;===============================================================================;
GetOffset proc uses esi edi edx ebx WorkingBuffer:DWORD,Index:DWORD
	mov edi,WorkingBuffer
	lea esi,[edi+TreeExOffset];В ESI - адрес LeafEx
	lea edx,[edi+IndexesOffset];В EDX - адрес массива индексов
;Ищем индекс - Index в массиве индексов
	mov ecx,NumberOfLeaf
	mov ebx,Index
	lea eax,[edi+IndexesOffset]
NextIndex:
	.if bx==word ptr [edx]
		mov edi,edx
		sub edi,eax;Получаем смещение в таблице индексов 
		mov eax,edi

		shr eax,1;Получаем номер элемента в таблице индексов
		mov ebx,6
		mul ebx
		add esi,eax;Получаем смещение в TreeEx
		mov eax,esi
		ret
	.endif
	add edx,2
	loop NextIndex
	ret
GetOffset endp
;===============================================================================;


;===============================================================================;
;Устанавливаем указатели. Возвращаем IndexFirst из исходной таблицы.
;===============================================================================;
GetIndexFirst proc uses esi edi ebx WorkingBuffer:DWORD
;Ищем первый индекс
	mov edi,WorkingBuffer
	lea esi,[edi+TreeExOffset];В ESI - адрес LeafEx
	lea edi,[edi+IndexesOffset];В EDI - адрес массива индексов 
	mov ecx,NumberOfLeaf;Счетчик цикла
	xor ebx,ebx;Очередной индекс
	xor edx,edx;Храним минимальный индекс соответсвтующий ненулевому значению частоты
	dec dx;Делаем этот индекс максимальным, для того чтобы найдя чуть-чуть меньше берем меньший, т.к. ищем мы минимальный.
NextIndex:
	push ecx
	mov bx,word ptr [edi];Получаем индекс отсортированного массива
	.if bx<dx
		;Ненулевая ли частота для данного индекса?
		push edi
		push WorkingBuffer
		call GetCounter
		.if eax!=0
			mov dx,bx;Сохраняем минимальный индекс
		.endif
	.endif
	add edi,2
	pop ecx
	loop NextIndex
	mov eax,edx	
	ret
GetIndexFirst endp
;===============================================================================;


;===============================================================================;
;Создание новой таблицы
;===============================================================================;
CreateNewTable proc uses edi WorkingBuffer:DWORD,IndexFirst:DWORD,IndexLast:DWORD
	mov edi,WorkingBuffer
	lea edi,[edi+IndexesOffset]
	mov ecx,NumberOfLeaf
	mov eax,IndexFirst	
NextIndex:;Цикл для задания частот, которые мы уже использовали и их использовать мы больше не будем 
	.if word ptr [edi]==-1
		jmp x
	.endif 
	.if word ptr [edi]<ax
		mov word ptr [edi],-1;Фиктивное значение означающее, что эту частоту мы не берем в расчет, т.к. она уже была использована	
	.else
		mov word ptr [edi],0;Эти значения, т.е. частоты им соотвествующие мы берем для сортировки
	.endif
x:
	add edi,2
	loop NextIndex
	;Т.к. в таблицу частот была добавлена новая частота, то в таблицу индексов добавляем дополнительный просматриваемый элемент, а именно просто делаем его 0
	mov eax,IndexLast
	lea eax,[eax*2];Здесь будет смещение нового элемента в таблице индексов
	mov edi,WorkingBuffer
	lea edi,[edi+IndexesOffset]
	lea edi,[edi][eax];Адрес нового элемента в таблице индексов, который мы будем просматривать
	mov word ptr [edi],0
	ret 
CreateNewTable endp
;===============================================================================;

;===============================================================================;
;Получение номера элемента в TreeEx по смещению
;===============================================================================;
GetTreeExIndex proc uses edi esi WorkingBuffer:DWORD, Offset1:DWORD
	mov edi,Offset1
	mov esi,WorkingBuffer
	lea esi,[esi+TreeExOffset]
	sub edi,esi;Разница 
	xor edx,edx
	mov eax,edi
	mov ecx,6
	div ecx;Делим на 6
	ret
GetTreeExIndex endp
;===============================================================================;


;===============================================================================;
;Дерево построено? 1 - да, 0 - нет
;===============================================================================;
IsRoot proc uses esi edi WorkingBuffer:DWORD
	mov edi,WorkingBuffer
	lea edi,[edi+IndexesOffset]
	mov ecx,NumberOfLeaf
	xor eax,eax
NextLeaf: 
	.if word ptr [edi]!=-1
		inc eax
	.endif 
	add edi,2
	loop NextLeaf
	.if eax==1
		ret		
	.else
		xor eax,eax	
		ret
	.endif
IsRoot endp
;===============================================================================;


;===============================================================================;
;Адрес в маасиве Tree по номеру элемента
;===============================================================================;
GetAddress proc uses esi edi WorkingBuffer:DWORD,Number:DWORD
	mov esi,Number
	shl esi,2;Умножаем на 4
	mov edi,WorkingBuffer 
	lea edi,[edi+TreeOffset]
	add edi,esi
	mov eax,edi
	ret
GetAddress endp
;===============================================================================;


;===============================================================================;
;Новый нулевой бит представления
;===============================================================================;
AddNullBit proc uses esi edi HCode:DWORD
	mov esi,HCode
	assume esi:ptr HuffmanCode
;Вычисляем в каком байте наш бит
	xor eax,eax
	mov al,[esi].Length1
	dec eax
	mov ecx,8
	xor edx,edx
	div ecx;Делим на 8, чтобы получить номер байта(у нас он 0-3) и номер бита в edx
	push eax
	mov al,[esi+1][eax];Получаем нужный байт
	mov ecx,1
	xchg ecx,edx
	shl dl,cl;1 shl (номер бита в байте)
	not dl
	and al,dl
	pop edi
	mov byte ptr [esi+1][edi],al;Установить в 0 [esi].Length бит	
	ret
AddNullBit endp
;===============================================================================;


;===============================================================================;
;Новый единичный бит представления
;===============================================================================;
AddOneBit proc uses esi edi HCode:DWORD
	mov esi,HCode
	assume esi:ptr HuffmanCode
;Вычисляем в каком байте наш бит
	xor eax,eax
	mov al,[esi].Length1
	dec eax
	mov ecx,8
	xor edx,edx
	div ecx;Делим на 8, чтобы получить номер байта(у нас он 0-3) и номер бита
	push eax
	mov al,[esi+1][eax];Получаем нужный байт
	mov ecx,1
	xchg ecx,edx
	shl dl,cl;1 shl (номер бита в байте)
	or al,dl
	pop edi
	mov byte ptr [esi+1][edi],al;Установить в 0 [esi].Length-1 бит	
	ret
AddOneBit endp
;===============================================================================;


;===============================================================================;
;Вычисление размера упакованного дерева
;===============================================================================;
GetTreeSize proc WorkingBuffer:DWORD
	;Подсчет количества узлов в дереве
	;Для этого берем таблицу TreeEx с частотами. У тех где частоты ненулевые те и берем. Первые 256 не берем.
	mov esi,WorkingBuffer
	mov eax,256
	mov ecx,6
	mul ecx;Смещение в TreeEx
	lea esi,[esi][eax];Адрес в TreeEx
	mov ecx,256
	xor eax,eax
	assume esi:ptr LeafEx
NextCount1:
	.if [esi].Counter!=0
		inc eax
	.endif
	add esi,sizeof LeafEx
	loop NextCount1   
	
	ret
GetTreeSize endp
;===============================================================================;


;===============================================================================;
;Сравнить цепочку битов по адресу String1 с битовым смещением Offset1
;с цепочкой битов по адресу String2 и с битовым смещением Offset2.
;Количество битов которые надо сравнивать равно Number. Это число битов в String2
;===============================================================================;
CmpBits proc uses esi edi ebx String1:DWORD,Offset1:DWORD,String2:DWORD,Offset2:DWORD,Number:DWORD
LOCAL Flag1:BYTE
LOCAL Flag2:BYTE
	mov esi,String1;С чем сравнивать
	mov edi,String2;Что сравнивать
	mov ecx,Number;Сколько битов сравнивать
NextBit:
	push ecx
;Получаем байт String1 из битового смещения Offset1
	mov eax,Offset1
	mov ecx,8
	xor edx,edx
	div ecx;В EAX - номер байта, в EDX - номер бита в байте
	mov al,byte ptr [esi][eax];Вот и нужный байт
	and eax,0FFh;Обнуляем все после al в eax
	bt eax,edx;Тестим нулик в нужном бите или единица
	jnc IsNull;Переход если нуль
	mov Flag1,1;В первой битовой строке указанный бит равен единице
	jmp NextByte
IsNull:
	mov Flag1,0;В первой битовой строке указанный бит равен нуль
;Получаем байт String2 из битового смещения Offset2
NextByte:
	mov eax,Offset2
	mov ecx,8
	xor edx,edx
	div ecx;В EAX - номер байта, в EDX - номер бита в байте
	mov al,byte ptr [edi][eax];Вот и нужный байт
	and eax,0FFh;Обнуляем все после al в eax
	bt eax,edx;Тестим нулик в нужном бите или единица
	jnc IsNull2;Переход если нуль
	mov Flag2,1;Во второй битовой строке указанный бит равен единице
	jmp NextNext
IsNull2:
	mov Flag2,0;Во второй битовой строке указанный бит равен нулю
NextNext:
	mov al,Flag2
	.if Flag1==al;Биты равны
		pop ecx
		inc Offset1
		inc Offset2
		loop NextBit
		xor eax,eax
		inc eax
		ret
	.else
		pop ecx
		xor eax,eax;Битовые строки не равны
		ret
	.endif
CmpBits endp
;===============================================================================;


;===============================================================================;
;Вычисление размера кодированных данных
;===============================================================================;
GetCodeSize proc uses esi edi WorkingBuffer:DWORD
LOCAL Summa:DWORD
	mov ecx,256
	xor eax,eax
	mov edi,WorkingBuffer
	lea esi,[edi+TreeExOffset];Адрес для получения частот
	lea edi,[edi+HuffmanCodesOffset];Длины представлений
	assume esi:ptr LeafEx
	assume edi:ptr HuffmanCode
	mov Summa,0
NextByte: 
	mov eax,[esi].Counter
	xor edx,edx
	mov dl,[edi].Length1	
	mul edx;Частоту*(Длину_кода_в_битах)
	add Summa,eax
	add esi,6
	add edi,33
	loop NextByte
	add Summa,7
	mov eax,Summa
	xor edx,edx
	mov ecx,8
	div ecx;Разделить на 8
	ret
GetCodeSize endp
;===============================================================================;

;===============================================================================;
;Вычисление кодов Хаффмана
;Root - адрес элемента, где искать
;CodeOffset - байт, представление которого кодируем 
;pHCode - Указатель на уже построенный массив битов
;WorkingBuffer - рабочий буфер
;===============================================================================;
GetHuffmanCodes proc uses esi edi eax ecx ebx edx WorkingBuffer:DWORD,CodeOffset:DWORD,pHCode:DWORD,Root:DWORD
LOCAL HCode:HuffmanCode
	mov ecx,sizeof HCode
	mov esi,pHCode
	lea edi,HCode
	rep movsb;В нашу локальную переменную HCode копируем 33 байта с адреса pHCode. 
	mov esi,Root
	assume esi:ptr Leaf
	.if ([esi].Left==-1)&&([esi].Right==-1)
		;Вычисляем индекс esi
		mov edi,WorkingBuffer
		lea edi,[edi+TreeOffset]
		sub esi,edi;Разница 
		shr esi,2;Делим на 4 получаем индекс
		.if esi==CodeOffset;Код вычислен
			;Копируем из HCode в массив HuffmanCodes[CodeOffset]
			mov eax,CodeOffset
			mov ecx,33
			mul ecx;Умножаем на 33
			mov edi,WorkingBuffer
			lea edi,[edi+HuffmanCodesOffset][eax];В ESI адрес нужного элемента
			mov ecx,sizeof HCode
			lea esi,HCode
			rep movsb;Получаем новое представление и длинну
			ret
		.endif
		ret
	.endif
	lea edi,HCode
	assume edi:ptr HuffmanCode
	inc [edi].Length1;Увеличиваем длину представления
	.if [esi].Left!=-1
		push edi;Адрес HuffmanCode
		call AddNullBit;Новый нулевой бит представления
		
		xor eax,eax
		mov ax,[esi].Left
		push eax
		push WorkingBuffer
		call GetAddress;Вычисляем по [esi].Left его адрес

		push eax
		lea ecx,HCode
		push ecx
		push CodeOffset
		push WorkingBuffer
		call GetHuffmanCodes
	.endif
	.if [esi].Right!=-1
		push edi;Адрес HuffmanCode
		call AddOneBit;Новый единичный бит представления

		xor eax,eax
		mov ax,[esi].Right
		push eax
		push WorkingBuffer
		call GetAddress;

		push eax
		lea ecx,HCode
		push ecx
		push CodeOffset
		push WorkingBuffer
		call GetHuffmanCodes
		ret
	.endif
	ret
GetHuffmanCodes endp
;===============================================================================;


;===============================================================================;
;Копирование битов
;CompressData - куда записать
;BitsString - что записать
;Number - Сколько бит записать
;BitsOffset - По какому битовому смещению записать
;===============================================================================;
CopyBits proc uses esi edi ebx CompressData:DWORD,BitsOffset:DWORD,BitsString:DWORD,Number:DWORD
LOCAL ByteOffset:DWORD
	mov esi,CompressData
	mov ecx,Number
	mov edi,BitsString
	xor ebx,ebx;Битовое смещение в исходной строке битов
NextBit:
	push ebx
	push ecx

	mov eax,ebx
	mov ecx,8
	xor edx,edx;Старшая часть операнда
	div ecx;Получаем номер байта в котором находиться бит который надо переслать
	mov al,byte ptr [edi][eax];Получаем байт в котором находиться нужный бит
	bt eax,edx;Проверяем ноль это или единица
	pushf;Сохраняем регистр флагов
	mov eax,BitsOffset
	mov ecx,8
	xor edx,edx;Старшая часть операнда
	div ecx;Получаем байт в котором надо установить ноль
	mov ByteOffset,eax;Сохраняем байтовое смещение
	mov al,byte ptr [esi][eax]
	popf
	jnc IsNull;Если это ноль, то переход на IsNull
	bts eax,edx;Установим нужный бит в 1
	jmp NextNext
IsNull:
	btr eax,edx;Установим нужный бит в 0
NextNext:
	mov ebx,ByteOffset
	mov byte ptr [esi][ebx],al
	inc BitsOffset
	pop ecx
	pop ebx
	inc ebx;Следующий бит
	loop NextBit
	ret
CopyBits endp
;===============================================================================;


;===============================================================================;
;Кодирование данных
;===============================================================================;
Compressing proc WorkingBuffer:DWORD,Buffer:DWORD,Size1:DWORD,CompressData:DWORD
	mov esi,WorkingBuffer
	mov edi,Buffer
	lea esi,[esi+HuffmanCodesOffset]
	mov ecx,Size1
	xor ebx,ebx;Битовое смещение приемника
NextByte:
	push ecx
	xor eax,eax
	mov al,byte ptr [edi];Получаем байт который надо закодировать
	;Ищем соответствие в таблице кодов Хаффмана
	mov ecx,33
	mul ecx;Получаем смещение в массиве кодов Хаффмана
	lea edx,[esi][eax];Адрес элемента в массиве кодов Хаффмана
	assume edx:ptr HuffmanCode

	xor eax,eax
	mov al,[edx].Length1
	push edx;Сохраняем EDX

	push eax;Сколько битов надо переслать
	inc edx
	push edx;что надо переслать
	push ebx;Битовое смещение приемника
	push CompressData;Куда пересылаем
	call CopyBits

	pop edx;Восстанавливаем EDX
	xor eax,eax
	mov al,byte ptr [edx]
	add ebx,eax

	inc edi;Кодируем следующий байт
	pop ecx
	loop NextByte
	ret
Compressing endp
;===============================================================================;


;===============================================================================;
;Создаем упакованную таблицу кодов Хаффмана
;Возвращаем размер упакованной таблицы
;===============================================================================;
CreatePackHuffmanCode proc uses edi esi WorkingBuffer:DWORD,Buffer:DWORD
	mov edi,WorkingBuffer
	lea edi,[edi+HuffmanCodesOffset];В EDI - таблица кодов Хаффмана
	assume edi:ptr HuffmanCode
	mov esi,Buffer;В ESI - куда будем ложить упакованную таблицу кодов Хаффмана
	mov ecx,256
	xor eax,eax;Номер текущего проверяемого бита
;Создаем таблицу присутствия
NextByte:
	.if [edi].Length1!=0;Если для этого байта код есть, то добавляем его в таблицу присутствия
		push ecx
		push eax;Сохраняем номер бита 
		mov ecx,8
		xor edx,edx
		div ecx;В EAX - номер байта, в EDX - номер бита
		push eax
		mov al,byte ptr [esi][eax];Получаем нужный байт в таблице присутствия
		and eax,0FFh
		bts eax,edx;Устанавливаем номер бита в таблицу присутствия
		mov edx,eax
		pop ebx
		mov byte ptr [esi][ebx],dl;Сохраняем байт в таблице присутствия
		pop eax;Восстанавливаем счетчик битов
		pop ecx
	.endif
	add edi,sizeof HuffmanCode
	inc eax
	loop NextByte
;Создаем упакованную таблицу кодов Хаффмана	
;Сначала идет длина кода Хаффмана. Она занимает 1 байт.
;Потом идет сам код и его биты выровненные по байту.
	mov ecx,256
	mov edi,WorkingBuffer
	lea edi,[edi+HuffmanCodesOffset]
	xor edx,edx;Индексный регистр для упакованной таблицы кодов Хаффмана
	mov edx,32;Также здесь в итоге будет размер упакованной таблицы
NextCode:
	push ecx
	.if [edi].Length1!=0
		mov al,[edi].Length1
		mov byte ptr [esi][edx],al;Записываем длину кода в таблицу упакованных кодов
		inc edx;Следующий байт
		;Копируем al бит по адресу [esi][edx] с битовым смещением 0 из [edi+1]
		push edx;Сохраняем счетчик байтов

		push eax;Сколько бит
		lea ebx,[edi+1]
		push ebx;Откуда копировать
		push 0;Смещение 0
		lea ebx,[esi][edx]
		push ebx;Куда копировать
		call CopyBits

		
		;Сколько байт занимает код для данного байта
		mov al,[edi].Length1;Длина кода в битах
		mov ecx,8

		xor edx,edx
		div ecx;Получаем количество байт. Выровненное количество = EAX + 1
		inc eax;Выровненное количество
		pop edx;Восстанавливаем счетчик байтов
		
		
		add edx,eax;Следующие байта в упакованное таблице
	.endif 
	add edi,sizeof HuffmanCode
	pop ecx
	loop NextCode
	mov eax,edx
	ret
CreatePackHuffmanCode endp
;===============================================================================;


;===============================================================================;
;Распаковка данных по методу Хаффмана
;===============================================================================;
bp_decompress proc Buffer:DWORD,CompressData:DWORD,WorkingBuffer:DWORD
LOCAL Offset1:DWORD;Смещение в распакованных данных
	mov Offset1,0
;Обнуляем первые 256*33 байт, т.е. рабочий буфер
	mov ecx,256*33
	mov edi,WorkingBuffer
	mov eax,0
	rep stosb

	mov edi,CompressData
	assume edi:ptr Header
	.if [edi].Signature!="PB"
		xor eax,eax
		ret;Отсутствует правильная сигнатура
	.endif
	mov ebx,[edi].CRC32

	mov [edi].CRC32,0

	push [edi].CompressSize
	push edi
	call CRC32
	.if eax!=ebx
		xor eax,eax
		ret;неправильная CRC32
	.endif
	mov [edi].CRC32,eax
	add edi,sizeof Header;Теперь в EDI - упакованная таблица кодов Хаффмана, первые 32 байт которой - таблица присутствия
	mov ebx,edi;
	add ebx,32;Упакованная таблица кодов Хаффмана
;1)Распоковываем упакованную таблицу Хаффмана
	mov esi,WorkingBuffer;Распакованная таблица
	mov ecx,256
	xor eax,eax;Счетчик для битов таблицы представлений
NextCode:
	push ecx
	push eax 
	
	mov ecx,8
	xor edx,edx
	div ecx

	mov al,byte ptr [edi][eax];Получаем байт таблицы присутствия
	bt eax,edx
	jnc IsNull
	;Если не ноль, то...
	pop eax
	push eax
	
	mov al,byte ptr [ebx];Длина кода
	mov byte ptr [esi],al;Записываем длину кода
	inc esi
	inc ebx

	push eax

	push eax;Сколько бит
	push ebx;Откуда копировать
	push 0;Смещение 0
	push esi;Куда копировать
	call CopyBits;Теперь записываем сам код

	pop eax;Количество бит
	mov ecx,8
	xor edx,edx
	div ecx;Количество байт
	inc eax;Количество байт + 1
	add ebx,eax;Следующие байты в таблице кодов
	dec esi
	add esi,sizeof HuffmanCode
	pop eax
	pop ecx
	jmp NextNext
IsNull:
	pop eax
	pop ecx
	add esi,sizeof HuffmanCode
NextNext:
	inc eax
	loop NextCode
;Декодирование
	mov edi,CompressData
	mov ecx,[edi].DecompressSize
	xor ebx,ebx
	mov bx,[edi].HuffmanCodesSize
	lea edi,[edi+sizeof Header][ebx];Указатель на сжатые данные
	xor ebx,ebx;Битовый указатель на сжатые данные

NextByte: 
	push ecx
	;Каждый код сравниваем с указателем на закодированные данные
	mov ecx,256
	assume esi:ptr HuffmanCode
	mov esi,WorkingBuffer;Таблица кодов Хаффмана
NextCode1:
	push ecx
	.if [esi].Length1!=0;Ненулевой код найден
		;Сравниваем этот код с нашей битовой строкой
		xor eax,eax
		mov al,[esi].Length1
		push eax;Длина сравниваемой строки
		push 0;Смещение в коде нулевое, т.к. он начинается с байта
		inc esi
		push esi;С чем сравниваем
		dec esi
		push ebx;По смещению ebx
		push edi;Что стравниваем
		call CmpBits
		.if eax==1;Нашли представление
			xor eax,eax
			
			mov al,byte ptr [esi]
			add ebx,eax;Новое битовое смещение

			push edi
			push esi

			;Номер элемента в таблице кодов Хаффмана
			mov edi,WorkingBuffer
			sub esi,edi;Разница
			mov ecx,33
			xor edx,edx
			mov eax,esi
			div ecx;В eax - номер байта и => номер элемента

			mov edi,Buffer
			mov edx,Offset1
			mov byte ptr [edi][edx],al
			inc Offset1

			pop esi
			pop edi
			pop ecx
			jmp GoodByte
		.endif
	.endif
	pop ecx
	add esi,33;Следующий код
	loop NextCode1
GoodByte:
	pop ecx
	loop NextByte
	xor eax,eax
	inc eax
	ret
bp_decompress endp
;===============================================================================;


;===============================================================================;
;Сжатие данных по методу Хаффмана
;===============================================================================;
bp_compress proc uses edi esi WorkingBuffer:DWORD,Buffer:DWORD,size1:DWORD,CompressData:DWORD
LOCAL IndexLast:DWORD
LOCAL IndexFirst:DWORD
LOCAL IndexNewLeaf:DWORD
LOCAL HCode:HuffmanCode
LOCAL Flag:DWORD
LOCAL Flag2:DWORD
;1) Заполняем массив узлов начальными значениями
	push WorkingBuffer
	call TreeInit
;2) Заполняем массив индексов
	push WorkingBuffer
	call IndexInit
;3) Подсчитываем число вхождений байтов
	push size1
	push Buffer
	push WorkingBuffer
	call CountBytes
;4) Сортировка по возрастанию частот
	push WorkingBuffer
	call SetIndexes
;5а) Устанавливаем IndexFirst
	push WorkingBuffer
	call GetIndexFirst
	mov IndexFirst,eax
;5б) Устанавливаем IndexLast
	mov IndexLast,0FFh
;6) Выбираем из исходной таблицы два левых узла и объединяем в новый
NextLeaf:;Следующий узел
;Вычисляем адрес 1-го элемента с минимальной частотой
	push IndexFirst
	push WorkingBuffer
	call GetOffset
	push eax;Сохраняем адрес 1-го элемента с минимальной частотой
;Вычисляем адрес 2-го элемента с минимальной частотой IndexFirst + 1
	mov eax,IndexFirst
	inc eax
	push eax
	push WorkingBuffer
	call GetOffset
;Получаем нужные адреса для элементов
	pop edi;Восстанавливаем адрес 1-го элемента с минимальной частотой
	mov esi,eax;Адрес 2-го элемента с минимальной частотой
	xchg esi,edi
	;ESI - адрес с минимальной частотой
	;EDI - адрес с минимальной частотой после EDI

	assume esi:ptr LeafEx
	assume edi:ptr LeafEx
;Вычисляем адрес первого свободного элемента для добавления нового узла в дерево. Первый свободный элемент будет иметь индекс IndexLast + 1
	push edi
	inc IndexLast
	mov eax,IndexLast;Получаем смещение IndexLast + 1
	mov ecx,6
	xor edx,edx
	mul ecx;Умножаем на 6

	mov edi,WorkingBuffer
	lea edi,[edi+TreeExOffset][eax]
	mov eax,edi
	assume eax:ptr LeafEx
	pop edi
;Заполняем поля для нового узла
	xor edx,edx
	mov edx,[esi].Counter
	add edx,[edi].Counter;Сумма частот для нового узла
	mov ecx,[esi].Counter
;Частота для нового узла
	mov [eax].Counter,edx
;Вычисляем индекс из EDI
	push edi
	push WorkingBuffer
	call GetTreeExIndex
	mov ecx,eax
	
	mov eax,IndexLast
	shl eax,2;Умножаем на 4
	mov edi,WorkingBuffer
	lea edi,[edi][eax]
	assume edi:ptr Leaf
;Добавляем левый элемент в дерево
	mov [edi].Left,cx
;Вычисляем индекс из ESI
	push esi
	push WorkingBuffer
	call GetTreeExIndex
	mov ecx,eax
;Добавляем правый элемент в дерево
	mov [edi].Right,cx
;Создается новая таблица частот
	add IndexFirst,2
;При создании новой таблицы частот таблица индексов будет новая, т.к. все соотношения порядков сортируемого массива изменяться
;В процедуре CreateNewTable передаем IndexFirst. В новой таблице мы исключаем элементы с индексами меньше IndexFirst + 1
;Сортировка будет идти так - если данный индекс -1, то частота которая ему соответствует уже использовалась для создания нового узла. 
;Значит эту частоту мы не берем. Иначе если индекс равен нулю, то элементы соответсвующие этим нулям сортируются по частотам. 
	push IndexLast
	push IndexFirst
	push WorkingBuffer
	call CreateNewTable
;Сортируем новую таблицу по возрастанию частот вхождений
	push WorkingBuffer
	call SortIndexes
;Проверяем сколько узлов в таблице
	push WorkingBuffer
	call IsRoot
	.if eax==1
		mov esi,WorkingBuffer
		lea esi,[esi+IndexesOffset]
		mov ecx,512
NextByte: 
		.if word ptr [esi]==0
			jmp Ok;Корень почти в ESI
		.endif
		add esi,2
		loop NextByte
Ok:
		;Получаем адрес в Tree корневого элемента
		;Вычисляем коды Хаффмана и записываем их в буфер
		mov eax,512
		sub eax,ecx;В EAX - индекс корневого элемента
		mov edi,WorkingBuffer;Получаем адрес корня
		lea edi,[edi+TreeOffset]
		shl eax,2;Умножаем eax на 4
		lea edi,[edi][eax]

		mov ecx,256
		xor esi,esi
		mov ebx,edi;Адрес корня
NextPred:;Следующее представление
		push ebx

		push ecx
		;Обнуляем HCode
		lea edi,HCode
		mov eax,0
		mov ecx,33
		rep stosb
		pop ecx


		lea eax,HCode
		push eax
		push esi;Какой байт ищем, надо сделать цикл по всем имеющимся байтам
		push WorkingBuffer
		call GetHuffmanCodes
		push ecx
		;Меняем порядок бит в представлении
		mov edi,WorkingBuffer
		lea edi,[edi+HuffmanCodesOffset]
		mov ecx,33
		mov eax,esi
		mul ecx
		lea edi,[edi][eax]
		assume edi:ptr HuffmanCode
		;.if [edi].Length1!=0
		;	push edi
		;	call BitsSwap
		;.endif
		inc esi
		pop ecx
		loop NextPred
		;Далее создаем структуру |Заголовок|Таблица_Кодов_Хаффмана|Данные|. Это структура и будет сжатыми данными.
		push WorkingBuffer
		call GetCodeSize;Вычисление размера кодированных данных
		mov ecx,eax
		add ecx,sizeof Header
		add ecx,32
		.if ecx>size1;Если размер сжатых данные превышает размер исходных данных, то ошибка
			xor eax,eax
			ret
		.endif
		;Создаем заголовок
		mov edi,CompressData
		assume edi:ptr Header
		push edi
		mov [edi].Signature,"PB"
		mov [edi].CompressSize,eax
		add [edi].CompressSize,sizeof Header;Добавляем к размеру сжатых данных заголовок



		mov [edi].CRC32,eax
		mov eax,size1
		mov [edi].DecompressSize,eax


		lea esi,[edi+sizeof Header];Далее должна идти упакованная таблица кодов Хаффмана
		
		push esi
		push WorkingBuffer
		call CreatePackHuffmanCode;Создаем массив упакованных кодов Хаффмана
		mov [edi].HuffmanCodesSize,ax
		add [edi].CompressSize,eax
		add esi,eax;Смещение для сжатых данных

		push esi;Буфер для сжатых данных
		push size1;Размер исходных данных
		push Buffer;Исходные данные
		push WorkingBuffer;Рабочий буфер
		call Compressing;Выполняем кодирование
		;Вычисление CRC32
		pop edi
		mov ecx,[edi].CompressSize
		mov [edi].CRC32,0
		push ecx
		push edi
		call CRC32
		mov [edi].CRC32,eax
		ret;Конец процесса сжатия
	.else
		mov IndexFirst,0;IndexFirst будет 0, IndexLast будет таким же 
		jmp NextLeaf;Следующий узел
	.endif
bp_compress endp
;===============================================================================;