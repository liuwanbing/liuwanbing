/*----------------------------------------------------------------
            // Copyright (C) 2007 尹学渊
            // 版权所有。
            //
            // 文件名：yxyDES.cpp
            // 文件功能描述：DES加密类 cpp文件
            //
            //
            // 创建人：尹学渊
            //
            // 修改人：
            // 修改描述：修正加密及解密左右顺序，修正待加密字符不足8位时填充字符问题，修正密钥不            // 足7位时数组越界问题
            //
            // 修改人：
            // 修改描述：
//----------------------------------------------------------------*/

#include "stdafx.h"
#include "yxyDES.h"

// permuted choice table (PC1)
const static int PC1_Table[56] = {
	57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};
// permuted choice key (PC2)
const static int PC2_Table[48] = {
	14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};
// number left rotations of pc1 
const static int Shift_Table[16] = {
	1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};
// initial permutation (IP)
const static int IP_Table[64] = {
	58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};
// expansion operation matrix (E)
static const int E_Table[48] = {
	32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
	 8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};
// The (in)famous S-boxes 
const static int S_Box[8][4][16] = {
	// S1
	14,  4,	13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
        15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
	// S2 
        15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
        13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
	// S3 
        10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
         1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
	// S4 
         7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
         3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
	// S5 
         2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
        11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
	// S6 
        12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
         4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
	// S7 
         4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
         6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
	// S8 
        13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
         2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};
// 32-bit permutation function P used on the output of the S-boxes 
const static char P_Table[32] = {
	16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
	2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25
};
// final permutation IP^-1 
const static char IPR_Table[64] = {
	40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41,  9, 49, 17, 57, 25
};

yxyDES::yxyDES()
{}
yxyDES::~yxyDES()
{}
void yxyDES::InitializeKey(string s)
{
        //convert 8 char-bytes key to 64 binary-bits
        char sz_64key[64] = {0};
        int iTmpBit[64] = {0};
        for(int i=0;i<64;i++)
        {
			int it = i>>3;
			if(it < s.length())
                iTmpBit[i] = (s[it]>>(i&7)) & 1;
			else
				iTmpBit[i] = 0;
                //a = 0x61 = 0110,0001
                //after this , a is 1000,0110

        }
        //let me convert it to right
        for(int j=0;j<8;j++)
                for(int k=0;k<8;k++)
                        sz_64key[8*j+k] = SingleBinaryToChar(iTmpBit[8*(j+1)-(k+1)]);
        //PC 1
        char sz_56key[56] = {0};
        for(int k=0;k<56;k++)
        {
                sz_56key[k] = sz_64key[PC1_Table[k]-1];
        }
        CreateSubKey(sz_56key);
}

string yxyDES::HexCharToBinary(char ch)
{
        switch(ch)
        {
                case '0':
                        return "0000";
                case '1':
                        return "0001";
                case '2':
                        return "0010";
                case '3':
                        return "0011";
                case '4':
                        return "0100";
                case '5':
                        return "0101";
                case '6':
                        return "0110";
                case '7':
                        return "0111";
                case '8':
                        return "1000";
                case '9':
                        return "1001";
                case 'a':
                        return "1010";
                case 'b':
                        return "1011";
                case 'c':
                        return "1100";
                case 'd':
                        return "1101";
                case 'e':
                        return "1110";
                case 'f':
                        return "1111";
                default:
                        return "";
        }
}

string yxyDES::HexIntToBinary(int i)
{
        switch(i)
        {
                case 0:
                        return "0000";
                case 1:
                        return "0001";
                case 2:
                        return "0010";
                case 3:
                        return "0011";
                case 4:
                        return "0100";
                case 5:
                        return "0101";
                case 6:
                        return "0110";
                case 7:
                        return "0111";
                case 8:
                        return "1000";
                case 9:
                        return "1001";
                case 10:
                        return "1010";
                case 11:
                        return "1011";
                case 12:
                        return "1100";
                case 13:
                        return "1101";
                case 14:
                        return "1110";
                case 15:
                        return "1111";
                default:
                        return "";
        }
}
string yxyDES::BinaryToString(char* szSource,int len,bool bType)
{
        //bType == true is Binary to Hex
        //else is Binary to Char
        int ilen;
        if(len % 8 != 0)
                return "";
        else
                ilen = len / 8;

        string s_return = "";
        for(int i=0;i<ilen;i++)
        {
                char szTmp8[8] = {0};
                int iCh = 0;
                memcpy(szTmp8,szSource+8*i,8);
                for(int j=0;j<8;j++)
                {
                        iCh+=SingleCharToBinary(szTmp8[j]) * pow(2.0,7-j);
                }
                if(bType)
                {
                        char buffer[16] = {0};
                        itoa(iCh,buffer,16);
                        //if the integer less than 16,insert a zero to buffer
                        if(iCh < 16)
                        {
                                char cTmp = buffer[0];
                                buffer[0] = '0';
                                buffer[1] = cTmp;
                        }
                        s_return += buffer[0];
                        s_return += buffer[1];
						buffer[0] = '\0';
						buffer[1] = '\0';
                }
                else
                {
                        char ch = (char)iCh;
                        s_return += ch;
                }
        }
		
        return s_return;
}

