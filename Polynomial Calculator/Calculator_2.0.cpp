#include <iostream>
#include<fstream>
using namespace std;

int NumVariate = 0;
struct Variate {
	char name;
	float value;
};
Variate variate[26];

template <class T>
class Node {
public:
	T val;
	Node* next;
	Node() :next(NULL) {};
	Node(char c) :val(c), next(NULL) {};
};

template <class T>
class STACK : public Node<T> {
private:
	Node<T>* top; //栈顶指针
public:
	STACK();
	void MakeNull();
	void Push(T c);
	void Pop();
	T Top();
	bool Empty();
	bool Prior(char c);
	void Print();
};

template <class T>
STACK<T>::STACK()
{
	top = new Node<T>;
	top->next = NULL;
}

template <class T>
void STACK<T>::MakeNull()
{
	top->next = NULL;
}

template <class T>
void STACK<T>::Push(T c)
{
	Node<T>* stk = new Node<T>;
	stk->val = c;
	stk->next = top->next;
	top->next = stk;
}

template <class T>
void STACK<T>::Pop()
{
	Node<T>* stk = top;;
	if (stk->next)
	{
		stk = top->next;
		top->next = stk->next;
		delete stk;
	}
}

template <class T>
T STACK<T>::Top()
{
	if (top->next)
		return top->next->val;
	else
		return '@';
}

template <class T>
bool STACK<T>::Empty()
{
	if (top->next)
		return false;
	else
		return true;
}

//将每种运算符的优先级用数字表示，比较优先级即比较对应数字的大小
template <class T>
bool STACK<T>::Prior(char c)
{
	if (top->next == NULL)
		return true;
	int priority1 = 0, priority2 = 0;
	//`、~、@、$分别代表>=、<=、==、!=
	if (c == '>' || c == '<' || c == '`' || c == '~' || c == '@' || c == '$')
		priority1 = 1;
	else if (c == '+' || c == '-')
		priority1 = 2;
	else if (c == '*' || c == '/')
		priority1 = 3;
	else if (c == '^')
		priority1 = 4;
	else if (c == '(')
		priority1 = 5;

	if (top->next->val == '(')
		priority2 = 0;
	else if (top->next->val == '>' || top->next->val == '<' || top->next->val == '`' || top->next->val == '~' || top->next->val == '@' || top->next->val == '$')
		priority2 = 1;
	else if (top->next->val == '+' || top->next->val == '-')
		priority2 = 2;
	else if (top->next->val == '*' || top->next->val == '/')
		priority2 = 3;
	else if (top->next->val == '^')
		priority2 = 4;

	return priority1 > priority2;
}

//打印栈的状态
template <class T>
void STACK<T>::Print()
{
	if (Empty())
	{
		cout << "Empty." << endl;
		return;
	}
	Node<T>* p = top;
	while (p->next != NULL)
	{
		cout << p->next->val << " ";
		p = p->next;
	}
	cout << endl;
}

enum Type { Char, Num };
struct Suffix {
	Type type;
	float n;
	char c;
};

Suffix Suf[50]; //中缀表达式保存在结构体数组中，表达式最大长度为50
int index = 0;

//对操作符和两个操作数进行运算
void Compute(char oper, STACK<float>& S)
{
	float operand1 = S.Top();
	S.Pop();
	cout << "The status of the operand stack:" << endl;
	S.Print();
	float operand2 = S.Top();
	S.Pop();
	cout << "The status of the operand stack:" << endl;
	S.Print();
	switch (oper)
	{
	case '+': S.Push(operand2 + operand1); break;
	case '-': S.Push(operand2 - operand1); break;
	case '*': S.Push(operand2 * operand1); break;
	case '/':
		if (operand1 == 0)
		{
			cout << "Divided by 0!" << endl;
			return;
		}
		S.Push(operand2 / operand1); break;
	case '^': S.Push(pow(operand2, operand1)); break;
	case '>': S.Push(operand2 > operand1); break;
	case '<': S.Push(operand2 < operand1); break;
	case '`': S.Push(operand2 >= operand1); break;
	case '~': S.Push(operand2 <= operand1); break;
	case '@': S.Push(abs(operand2 - operand1) < 0.00001); break;
	case '$': S.Push(operand2 != operand1); break;
	default:
		break;
	}
	cout << "The status of the operand stack:" << endl;
	S.Print();
}

