#include <iostream>
#include <Windows.h>
#include <stack>
#include <queue>
#include <fstream>
using namespace std;
#define FILE_NAME_DATA "D:\\Graph\\data.txt"

enum Status { VISITED, UNVISITED }; //标记结点是否访问过的枚举变量

enum Storage { LI, MA }; //标记当前使用的是邻接表还是邻接矩阵

typedef char VertextData; //图顶点包含的信息为一个字符

//邻接表的边结点
typedef struct edgenode {
	int adjvex;
	struct edgenode* next = NULL;
} EdgeNode;

//顶点结点
typedef struct vertexnode {
	int index;
	VertextData info;
	EdgeNode* firstedge = NULL;
	Status s = UNVISITED;
} VertexNode;

//深搜或广搜生成树的树结点
typedef struct treenode {
	vertexnode node;
	struct treenode* mostleftchild = NULL;
	struct treenode* nextbrother = NULL;
} TreeNode;

//图类
class AdjGraph {
public:
	VertexNode* verlist; //图的顶点列表
	int NumVerticies, NumEdges; //顶点数和边数
	int** Matrix; //邻接矩阵
	TreeNode** DFS_Root; //深搜树的根结点
	TreeNode** BFS_Root; //广搜树的根结点
	int Num_DFS_Root; //深搜生成森林中树的数量
	int Num_BFS_Root; //广搜生成森林中树的数量
	Storage storage; //存储方式
	AdjGraph(); //构造函数
	void SetUpLink(); //构造邻接表
	void SetUpMatrix(); //构造邻接矩阵
	void ShowLink(); //打印邻接表
	void ShowMatrix(); //打印邻接矩阵
	void LinkToMatrix(); //邻接表转邻接矩阵
	void MatrixToLink(); //邻接矩阵转邻接表
	void DFS(); //深度优先搜索
	void Recur_DFS_Link(int index, TreeNode*& n); //递归实现根据邻接表深搜
	void Recur_DFS_Matrix(int index, TreeNode*& n); //递归实现根据邻接矩阵深搜
	int GetNode_Matrix(int index); //从邻接矩阵中寻找下一个邻接点
	void NonRecur_DFS_Link(int index, TreeNode*& n); //非递归实现根据邻接表深搜
	void NonRecur_DFS_Matrix(int index, TreeNode*& n); //非递归实现根据邻接矩阵深搜
	void BFS(); //广度优先搜索
	void BFS_Link(int index, TreeNode*& n); //根据邻接表广搜
	void BFS_Matrix(int index, TreeNode*& n); //根据邻接矩阵广搜
	void Show_DFS_Result(); //显示深搜结果
	void Traverse_DFS_Tree(TreeNode* n); //递归地遍历深搜树
	void Show_BFS_Result(); //显示广搜结果
	void ShowSequence(); //打印遍历序列
};

//构造函数
AdjGraph::AdjGraph()
{
	//从文件中读入顶点数、边数、顶点信息、边信息
	ifstream in;
	in.open(FILE_NAME_DATA);
	in >> NumVerticies;
	while (NumVerticies <= 0)
	{
		cout << "The number of the vertexes should be more than 0!";
		system("pause");
		exit(0);
	}

	in >> NumEdges;
	if (NumEdges < 0)
	{
		cout << "The number of the edges should be more than 0!";
		system("pause");
		exit(0);
	}
	else if (NumEdges < 0 || NumEdges > NumVerticies * (NumVerticies - 1) / 2)
	{
		cout << "The number of the edges should be no more than " << NumVerticies * (NumVerticies - 1) / 2 << "!" << endl;
		system("pause");
		exit(0);
	}

	//构造顶点列表
	verlist = new VertexNode[NumVerticies];
	for (int i = 0; i < NumVerticies; ++i)
	{
		in >> verlist[i].info;
		verlist[i].index = i;
	}

	//邻接矩阵
	Matrix = new int* [NumVerticies];
	for (int i = 0; i < NumVerticies; ++i)
		Matrix[i] = new int[NumVerticies];
	for (int i = 0; i < NumVerticies; ++i)
		for (int j = 0; j < NumVerticies; ++j)
			Matrix[i][j] = 0;

	//深搜森林、广搜森林的根节点列表
	DFS_Root = new TreeNode * [NumVerticies];
	BFS_Root = new TreeNode * [NumVerticies];
	for (int i = 0; i < NumVerticies; ++i)
	{
		DFS_Root[i] = NULL;
		BFS_Root[i] = NULL;
	}

	//深搜森林、广搜森林中树的数量
	Num_DFS_Root = 0;
	Num_BFS_Root = 0;
}