void yxyDES::CreateSubKey(char* sz)
{
        char szTmpL[28] = {0};
        char szTmpR[28] = {0};
        char szCi[28] = {0};
        char szDi[28] = {0};
        memcpy(szTmpL,sz,28);
        memcpy(szTmpR,sz+28,28);

        for(int i=0;i<16;i++)
        {
                //shift to left
                //Left 28 bits
                memcpy(szCi,szTmpL + Shift_Table[i],28 - Shift_Table[i]);
                memcpy(szCi + 28 - Shift_Table[i],szTmpL,Shift_Table[i]);
                //Right 28 bits
                memcpy(szDi,szTmpR + Shift_Table[i],28 - Shift_Table[i]);
                memcpy(szDi + 28 - Shift_Table[i],szTmpR,Shift_Table[i]);

                //permuted choice 48 bits key
                char szTmp56[56] = {0};
                memcpy(szTmp56,szCi,28);
                memcpy(szTmp56 + 28,szDi,28);
                for(int j=0;j<48;j++)
                {
                        SubKeys[i][j] = szTmp56[PC2_Table[j]-1];
                }
                //Evaluate new szTmpL and szTmpR
                memcpy(szTmpL,szCi,28);
                memcpy(szTmpR,szDi,28);
        }
}

void yxyDES::EncryptData(string s)
{
        char sz_IP[64] = {0};
        char sz_Li[32] = {0};
        char sz_Ri[32] = {0};
        char sz_Final64[64] = {0};
        char szCiphertextBinary[64] = {0};
        //IP
        InitialPermuteData(s,sz_IP,true);
        memcpy(sz_Li,sz_IP,32);
        memcpy(sz_Ri,sz_IP + 32,32);

        for(int i=0;i<16;i++)
        {
                FunctionF(sz_Li,sz_Ri,i);
        }
        //so D=LR

        memcpy(sz_Final64,sz_Ri,32);//原版为memcpy(sz_Final64,sz_Li,32)
        memcpy(sz_Final64 + 32,sz_Li,32);//原版为memcpy(sz_Final64 + 32,sz_Ri,32)

        //~IP
        for(int j=0;j<64;j++)
        {
                szCiphertextBinary[j] = sz_Final64[IPR_Table[j]-1];
        }
        memcpy(szCiphertext,BinaryToString(szCiphertextBinary,64,true).c_str(),16);
}

void yxyDES::DecryptData(string s)
{
        char sz_IP[64] = {0};
        char sz_Li[32] = {0};
        char sz_Ri[32] = {0};
        char sz_Final64[64] = {0};
        char szPlaintextBinary[64] = {0};
        //IP --- return is sz_IP
        InitialPermuteData(s,sz_IP,false);
        //divide the 64 bits data to two parts
        memcpy(sz_Ri,sz_IP,32); //exchange L to R
        memcpy(sz_Li,sz_IP + 32,32);  //exchange R to L

        //16 rounds F and xor and exchange
        for(int i=0;i<16;i++)
        {
                FunctionF(sz_Ri,sz_Li,15-i);//原版解密时FunctionF(sz_Li,sz_Ri,15-i)左右顺序弄反了
        }
        //the round 16 will not exchange L and R
        memcpy(sz_Final64,sz_Li,32);//原版 memcpy(sz_Final64,sz_Ri,32)左右顺序弄反
        memcpy(sz_Final64 + 32,sz_Ri,32);//原版 memcpy(sz_Final64 + 32,sz_Li,32)左右顺序弄反

        // ~IP
        for(int j=0;j<64;j++)
        {
                szPlaintextBinary[j] = sz_Final64[IPR_Table[j]-1];
        }
        memcpy(szPlaintext,BinaryToString(szPlaintextBinary,64,false).c_str(),8);
}

void yxyDES::FunctionF(char* sz_Li,char* sz_Ri,int iKey)
{
        char sz_48R[48] = {0};
        char sz_xor48[48] = {0};
        char sz_P32[32] = {0};
        char sz_Rii[32] = {0};
        char sz_Key[48] = {0};
        memcpy(sz_Key,SubKeys[iKey],48);
        ExpansionR(sz_Ri,sz_48R);
        XOR(sz_48R,sz_Key,48,sz_xor48);
        string s_Compress32 = CompressFuncS(sz_xor48);
        PermutationP(s_Compress32,sz_P32);
        XOR(sz_P32,sz_Li,32,sz_Rii);
        memcpy(sz_Li,sz_Ri,32);
        memcpy(sz_Ri,sz_Rii,32);
}

