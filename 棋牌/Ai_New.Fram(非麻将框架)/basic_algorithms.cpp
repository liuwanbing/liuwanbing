#include "stdafx.h"
#include "basic_algorithms.h"

namespace basic_algorithms {

	bool CanChi(int hand_cards[], int num, int card, ChiType *p_type)
	{
		int tou[3];
		tou[0] = card;
		tou[1] = card+1;
		tou[2] = card+2;
		if (IsThreeCardsExist(tou[0], tou[1], tou[2]) &&
			IsSubArrExist(hand_cards, num, tou, 3)) {

				p_type->chi_tou = true;
		}

		int zhong[3];
		zhong[0] = card-1;
		zhong[1] = card;
		zhong[2] = card+1;
		if (IsThreeCardsExist(zhong[0], zhong[1], zhong[2]) &&
			IsSubArrExist(hand_cards, num, zhong, 3)) {

				p_type->chi_zhong = true;
		}

		int wei[3];
		wei[0] = card-2;
		wei[1] = card-1;
		wei[2] = card;
		if (IsThreeCardsExist(wei[0], wei[1], wei[2]) &&
			IsSubArrExist(hand_cards, num, wei, 3)) {

				p_type->chi_wei = true;
		}

		return (p_type->chi_tou || p_type->chi_zhong || p_type->chi_wei);
	}

	bool CanPeng(int hand_cards[], int num, int card)
	{
		if (!IsLegal(card)) return false;

		int num_chi = 0;
		for (int i = 0; i < num; i ++) {

			if (card == hand_cards[i])
				num_chi ++;
		}
		return (num_chi>=3);
	}

	bool CanGang(int hand_cards[], int num, int card)
	{
		if (!IsLegal(card)) return false;

		int num_gang = 0;
		for (int i = 0; i < num; i ++) {

			if (card == hand_cards[i])
				num_gang++;
		}
		return (num_gang==4);
	}

	bool CanHu(int hand_cards[], int num, int card)
	{
		if (!IsLegal(card)) return false;

		int *p_arr = new int [num];
		int i = 0;
		for (i = 0; i < num; i ++)
			p_arr[i] = hand_cards[i];
		p_arr[i] = card;

		int hu_cards[4][10];
		for (int i = 0; i < 4; i ++)
			for (int j = 0; j < 10; j ++)
				hu_cards[i][j] = 0;

		return IsBasicHu(hu_cards);
	}

	bool IsBasicHu(int allPai[4][10])
	{
		int jiangPos;			//将的位置
		int yuShu;			//余下的数
		bool jiangExisted = false;	//将是否存在

		//是否满足3, 3, 3, 3, 2的模式
		int i = 0;
		for (i = 0; i < 4; i ++) {

			yuShu = allPai[i][0] % 3;

			if (yuShu == 1) {

				return false;
			}

			if (yuShu == 2) {

				if (jiangExisted) {

					return false;
				}
				jiangPos = i;
				jiangExisted = true;
			}
		}

		for (i = 0; i < 4; i ++) {

			if (i != jiangPos) {

				if (!Analyze(allPai[i], i == 3)) {

					return false;
				}
			}
		}

		bool success = false;
		int j = 1;
		for (j = 1; j < 10; j ++) {

			if (allPai[jiangPos][j] >= 2) {

				//除去这两张牌
				allPai[jiangPos][j] -= 2;
				allPai[jiangPos][0] -= 2;

				if (Analyze(allPai[jiangPos], jiangPos == 3)) {

					success = true;
				}

				//还原这两张牌
				allPai[jiangPos][j] += 2;
				allPai[jiangPos][0] += 2;
				if (success)
					break;
			}	
		}

		return success;
	}

	bool Analyze(int aKindPai[], int zipai)
	{
		if (aKindPai[0] == 0) {

			return true;	
		}

		//寻找第一张字牌
		int j = 1;
		for (j = 1; j < 10; j ++) {

			if (aKindPai[j] != 0) 
				break;
		}

		bool result;
		if (aKindPai[j] >= 3)	//作为壳子
		{
			aKindPai[j] -= 3;
			aKindPai[0] -= 3;
			result = Analyze(aKindPai, zipai);
			//还原这三张
			aKindPai[j] += 3;
			aKindPai[0] +=3;
			return result;
		}

		//作为顺牌
		if ((!zipai) && (j < 8) 
			&& (aKindPai[j+1] > 0)
			&& (aKindPai[j+2]) > 0)
		{
			//除去这三张
			aKindPai[j] --;
			aKindPai[j+1] --;
			aKindPai[j+2] --;
			aKindPai[0] -= 3;
			result = Analyze(aKindPai, zipai);
			//还原这三张顺子
			aKindPai[j] ++;
			aKindPai[j+1] ++;
			aKindPai[j+2] ++;
			aKindPai[0] += 3;
			return result;
		}

		return false;
	}

