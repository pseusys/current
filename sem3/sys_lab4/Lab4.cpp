#include <iostream>
#include <fstream>
#include <Windows.h>
#include <clocale>

using namespace std;

void hello();

int main() {
	hello();
	char input_string[80], output_string[80];
	cin.getline(input_string, 80, '\n');

	__asm {
		push si
		push di
		push ax
		push dx
		push bx

		mov dl, 0Fh
		mov cl, 0h

		lea si, input_string
		lea di, output_string

		reader :
		lodsb
			test al, al
			je ender


			cyrill_checker :
		cmp al, 0E0h
			jb io_checker
			sub al, 20h
			jmp printer

			io_checker :
		cmp al, 0B8h
			jne number_checker
			sub al, 10h
			jmp printer


			number_checker :
		mov bl, 30h
			sub al, bl
			cmp al, 0h
			jl partly_printer
			cmp al, 9h
			jle inverter

			add bl, 11h
			sub al, 11h
			cmp al, 0h
			jl partly_printer
			cmp al, 5h
			jle inverter_tenth

			add bl, 20h
			sub al, 20h
			cmp al, 0h
			jl partly_printer
			cmp al, 5h
			jle inverter_tenth


			partly_printer :
		add al, bl

			printer :
		stosb
			jmp reader


			inverter_tenth :
		add al, 0Ah

			inverter :
		xor al, dl
			cmp al, 0Ah
			jge inverter_alpha
			add al, 30h
			jmp printer

			inverter_alpha :
		add al, 37h
			jmp printer


			ender :
		stosb
			pop bx
			pop dx
			pop ax
			pop di
			pop si
	}
	cout << "Строка, полученная после преобразований: " << output_string << endl;

	ofstream ff("out.txt");
	ff << output_string << endl;
	ff.flush();
	ff.close();

	system("PAUSE");
	return 0;
}

void hello() {
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	cout << "Лабораторная работа №4" << endl;
	cout << "Выполнил cтудент группы 8381 Сергеев А.Д." << endl;
	cout << "Задание:\nИнвертирование введенных во входной строке цифр в шестнадцатиричной системе счисления (СС) и преобразование строчных "
		"русских букв в заглавные, остальные символы входной строки передаются в выходную строку непосредственно." << endl;
	cout << "Введите строку: ";
}
