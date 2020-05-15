/* Написать программу реализующую игру «Угадай число». Компьютер загадывает число от 0 до 999 (используйте генерацию случайных чисел), 
а пользователь угадывает его. На каждом шаге угадывающий делает предположение, а задумавший число — говорит сколько цифр из числа угаданы и сколько 
из угаданных цифр занимают правильные позиции в числе. Например, если задумано число 725 и выдвинуто предположение, что задумано число 523, 
то угаданы две цифры (5 и 2) и одна из них занимает верную позицию. */

#include <iostream>
#include <string>  // для std::string
#include <array>   // для std::array
#include <clocale> //для вывода на кириллице
#include <cstdlib> //для функций srand и rand
#include <ctime>   //чтобы сделать случайный выбор, привязанный ко времени
using std::cout; using std::cin; using std::endl; using std::string; using std::array;

// Функция возвращает случайное число в заданных границах
unsigned short createRandomNumber(unsigned short min, unsigned short max)
{
	static const double fraction = 1.0 / static_cast<double>(RAND_MAX + 1.0);
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

// функция получает от пользователя число от 0 до 999
unsigned int getUsersNumber()
{
	cout << "Ваш вариант: ";
	int usersNumber{ 0 };
	cin >> usersNumber;
	//если предыдущее извлечение было неудачным, возвращаем cin в рабочий режим
	while (cin.fail() || (usersNumber < 0) || (usersNumber>999))  // если вводит не то или число за рамками [0, 999]
	{
		cin.clear();
		//очищаем буфер cin
		cin.ignore(1000, '\n');
		cout << "Вы ввели нечто некорректное, введите число от 0 до 999: ";
		cin >> usersNumber;      //снова вводит число
		continue;
	}
	return usersNumber;
}

// функция раскладывает переданное число n на три цифры - f1, f2 и f3, из которых оно состоит.
void numberToFigures(const unsigned int n, array<unsigned short, 3>& figures)
{
	/* Мы не предполагаем чисел в виде 001 или 033, впереди нуля быть не может, 
	Если число однозначное или двузначное, нам понадобится специальное обозначение для того, что разряд отсутствует.
	0 для этой цели не подходит, поэтому будем использовать число, не похожее на цифру, например двузначное 
	Для числа от пользователя и числа от компьютера это спецобозначение должно быть разным, чтобы случайно не засчитать отсутствие разряда за совпадение.
	Поэтому будем использовать статическую переменную, она инициализируется один раз, и при каждом вызове ф-ии будет увеличиваться на 1*/
	static unsigned short no_figure{ 10 }; // двузначное число, не похожее на цифру, означающее, что в данном разряде пусто. Инициализируем самым маленьким двузначным числом.
	no_figure++;                           // каждый следующий вызов это особое число будет отличаться от предыдущего, т.к. это статическая переменная

	if (n < 10)      // n  - однозначное число
	{
		figures[0] = no_figure;
		figures[1] = no_figure;
		figures[2] = n;
	}
	else
	{ 
		if (n < 100) // n - двухзначное число
		{
			figures[0] = no_figure;
			figures[1] = n / 10;
			figures[2] = n % 10;
		}
		else         // n - трёхзначное число
		{
			figures[0] = n / 100;
			figures[1] = (n - (figures[0] * 100)) / 10;
			figures[2] = n % 10;
		}
	}
}

// Функция сравнивает задуманное число с числом, введённым пользователем и даёт обратную связь
// Если число угадано, возвращает true, иначе - false
bool compareWithNumber(unsigned int theNumber, unsigned int usersNumber)
{
	array <unsigned short, 3> figures{ 0, 0, 0 };
	array <unsigned short, 3> users_figures{ 0, 0, 0 };
	numberToFigures(theNumber, figures);
	numberToFigures(usersNumber, users_figures);

	unsigned short guessed_figures{ 0 }; // счётчик угаданных цифер
	unsigned short guessed_places{ 0 };  // счётчик угаданных цифер на своих местах
	array <bool, 3> available{ 1, 1, 1 }; // массив для обозначения, какие цифры в загаданном числе уже нашли, сначала ни одной не нашли, то есть все доступны
	for (unsigned short i = 0; i < 3; i++)  // идём по цифрам числа от пользователя
	{
		if (figures[i] == users_figures[i]) // цифра загаданного числа совпадает с цифрой на том же месте у числа от пользователя
		{
			guessed_places++;
			if (available[i] == 1) 
			{
				guessed_figures++;
				available[i] = 0;   // делаем эту цифру в загаданном числе недоступной для дальнейших сравнений
			}
		}
		else // цифра загаданного числа не на своём месте в числе пользователя
		{
			bool found = false; // булева переменная, означающая, что такая цифра в записи есть. Нужна для того, что если мы её нашли, то дальше не ищем, переходим к следующему i
			for (unsigned short j = 0; (j < 3) && (found == false); j++) // идём по доступным цифрам загаданного числа
			{
				if (i == j) continue; // это мы уже проверяли, поэтому смотрим следующую
				if ((users_figures[i] == figures[j])&&(available[j] ==1)) // цифра числа пользователя совпадает с доступной цифрой загаданного числа, но на другом месте
				{
					guessed_figures++;
					found = true;
					available[j] = 0;  // делаем эту цифру в загаданном числе недоступной для дальнейших сравнений
				}
			}
		}
	}
	cout << "Угадано: " << guessed_figures << ". На своих местах: " << guessed_places << ". " << endl;
	return false; // возвращать true смысла нет, так как полное совпадение чисел проверяется в main
}

// Функция возвращает true, если пользователь хочет продолжать и false - если нет 
bool wantsContinue()
{
	cout << endl;
	cout << "Попробовать ещё раз? Нажмите 'y', если да или 'n', если нет ";
	string answer{""};
	cin.ignore(1000, '\n');   // очищаем cin
	std::getline(cin, answer);
	while ((answer != "y") && (answer != "n"))
	{
		cout << "Ваш ответ не понятен! Хотите угадать число ещё раз? Нажмите 'y', если да или 'n', если нет ";
		std::getline(cin, answer);
	}
	cout << endl;
	if (answer == "y") return true;
	else return false;
}

int main()
{
	//для вывода на кириллице
	setlocale(LC_CTYPE, "rus");

	//делаем что-то типа randomize 
	srand(static_cast<unsigned int>(time(0)));
	rand();

	//инициализируем задуманное число нулём
	unsigned short theNumber{ 0 };

	while (true) // пока пользователь хочет играть
	{
		theNumber = createRandomNumber(0, 999);
		cout << "Компьютер загадал трёхзначное число. Вы должны его отгадать. " << endl << "После очередного числа вам будет сообщено, сколько цифр угадано и сколько из них находятся на своих местах." << endl;
		bool didntQuess = true;  // логический индикатор "не угадал"
		while (didntQuess) // пока не угадал
		{
			int usersNumber = getUsersNumber();
			if (theNumber == usersNumber)
				didntQuess = false;      // угадал
			else
				didntQuess = !compareWithNumber(theNumber, usersNumber);
		}
		cout << "***Вы угадали число " << theNumber << "! ***" << endl;
		if (wantsContinue() == false) break;
	}

	return 0;
}
