#include <iostream>
#include <stack>
#include <queue>
#include <fstream>
#include <Windows.h>
using namespace std;

enum Tag { Left, Right };

template <class T>
class BinaryTreeNode
{
	template <class T>
	friend class BinaryTree;
private:
	T info;
public:
	int depth;
	Tag tag;
	BinaryTreeNode<T>* lc;
	BinaryTreeNode<T>* rc;
	BinaryTreeNode();
	BinaryTreeNode(const T& ele);
	BinaryTreeNode(const T& ele, BinaryTreeNode<T>* l, BinaryTreeNode<T>* r);
	T Value();
	BinaryTreeNode<T>* LeftChild() const;
	BinaryTreeNode<T>* RightChild() const;
	void SetLeftChild(BinaryTreeNode<T>* l);
	void SetRightChild(BinaryTreeNode<T>* r);
	void SetValue(const T& val);
	bool IsLeaf() const;
	void Print();
	void Write();
	BinaryTreeNode<T>& operator = (const BinaryTreeNode<T>& node);
};

//默认构造函数
template<class T>
BinaryTreeNode<T>::BinaryTreeNode()
{
	info = 0;
	tag = Left;
	lc = NULL;
	rc = NULL;
	depth = 0;
}

//返回结点信息
template<class T>
T BinaryTreeNode<T>::Value()
{
	return info;
}

//返回左孩子的指针
template<class T>
BinaryTreeNode<T>* BinaryTreeNode<T>::LeftChild() const
{
	return lc;
}

//返回右孩子的指针
template<class T>
BinaryTreeNode<T>* BinaryTreeNode<T>::RightChild() const
{
	return rc;
}

//设置左孩子域
template<class T>
void BinaryTreeNode<T>::SetLeftChild(BinaryTreeNode<T>* l)
{
	lc = l;
}

//设置右孩子域
template<class T>
void BinaryTreeNode<T>::SetRightChild(BinaryTreeNode<T>* r)
{
	rc = r;
}

//设置结点信息
template<class T>
void BinaryTreeNode<T>::SetValue(const T& val)
{
	info = val;
}

//打印结点信息
template<class T>
void BinaryTreeNode<T>::Print()
{
	cout << info << " ";
}

//写入文件
template<class T>
void BinaryTreeNode<T>::Write()
{
	ofstream out;
	out.open("D:\\Binary tree\\result.txt", ios::app);
	out << info << " ";
}

//运算符'='的重载
template<class T>
BinaryTreeNode<T>& BinaryTreeNode<T>::operator=(const BinaryTreeNode<T>& node)
{
	info = node.info;
	return node;
}

template <class T>
class BinaryTree
{
private:
	BinaryTreeNode<T>* ROOT;
public:
	int nodeNum;
	BinaryTree();
	~BinaryTree() {};
	bool isEmpty() const;
	BinaryTreeNode<T>* Root();
	void createTree();
	void CreateBT(BinaryTreeNode<char>*& root);
	void Visit(T val, int depth);
	void ShowTree();
	void PreOrder();
	void InOrder();
	void PostOrder();
	void LevelOrder();
	void RecurPreOrder(BinaryTreeNode<char>* root);
	void RecurInOrder(BinaryTreeNode<char>* root);
	void RecurPostOrder(BinaryTreeNode<char>* root);
	bool IsCompleteBT();
	int GetWidth();
};

//默认构造函数
template<class T>
BinaryTree<T>::BinaryTree()
{
	ROOT = NULL;
	nodeNum = 0;
};

//判断树是否为空
template<class T>
bool BinaryTree<T>::isEmpty() const {
	if (ROOT == NULL)
		return true;
	else
		return false;
}

template<class T>
BinaryTreeNode<T>* BinaryTree<T>::Root()
{
	return ROOT;
}

