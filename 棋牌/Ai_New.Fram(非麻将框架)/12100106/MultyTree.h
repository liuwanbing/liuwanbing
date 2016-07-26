


///  MultyTree.h

/*
* 多路决策分析树的建立
*/
#include "realData.h"
#include <queue>
using namespace std;

/// 一个多路决策分析树的结点
class Mnode
{
	friend class multyTree;
///函数成员
	///构造一个多路树的结点
	Mnode(realData &t,Mnode *p=NULL)     
	{
		data = t;
        pfather = p;
	}
    ///自己死了的时候要杀死所有的子孙
    ~Mnode()                            
	{
        killMySun();
	}
    
	/// 杀死自己所有的孩子
	void killMySun()
	{
		list<Mnode*>::iterator it;
		for(it=lnode.begin();it!=lnode.end();it++)
		{
			delete *it;
		}
		lnode.clear();
	}

    ///生成一个孩子
	Mnode *born(realData &t)             
	{
		Mnode *mySun = new Mnode(t,this);
		lnode.push_back(mySun);
		return mySun;
	}
    
	///生出自己的孩子
	void born()
	{
		list<realData> lData;
		data.born(lData);
		list<realData>::iterator it;
		for(it=lData.begin();it!=lData.end();it++)
		{
			born(*it);
		}
	}

///数据
	list<Mnode*> lnode;                 ///自己的孩子结点
    Mnode* pfather;                     ///父节点
    realData data;                             ///本节点所携带的状态数据
};
///多路树
class multyTree
{
public:
	///构造函数,生成根节点
	multyTree(realData &t)               
	{
		root = new Mnode(t);
	}
    
	///构造函数
	multyTree(Mnode* r) 
	{
		root = r;
	}

    ~multyTree()
	{
		delete root;
	}
	/// 回溯法实现
	bool depth(realData &t)
	{
		bool result = depth();         ///为了算法的通用性，不管是否找到正确的解都要得到节点数据
		t = root->data;
		return result;
	}

protected:
    /// 回溯法实现，发现正确解就返回true，否则返回false
	bool depth()
	{
		if(root->data.isRightResult())               ///如果自己就是一个正确的解返回
			return true;
		
		root->born();          ///生出一些子节点数据
        multyTree* pTree = NULL;
        list<Mnode*>::iterator it;
		for(it = root->lnode.begin();it!=root->lnode.end();it++)
		{
			pTree = new multyTree(*it);
			if(pTree->depth())                         ///孩子中找到
			{
				root->data.acess(pTree->root->data);
				return true;
			}
			root->data.acess(pTree->root->data);
			
			delete pTree;
		}
        root->lnode.clear();       ///因为他的儿子已经死了，所以要去掉儿子的索引
		return false;
	}
	///分支限界时的实现,用于寻找问题的一个解
	void extent()
	{

	}
protected:
	Mnode* root;
};