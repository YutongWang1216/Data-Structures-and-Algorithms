#include <iostream>
#include <windows.h>
#include <stack>
#include <fstream>
using namespace std;

typedef int value;

struct node {
    value key;
    node* lchild = NULL, * rchild = NULL;
    int lsize = 0, rsize = 0;
    int depth;
};

typedef node* BST;

class AVLTree {
public:
    BST Root;
    AVLTree();
    void AVLInsert(BST& T, value v, bool& increased);
    void Adjust(BST& T);
    void LeftRotation(BST& T);
    void RightRotation(BST& T);
    void ShowTree();
    void Visit(BST T);
    void AVLDelete(BST& T, value v);
    BST FindSub(BST T);
    void Search(value v, BST T);
    void Sort(BST T);
};

//AVL树的构造函数
AVLTree::AVLTree()
{
    Root = NULL;
}

//插入一个数
void AVLTree::AVLInsert(BST& T, value v, bool& increased)
{
    if (!T) //当前结点为待插入处
    {
        T = new node;
        T->key = v;
        return;
    }
    if (v < T->key) //左子树上插入
    {
        if (!T->lchild) //发现待插入处
        {
            T->lchild = new node;
            T->lchild->key = v;
            T->lsize++; //左子树高度加1
            if (!T->rchild)
                increased = true; //如果插入结点没有右兄弟，则树高改变，需要重新计算平衡因子
        }
        else //需要继续寻找插入点
        {
            AVLInsert(T->lchild, v, increased);
            if (increased) //如果左子树高度发生变化
            {
                T->lsize++; //平衡因子可能变为0、1、2
                if (T->lsize == T->rsize) //平衡因子变为0，则不需要再向上调整
                    increased = false;
                else if (T->lsize - T->rsize == 2) //平衡因子变为2，需要进行旋转操作
                {
                    Adjust(T);
                    increased = false;
                }
            }
        }
    }
    else if (v > T->key) //右子树上插入
    {
        if (!T->rchild) //发现待插入处
        {
            T->rchild = new node;
            T->rchild->key = v;
            T->rsize++; //右子树高度加1
            if (!T->lchild)
                increased = true; //如果插入结点没有左兄弟，则树高改变，需要重新计算平衡因子
        }
        else
        {
            AVLInsert(T->rchild, v, increased);
            if (increased) //如果右子树高度发生变化
            {
                T->rsize++; //平衡因子可能变为0、1、2
                if (T->lsize == T->rsize) //平衡因子变为0，则不需要再向上调整
                    increased = false;
                else if (T->rsize - T->lsize == 2) //平衡因子变为2，需要进行旋转操作
                {
                    Adjust(T); 
                    increased = false;
                }
            }
        }
    }
    else //数据已经存在
        cout << "The number " << v << " has already existed!" << endl;
}

void AVLTree::Adjust(BST& T)
{
    BST pn;
    if (T->lsize - T->rsize == 2) //L型
    {
        if (T->lchild->lsize - T->lchild->rsize == 1) //LL型
        {
            RightRotation(T); //右旋处理
            cout << "LL ";
        }
        else //LR型
        {
            LeftRotation(T->lchild); //左旋处理
            RightRotation(T); //右旋处理
            cout << "LR ";
        }
    }
    else //R型
    {
        if (T->rchild->rsize - T->rchild->lsize == 1) //RR型
        {
            LeftRotation(T); //左旋处理
            cout << "RR ";
        }
        else //RL型
        {
            RightRotation(T->rchild); //右旋处理
            LeftRotation(T); //左旋处理
            cout << "RL ";
        }
    }
}

//左旋操作
void AVLTree::LeftRotation(BST& T)
{
    BST rc = T->rchild;
    T->rchild = rc->lchild;
    T->rsize = rc->lsize;
    rc->lchild = T;
    rc->lsize = 1 + (T->lsize > T->rsize ? T->lsize : T->rsize); //重新计算该点的平衡因子
    T = rc;
}

//右旋操作
void AVLTree::RightRotation(BST& T)
{
    BST lc = T->lchild;
    T->lchild = lc->rchild;
    T->lsize = lc->rsize;
    lc->rchild = T;
    lc->rsize = 1 + (T->lsize > T->rsize ? T->lsize : T->rsize); //重新计算该点的平衡因子
    T = lc;
}

//图形化显示AVL树
void AVLTree::ShowTree()
{
    if (Root == NULL)
    {
        cout << "The tree is Empty!" << endl;
        cout << endl;
        return;
    }
    using std::stack;
    stack<BST> aStack;
    BST pointer = Root;
    int depth_counter = 0;
    pointer->depth = 0;
    while (!aStack.empty() || pointer) //遍历整棵树，计算每个结点的高度
    {
        if (pointer)
        {
            if (pointer->rchild != NULL)
                pointer->rchild->depth = pointer->depth + 1;
            if (pointer->lchild != NULL)
                pointer->lchild->depth = pointer->depth + 1;
            aStack.push(pointer);
            pointer = pointer->rchild;
        }
        else
        {
            pointer = aStack.top();
            aStack.pop();
            if (pointer != NULL)
            {
                Visit(pointer);
                pointer = pointer->lchild;
            }
        }
    }
    cout << endl;
}

//打印树结点
void AVLTree::Visit(BST T)
{
    for (int i = 0; i < T->depth; ++i) //左侧缩进即表示高度
        cout << '\t';
    cout << T->key << "(" << T->lsize - T->rsize << ")";
    for (int i = 0; i < 60 - 8 * T->depth; ++i) //高度越大，后面的线条越短
        cout << "-";
    cout << endl;
}