//由用户构造二叉树
template<class T>
void BinaryTree<T>::CreateBT(BinaryTreeNode<char>*& root)
{
	char ch;
	cin >> ch; //从输入缓冲区读入一个字符
	if (ch == '#') //'#’代表空结点
		root = NULL;
	else
	{
		root = new BinaryTreeNode<char>;
		root->SetValue(ch); //将信息赋值给结点
		nodeNum++;
		BinaryTreeNode<char>* lc = new BinaryTreeNode<char>;
		BinaryTreeNode<char>* rc = new BinaryTreeNode<char>;
		CreateBT(lc); //从左子结点递归构造二叉树
		root->SetLeftChild(lc);
		CreateBT(rc); //从右子结点递归构造二叉树
		root->SetRightChild(rc);
	}
}

//返回根结点指针
template<class T>
void BinaryTree<T>::createTree()
{
	cout << "Input the information of each node(use '#' to add null node)" << endl;
	CreateBT(ROOT);
}

template<class T>
void BinaryTree<T>::Visit(T val, int depth)
{
	ofstream out;
	out.open("D:\\Binary tree\\result.txt", ios::app);
	for (int i = 0; i < depth; ++i) //由深度决定其前制表符的数量，深度越深，制表符越多
	{
		cout << '\t';
		out << '\t';
	}
	cout << val;
	out << val;
	for (int i = 0; i < 37 - 4 * depth; ++i) //由深度决定其后辅助线的长度，深度越深，辅助线越短
	{
		cout << "—";
		out << "—";
	}
	cout << endl;
	out << endl;
}

//横向可视化打印二叉树
template<class T>
void BinaryTree<T>::ShowTree()
{
	if (ROOT == NULL)
	{
		cout << "The tree is Empty!" << endl;
		cout << endl;
		return;
	}
	using std::stack;
	stack<BinaryTreeNode<char>*> aStack;
	BinaryTreeNode<char>* pointer = ROOT;
	int depth_counter = 0;
	pointer->depth = 0;
	while (!aStack.empty() || pointer) //遍历整棵二叉树，统计每个结点的深度，便于可视化输出
	{
		if (pointer)
		{
			if (pointer->LeftChild() != NULL)
				pointer->LeftChild()->depth = pointer->depth + 1; //左子结点深度比父结点多一
			if (pointer->RightChild() != NULL)
				pointer->RightChild()->depth = pointer->depth + 1; //右子结点深度比父结点多一
			aStack.push(pointer);
			pointer = pointer->RightChild();
		}
		else
		{
			pointer = aStack.top();
			aStack.pop();
			if (pointer != NULL)
			{
				Visit(pointer->Value(), pointer->depth);
				pointer = pointer->LeftChild();
			}
		}
	}
}

//非递归的先序遍历
template<class T>
void BinaryTree<T>::PreOrder()
{
	using std::stack;
	stack<BinaryTreeNode<char>*> S;
	BinaryTreeNode<char>* pointer = ROOT;

	while (!S.empty() || pointer)
	{
		if (pointer)
		{
			pointer->Print(); //当前结点不为空，则先访问当前结点
			pointer->Write();
			S.push(pointer); //将当前结点压入栈中
			pointer = pointer->LeftChild(); //访问其左子结点
		}
		else //当前结点为空，则从栈中弹出一个结点，访问其右子结点
		{
			pointer = S.top();
			S.pop();
			if (pointer)
				pointer = pointer->RightChild();
		}
	}
}

//非递归的中序遍历
template<class T>
void BinaryTree<T>::InOrder()
{
	using std::stack;
	stack<BinaryTreeNode<char>*> S;
	BinaryTreeNode<char>* pointer = ROOT;

	while (!S.empty() || pointer)
	{
		if (pointer) //当前结点为非空结点
		{
			S.push(pointer); //先不访问当前结点，而是压入栈中，指针移向其左子结点
			pointer = pointer->LeftChild();
		}
		else //当前结点为空结点
		{
			pointer = S.top(); //从栈顶弹出一个结点
			S.pop();
			if (pointer) //结点非空，则先访问该结点
			{
				pointer->Print();
				pointer->Write();
				pointer = pointer->RightChild(); //指针移向其右子结点
			}
		}
	}
}