//构造邻接表
void AdjGraph::SetUpLink()
{
	system("cls");
	ifstream in;
	in.open(FILE_NAME_DATA);

	//跳到保存边信息的位置
	int n;
	char c;
	in >> n >> n;
	for (int i = 0; i < NumVerticies; ++i)
		in >> c;

	int head, tail;
	EdgeNode* pe;
	for (int i = 0; i < NumEdges; ++i)
	{
		in >> head >> tail; //读入边的头部和尾部顶点的序号
		if (head < 0 || head >= NumVerticies || tail < 0 || tail >= NumVerticies)
		{
			cout << "Wrong number of the edge No." << i << "!" << endl;
			system("pause");
			exit(0);
		}
		if (head == tail)
		{
			cout << "Wrong number of the edge No." << i << "! The head should not equal to the tail!" << endl;
			system("pause");
			exit(0);
		}

		//将边前插到边链表中
		pe = new EdgeNode;
		pe->adjvex = tail;
		pe->next = verlist[head].firstedge;
		verlist[head].firstedge = pe;

		pe = new EdgeNode;
		pe->adjvex = head;
		pe->next = verlist[tail].firstedge;
		verlist[tail].firstedge = pe;
	}

	ShowLink(); //打印邻接表
	system("pause");
}

//构造邻接矩阵
void AdjGraph::SetUpMatrix()
{
	system("cls");
	ifstream in;
	in.open(FILE_NAME_DATA);

	//跳到保存边信息的位置
	int n;
	char c;
	in >> n >> n;
	for (int i = 0; i < NumVerticies; ++i)
		in >> c;

	int head, tail;
	for (int i = 0; i < NumEdges; ++i)
	{
		in >> head >> tail; //读入边的头部和尾部顶点的序号
		if (head < 0 || head >= NumVerticies || tail < 0 || tail >= NumVerticies)
		{
			cout << "Wrong number of the edge No." << i << "!" << endl;
			system("pause");
			exit(0);
		}
		if (head == tail)
		{
			cout << "Wrong number of the edge No." << i << "! The head should not equal to the tail!" << endl;
			system("pause");
			exit(0);
		}

		//将边加入到邻接表中
		Matrix[head][tail] = 1;
		Matrix[tail][head] = 1;
	}

	ShowMatrix(); //打印邻接矩阵
	system("pause");
}

//打印邻接矩阵
void AdjGraph::ShowMatrix()
{
	system("cls");
	cout << "The adjacency matrix of the graph is:" << endl;
	cout << "   ";
	for (int i = 0; i < NumVerticies; ++i)
	{
		cout << verlist[i].info;
		cout << "  ";
	}
	cout << endl;

	for (int i = 0; i < NumVerticies; ++i)
	{
		cout << verlist[i].info;
		cout << "  ";
		for (int j = 0; j < NumVerticies; ++j)
			cout << Matrix[i][j] << "  ";
		cout << endl;
	}
	cout << endl;
}

//打印邻接表
void AdjGraph::ShowLink()
{
	system("cls");
	cout << "The adjacency list of the graph is:" << endl;
	EdgeNode* pe;
	for (int i = 0; i < NumVerticies; ++i)
	{
		cout << verlist[i].info << ": ";
		pe = verlist[i].firstedge;
		while (pe != NULL)
		{
			cout << verlist[pe->adjvex].info << " ";
			pe = pe->next;
		}
		cout << endl;
	}
	cout << endl;
}

//邻接表转邻接矩阵
void AdjGraph::LinkToMatrix()
{
	EdgeNode* pe;
	//遍历顶点列表中所有顶点
	for (int i = 0; i < NumVerticies; ++i)
	{
		pe = verlist[i].firstedge;
		while (pe != NULL)
		{
			Matrix[i][pe->adjvex] = 1;
			Matrix[pe->adjvex][i] = 1;
			pe = pe->next;
		}
	}
}

