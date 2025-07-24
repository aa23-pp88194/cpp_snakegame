#pragma warning(disable: 4996)
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <Windows.h>
#include <conio.h>
#include <time.h>

#define SLP(X) Sleep(X)
#define CLS system("cls")
#define MAP_SIZE_X 50
#define MAP_SIZE_Y 50
using namespace std;

enum tileType {
	None = 0,
	Wall = 1
};

char snake_map[MAP_SIZE_Y][MAP_SIZE_X] = {};
HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

void GotoXY(COORD pos) {
	SetConsoleCursorPosition(std_output_handle, pos);
}
void GotoXY(short x, short y) {
	GotoXY({ x, y });
}

void PrintMap() {
	for (int i = -1; i <= MAP_SIZE_Y; ++i) {
		for (int j = -1; j <= MAP_SIZE_X; ++j) {
			if (i == -1 || i == MAP_SIZE_Y || j == -1 || j == MAP_SIZE_X) {
				cout << "■";
				continue;
			}

			switch (snake_map[i][j])
			{
			case None:
				cout << "  ";
				break;
			}
		}
		cout << "\n";
	}
}

class Snake
{
private:
	COORD curPos;
	COORD curDir;

	int length = 1;
	deque<COORD> path;

public:
	Snake(COORD startPos, COORD startDir) {
		curPos = startPos;
		curDir = startDir;
		length = 5;
	}
	~Snake() {

	}
	COORD GetPos() { return curPos; }
	void SetDir(COORD dir) {
		curDir = dir;
	}
	void Next() {
		if (path.size() > length - 1) {
			path.pop_back();
		}
		path.push_front(curPos);
		short nextX = curPos.X + curDir.X;
		short nextY = curPos.Y + curDir.Y;
		curPos = { nextX, nextY };

	}
	void PrintSnake() {
		for (COORD t : path) {
			GotoXY({ (short)(t.X * 2), (short)(t.Y) });

			cout << "★";
		}
	}
};

int main() {
	//INIT
	srand(time(NULL));
	system("mode con cols=120 lines=54 | title snake_game");
	CONSOLE_CURSOR_INFO cursorInfo = {};
	cursorInfo.bVisible = false;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(std_output_handle, &cursorInfo);

	bool isAppRunning = true;

	Snake snake = Snake({ 25, 25 }, { 1, 0 });

	while (isAppRunning) {
		//키입력
		if (_kbhit()) {
			char key = _getch();

			switch (key)
			{
			case 'W':
			case 'w':
				snake.SetDir({ 0, -1 });
				break;
			case 'S':
			case 's':
				snake.SetDir({ 0, 1 });
				break;
			case 'A':
			case 'a':
				snake.SetDir({ -1, 0 });
				break;
			case 'D':
			case 'd':
				snake.SetDir({ 1, 0 });
				break;
			}
		}
		snake.Next();

		GotoXY(0, 0);
		PrintMap();
		snake.PrintSnake();
		SLP(100);
	}

	//콘솔창 일시정지 (종료 로그 안띄우게)
	system("pause>nul");
}