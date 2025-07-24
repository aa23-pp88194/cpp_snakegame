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
	Apple = 1
};

HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

char snake_map[MAP_SIZE_Y][MAP_SIZE_X] = {};
int maxAppleCount = 5;
int curAppleCount = 0;

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
				cout << "◇";
				break;
			case Apple:
				cout << "◎";
				break;
			}
		}
		cout << "\n";
	}
}
class Map {
private:
	char map[50][50];
public:
	void PrintMap() {
		for (int i = -1; i <= MAP_SIZE_Y; ++i) {
			for (int j = -1; j <= MAP_SIZE_X; ++j) {
				if (i == -1 || i == MAP_SIZE_Y || j == -1 || j == MAP_SIZE_X) {
					cout << "■";
					continue;
				}

				switch (map[i][j])
				{
				case None:
					cout << "◇";
					break;
				case Apple:
					cout << "◎";
					break;
				}
			}
			cout << "\n";
		}
	}
};
class Snake
{
private:
	COORD curPos;
	COORD curDir;

	int length = 1;
	deque<COORD> path;

	bool isAlive = true;

public:
	Snake(COORD startPos, COORD startDir) {
		curPos = startPos;
		curDir = startDir;
		length = 5;
		isAlive = true;
	}
	~Snake() {

	}
	bool GetAlive() { return isAlive; }
	COORD GetPos() { return curPos; }
	void SetDir(COORD dir) {
		if (dir.X == -curDir.X || dir.Y == -curDir.Y)
			return;
		curDir = dir;
	}
	void Next() {
		if (path.size() > length - 1) {
			path.pop_back();
		}
		path.push_front(curPos);
		short nextX = curPos.X + curDir.X;
		short nextY = curPos.Y + curDir.Y;
		if (nextX < 0 || nextX >= MAP_SIZE_X || nextY < 0 || nextY >= MAP_SIZE_Y)
			isAlive = false;
		for (COORD t : path) {
			if (t.X == nextX && t.Y == nextY) {
				isAlive = false;
				return;
			}
		}
		curPos = { nextX, nextY };
		if (snake_map[curPos.Y][curPos.X] == Apple) {
			snake_map[curPos.Y][curPos.Y] = None;
			GotoXY({ (short)(nextX * 2 + 2), (short)(nextY) });
			//cout << "  ";
			curAppleCount--;
			length++;
		}
	}
	void PrintSnake() {
		for (COORD t : path) {
			GotoXY({ (short)(t.X * 2 + 2), (short)(t.Y + 1) });

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

	for (int i = 0; i < 5; ++i) {
		curAppleCount++;
		snake_map[rand() % 51][rand() % 51] = Apple;
	}

	while (snake.GetAlive()) {
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

		if (curAppleCount < maxAppleCount) {
			curAppleCount++;
			snake_map[rand() % 51][rand() % 51] = Apple;
		}

		GotoXY(0, 0);
		PrintMap();
		snake.PrintSnake();
		SLP(50);
	}

	//콘솔창 일시정지 (종료 로그 안띄우게)
	system("pause>nul");
}