//将中缀表达式转换成后缀表达式的同时进行计算
void InfixToSuffix()
{
	ifstream in;
	in.open("D:\\calculator\\data.txt");
	char c, e;
	float n;
	STACK<char> operS;
	STACK<float> numS;
	in >> c; //读取第一个'#'
	in >> c;
	while (c == ' ')
		in >> c;
	if (c == '-' || c == '+') //可能出现开头为+或-的情况，则在最前补一操作数0
	{
		Suf[index].type = Num;
		Suf[index++].n = 0;
		numS.Push(0);
		cout << "The status of the operand stack:" << endl;
		numS.Print();
	}
	while (1)
	{
		if (c == '#') //读到结尾的'#'弹出所有运算符
		{
			while (!operS.Empty())
			{
				e = operS.Top();
				Suf[index].type = Char;
				Suf[index++].c = e;
				Compute(e, numS);
				operS.Pop();
				cout << "The status of the operator stack:" << endl;
				operS.Print();
			}
			break;
		}
		if (c >= '0' && c <= '9') //读入的是数字，则将指针左移一个字节，重新以数字格式读入
		{
			in.seekg(-1, ios::cur);
			in >> n;
			Suf[index].type = Num;
			Suf[index++].n = n;
			numS.Push(n);
			cout << "The status of the operand stack:" << endl;
			numS.Print();
			in >> c;
			while (c == ' ')
				in >> c;
			if (c == '(' || (c >= 'a' && c <= 'z')) //如果读入了'('或变量（小写字母），此时为系数后缺省'*'的情况
			{
				if (c >= 'a' && c <= 'z')
					if (variate[c - 'a'].name == '\0') //如果不在变量集中，则加入
					{
						variate[c - 'a'].name = c;
						NumVariate++;
						cout << endl << "Input the value of " << c << ": ";
						cin >> variate[c - 'a'].value;
					}
				in.seekg(-1, ios::cur);
				c = '*';
				continue;
			}
			in.seekg(-1, ios::cur);
		}
		else if (c >= 'a' && c <= 'z') //读入的是变量（小写字母）
		{
			Suf[index].type = Char;
			Suf[index++].c = c;
			if (variate[c - 'a'].name == '\0') //如果不在变量集中，则加入
			{
				variate[c - 'a'].name = c;
				NumVariate++;
				cout << endl << "Input the value of " << c << ": ";
				cin >> variate[c - 'a'].value;
			}
			numS.Push(variate[c - 'a'].value);
			cout << "The status of the operand stack:" << endl;
			numS.Print();
			in >> c;
			while (c == ' ')
				in >> c;
			if (c == '(' || (c >= 'a' && c <= 'z')) //多个变量连乘，'*'缺省的情况
			{
				if (c >= 'x' && c <= 'z')
					if (variate[c - 'a'].name == '\0') //如果不在变量集中，则加入
					{
						variate[c - 'a'].name = c;
						NumVariate++;
						cout << endl <<"Input the value of " << c << ": ";
						cin >> variate[c - 'a'].value;
					}
				in.seekg(-1, ios::cur);
				c = '*';
				continue;
			}
			in.seekg(-1, ios::cur);
		}
		else //读入的是运算符
		{
			if (c == '<' || c == '>' || c == '=' || c == '!') //读取的是比较运算符
			{
				char d;
				in >> d;
				if (d != '=')
					in.seekg(-1, ios::cur);
				else
				{
					switch (c) //由于个别比较运算符（>=、<=、==、!=）占两个字符，故用一个特殊字符代替
					{
					case '>': c = '`'; break;
					case '<': c = '~'; break;
					case '=': c = '@'; break;
					case '!': c = '$'; break;
					default:
						break;
					}
				}
			}
			if (c == ')') //读到')'弹出第一个'('前的所有运算符
			{
				while (operS.Top() != '(')
				{
					e = operS.Top();
					Suf[index].type = Char;
					Suf[index++].c = e;
					Compute(e, numS);
					operS.Pop();
					cout << "The status of the operator stack:" << endl;
					operS.Print();
				}
				operS.Pop(); //'('出栈
				cout << "The status of the operator stack:" << endl;
				operS.Print();

				in >> c;
				while (c == ' ')
					in >> c;
				if (c == '(' || (c >= 'a' && c <= 'z')) //')'后读到'('或变量时'*'缺省的情况
				{
					if (c >= 'a' && c <= 'z')
						if (variate[c - 'a'].name == '\0') //如果不在变量集中，则加入
						{
							variate[c - 'a'].name = c;
							NumVariate++;
							cout << endl << "Input the value of " << c << ": ";
							cin >> variate[c - 'a'].value;
						}
					in.seekg(-1, ios::cur);
					c = '*';
					continue; //如果情况没有出现，则将'('入栈后进行新一轮的读入
				}
				in.seekg(-1, ios::cur);
			}
			else
			{
				if (c == '(') //可能出现的前括号后面紧跟负数的情况
				{
					operS.Push(c);
					cout << "The status of the operator stack:" << endl;
					operS.Print();
					in >> e;
					if (e == '-' || e == '+')
					{
						Suf[index].type = Num;
						Suf[index++].n = 0;
						numS.Push(0);
						cout << "The status of the operand stack:" << endl;
						numS.Print();
						c = e;
					}
					else
					{
						in.seekg(-1, ios::cur);
						in >> c;
						while (c == ' ')
							in >> c;
						continue;
					}
				}
				if (operS.Empty())
				{
					operS.Push(c);
					cout << "The status of the operator stack:" << endl;
					operS.Print();
				}
				else
				{
					while (!operS.Prior(c)) //若待入栈的运算符优先度不大于栈顶运算符，则弹栈直到符合优先度要求为止
					{
						e = operS.Top();
						Suf[index].type = Char;
						Suf[index++].c = e;
						Compute(e, numS); //弹出运算符的同时从数字栈中取两个操作数进行计算，并将所得结果压回栈中
						operS.Pop();
						cout << "The status of the operator stack:" << endl;
						operS.Print();
						if (operS.Empty())
							break;
					}
					operS.Push(c);
					cout << "The status of the operator stack:" << endl;
					operS.Print();
				}
			}
		}
		in >> c;
		while (c == ' ')
			in >> c;
	}
	cout << endl << "The result is: " << numS.Top() << endl;
}

