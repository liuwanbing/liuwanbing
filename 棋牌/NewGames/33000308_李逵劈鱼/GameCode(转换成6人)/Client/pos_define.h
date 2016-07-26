
#ifndef POS_DEFINE_H_
#define POS_DEFINE_H_

// 自己信息高度
const int kBottomHeight = 40;

// 炮背底座	
const FPoint kPosBoard[GAME_PLAYER] = { {233,47+32}, {700,47+32}, {1155,47+32}, {1137,721-kBottomHeight}, {670,721-kBottomHeight}, {215,721-kBottomHeight}, {1319,397}, {49,371} };

// 炮辅助圈DWJ
const FPoint kPosDeco[GAME_PLAYER] = { {233,41+32}, {700,41+32}, {1155,41+32}, {1137,727-kBottomHeight}, {670,727-kBottomHeight}, {215,727-kBottomHeight}, {1325,397}, {42,371} };

// 炮后底座DWJ
const FPoint kPosSeat[GAME_PLAYER] = { {233,21+32}, {700,21+32}, {1155,21+32}, {1137,748-kBottomHeight}, {670,748-kBottomHeight}, {215,748-kBottomHeight}, {1345,397}, {22,371} };

// 炮筒子dwj-
const FPoint kPosGun[GAME_PLAYER] = { {233,67+32}, {700,67+32}, {1155,67+32}, {1137,701-kBottomHeight}, {670,701-kBottomHeight}, {215,701-kBottomHeight}, {1299,397}, {67,371} };

// 炮倍数	DWJ
const FPoint kPosCannonMul[GAME_PLAYER] = { {233,25+32}, {700,25+32}, {1155,25+32}, {1137,742-kBottomHeight}, {670,742-kBottomHeight}, {215,742-kBottomHeight}, {1340,397}, {25,371} };

// 分值框dwj
const FPoint kPosScoreBox[GAME_PLAYER] = { {80,45+32}, {528,45+32}, {995,45+32}, {1295,723-kBottomHeight}, {828,723-kBottomHeight}, {375,723-kBottomHeight}, {1321,236},{46,530} };

// 分数	DWJ
const FPoint kPosScore[GAME_PLAYER] = { {22,45+32}, {470,45+32}, {935,45+32}, {1353,723-kBottomHeight}, {888,723-kBottomHeight}, {434,723-kBottomHeight}, {1321,176}, {46,590} };

// 金币堆
const FPoint kPosStack[GAME_PLAYER] = { {358,0+32}, {825,0+32}, {1280,0+32}, {1012,768-kBottomHeight}, {545,768-kBottomHeight}, {90,768-kBottomHeight}, {1366,522}, {0,246} };

// 能量炮卡片DWJ
const FPoint kPosCardIon[GAME_PLAYER] = { {180,162+32}, {650,162+32}, {1100,162+32}, {1190,606-kBottomHeight}, {720,606-kBottomHeight}, {270,606-kBottomHeight}, {1204,345}, {162,422} };

// 锁定卡片
const FPoint kPosLockFlag[GAME_PLAYER] = { {286,162+32}, {750,162+32}, {1210,162+32}, {1085,606-kBottomHeight}, {620,606-kBottomHeight}, {160,606-kBottomHeight}, {1204,450}, {162,320} };

#endif	// POS_DEFINE_H_