//非递归的后续遍历
template<class T>
void BinaryTree<T>::PostOrder()
{
	using std::stack;
	stack<BinaryTreeNode<char>*> S;
	BinaryTreeNode<char>* pointer = ROOT;

	while (!S.empty() || pointer)
	{
		if (pointer)
		{
			S.push(pointer); //先将当前结点压入栈中，指针移向其左子结点
			pointer = pointer->LeftChild();
		}
		else //当前结点为空
		{
			while (!S.empty() && S.top()->tag == Right) //访问已经遍历过左子结点的所有结点
			{
				pointer = S.top();
				pointer->Print();
				pointer->Write();
				S.pop();
				pointer = NULL;
			}
			if (!S.empty())
			{
				S.top()->tag = Right; //将当前结点标记为左子结点已经访问过
				pointer = S.top()->RightChild(); //开始访问右子结点
			}
		}
	}
}

//层序遍历
template<class T>
void BinaryTree<T>::LevelOrder()
{
	if (ROOT == NULL)
		return;

	using std::queue;
	queue<BinaryTreeNode<char>*> Q;
	BinaryTreeNode<char>* pointer;

	Q.push(ROOT);
	while (!Q.empty()) //队列非空，则未遍历完所有结点
	{
		pointer = Q.front(); //访问队首结点
		pointer->Print();
		pointer->Write();
		Q.pop();
		if (pointer->LeftChild() != NULL) //有左子结点，则将其压入队列
			Q.push(pointer->LeftChild());
		if (pointer->RightChild() != NULL) //有右子结点，则将其压入队列
			Q.push(pointer->RightChild());
	}
}

//递归的先序遍历
template<class T>
void BinaryTree<T>::RecurPreOrder(BinaryTreeNode<char>* root)
{
	root->Print(); //先访问当前结点
	root->Write();
	if (root->LeftChild() != NULL) //有左子结点，则以左子结点为当前结点进行递归遍历
		RecurPreOrder(root->LeftChild());
	if (root->RightChild() != NULL) //有右子结点，则以左子结点为当前结点进行递归遍历
		RecurPreOrder(root->RightChild());
}

//递归的中序遍历
template<class T>
void BinaryTree<T>::RecurInOrder(BinaryTreeNode<char>* root)
{
	if (root->LeftChild() != NULL) //有左子结点，则以左子结点为当前结点进行递归遍历
		RecurInOrder(root->LeftChild());
	root->Print(); //左子结点访问完成后再访问当前结点
	root->Write();
	if (root->RightChild() != NULL) //有右子结点，则以左子结点为当前结点进行递归遍历
		RecurInOrder(root->RightChild());
}

//递归的后序遍历
template<class T>
void BinaryTree<T>::RecurPostOrder(BinaryTreeNode<char>* root)
{
	if (root->LeftChild() != NULL) //有左子结点，则以左子结点为当前结点进行递归遍历
		RecurPostOrder(root->LeftChild());
	if (root->RightChild() != NULL) //有右子结点，则以左子结点为当前结点进行递归遍历
		RecurPostOrder(root->RightChild());
	root->Print(); //左右子结点均访问完成后再访问当前结点
	root->Write();
}

//判断是否为完全二叉树
template<class T>
bool BinaryTree<T>::IsCompleteBT()
{
	if (ROOT == NULL)
		return true;

	using std::queue;
	queue<BinaryTreeNode<char>*> Q;
	BinaryTreeNode<char>* pointer;
	bool Is_Final = false; //标记是否已经访问过最后一个非叶子节点

	Q.push(ROOT);
	while (!Q.empty()) //层序遍历二叉树
	{
		pointer = Q.front();
		Q.pop();
		if (pointer->LeftChild() == NULL && pointer->RightChild() != NULL) //左子结点为空，右子结点非空，一定不是完全二叉树
			return false;
		else if (Is_Final && (pointer->LeftChild() != NULL || pointer->RightChild() != NULL)) //已经访问过最后一个非叶子节点，如果还有非叶子结点则不是完全二叉树
			return false;
		else if ((pointer->LeftChild() != NULL && pointer->RightChild() == NULL) || (pointer->LeftChild() == NULL && pointer->RightChild() == NULL))
			Is_Final = true; //如果一个节点只有左子结点或者没有子结点，则标记为已经访问过最后一个非叶子结点

		if (pointer->LeftChild() != NULL)
			Q.push(pointer->LeftChild());
		if (pointer->RightChild() != NULL)
			Q.push(pointer->RightChild());
	}
	return true;
}

