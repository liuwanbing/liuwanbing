/***************************************************************
**
**	basic algorithms for GDMJ
**
**	johhny
**
**	2011-11-15 10:08
**
****************************************************************/

#ifndef BASIC_ALGORITHMS_H
#define BASIC_ALGORITHMS_H

namespace basic_algorithms {

	struct ChiType_;
	typedef struct ChiType_ ChiType;

	struct ChiType_ {

		bool chi_tou;
		bool chi_zhong;
		bool chi_wei;
		void init_chitype() {

			chi_tou		= false;
			chi_zhong	= false;
			chi_wei		= false;
		}
	};

	bool CanChi(int hand_cards[], int num, int card, ChiType *p_type);

	bool CanPeng(int hand_cards[], int num, int card);

	bool CanGang(int hand_cards[], int num, int card);

	bool CanHu(int hand_cards[], int num, int card);

	bool IsBasicHu(int allPai[4][10]);

	bool Analyze(int aKindPai[], int zipai);

	bool IsLegal(int card);

	bool IsThreeCardsExist(int card1, int card2, int card3);

	bool IsSubArrExist(int arr[], int num, int sub_arr[], int sub_num);

	int FindLowValueCard(int arr[], int num, int caishen);

	bool IsArrInSameGrap(int arr[], int num);
};

#endif //basic_algorithms.h