void yxyDES::InitialPermuteData(string s,char* Return_value,bool bType)
{
        //if bType==true is encrypt
        //else is decrypt
        if(bType)
        {
                char sz_64data[64] = {0};
                int iTmpBit[64] = {0};
                for(int i=0;i<64;i++)
                {
                        iTmpBit[i] = (s[i>>3]>>(i&7)) & 1;
                        //a = 0x61 = 0110,0001
                        //after this , a is 1000,0110

                }
                //let me convert it to right
                for(int j=0;j<8;j++)
                        for(int k=0;k<8;k++)
                                sz_64data[8*j+k] = SingleBinaryToChar(iTmpBit[8*(j+1)-(k+1)]);
                //IP
                char sz_IPData[64] = {0};
                for(int k=0;k<64;k++)
                {
                        sz_IPData[k] = sz_64data[IP_Table[k]-1];
                }
                memcpy(Return_value,sz_IPData,64);
        }
        else
        {
                string sz_64data;
                for(unsigned int ui=0;ui<s.length();ui++)
                {
                        char ch = s[ui];
                        sz_64data += HexCharToBinary(tolower(ch));
                }
                char sz_IPData[64] = {0};
                for(int i=0;i<64;i++)
                {
                        sz_IPData[i] = sz_64data[IP_Table[i]-1];
                }
                memcpy(Return_value,sz_IPData,64);
        }

}

void yxyDES::ExpansionR(char* sz,char* Return_value)
{
        char sz_48ER[48] = {0};
        for(int i=0;i<48;i++)
        {
                sz_48ER[i] = sz[E_Table[i]-1];
        }
        memcpy(Return_value,sz_48ER,48);
}

void yxyDES::XOR(char* sz_P1,char* sz_P2,int len,char* Return_value)
{
        char sz_Buffer[256] = {0};
        for(int i=0;i<len;i++)
        {
                sz_Buffer[i] = SingleBinaryToChar(SingleCharToBinary(sz_P1[i]) ^ SingleCharToBinary(sz_P2[i]));
        }
        memcpy(Return_value,sz_Buffer,len);
}

string yxyDES::CompressFuncS(char* sz_48)
{
        char sTmp[8][6] = {0};
        string sz_32 = "";
        for(int i=0;i<8;i++)
        {
                memcpy(sTmp[i],sz_48 + 6 * i,6);
                int iX = SingleCharToBinary(sTmp[i][0]) * 2 + SingleCharToBinary(sTmp[i][5]);
                int iY = 0;
                for(int j=1;j<5;j++)
                {
                        iY += SingleCharToBinary(sTmp[i][j]) * pow(2.0,4-j);
                }
                sz_32 += HexIntToBinary(S_Box[i][iX][iY]);
        }
        return sz_32;
}

void yxyDES::PermutationP(string s,char* Return_value)
{
        char sz_32bits[32] = {0};
        for(int i=0;i<32;i++)
        {
                sz_32bits[i] = s[P_Table[i]-1];
        }
        memcpy(Return_value,sz_32bits,32);
}

int yxyDES::SingleCharToBinary(char ch)
{
        if(ch == '1')
                return 1;
        else
                return 0;
}

char yxyDES::SingleBinaryToChar(int i)
{
        if(i == 1)
                return '1';
        else
                return '0';
}
void yxyDES::SetCiphertext(char* value)
{
        memcpy(szCiphertext,value,16);
}
char* yxyDES::GetCiphertext()
{
        return szCiphertext;
}

