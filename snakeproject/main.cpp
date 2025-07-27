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
using namespace std;

enum tileType {
	None = 0,
	Apple = 1
};

class Console {
private:
	HANDLE h_stdConsole;
	HANDLE h_console[2] = {};
	bool screenIndex = false;
	CONSOLE_SCREEN_BUFFER_INFOEX _consoleInfo = {};
	CONSOLE_CURSOR_INFO _cursorInfo = {};

	void Init(int fontSize, COORD consoleSize) {
		h_stdConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_FONT_INFOEX _fontInfo;
		_fontInfo.cbSize = sizeof(_fontInfo);
		_fontInfo.nFont = 0;
		_fontInfo.dwFontSize.X = fontSize;
		_fontInfo.dwFontSize.Y = fontSize;
		SetCurrentConsoleFontEx(h_stdConsole, FALSE, &_fontInfo);

		screenIndex = false;

		//CursorInfo Setting
		_cursorInfo.bVisible = false;
		_cursorInfo.dwSize = 1;
		//ConsoleInfo Setting
		ZeroMemory(&_consoleInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));
		_consoleInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(h_stdConsole,&_consoleInfo);
		_consoleInfo.srWindow.Left = 0;
		_consoleInfo.srWindow.Top = 0;
		_consoleInfo.srWindow.Right = consoleSize.X - 1;
		_consoleInfo.srWindow.Bottom = consoleSize.Y - 1;
		SetConsoleWindowInfo(h_stdConsole, true, &_consoleInfo.srWindow);

		for (int i = 0; i < 2; ++i) {
			h_console[i] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
			SetConsoleCursorInfo(h_console[i], &_cursorInfo);
			SetConsoleScreenBufferInfoEx(h_console[i], &_consoleInfo);
		}
	}

	HANDLE CurConsole() {
		return h_console[screenIndex?1:0];
	}
	void Flip() {
		SetConsoleActiveScreenBuffer(CurConsole());
		screenIndex = !screenIndex;
	}
public:
	Console(int fontSize, COORD consoleSize) {
		Init(fontSize, consoleSize);
	}
	Console() : Console(14, { 120, 54 }) {}
	~Console() {
		for(int i = 0; i < 2; ++i)
			CloseHandle(h_console[i]);
		SetConsoleActiveScreenBuffer(h_stdConsole);
	}
	void Print(string s, COORD pos) {
		DWORD dw;
		SetConsoleCursorPosition(CurConsole(), pos);
		WriteFile(CurConsole(), s.c_str(), strlen(s.c_str()), &dw, NULL);
	}
	void Print(string s) {
		Print(s, { 0, 0 });
	}
	void Clear() {
		FillConsoleOutputCharacter();
	}
	void Render() {
		Flip();
	}
};
class Map {
private:
	int mapSizeX;
	int mapSizeY;
	char map[50][50] = {};

	void SetApple() {
		int randX;
		int randY;
		do {
			randX = rand() % 50;
			randY = rand() % 50;
		} while (map[randY][randX] == Apple);
		map[randY][randX] = Apple;
	}
public:
	Map() {
		mapSizeX = 50;
		mapSizeY = 50;
		for (int i = 0; i < 20; ++i) {
			SetApple();
		}
	}
	bool isValidCoord(COORD pos) {
		return (pos.X >= 0 && pos.X < mapSizeX && pos.Y >= 0 && pos.Y < mapSizeY);
	}
	char GetTile(COORD pos) {
		return map[pos.Y][pos.X];
	}
	void PrintMap(Console console) {
		for (int i = -1; i <= mapSizeY; ++i) {
			for (int j = -1; j <= mapSizeX; ++j) {
				COORD printPos = {(j + 1) * 2, i + 1};
				if (i == -1 || i == mapSizeY || j == -1 || j == mapSizeX) {
					console.Print("■", printPos);
					continue;
				}

				switch (map[i][j])
				{
				case None:
					console.Print("  ", printPos);
					break;
				case Apple:
					console.Print("◎", printPos);
					break;
				}
			}
		}
	}
	void Eat(COORD pos) {
		map[pos.Y][pos.X] = None;
		SetApple();
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
	void Next(Map &map) {
		if (map.GetTile(curPos) == Apple) {
			map.Eat(curPos);
			length++;
		}
		if (path.size() > length - 1) {
			path.pop_back();
		}
		path.push_front(curPos);
		short nextX = curPos.X + curDir.X;
		short nextY = curPos.Y + curDir.Y;
		if (!map.isValidCoord(curPos))
			isAlive = false;
		for (COORD t : path) {
			if (t.X == nextX && t.Y == nextY) {
				isAlive = false;
				return;
			}
		}
		curPos = { nextX, nextY };
	}
	void PrintSnake(Console console) {
		for (COORD t : path) {
			COORD pos = { (short)(t.X * 2 + 2), (short)(t.Y + 1) };

			if (t.X == path.front().X && t.Y == path.front().Y)
				console.Print("★", pos);
			else
				console.Print("☆", pos);
		}
	}
};

int main() {
	//INIT
	srand(time(NULL));
	/*system("mode con cols=120 lines=54 | title snake_game");
	CONSOLE_CURSOR_INFO cursorInfo = {};
	cursorInfo.bVisible = false;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(std_output_handle, &cursorInfo);*/
	Console console = Console(12, {120, 54});

	bool isAppRunning = true;

	Map map = Map();
	Snake snake = Snake({ 25, 25 }, { 1, 0 });

	while (snake.GetAlive()) {
		snake.Next(map);

		map.PrintMap(console);
		snake.PrintSnake(console);
		console.Render();
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
		SLP(10);
	}


	//콘솔창 일시정지 (종료 로그 안띄우게)
	system("pause>nul");
}