//删除数据
void AVLTree::AVLDelete(BST& T, value v)
{
    if (!T) //没有找到待删除的数据
    {
        cout << "Not Found!" << endl;
        return;
    }
    if (T->key > v) //在左子树上删除
    {
        AVLDelete(T->lchild, v); //递归调用删除函数
        //重新计算该结点的平衡因子
        if (!T->lchild)
            T->lsize = 0;
        else
            T->lsize = 1 + (T->lchild->lsize > T->lchild->rsize ? T->lchild->lsize : T->lchild->rsize);
        if (T->rsize - T->lsize == 2) //平衡因子变为-2，需要调整
            Adjust(T);
    }
    else if (T->key < v) //在右子树上删除
    {
        AVLDelete(T->rchild, v); //递归调用删除函数
        //重新计算该结点的平衡因子
        if (!T->rchild)
            T->rsize = 0;
        else
            T->rsize = 1 + (T->rchild->lsize > T->rchild->rsize ? T->rchild->lsize : T->rchild->rsize);
        if (T->lsize - T->rsize == 2)  //平衡因子变为2，需要调整
            Adjust(T);
    }
    else //当前结点就是待删除结点
    {
        if (!T->rchild && T->lchild) //左子树为空，右子树不空，则直接将右子树的根节点接到删除节点的位置
        {
            BST p = T;
            T = T->lchild;
            delete p;
        }
        else if (!T->lchild && T->rchild) //右子树为空，左子树不空，则直接将左子树的根节点接到删除节点的位置
        {
            BST p = T;
            T = T->rchild;
            delete p;
        }
        else if (!T->lchild && !T->rchild) //左右子树都空，直接删除该结点
        {
            delete T;
            T = NULL;
        }
        else //左右子树都不为空，则需要寻找右子树的最左结点作为删除的替代结点
        {
            if (!FindSub(T)) //寻找替代结点，若替代结点就是该结点的右子树的根
            {
                T->rchild->lchild = T->lchild;
                T->rchild->lsize = T->lsize;
                BST p = T;
                T = T->rchild;
                delete p;
                if (T->lsize - T->rsize == 2) //平衡因子变为2，需要进行调整
                    Adjust(T);
            }
            else //找到替代的叶子结点
            {
                T->key = FindSub(T)->key;
                AVLDelete(T->rchild, T->key); //删除替代的叶子结点即可
                if (!T->rchild)
                    T->rsize = 0;
                else
                    T->rsize = 1 + (T->rchild->lsize > T->rchild->rsize ? T->rchild->lsize : T->rchild->rsize);
                if (T->lsize - T->rsize == 2) //平衡因子变为2，需要进行调整
                    Adjust(T);
            }
        }
    }
}

//寻找删除的替代结点
BST AVLTree::FindSub(BST T)
{
    BST p = T->rchild;
    p = p->lchild;
    if (!p)
        return NULL;
    while (p->lchild)
        p = p->lchild;
    return p;
}

//查找
void AVLTree::Search(value v, BST T)
{
    if (T == NULL) //当前结点为空，说明查找失败
    {
        cout << "The data " << v << " was not found!" << endl;
        system("pause");
    }
    else
    {
        if (v < T->key) //查找值小于当前结点，则在右子树上查找
            Search(v, T->lchild);
        else if (v > T->key) //查找值大于当前结点，则在左子树上查找
            Search(v, T->rchild);
        else //当前结点的值就是查找值
        {
            cout << "Search successfully!" << endl;
            system("pause");
        }
    }
}

//排序
void AVLTree::Sort(BST T)
{
    //先根遍历所有结点得到的序列就是递增序列
    if (T->lchild)
        Sort(T->lchild);
    cout << T->key << " ";
    if (T->rchild)
        Sort(T->rchild);
}

int main()
{
    ifstream in;
    in.open("D:\\AVL\\input.txt");
    value vin;
    AVLTree AT;
    int num;
    in >> num;
    cout << "Routation(s) during setting up:" << endl; //打印旋转操作的类型
    for (int i = 0; i < num; ++i)
    {
        bool increased = false;
        in >> vin;
        AT.AVLInsert(AT.Root, vin, increased);
    }
    cout << endl;
    AT.ShowTree();
    system("pause");

    int choice;
    while (1)
    {
        do {
            system("cls");
            AT.ShowTree();
            cout << "1. Insert a number" << endl
                << "2. Delete a number" << endl
                << "3. Search a number" << endl
                << "4. Sort" << endl
                << "0. Exit" << endl
                << "Input your choice: ";
            cin >> choice;
        } while (choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 0);
        if (choice == 1)
        {
            system("cls");
            AT.ShowTree();
            bool increased = false;
            cout << "Input a number: ";
            cin >> vin;
            cout << "Routation(s) during inserting:" << endl; //打印旋转操作的类型
            AT.AVLInsert(AT.Root, vin, increased);
            cout << endl;
            AT.ShowTree(); //打印AVL树
            system("pause");
        }
        else if (choice == 2)
        {
            system("cls");
            AT.ShowTree(); //打印AVL树
            cout << "Input a number: ";
            cin >> vin;
            cout << "Routation(s) during deleting:" << endl; //打印旋转操作的类型
            AT.AVLDelete(AT.Root, vin);
            cout << endl;
            AT.ShowTree(); //打印AVL树
            system("pause");
        }
        else if (choice == 3)
        {
            system("cls");
            AT.ShowTree(); //打印AVL树
            cout << "Input a number: ";
            cin >> vin;
            AT.Search(vin, AT.Root);
        }
        else if (choice == 4)
        {
            system("cls");
            AT.ShowTree(); //打印AVL树
            AT.Sort(AT.Root);
            cout << endl;
            system("pause");
        }
        else
            return 0;
    }
    return 0;
}