//邻接矩阵转邻接表
void AdjGraph::MatrixToLink()
{
	for (int i = 0; i < NumVerticies; ++i)
		verlist[i].firstedge = NULL;

	EdgeNode* pe;
	//遍历矩阵中所有顶点
	for (int i = 0; i < NumVerticies - 1; ++i)
	{
		for (int j = i + 1; j < NumVerticies; ++j)
		{
			if (Matrix[i][j] == 1)
			{
				pe = new EdgeNode;
				pe->adjvex = j;
				pe->next = verlist[i].firstedge;
				verlist[i].firstedge = pe;

				pe = new EdgeNode;
				pe->adjvex = i;
				pe->next = verlist[j].firstedge;
				verlist[j].firstedge = pe;
			}
		}
	}
}

char* sequence; //保存遍历序列的数组
int cur_num; //数组的当前大小

//深度优先搜索
void AdjGraph::DFS()
{
	cur_num = 0;
	sequence = new char[NumVerticies];

	//将顶点的访问标记全部初始化为未访问过
	for (int i = 0; i < NumVerticies; ++i)
		verlist[i].s = UNVISITED;
	Num_DFS_Root = 0; //树的数量置0

	//用户选择遍历方式
	int choice;
	do {
		system("cls");
		cout << "1. Traverse the graph recursively" << endl
			<< "2. Traverse the graph non-recursively" << endl
			<< "3. Return to the main manu" << endl
			<< "Input your choice: ";
		cin >> choice;
	} while (choice != 1 && choice != 2 && choice != 3);

	if (choice == 3)
		return;

	//建立生成森林，一次遍历后没有访问过的顶点位于其他连通分量中，需要构造额外的树，每棵树以此类推
	for (int i = 0; i < NumVerticies; ++i)
	{
		if (verlist[i].s == VISITED)
			continue;

		DFS_Root[Num_DFS_Root] = new TreeNode;
		if (storage == LI) //用邻接表存储
		{
			if (choice == 1)
				Recur_DFS_Link(i, DFS_Root[Num_DFS_Root]); //递归
			else
				NonRecur_DFS_Link(i, DFS_Root[Num_DFS_Root]); //非递归
		}
		else //用邻接矩阵存储
		{
			if (choice == 1)
				Recur_DFS_Matrix(i, DFS_Root[Num_DFS_Root]); //递归
			else
				NonRecur_DFS_Matrix(i, DFS_Root[Num_DFS_Root]); //非递归
		}
		Num_DFS_Root++; //当前树的数量加1
	}

	system("cls");
	ShowSequence(); //显示遍历序列
	Show_DFS_Result(); //显示深搜结果
	system("pause");
}

//递归实现根据邻接表深搜
void AdjGraph::Recur_DFS_Link(int index, TreeNode*& n)
{
	//访问当前顶点，设置树结点的信息
	n->node = verlist[index];
	sequence[cur_num++] = n->node.info;
	verlist[index].s = VISITED;

	EdgeNode* pe = verlist[index].firstedge;

	//if ((i = GetNode_Link(n->node.index)) == -1) //如果当前顶点没有未访问过的后继顶点，返回
	//	return;

	TreeNode* pn = n->mostleftchild;
	//设置最左子结点域
	while (pe != NULL)
	{
		if (verlist[pe->adjvex].s == UNVISITED)
		{
			n->mostleftchild = new TreeNode;
			pn = n->mostleftchild;
			Recur_DFS_Link(pe->adjvex, pn);
			break;
		}
		pe = pe->next;
	}
	if (pe == NULL)
		return;

	pe = pe->next;
	//while ((i = GetNode_Link(index)) != -1)
	//{
	//	pn->nextbrother = new TreeNode;
	//	pn = pn->nextbrother;
	//	Recur_DFS_Link(i, pn); //递归进行遍历
	//}

	//访问当前顶点的所有邻接顶点
	while (pe != NULL)
	{
		if (verlist[pe->adjvex].s == UNVISITED)
		{
			pn->nextbrother = new TreeNode;
			pn = pn->nextbrother;
			Recur_DFS_Link(pe->adjvex, pn); //递归进行遍历
		}
		pe = pe->next;
	}
}

