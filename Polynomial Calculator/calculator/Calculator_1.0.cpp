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
	Type type; //注明存储的是符号还是数字
	float n;
	char c;
};

Suffix Suf[50]; //中缀表达式保存在结构体数组中，表达式最大长度为50
int index = 0;

//将中缀表达式转换成后缀表达式
void InfixToSuffix()
{
	ifstream in;
	in.open("D:\\calculator\\data.txt");
	char c, e;
	float n;
	cout << "The status of the operator stack:" << endl;
	STACK<char> S;
	do {
		in >> c; //读取第一个'#'
	} while (c != '#');

	do {
		in >> c;
	} while (c == ' ');
	if (c == '-' || c == '+') //可能出现开头为+或-的情况，则在最前补一操作数0
	{
		Suf[index].type = Num;
		Suf[index++].n = 0;
	}
	while (1)
	{
		if (c == '#') //读到结尾的'#'弹出所有运算符
		{
			while (!S.Empty())
			{
				e = S.Top();
				Suf[index].type = Char;
				Suf[index++].c = e;
				S.Pop();
				S.Print();
			}
			break;
		}
		if (c >= '0' && c <= '9') //读入的是数字，则将指针左移一个字节，重新以数字格式读入
		{
			in.seekg(-1, ios::cur);
			in >> n;
			Suf[index].type = Num;
			Suf[index++].n = n;
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
			}

			in >> c;
			while (c == ' ')
				in >> c;
			if (c == '(' || (c >= 'a' && c <= 'z')) //多个变量连乘，'*'缺省的情况
			{
				if (c >= 'a' && c <= 'z')
					if (variate[c - 'a'].name == '\0') //如果不在变量集中，则加入
					{
						variate[c - 'a'].name = c;
						NumVariate++;
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
				while (S.Top() != '(')
				{
					e = S.Top();
					Suf[index].type = Char;
					Suf[index++].c = e;
					S.Pop();
					S.Print();
				}
				S.Pop(); //'('出栈
				S.Print();

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
						}
					in.seekg(-1, ios::cur);
					c = '*';
					continue;
				}
				in.seekg(-1, ios::cur);
			}
			else
			{
				if (c == '(')
				{
					S.Push(c);
					S.Print();
					in >> e;
					if (e == '-' || e == '+') //可能出现'('里面紧跟正负号的情况
					{
						Suf[index].type = Num;
						Suf[index++].n = 0;
						c = e;
					}
					else
					{
						in.seekg(-1, ios::cur);
						in >> c;
						while (c == ' ')
							in >> c;
						continue; //如果情况没有出现，则将'('入栈后进行新一轮的读入
					}
				}
				if (S.Empty())
				{
					S.Push(c);
					S.Print();
				}
				else
				{
					while (!S.Prior(c)) //若待入栈的运算符优先度不大于栈顶运算符，则弹栈直到符合优先度要求为止
					{
						e = S.Top();
						Suf[index].type = Char;
						Suf[index++].c = e;
						S.Pop();
						S.Print();
						if (S.Empty())
							break;
					}
					S.Push(c);
					S.Print();
				}
			}
		}
		in >> c;
		while (c == ' ')
			in >> c;
	}
	cout << endl;
	in.close();
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
			switch (Suf[i].c) //将个别替换了的比较运算符换回原本的运算符
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

//对操作符和两个操作数进行运算
void Compute(char oper, STACK<float>& S)
{
	float operand1 = S.Top();
	S.Pop();
	S.Print();
	float operand2 = S.Top();
	S.Pop();
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
			system("pause");
			exit(0);
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
	S.Print();
}

//将变量替换成相应的值
void Substitute()
{
	cout << endl;
	float v;
	for (int i = 0; i < 26; ++i)
	{
		if (variate[i].name != '\0') //将所有加入变量符号集合中的变量赋值
		{
			cout << "Input the value of the variate " << variate[i].name << ": ";
			cin >> v;
			variate[i].value = v;
		}
	}
	for (int i = 0; i < index; ++i) //将中缀表达式中的变量替换成数字
		if (Suf[i].c >= 'a' && Suf[i].c <= 'z')
		{
			Suf[i].type = Num;
			Suf[i].n = variate[Suf[i].c - 'a'].value;
		}
}

//对中缀表达式进行计算
void CalculateSuffix()
{
	STACK<float> STK;
	ifstream in;
	if (NumVariate > 0)
		Substitute(); //将变量的值代入到表达式中

	cout << endl << "The status of the operand stack:" << endl;
	for (int i = 0; i < index; ++i)
	{
		if (Suf[i].type == Num) //遇到数字入栈
		{
			STK.Push(Suf[i].n);
			STK.Print();
		}
		else
			Compute(Suf[i].c, STK); //遇到运算符则从数字栈中取出两个操作数进行运算，并把结果压回栈中
	}
	cout << endl << "The result is: " << STK.Top() << endl;
}

int main()
{
	InfixToSuffix();
	PrintSuffix();
	WriteSuffix();
	CalculateSuffix();
	system("pause");
}