//打印获得的中缀表达式
void PrintSuffix()
{
	cout << endl << "The suffix expression is:" << endl;
	for (int i = 0; i < index; ++i)
	{
		if (Suf[i].type == Num)
			cout << Suf[i].n << " ";
		else
			switch (Suf[i].c)
			{
			case '`':
				cout << ">= ";
				break;
			case '~':
				cout << "<= ";
				break;
			case '@':
				cout << "== ";
				break;
			case '$':
				cout << "!= ";
				break;
			default:
				cout << Suf[i].c << " ";
				break;
			}
	}
	cout << endl;
}

//将中缀表达式结果保存到文件中
void WriteSuffix()
{
	ifstream in;
	in.open("D:\\calculator\\data.txt");
	ofstream out;
	out.open("D:\\calculator\\suffix.txt", ios::app);
	char c;

	out << "Inffix:" << endl;
	do {
		in >> c;
	} while (c != '#');
	out << c;
	do {
		do {
			in >> c;
		} while (c == ' ');
		out << c;
	} while (c != '#');
	out << endl;

	out << "Suffix:" << endl;
	for (int i = 0; i < index; ++i)
	{
		if (Suf[i].type == Num)
			out << Suf[i].n << " ";
		else
			switch (Suf[i].c) //将个别替换了的比较运算符换回原本的运算符
			{
			case '`':
				out << ">= ";
				break;
			case '~':
				out << "<= ";
				break;
			case '@':
				out << "== ";
				break;
			case '$':
				out << "!= ";
				break;
			default:
				out << Suf[i].c << " ";
				break;
			}
	}
	out << endl << endl;

	in.close();
	out.close();
}

int main()
{
	InfixToSuffix();
	PrintSuffix();
	WriteSuffix();
	system("pause");
}