//递归实现根据邻接矩阵深搜
void AdjGraph::Recur_DFS_Matrix(int index, TreeNode*& n)
{
	//访问当前顶点，设置树结点的信息
	n->node = verlist[index];
	verlist[index].s = VISITED;
	sequence[cur_num++] = n->node.info;

	int i;
	if ((i = GetNode_Matrix(index)) == -1) //如果当前顶点没有未访问过的后继顶点，返回
		return;

	//设置最左子结点域
	n->mostleftchild = new TreeNode;
	TreeNode* pn = n->mostleftchild;
	Recur_DFS_Matrix(i, pn);

	//访问当前顶点的所有后继顶点
	while ((i = GetNode_Matrix(index)) != -1)
	{
		pn->nextbrother = new TreeNode;
		pn = pn->nextbrother;
		Recur_DFS_Matrix(i, pn); //递归进行遍历
	}
}

//从邻接矩阵中寻找下一个邻接点
int AdjGraph::GetNode_Matrix(int index)
{
	int i = 0;
	for (i = 0; i < NumVerticies; ++i)
	{
		if (Matrix[index][i] == 0)
			continue;
		if (verlist[i].s == VISITED)
			continue;
		break;
	}
	if (i == NumVerticies)
		return -1;
	return i;
}

//非递归基于邻接表进行DFS时，栈中既要保存当前的树结点，又要保存当前位于的边表结点，以确保每条边只遍历两遍（无向图）
typedef struct snode {
	TreeNode* tn;
	EdgeNode* ce;
}Snode;

//非递归实现根据邻接表深搜
void AdjGraph::NonRecur_DFS_Link(int index, TreeNode*& n)
{
	using std::stack;
	stack<Snode> S;
	Snode sn;
	EdgeNode* pe = NULL;

	//访问当前顶点，设置树结点的信息
	TreeNode* pn = n;
	pn->node = verlist[index];
	sequence[cur_num++] = pn->node.info; //加入遍历序列中
	verlist[index].s = VISITED; //标志置成已访问过

	//将当前树结点与下一次要经过的边压入栈中
	sn.tn = pn;
	sn.ce = pn->node.firstedge;
	S.push(sn);

	while (!S.empty()) //栈空时停止循环
	{
		pe = S.top().ce;
		pn = S.top().tn;
		S.pop();
		while (pe) //经过当前顶点关联的所有边
		{
			if (verlist[pe->adjvex].s == VISITED) //边指向的顶点已经访问过，则下一条边
				pe = pe->next;
			else
			{
				sequence[cur_num++] = verlist[pe->adjvex].info;
				verlist[pe->adjvex].s = VISITED;
				sn.tn = pn;
				sn.ce = pe->next;
				S.push(sn);
				if (pn->mostleftchild == NULL) //最左子结点为空，则先写入最左子结点
				{
					pn->mostleftchild = new TreeNode;
					pn = pn->mostleftchild;
					pn->node = verlist[pe->adjvex];
				}
				else //否则写入兄弟结点
				{
					pn = pn->mostleftchild;
					while (pn->nextbrother != NULL)
						pn = pn->nextbrother;
					pn->nextbrother = new TreeNode;
					pn = pn->nextbrother;
					pn->node = verlist[pe->adjvex];
				}
				pe = pn->node.firstedge; //开始遍历后继顶点的关联边
			}
		}
	}
}

//非递归实现根据邻接矩阵深搜
void AdjGraph::NonRecur_DFS_Matrix(int index, TreeNode*& n)
{
	using std::stack;
	stack<TreeNode*> node_S;
	int i = index;

	TreeNode* p = n;
	p->node = verlist[i];
	sequence[cur_num++] = p->node.info;
	verlist[i].s = VISITED;
	if (GetNode_Matrix(i) == -1)
		return;
	do {
		//先进行深度优先的访问
		while ((i = GetNode_Matrix(p->node.index)) != -1)
		{
			node_S.push(p);
			if (p->mostleftchild == NULL) //最左子结点为空，则先写入最左子结点
			{
				p->mostleftchild = new TreeNode;
				p->mostleftchild->node = verlist[i];
				p = p->mostleftchild;
			}
			else //否则写入兄弟结点
			{
				p = p->mostleftchild;
				while (p->nextbrother != NULL)
					p = p->nextbrother;
				p->nextbrother = new TreeNode;
				p->nextbrother->node = verlist[i];
				p = p->nextbrother;
			}
			sequence[cur_num++] = verlist[i].info; //加入遍历序列
			verlist[i].s = VISITED; //当前顶点标记为已访问过
		}

		do {
			p = node_S.top();
			node_S.pop();
		} while ((i = GetNode_Matrix(p->node.index)) == -1 && !node_S.empty()); //不断弹栈直到找到有为访问过的后继顶点的顶点

	} while (i != -1 || !node_S.empty()); //当栈为空且当前顶点再无未访问过的后继顶点时，结束循环
}

