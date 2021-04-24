#include <iostream>
#include <fstream>
#include <ctime>
#include <Windows.h>
using namespace std;

int Move[9][2] = { {0, 0},  {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, { -1, -1}, {-1, 0}, {-1, 1} };

class Status {
public:
	int x;
	int y;
	Status* next;
	Status() :x(0), y(0), next(NULL) {};
	Status(int i, int j, int v) :x(i), y(j), next(NULL) {};
};

class STACK {
public:
	int lenth;
	Status* top; //栈顶指针
	STACK();
	void MakeNull();
	void Push(Status s);
	void Pop();
	Status Top();
	bool Empty();
};

STACK::STACK()
{
	lenth = 0;
	top = new Status;
	top->next = NULL;
}

void STACK::MakeNull()
{
	top->next = NULL;
}

void STACK::Push(Status s)
{
	Status* stk = new Status;
	*stk = s;
	stk->next = top->next;
	top->next = stk;
}

void STACK::Pop()
{
	Status* stk = top;;
	if (stk->next)
	{
		stk = top->next;
		top->next = stk->next;
		delete stk;
	}
}

Status STACK::Top()
{
	if (top->next)
		return *top->next;
	else
		return *new Status;
}

bool STACK::Empty()
{
	if (top->next)
		return false;
	else
		return true;
}

//设置迷宫
void  SetMaze(int**& map, int**& mark, int& NumRow, int& NumColumn)
{
	do {
		cout << "Input the number of the rows: ";
		cin >> NumRow;
		if (NumRow < 2)
			cout << "The number of the rows should be no less than 2!" << endl;
	} while (NumRow < 2);
	do {
		cout << "Input the number of the columns: ";
		cin >> NumColumn;
		if (NumColumn < 2)
			cout << "The number of the columns should be no less than 2!" << endl;
	} while (NumColumn < 2);
	map = new int* [NumRow + 2];
	mark = new int* [NumRow + 2];
	for (int i = 0; i < NumRow + 2; ++i)
	{
		map[i] = new int[NumColumn + 2];
		mark[i] = new int[NumColumn + 2];
	}

	//边界置为墙
	for (int i = 0; i < NumColumn + 2; ++i)
	{
		map[0][i] = 1;
		map[NumRow + 1][i] = 1;
	}
	for (int i = 0; i < NumRow; ++i)
	{
		map[i][0] = 1;
		map[i][NumColumn + 1] = 1;
	}
	for (int i = 0; i < NumRow + 2; ++i)
		for (int j = 0; j < NumColumn + 2; ++j)
			mark[i][j] = 0;

	cout << "1. Generate manually" << endl
		<< "2. Generate randomly" << endl
		<< "Choose the mode to generate the maze: ";
	int choice;
	cin >> choice;
	if (choice == 1) //手动生成迷宫
	{
		system("cls");
		cout << "Input the map of the maze:" << endl;
		for (int i = 1; i < NumRow + 1; ++i)
			for (int j = 1; j < NumColumn + 1; ++j)
				cin >> map[i][j];
	}
	else //自动生成迷宫
	{
		system("cls");
		int proportion;
		cout << "Input the percentage of the walls (0 - 100): "; //输入墙所占百分比
		cin >> proportion;
		srand((int)time(0));
		for (int i = 1; i < NumRow + 1; ++i)
			for (int j = 1; j < NumColumn + 1; ++j)
			{
				if ((rand() % 100) < proportion)
					map[i][j] = 1;
				else
					map[i][j] = 0;
			}
		map[1][1] = 0;
		map[NumRow][NumColumn] = 0;
	}
	system("cls");
	cout << "The generated maze is:" << endl;
	for (int i = 1; i < NumRow + 1; ++i)
	{
		for (int j = 1; j < NumColumn + 1; ++j)
			cout << map[i][j] << " ";
		cout << endl;
	}
	cout << "Do you want to figure out this maze?(Y/N): ";
	char input;
	cin >> input;
	if (input == 'Y' || input == 'y')
		return;
	else
		exit(0);
}

STACK S, temp, Shortest; //shortest栈结构用于保存最短路径

int counter = 0;

//输出所有路径和最短路径
void GetMaze(int** map, int** mark, int NumRow, int NumColumn, int x, int y)
{
	int i = x, j = y, m = NumRow, n = NumColumn;
	int g, h;
	mark[1][1] = 1;
	Status sta;
	if (i == m && j == n) //判定是否走到出口
	{
		cout << "Routine No." << ++counter << ":" << endl;
		Status* p = S.top->next;
		while (p != NULL)
		{
			Status s = *p;
			temp.Push(s);
			p = p->next;
		}
		if (S.lenth < Shortest.lenth || Shortest.lenth == 0) //如果新发现的路径比当前最短路径短，则将最短路径更新为当前路径
		{
			Shortest.MakeNull();
			p = S.top->next;
			Shortest.lenth = S.lenth;
			while (p != NULL)
			{
				Status s = *p;
				Shortest.Push(s);
				p = p->next;
			}
		}

		cout << "(1, 1)";
		while (!temp.Empty())
		{
			Status s;
			s = temp.Top();
			temp.Pop();
			cout << "->(" << s.x << ", " << s.y << ")";
		}
		cout << endl;
		return;
	}

	if (i == 0 || j == 0 || i == NumRow + 1 || j == NumColumn + 1) //边界检测
		return;

	for (int k = 1; k <= 8; ++k) //从1号方向（正东）开始遍历所有方向
	{
		g = i + Move[k][0];
		h = j + Move[k][1];
		if (map[g][h] == 0 && mark[g][h] == 0) //如果不是墙且未到过此位置，则前进一步
		{
			mark[g][h] = 1;
			sta.x = g;
			sta.y = h;
			S.Push(sta);
			S.lenth++;

			GetMaze(map, mark, NumRow, NumColumn, g, h); //以此位置为起点开始向前递归

			//回溯到上一位置，继续遍历所有方向
			mark[g][h] = 0;
			S.Pop();
			S.lenth--;
		}
	}
}

//打印所有路径和最短路径，并且将最短路径写入到文件中
void PrintShortest(int** map, int NumRow, int NumColumn)
{
	ofstream out;
	out.open("D:\\Maze\\result2.txt", ios::app);
	if (Shortest.lenth == 0) //如果没有最短路径，证明未找到任何一条路径
	{
		cout << "The path was not found!" << endl;
		return;
	}
	//在文件中写入迷宫地图
	for (int i = 1; i < NumRow + 1; ++i)
	{
		for (int j = 1; j < NumColumn + 1; ++j)
			out << map[i][j] << " ";
		out << endl;
	}
	//如果找到了，则打印最短路径
	cout << "The shortest path is:" << endl << "(1, 1)";
	out << "The shortest path is:" << endl << "(1, 1)";
	while (!Shortest.Empty())
	{
		Status s;
		s = Shortest.Top();
		Shortest.Pop();
		cout << "->(" << s.x << ", " << s.y << ")";
		out << "->(" << s.x << ", " << s.y << ")";
	}
	cout << endl;
	out << endl << endl;
}

int main()
{
	int** map = NULL, ** mark = NULL;
	int NumRow, NumColumn;
	SetMaze(map, mark, NumRow, NumColumn);
	GetMaze(map, mark, NumRow, NumColumn, 1, 1);
	PrintShortest(map, NumRow, NumColumn);
	system("pause");
}

/*
0 1 1 1 1
1 0 1 1 1
1 1 0 1 1
1 1 1 0 1
1 1 1 1 0

0 0 1 1 1
1 1 0 1 1
1 0 1 0 1
0 1 1 1 0
1 0 0 0 0
*/