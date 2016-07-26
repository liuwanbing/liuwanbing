

#ifndef ADVANT_H
#define ADVANT_H

/// 添加的便利函数
/// 删除牌中的某张牌
/// 其中m_Card，num是手中牌，index是要删除的位置
void del(BYTE *m_Card,int &num,int index);
/// 添加的便利函数
/// 删除牌中的某张牌
/// 其中m_Card，num是手中牌，card是要删除的牌
void del(BYTE *m_Card,int &num,BYTE card);

/// 添加便利函数
/// 删除牌中的某些牌
/// m_Card,num为自己的牌，m_delCard,delNum为要删除的牌
void del(BYTE *m_Card,int &num,BYTE *m_delCard,int delNum);
/// 添加便利函数
/// 删除牌中的某些牌
/// m_Card,num为自己的牌，bulk为删除的牌等于
void delBulk(BYTE *m_Card,int &num,int bulk);
/// 提取2和大小王的操作
void tiDX2Card(BYTE *m_Card,int &num,std::vector<peiOutCardStruct>& myCard);

/// 配牌的操作，主要为配当前手中的牌，其中牌中不包括2和大小王
bool peiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),CClientGameDlg *pClientGameDlg);

/// 生成含有炸弹的孩子
bool zhaPeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg);
/// 生成含有三顺的孩子
bool shun3PeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg);
/// 生成含有双顺的孩子
bool shun2PeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg);
/// 生成含有单顺的孩子
bool shun1PeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg);
/// 生成含有三张的孩子
bool sanPeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg);
/// 生成含有对子的孩子
bool duiPeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg);

/// 用于打印一张牌的内容
void printCard(BYTE card,CString &str);

/// 用于打印一手牌的内容
void printCard(BYTE *m_Card,int num,CString &str);

/// 求手中某种牌的个数
int  getSomeCount(BYTE *m_Card,int cardNum,int num);


/// 根据自己的运算规则，将可以配成的出牌牌型排序，从而判断一个新的牌型是否可以添加到配好的数据当中
bool canPush(std::vector<peiOutCardStruct>& myCard,peiOutCardStruct &out);
#endif