void yxyDES::SetPlaintext(char* value)
{
        memcpy(szPlaintext,value,8);
}
char* yxyDES::GetPlaintext()
{
        return szPlaintext;
}
//any length
void yxyDES::EncryptAnyLength(string szSource)
{
        int iLength = szSource.length();
        //if the length is 8 , call EncyptData
        if(iLength == 8)
        {
                EncryptData(szSource);
                memcpy(szFCiphertextAnyLength,szCiphertext,16);
                //set the last char to '\0'
                szFCiphertextAnyLength[16] = '\0';
        }
        //else if the length less than 8
        //call FillToEightBits
        else if(iLength < 8)
        {
                szSource = FillToEightBits(szSource);
                EncryptData(szSource);
                memcpy(szFCiphertextAnyLength,szCiphertext,16);
                //set the last char to '\0'
                szFCiphertextAnyLength[16] = '\0';
        }
        //else if then lenth bigger than 8
        //divide the string to multi-parts
        else if(iLength > 8)
        {
                int iParts = ceil(iLength/8.0);
                int iResidue = iLength % 8;
                string szLast8Bits;
                //encrypt the data 8 by 8 except the last part
                for(int i=0;i<iParts-1;i++)
                {
                        string szTmp = szSource.substr(i*8,8);
                        EncryptData(szTmp);
                        //after call EncryptData
                        //cpoy the temp result to szFCiphertextAnyLength
                        memcpy(szFCiphertextAnyLength + 16*i,szCiphertext,16);
                }
                //now , the last part
                if(iResidue != 0) //can't be divided exactly by 8
                {
                        szLast8Bits = szSource.substr((iParts-1)*8,iResidue);
                        szLast8Bits = FillToEightBits(szLast8Bits);
                }
                else //be divided exactly by 8
                {
                        szLast8Bits = szSource.substr((iParts-1)*8,8);
                }
                //encrypt the last part
                EncryptData(szLast8Bits);
                memcpy(szFCiphertextAnyLength + 16*(iParts - 1),szCiphertext,16);
                //set the last char to '\0'
                szFCiphertextAnyLength[(iParts)*16] = '\0';
        }
}

//fill the data to 8 bits
string yxyDES::FillToEightBits(string sz)
{
        //length less than 8 , add zero(s) to tail
	char temp[8]={0};
	for(int i = 0;i<sz.length();i++)//把SZ的内容存储到临时数组中
	{
		temp[i] = sz[i];
	}
	for(int j = sz.length();j<8;j++)  //增加SZ长度，
	{
		sz +="0";
	}
	for(int k = 0;k<8;k++)//把数组中的字符全部给SZ，相当于不足的填0
	{
		sz[k] = temp[k];
	}
	
        return sz;
}

char* yxyDES::GetCiphertextAnyLength()
{
        return szFCiphertextAnyLength;
}

void yxyDES::DecryptAnyLength(string szSource)
{
        int iLength = szSource.length();
		int iRealLengthOfPlaintext = 0;
        //if the length is 16 , call DecyptData
        if(iLength == 16)
        {
                DecryptData(szSource);
                memcpy(szFPlaintextAnyLength,szPlaintext,8);
				iRealLengthOfPlaintext = 8;
        }
        //it's not impossible the length less than 16
        else if(iLength < 16)
        {
                sprintf(szFPlaintextAnyLength,"待解密字符长度必须为16的倍数！");
        }
        //else if then lenth bigger than 16
        //divide the string to multi-parts
        else if(iLength > 16)
        {
                int iParts = ceil(iLength/16.0);
                int iResidue = iLength % 16;
                //if the data can't be divided exactly by 16
                //it's meaning the data is a wrong !
                if(iResidue != 0)
                {
                        sprintf(szFPlaintextAnyLength,"待解密字符长度必须为16的倍数！");
                        return;
                }
				iRealLengthOfPlaintext = iParts * 8;
                //Decrypt the data 16 by 16
                for(int i=0;i<iParts;i++)
                {
                        string szTmp = szSource.substr(i*16,16);
                        DecryptData(szTmp);
                        //after call DecryptData
                        //cpoy the temp result to szFPlaintextAnyLength
                        memcpy(szFPlaintextAnyLength + 8*i,szPlaintext,8);
                }
        }
        //find and clear the mark
        //which is add by program when ciphertext is less than 8
       CleanPlaintextMark(iRealLengthOfPlaintext);//清理末尾的美元符，这个应该用不上了
}

char* yxyDES::GetPlaintextAnyLength()
{
        return szFPlaintextAnyLength;
}

void yxyDES::CleanPlaintextMark(int iPlaintextLength)
{
		if(iPlaintextLength > 0 && iPlaintextLength < 8192)
		{
			szFPlaintextAnyLength[iPlaintextLength] = '\0';
		}
        char szLast7Chars[7] = {0};
        memcpy(szLast7Chars,szFPlaintextAnyLength + iPlaintextLength - 7,7);
        for(int i=0;i<7;i++)
        {
                //char* pDest = strrchr(szLast7Chars,'$');//原版的清除美元符号
				char* pDest =szLast7Chars;
                if(pDest == NULL)
                {
                        break;
                }
                else
                {
                        int iPos = (int)(pDest - szLast7Chars + 1);
                        if(iPos != 7 - i)
                        {
                                break;
                        }
                        else
                        {
                                szLast7Chars[6-i]='\0';
                                continue;
                        }
                }
        }
        memcpy(szFPlaintextAnyLength + iPlaintextLength - 7,szLast7Chars,7);
}