//广度优先搜索
void AdjGraph::BFS()
{
	cur_num = 0;
	sequence = new char[NumVerticies];

	//将顶点的访问标记全部初始化为未访问过
	for (int i = 0; i < NumVerticies; ++i)
		verlist[i].s = UNVISITED;
	Num_BFS_Root = 0; //树的数量置0

	//建立生成森林，一次遍历后没有访问过的顶点位于其他连通分量中，需要构造额外的树，每棵树以此类推
	for (int i = 0; i < NumVerticies; ++i)
	{
		if (verlist[i].s == VISITED)
			continue;
		BFS_Root[Num_BFS_Root] = new TreeNode;
		if (storage == LI)
			BFS_Link(i, BFS_Root[Num_BFS_Root]);
		else
			BFS_Matrix(i, BFS_Root[Num_BFS_Root]);
		Num_BFS_Root++; //当前树的数量加1
	}

	system("cls");
	ShowSequence(); //显示遍历序列
	Show_BFS_Result(); //显示广搜结果
	system("pause");
}

//根据邻接表广搜
void AdjGraph::BFS_Link(int index, TreeNode*& n)
{
	using std::queue;
	queue<TreeNode*> Q;
	TreeNode* p = n;
	TreeNode* pp;
	EdgeNode* pe;

	//访问当前顶点，设置树结点的信息
	p->node = verlist[index];
	sequence[cur_num++] = p->node.info; //加入遍历序列
	verlist[index].s = VISITED; //标志置为已访问过
	Q.push(p);

	while (!Q.empty()) //队列空时循环结束
	{
		p = Q.front();
		Q.pop();
		pe = p->node.firstedge;
		while (pe != NULL) //遍历当前顶点关联的所有边
		{
			if (verlist[pe->adjvex].s == VISITED) //边指向的顶点已经访问过，则下一条边
			{
				pe = pe->next;
				continue;
			}
			verlist[pe->adjvex].s = VISITED;
			sequence[cur_num++] = verlist[pe->adjvex].info;
			if (p->mostleftchild == NULL) //最左子结点为空，则先写入最左子结点
			{
				p->mostleftchild = new TreeNode;
				pp = p->mostleftchild;
				pp->node = verlist[pe->adjvex];
			}
			else //否则写入兄弟结点
			{
				pp = p->mostleftchild;
				while (pp->nextbrother != NULL)
					pp = pp->nextbrother;
				pp->nextbrother = new TreeNode;
				pp = pp->nextbrother;
				pp->node = verlist[pe->adjvex];
			}
			Q.push(pp); //后继顶点压入队列
			pe = pe->next; //下一条边
		}
	}
}

//根据邻接矩阵广搜
void AdjGraph::BFS_Matrix(int index, TreeNode*& n)
{
	using std::queue;
	queue<TreeNode*> Q;
	int i = index;
	int j;
	TreeNode* p = n;
	TreeNode* pp;

	//访问当前顶点，设置树结点的信息
	p->node = verlist[i];
	sequence[cur_num++] = p->node.info;
	verlist[i].s = VISITED;
	if (GetNode_Matrix(i) == -1)
		return;
	do {
		while ((j = GetNode_Matrix(i)) != -1)
		{
			if (p->mostleftchild == NULL) //最左子结点为空，则先写入最左子结点
			{
				p->mostleftchild = new TreeNode;
				pp = p->mostleftchild;
				pp->node = verlist[j];
				sequence[cur_num++] = verlist[j].info;
				verlist[j].s = VISITED;
			}
			else //否则写入兄弟结点
			{
				pp = p->mostleftchild;
				while (pp->nextbrother != NULL)
					pp = pp->nextbrother;
				pp->nextbrother = new TreeNode;
				pp = pp->nextbrother;
				pp->node = verlist[j];
				sequence[cur_num++] = verlist[j].info; //加入遍历序列
				verlist[j].s = VISITED; //当前顶点标记为已访问过
			}
			Q.push(pp);
		}

		do {
			p = Q.front();
			Q.pop();
			i = p->node.index;
		} while ((j = GetNode_Matrix(i)) == -1 && !Q.empty()); //不断弹栈直到找到有为访问过的后继顶点的顶点

	} while (j != -1 || !Q.empty()); //当栈为空且当前顶点再无未访问过的后继顶点时，结束循环
}