	bool IsLegal(int card) 
	{
		if ((card>=1 && card<=9) || (card>=11 && card<=19) ||(card>=21 && card<=29) || (card>=31 && card<=37))
			return true;
		return false;
	}

	bool IsThreeCardsExist(int card1, int card2, int card3)
	{
		return (IsLegal(card1) && IsLegal(card2) && IsLegal(card3));
	}

	bool IsSubArrExist(int arr[], int num, int sub_arr[], int sub_num)
	{
		if (num < sub_num) return false;
		bool *p_sub = new bool [sub_num];
		for (int i = 0; i < sub_num; i ++)
			p_sub[i] = false;

		int *p_arr = new int [num];
		for (int i = 0; i < num; i ++)
			p_arr[i] = arr[i];

		for (int i = 0; i < sub_num; i ++) {

			int temp = sub_arr[i];
			for (int j = 0; j < num; j ++) {

				if (temp == p_arr[j]) {

					p_arr[j] = 255;
					p_sub[i] = true;
					break;
				}
			}
		}

		bool b_return = true;
		for (int i = 0; i < sub_num; i ++)
			b_return |= p_sub[i];

		delete [] p_sub;
		delete [] p_arr;

		return b_return;
	}

	int FindLowValueCard(int arr[], int num, int caishen) 
	{
		int temp = 255;
		int bfind = false;
		for (int i = 0; i < num; i ++) {

			temp = arr[i];
			int kezi[3];
			int shunzi1[3];
			int shunzi2[3];
			int shunzi3[3];
			int duizi[2];
			kezi[0] = temp; kezi[1] = temp; kezi[2] = temp;
			shunzi1[0] = temp; shunzi1[1] = temp+1; shunzi1[2] = temp+2;
			shunzi2[0] = temp-1; shunzi2[1] = temp; shunzi2[2] = temp+1;
			shunzi3[0] = temp-2; shunzi3[1] = temp-1; shunzi3[2] = temp;
			duizi[0] = temp; duizi[1] = temp;

			if (temp == caishen) {

				continue;
			}

			if (IsLegal(kezi[0]) && IsSubArrExist(arr, num, kezi, 3)) {

				continue;
			}

			if (IsThreeCardsExist(shunzi1[0], shunzi1[1], shunzi1[2]) && 
				IsSubArrExist(arr, num, shunzi1, 3) && IsArrInSameGrap(shunzi1, 3)) {

					continue;
			}

			if (IsThreeCardsExist(shunzi2[0], shunzi2[1], shunzi2[2]) && 
				IsSubArrExist(arr, num, shunzi2, 3) && IsArrInSameGrap(shunzi2, 3)) {

					continue;
			}

			if (IsThreeCardsExist(shunzi3[0], shunzi3[1], shunzi3[2]) && 
				IsSubArrExist(arr, num, shunzi3, 3) && IsArrInSameGrap(shunzi3, 3)) {

					continue;
			}

			if (IsLegal(duizi[0]) && IsSubArrExist(arr, num, duizi, 2)) {

				continue;
			}

			if (temp>=31 && temp<=37)
				return temp;
			bfind = true;
		}

		if (!bfind) {

			for (int i = 0; i < num; i ++) {

				if (arr[i] != caishen) {

					temp = arr[i];
					break;
				}
			}
		}

		return temp;
	}

	bool IsArrInSameGrap(int arr[], int num) 
	{
		bool bSameGrap1 = false;
		bool bSameGrap2 = false;
		bool bSameGrap3 = false;
		for (int i = 0; i < num; i ++) {

			if (arr[i]>=1 && arr[i]<=9)
				bSameGrap1 = true;
			else {

				bSameGrap1 = false; break;
			}
		}

		for (int i = 0; i < num; i ++) {

			if (arr[i]>=11 && arr[i]<=19) 
				bSameGrap2 = true;
			else {

				bSameGrap2 = false; break;
			}
		}

		for (int i = 0; i < num; i ++) {

			if (arr[i]>=21 && arr[i]<=29)
				bSameGrap3 = true;
			else {

				bSameGrap3 = false; break;
			}
		}

		return (bSameGrap1 || bSameGrap2 || bSameGrap3);
	}
};