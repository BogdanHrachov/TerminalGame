#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int is_run = 0;

struct t_key {
  char ckey;
  bool skey;
} key;

class crt {
	private:
		struct termios tty, savetty;
		pthread_t thread;
	public:
	  crt() {
		tcgetattr(0, &tty);
		savetty = tty;
		tty.c_lflag &= ~(ICANON | ECHO | ISIG);
		tty.c_cc[VMIN] = 1;
		tcsetattr(0, TCSAFLUSH, &tty);
		
		is_run = pthread_create(&thread,  NULL, &run, &key);
	  }
	  ~crt() {
  		tcsetattr(0, TCSAFLUSH, &savetty);
	  }
	  // Очистить экран
	  void clear() {
		  system("clear");
	  }
	  // Управление положением курсора в окне
	  void gotoxy(int x, int y) {
		  printf("%c[%d;%df", 0x1B, y + 1, x + 1);
	  }
	  // Вывод символа
	  void print(char c) {
		  printf("%c", c);
		  fflush(stdout);
	  }
	  // Тело вычислительного потока в котором происходит опрос клавиатуры
	  static void* run(void* key) {
		  while (true) {
			read(0, &((t_key*) key) -> ckey, 1);
			((t_key*) key) -> skey = true;
		  }
		  return key;
		  // Завершение потока (никогда не будет вызвана)
  		  pthread_exit(0);
	  }
	  // Чтение кода нажатой клавиши
	  char readkey() {
		  key.skey = false;
		  return key.ckey;
	  }
	  bool pressedkey() {
		  return key.skey;
	  }
};

