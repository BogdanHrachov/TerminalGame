#include <iostream>
#include "labirint.cpp"

using namespace std;

crt      trm;

char**   level;
char     kod;

int result = 0; // Счет игры
int life   = 3; // Жизни

class person {
	protected:
	  int x, y;
	  int vx, vy;
 	  int x0, y0; // Начальные координаты персонажа
	public:
	  person() {
	  }
	  
	  person(int x, int y) : x(x), y(y) {
		  x0 = x;
		  y0 = y;
		  vx = 0;
		  vy = 0;
	  }
	  
	  person(int x, int y, int vx, int vy) : x(x), y(y), vx(vx), vy(vy) {
	  }
	  
	  void change(int vx_new, int vy_new) {
		  vx = vx_new;
		  vy = vy_new;
	  }
	  
	  int get_location() {
		  return y * 100 + x;
	  }
	  
	  virtual bool calc() = 0; // Расчитывать направление движения персонажа
	  virtual void test() = 0; // Проверка возможности движения по направлению
	  virtual void hide() = 0;
	  virtual void show() = 0;
	  
	  void begin() { // Сброс на начальные позиции
		  hide();
		  x = x0;
		  y = y0;
		  vx = vy = 0;
		  show();
	  }
	  
	  void move() { // Перемещение персонажа
		  calc();
		  test();
		  hide();
		  x += vx;
		  y += vy;
		  show();
	  }
};

person** prs;

char lowchar(char c) { // Переводим символ в нижний регистр
	if (c >= 'A' && c <= 'Z')
	  return c + 32;
	return c;
}

class bot : public person {
	private:
	  char face = '@';
	public:
	  bot(int x, int y) : person(x, y) {
	  }
	  bool calc() override {
		  int xh = prs[0] -> get_location() % 100;
		  int yh = prs[0] -> get_location() / 100;
		  
		  double m = 0;
		  
		  if (vx == 0) {
			  if (x < xh) {
				  m = 0.7;
			  }
			  else {
				  m = 0.3;
			  }
			  if ((double) rand() / RAND_MAX < m) {
				  if (level[y][x + 1] != '#') {
					  change(+1, +0);
					  return true;
				  }
			  }
			  if ((double) rand() / RAND_MAX > m) {
			      if (level[y][x - 1] != '#') {
					  change(-1, +0);
					  return true;
				  }
			  }
		  }
		  if (vy == 0) {
			  if (y < yh) {
				  m = 0.7;
			  }
		      else {
			      m = 0.3;
		      }
			  if ((double) rand() / RAND_MAX < m) {
				  if (level[y + 1][x] != '#') {
					  change(+0, +1);
					  return true;
				  }
			  }
			  if ((double) rand() / RAND_MAX > m) {
				  if (level[y - 1][x] != '#') {
					  change(+0, -1);
					  return true;
				  }
			  }
		  }
		  return false;
	  }
	  void test() override {
		  // Тест на контакт с главным героем
		  if (get_location() == prs[0] -> get_location()) {
			  // Все персонажи на исходную позицию
			  for (int i = 1; prs[i] != NULL; ++i) {
				  prs[i] -> begin();
			  }
			  prs[0] -> begin(); // Главный герой на исходную позицию
			  trm.gotoxy(0, 22);
			  fflush(stdout);
			  sleep(1);
			  --life;
			  return;
		  }
		  // Тест на удар о стенку
		  if (level[y + vy][x + vx] == '#') {
			  if ((double) rand() / RAND_MAX > 0.4) {
				  if (calc()) {
					  return;
				  }
			  }
			  vx = -vx;
			  vy = -vy;
		  }
	  }
	  void hide() override {
		  trm.gotoxy(x, y);
		  trm.print(' ');
	  }
	  void show() override {
		  trm.gotoxy(x, y);
		  trm.print(face);
	  }
};

class heroy : public person {
	public:
	  heroy(int x, int y) : person(x, y) {
	  }
	  bool calc() override {
		  if (true || trm.pressedkey()) { // Отключаем проверку нажатия клавиши, всегда читаем код последней нажатой клавиши
			  switch (kod = lowchar(trm.readkey())) {
				  case 'w': if (level[y - 1][x] != '#')
				              change(+0, -1);
				            break;
				  case 'a': if (level[y][x - 1] != '#')
				              change(-1, +0);
				            break;
				  case 's': if (level[y + 1][x] != '#')
				              change(+0, +1);
				            break;
				  case 'd': if (level[y][x + 1] != '#')
				              change(+1, +0);
				            break;
			  }
		  }
		  return true;
	  }
	  void test() override {
		  if (level[y + vy][x + vx] == '#') {
			  change(+0, +0);
		  }
	  }
	  void hide() override {
		  trm.gotoxy(x, y);
		  trm.print(' ');
	  }
	  void show() override {
		  trm.gotoxy(x, y);
		  trm.print('&');
	  }
};

class clad : public person {
	private:
	  char face = '$';
	  int  ball =  10;
	public:
	  clad(int x, int y) : person(x, y) {
	  }
	  bool calc() override {
		  return true;
  	  }
	  void test() override {
		  if (get_location() == prs[0] -> get_location()) {
			  face = ' ';
			  result += ball;
			  ball = 0;
		  }
 	  }
	  void hide() override {
		  trm.gotoxy(x, y);
		  trm.print(' ');
	  }
	  void show() override {
		  trm.gotoxy(x, y);
		  trm.print(face);
	  }
};

void read_level(char** level, char* file_name) {
	int n = 0;
	FILE* F = fopen(file_name, "r");
	while (fgets(level[n++], 100, F));
	level[n] = NULL;
	fclose(F);
}

void show_level(char** level) {
	int n = 1;
	trm.clear();
	for (int y = 0; level[y] != NULL; ++y) {
		for (int x = 0; level[y][x] != '\0'; ++x) {
			switch (level[y][x]) {
				case '&': prs[0]   = new heroy(x, y);
				          break;
				case '$': prs[n++] = new clad(x, y);
				          break;
				case '@': prs[n++] = new bot(x, y);
				          break;
			}
			trm.print(level[y][x]);
		}
	}
	prs[n] = NULL;
}

int main() {
	// Карта игры
	level = new char*[26];
	for (int i = 0; i < 26; ++i) {
		level[i] = new char[100];
	}
	
	char fn[] = "pm01.dat";
	
    read_level(level, fn);

    prs = new person*[100];
    
    show_level(level);
    
    // Запуск игрового цикла
    do {
        for (int i = 0; prs[i] != NULL; ++i) {
			prs[i] -> move();
		}
		trm.gotoxy(0, 22);
		printf(" Счет: %d, Жизней: %d", result, life);
		trm.gotoxy(0, 22);
		fflush(stdout);
		usleep(50000);

	} while (kod != 'q' && life > 0);
}
