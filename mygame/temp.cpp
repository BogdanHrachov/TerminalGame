#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>

// Опрос клавиатуры без блокирования основного потока
class crt_t {
	private:

	int is_run = 127;

	struct t_key {
		char data = '\0', keyexit = '\1';
		bool status = false;
	} key;

	struct termios tty;
	struct termios savetty;

	pthread_t thread;

	public:
	const int black = 30;
	const int red = 31;
	const int green = 32;
	const int yellow = 33;
	const int blue = 34;
	const int purpure = 35;
	const int lightblue = 36;
	const int white = 37;

	const int normal = 0;
	const int bold = 1;
	const int underlined = 4;
	const int flashing = 5;
	const int inversion = 7;
	const int hide = 8;

	// Перевод терминала в неканонический режим работы
	crt_t() {
		tcgetattr(0, &tty);
		savetty = tty;
		tty.c_lflag &= ~(ICANON | ECHO | ISIG);
		tty.c_cc[VMIN] = 1;
		tcsetattr (0, TCSAFLUSH, &tty);
	}

	// Инициализация вычислительного потока
	int init() {
		if (is_run != 0) {
			// Создание вычислительного потока в котором осуществен опрос клавиатуры
			is_run = pthread_create(&thread,  NULL, &run, &key);
		}
		return is_run;
	}

	// Перегруженый метод инициализации вычислительного потока
	// с установкой кода символа, завершающего сеанс работы
	int init(char c) {
		setkeyexit(c);
		return init();
	}

	// Завершение сеанса работы с терминалом, возврат в канонический режим
	// Отключаемся от потока
	void done() {
		tcsetattr (0, TCSAFLUSH, &savetty);
		pthread_detach(thread);
	}

	// Тело вычислительного потока
	static void* run(void* data) {
		t_key* key = (t_key*) data;
		while (true) { // (key->data != key->keyexit) {
			read(0, &key->data, 1);
			key->status = true;
		}
		pthread_exit(0);
	}

	// Вернуть символ нажатой клавиши
	char readkey() {
		key.status = false;
		return key.data;
	}

	// Проверить, была ли нажата клавиша
	bool keypressed() {
		return key.status;
	}
	
	// Установить код выхода
	void setkeyexit(char c) {
		key.keyexit = c;
	}

	// Вернуть код выхода
	char getkeyexit() {
		return key.keyexit;
	}

	// Очистить экран
	void cls() {
		system("clear");
	}

	// Адресация курсора
	void gotoxy(int x, int y) {
		printf("%c[%d;%df", 0x1B, y + 1, x + 1);
	}

	// Цвет текста
	void textcolor(int color) {
		if (color >= 30 && color <= 37)
			printf("%c[%dm", 0x1B, color);
	}

	// Цвет фона
	void background(int color) {
		if (color >= 30 && color <= 37)
			printf("%c[%dm", 0x1B, color + 10);
	}

	// Стиль текста
	void textstyle(int color) {
		if (color >= 0 && color <= 8)
			printf("%c[%dm", 0x1B, color);
	}

	// Блокировать поток на d милисекунд
	void delay(int d) {
		usleep(d * 1000);
	}
	
	// Вывод на печать одного символа
	void printc(char c) {
		printf("%c", c);
	}

	// Вывод на печать массива символов (C-строка)
	void prints(char* c) {
		printf("%s", c);
	}

	// Очистить буфер вывода
	void flush() {
		printf("\n");
	}
};