//计算二叉树的宽度
template<class T>
int BinaryTree<T>::GetWidth()
{
	using std::queue;
	queue<BinaryTreeNode<char>*> Q;
	Q.push(ROOT);
	BinaryTreeNode<char>* pointer = ROOT;
	pointer->depth = 0;
	int curLevel = 0; //当前的层数
	int curWidth = 0; //当前层的宽度
	int maxWidth = 0; //目前的最大宽度

	while (!Q.empty())
	{
		pointer = Q.front();
		if (pointer->depth > curLevel) //结点深度大于当前深度，证明遍历已经进入下一层
		{
			if (curWidth > maxWidth) //更新最大宽度
				maxWidth = curWidth;
			curLevel++; //当前深度加1
			curWidth = 0; //当前宽度置0
		}
		curWidth++; //每遍历一个结点当前层的宽度加1
		Q.pop();
		if (pointer->LeftChild() != NULL)
		{
			pointer->LeftChild()->depth = pointer->depth + 1; //左子结点深度比当前结点多一
			Q.push(pointer->LeftChild());
		}
		if (pointer->RightChild() != NULL)
		{
			pointer->RightChild()->depth = pointer->depth + 1; //右子结点深度比当前结点多一
			Q.push(pointer->RightChild());
		}
	}
	if (curWidth > maxWidth) //更新最大宽度
		maxWidth = curWidth;
	return maxWidth; //最大宽度就是树的宽度
}

//遍历的主干函数
void Traverse(BinaryTree<char> BT)
{
	ofstream out;
	out.open("D:\\Binary tree\\result.txt", ios::app);
	cout << "Pre order:" << endl;
	out << "Pre order:" << endl;
	BT.PreOrder();
	cout << endl;
	out << endl;

	cout << "Recursive pre order:" << endl;
	out << "Recursive pre order:" << endl;
	BT.RecurPreOrder(BT.Root());
	cout << endl;
	out << endl;

	cout << "In order:" << endl;
	out << "In order:" << endl;
	BT.InOrder();
	cout << endl;
	out << endl;

	cout << "Recusive in order:" << endl;
	out << "Recusive in order:" << endl;
	BT.RecurInOrder(BT.Root());
	cout << endl;
	out << endl;

	cout << "Post order:" << endl;
	out << "Post order:" << endl;
	BT.PostOrder();
	cout << endl;
	out << endl;

	cout << "Recursive post order:" << endl;
	out << "Recursive post order:" << endl;
	BT.RecurPostOrder(BT.Root());
	cout << endl;
	out << endl;

	cout << "Level order:" << endl;
	out << "Level order:" << endl;
	BT.LevelOrder();
	cout << endl;
	out << endl;


}

int main()
{
	ofstream out;
	out.open("D:\\Binary tree\\result.txt", ios::app);
	out << "*******************************************************************" << endl;

	BinaryTree<char> BT;
	BT.createTree(); //由用户构造二叉树
	BT.ShowTree(); //可视化输出二叉树
	Traverse(BT); //二叉树的遍历

	if (BT.IsCompleteBT()) //判断是否为完全二叉树
	{
		cout << "This tree is a complete binary tree." << endl;
		out << "This tree is a complete binary tree." << endl;
	}
	else
	{
		cout << "This tree is NOT a complete binary tree." << endl;
		out << "This tree is NOT a complete binary tree." << endl;
	}
	//计算二叉树的宽度
	cout << "The width of the tree is " << BT.GetWidth() << "." << endl << endl;
	out << "The width of the tree is " << BT.GetWidth() << "." << endl << endl;
	out.close();
	system("pause");
}

/*
ABDH##I##E##CF#J##G##
ABC###D##
ABC##D##E##
ABC###DE###
ABCDE###FG#####
ABD##E##CFH###G##
*/