//打印遍历序列
void AdjGraph::ShowSequence()
{
	cout << "The traversal sequence is:" << endl;
	int j = 0;
	for (int i = 0; i < NumVerticies; ++i)
	{
		if (j < 4)
		{
			cout << "[" << i << "]" << sequence[i] << '\t';
			j++;
		}
		else
		{
			cout << "[" << i << "]" << sequence[i] << endl;
			j = 0;
		}

	}
	cout << endl << endl;
}

//显示深搜结果
void AdjGraph::Show_DFS_Result()
{
	cout << "The spinning forest is:" << endl;
	//打印所有的树
	for (int i = 0; i < Num_DFS_Root; ++i)
	{
		cout << "——————[Tree No." << i + 1 << "]——————" << endl;
		Traverse_DFS_Tree(DFS_Root[i]);
	}
}

//递归地遍历深搜树
void AdjGraph::Traverse_DFS_Tree(TreeNode* n)
{
	//打印每个结点及其所有的子结点
	cout << n->node.info << "(";
	TreeNode* p = n->mostleftchild;
	if (p != NULL)
	{
		cout << p->node.info;
		p = p->nextbrother;
		while (p != NULL)
		{
			cout << ", " << p->node.info;
			p = p->nextbrother;
		}
	}
	cout << ")" << endl;

	//遍历所有的子结点
	p = n->mostleftchild;
	while (p != NULL)
	{
		Traverse_DFS_Tree(p); //进行递归
		p = p->nextbrother;
	}
}

//显示广搜结果
void AdjGraph::Show_BFS_Result()
{
	using std::queue;
	queue<TreeNode*> Q;
	TreeNode* p;

	//打印所有的树

	cout << "The spinning forest is:" << endl;
	for (int i = 0; i < Num_BFS_Root; ++i)
	{
		cout << "——————[Tree No." << i + 1 << "]——————" << endl;
		p = BFS_Root[i];
		Q.push(p);
		while (!Q.empty())
		{
			p = Q.front();
			Q.pop();
			cout << p->node.info << "(";

			//括号中打印所有的子结点
			p = p->mostleftchild;
			if (p != NULL)
			{
				cout << p->node.info;
				Q.push(p);
				p = p->nextbrother;
			}
			while (p != NULL)
			{
				cout << ", " << p->node.info;
				Q.push(p);
				p = p->nextbrother;
			}
			cout << ")" << endl;
		}
	}
}

int main()
{
	AdjGraph G; //声明一个图类
	int choice1, choice2;
	do {
		system("cls");
		cout << "1. Set up the graph by adjacency list" << endl
			<< "2. Set up the graph by adjacency matrix" << endl
			<< "3. Quit" << endl
			<< "Input your choice: ";
		cin >> choice1;
	} while (choice1 != 1 && choice1 != 2 && choice1 != 3);

	if (choice1 == 3)
		return 0;
	if (choice1 == 1)
	{
		G.storage = LI; //存储结构标志置为邻接表
		G.SetUpLink(); //建立邻接表
	}
	else
	{
		G.storage = MA; //存储结构标志置为邻接矩阵
		G.SetUpMatrix(); //建立邻接矩阵
	}

	while (1)
	{
		do {
			system("cls");
			if (G.storage == LI) //邻接表
				G.ShowLink(); //打印邻接表
			else
				G.ShowMatrix(); //打印邻接矩阵

			if (G.storage == LI)
				cout << "1. Transform the adjacency list to adjacency matrix" << endl;
			else
				cout << "1. Transform the adjacency matrix to adjacency list" << endl;

			cout << "2. Depth first search" << endl
				<< "3. Breadth first search" << endl
				<< "4. Quit" << endl
				<< "Input your choice: ";
			cin >> choice2;
		} while (choice2 != 1 && choice2 != 2 && choice2 != 3 && choice2 != 4);

		if (choice2 == 4)
			return 0;
		if (choice2 == 1)
		{
			if (G.storage == LI)
			{
				G.storage = MA;
				G.LinkToMatrix(); //邻接表转邻接矩阵
			}
			else
			{
				G.storage = LI;
				G.MatrixToLink(); //邻接矩阵转邻接表
			}
		}
		else if (choice2 == 2)
			G.DFS(); //深搜
		else
			G.BFS(); //广搜
	}
}
