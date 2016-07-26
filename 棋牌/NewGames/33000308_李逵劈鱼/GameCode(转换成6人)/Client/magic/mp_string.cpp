//-----------------------------------------------------------------------------
// File: mp_string.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "mp_string.h"
#include <String.h>
#include <stdlib.h>

// eng: Constructors
// rus: Консруктор?
MP_String::MP_String()
{
	theString=new char[1];
	*theString=0;
	stringLen=0;
}

MP_String::MP_String(char s)
{
	theString=new char[2];
	theString[0]=s;
	theString[1]='\0';
	stringLen=1;
}

MP_String::MP_String(const char* s)
{
	int len=(int)strlen(s);
	theString=new char [len+1];
	strcpy(theString,s);
	stringLen=len;
	theString[len]=0;
}

MP_String::MP_String(const MP_String& so)
{
	theString=new char[so.stringLen+1];
	strcpy(theString,so.theString);
	theString[so.stringLen]=0;
	stringLen=so.stringLen;
}

// eng: Destructor
// rus: Деструктор
MP_String::~MP_String(void)
{
	if (theString)
		delete []theString;
}

MP_String& MP_String::operator=(const MP_String& so)
{
	if (this!=&so)
	{
		if (theString)
			delete []theString;
		theString=(char*) new char[so.stringLen+1];
		strcpy(theString,so.theString);
		stringLen=so.stringLen;
	}
	return *this;
}

MP_String& MP_String::operator=(char* s)
{
	int len=(int)strlen(s);
	if (theString)
		delete []theString;
	theString=(char*) new char[len+1];
	strcpy(theString,s);
	stringLen=len;
	return *this;
}

bool MP_String::operator==(const MP_String& so)
{
	int dl1=this->stringLen;
	int dl2=so.stringLen;
	if (dl1!=dl2)
		return false;
	char* s1=this->theString;
	char* s2=so.theString;
	for (int i=0;i<dl1;i++)
		if (s1[i]!=s2[i])
			return false;
	return true;
}

bool MP_String::operator!=(const MP_String& so)
{
	if (strcmp(this->theString,so.theString)==0)
		return false;
	return true;
}

bool MP_String::operator>(const MP_String& so)
{
	if (strcmp(this->theString,so.theString)>0)
		return true;
	return false;
}

bool MP_String::operator<(const MP_String& so)
{
	if (strcmp(this->theString,so.theString)<0)
		return true;
	return false;
}

char& MP_String::operator[](int n)
{
	if (n<0)
		return theString[0];
	else
		if (n>stringLen)
			return theString[stringLen-1];
		else
			return theString[n];
}

MP_String MP_String::operator+(const MP_String& so)
{
	int len=stringLen+so.stringLen;
	MP_String temp;
	delete []temp.theString;
	temp.theString=new char[len+1];
	strcpy(temp.theString,this->theString);
	strcat(temp.theString,so.theString);
	temp.stringLen=len;
	return temp;
}

MP_String MP_String::operator+(const char c)
{
	int len=stringLen+1;
	MP_String temp;
	delete []temp.theString;
	temp.theString=new char[len+1];
	strcpy(temp.theString,this->theString);
	*(temp.theString+len-1)=c;
	*(temp.theString+len)='\0';
	temp.stringLen=len;
	return temp;
}

MP_String& MP_String::operator+=(const MP_String& so)
{
	int len=stringLen+so.stringLen;
	char* temp=new char[len+1];
	strcpy(temp,this->theString);
	strcat(temp,so.theString);
	*this=temp;
	delete []temp;
	return *this;
}

MP_String& MP_String::operator+=(const char c)
{
	int len=stringLen+1;
	char* temp=new char[len+1];
	strcpy(temp,this->theString);
	temp[len-1]=c;
	temp[len]='\0';
	*this=temp;
	delete []temp;
	return *this;
}

// eng: Loading string from stream
// rus: Чтение строки из потока
void MP_String::LoadFromStream(FILE* stream)
{
	short dln;
    (void )fread((void*)&dln,sizeof(short),1,stream);
	if (dln!=stringLen)
	{
		delete []theString;
		theString=new char[dln+1];
		stringLen=dln;
	}
	if (dln)
		(void)fread((void*)theString,sizeof(char),dln,stream);
	theString[dln]=0;
}

// eng: Save string to stream
// rus: Запись строки ?пото?
void MP_String::SaveToStream(FILE* stream)
{
	short dln=(short)stringLen;
	(void )fwrite((void*)&dln,sizeof(short),1,stream);
    if (dln)
	{
		(void)fwrite((void*)theString,sizeof(char),dln,stream);
	}
}

// eng: Save string to stream as text
// rus: Запись строки ?пото??виде текста
void MP_String::SaveToStreamByText(FILE* stream)
{
	(void)fwrite((void*)theString,sizeof(char),stringLen,stream);
}

#ifndef MP_STRING_LITE
// eng: Reversing
// rus: Реверсирование
MP_String& MP_String::operator~(void)
{
	strrev(theString);
	return *this;
}
#endif

// eng: String length
// rus: Длин?строки
int MP_String::length(void)
{
	return stringLen;
}

#ifndef MP_STRING_LITE
// eng: Transformation to capital letters
// rus: Преобразование ?прописны?букв?
MP_String& MP_String::toUpper(void)
{
	strupr(theString);
	return *this;
}

// eng: Transformation to small letters
// rus: Преобразование ?строчные букв?
MP_String& MP_String::toLower(void)
{
	strlwr(theString);
	return *this;
}
#endif

// eng: Find symbol in string
// rus: Поис?символ??строке
int MP_String::find(char c)
{
	char* s=strchr(theString,c);
	if (s==NULL)
   		return -1;
    else
      	return (int)(s-theString);
}

// eng: Find substring in string
// rus: Поис?подстрок??строке
int MP_String::find(const char* subs)
{
	char* s=strstr(theString,subs);
	if (s==NULL)
   		return -1;
    else
      	return (int)(s-theString);
}

int MP_String::find(const MP_String& so)
{
	char* s=strstr(theString,so.theString);
	if (s==NULL)
   		return -1;
    else
      	return (int)(s-theString);
}

// eng: String pointer
// rus: Адре?строки
const char* MP_String::c_str()
{
	return (const char*)theString;
}

// eng: Returns substring using specified position and length
// rus: Получени?подстрок?из данной строки ?заданной позици??известны?числом символов
MP_String MP_String::substr(int pos,int plen)
{
	MP_String temp;
	delete []temp.theString;
	temp.theString=new char[plen+1];
	temp.theString[0]=0;
	temp.stringLen=plen;
	for (int i=0;i<plen;i++)
   		temp.theString[i]=theString[pos+i];
	*(temp.theString+plen)='\0';
	return temp;
}

// eng: Left substring
// rus: Левая подстрок?
MP_String MP_String::left(int len)
{
	return substr(0,len+1);
}

// eng: Rigth substring
// rus: Прав? подстрок?
MP_String MP_String::right(int len)
{
	return substr(length()-len,len);
}

bool MP_String::empty()
{
	return (length()==0);
}


// ------------------------------------------------------------------------------------------------------------------------

// ru:Возвращает длин?строки ?количестве wchar-ов, ?не ?количестве символов (один символ може?занимать нескольк?wchar-ов)
// en:Returns string length in wchar numbers
int wchar_length(const wchar_t* str)
{
	int len=0;
	int s=sizeof(wchar_t);
	if (s==1)
	{
		const char* v=(const char*)str;
		while (v[len])
			len++;
	}
	else
	{
		const short* v=(const short*)str;
		while (v[len])
			len++;
	}
	return len;
}

// eng: Constructors
// rus: Консруктор?
MP_StringW::MP_StringW()
{
	theString=new wchar_t[1];
	*theString=0;
	stringLen=0;
}

MP_StringW::MP_StringW(char s)
{
	char vs[2];
	vs[0]=s;
	vs[1]=0;

	wchar_t v[7];
	if (mbstowcs(v,vs,7)!=-1)
	{
		int len=wchar_length(v);

		theString=new wchar_t[len+1];
		wcscpy(theString,v);

		stringLen=1;
	}
	else
	{
		theString=new wchar_t[1];
		*theString=0;
		stringLen=0;
	}
}

MP_StringW::MP_StringW(const char* s)
{
	int str_len=(int)strlen(s);
	if (str_len)
	{
		wchar_t* v=new wchar_t[str_len*7];
		if (mbstowcs(v,s,str_len*7)!=-1)
		{
			int len=wchar_length(v);

			theString=new wchar_t[len+1];
			wcscpy(theString,v);

			stringLen=str_len;
		}
		else
		{
			theString=new wchar_t[1];
			*theString=0;
			stringLen=0;
		}

		delete []v;
	}
	else
	{
		theString=new wchar_t[1];
		*theString=0;
		stringLen=0;
	}
}

MP_StringW::MP_StringW(const wchar_t* s)
{
	int len=wchar_length(s);
	theString=new wchar_t[len+1];
	wcscpy(theString,s);
	stringLen=len;
	theString[len]=0;
}

MP_StringW::MP_StringW(const MP_StringW& so)
{
	int len=wchar_length(so.theString);
	theString=new wchar_t[len+1];
	wcscpy(theString,so.theString);
	theString[so.stringLen]=0;
	stringLen=so.stringLen;
}

// eng: Destructor
// rus: Деструктор
MP_StringW::~MP_StringW(void)
{
	if (theString)
		delete []theString;
}

MP_StringW& MP_StringW::operator=(const MP_StringW& so)
{
	if (this!=&so)
	{
		if (theString)
			delete []theString;
		int len=wchar_length(so.theString);
		theString=new wchar_t[len+1];
		wcscpy(theString,so.theString);
		stringLen=so.stringLen;
	}
	return *this;
}

MP_StringW& MP_StringW::operator=(const wchar_t* s)
{
	int len=wchar_length(s);
	if (theString)
		delete []theString;
	theString=new wchar_t[len+1];
	wcscpy(theString,s);
	stringLen=(int)wcslen(theString);
	return *this;
}

MP_StringW& MP_StringW::operator=(const char* s)
{
	if (theString)
		delete []theString;

	int str_len=(int)strlen(s);
	if (str_len)
	{
		wchar_t* v=new wchar_t[str_len*7];
		if (mbstowcs(v,s,str_len*7)!=-1)
		{
			int len=wchar_length(v);

			theString=new wchar_t[len+1];
			wcscpy(theString,v);

			stringLen=str_len;
		}
		else
		{
			theString=new wchar_t[1];
			*theString=0;
			stringLen=0;
		}

		delete []v;
	}
	else
	{
		theString=new wchar_t[1];
		*theString=0;
		stringLen=0;
	}
	return *this;
}

bool MP_StringW::operator==(const MP_StringW& so)
{
	if (wcscmp(theString,so.theString)==0)
		return true;
	return false;
}

bool MP_StringW::operator!=(const MP_StringW& so)
{
	if (wcscmp(theString,so.theString)==0)
		return false;
	return true;
}

bool MP_StringW::operator>(const MP_StringW& so)
{
	if (wcscmp(theString,so.theString)>0)
		return true;
	return false;
}

bool MP_StringW::operator<(const MP_StringW& so)
{
	if (wcscmp(theString,so.theString)<0)
		return true;
	return false;
}

MP_StringW MP_StringW::operator+(const MP_StringW& so)
{
	int len_so=wchar_length(so.theString);
	int len=wchar_length(theString)+len_so;
	MP_StringW temp;
	delete []temp.theString;
	temp.theString=new wchar_t[len+1];
	wcscpy(temp.theString,theString);
	wcscat(temp.theString,so.theString);
	temp.stringLen=(int)wcslen(temp.theString);
	return temp;
}

MP_StringW& MP_StringW::operator+=(const MP_StringW& so)
{
	int len_so=wchar_length(so.theString);
	int len=wchar_length(theString)+len_so;
	wchar_t* temp=new wchar_t[len+1];
	wcscpy(temp,this->theString);
	wcscat(temp,so.theString);
	*this=temp;
	delete []temp;
	return *this;
}

// eng: Loading string from stream
// rus: Чтение строки из потока
void MP_StringW::LoadFromStream(FILE* stream)
{
	short dln;
    (void )fread((void*)&dln,sizeof(short),1,stream);
	int len=wchar_length(theString);
	if (dln!=len)
	{
		delete []theString;
		theString=new wchar_t[len+1];
	}
	if (dln)
		(void)fread(theString,sizeof(wchar_t),dln,stream);

	theString[dln]=0;
	stringLen=(int)wcslen(theString);
}

// eng: Save string to stream
// rus: Запись строки ?пото?
void MP_StringW::SaveToStream(FILE* stream)
{
	short dln=(short)wchar_length(theString);
	(void )fwrite((void*)&dln,sizeof(short),1,stream);
    if (dln)
	{
		(void)fwrite((void*)theString,sizeof(wchar_t),dln,stream);
	}
}

#ifndef MP_STRING_LITE
// eng: Reversing
// rus: Реверсирование
MP_StringW& MP_StringW::operator~(void)
{
	wcsrev(theString);
	return *this;
}
#endif

// eng: String length
// rus: Длин?строки
int MP_StringW::length(void)
{
	return stringLen;
}

#ifndef MP_STRING_LITE
// eng: Transformation to capital letters
// rus: Преобразование ?прописны?букв?
MP_StringW& MP_StringW::toUpper(void)
{
	wcsupr(theString);
	return *this;
}

// eng: Transformation to small letters
// rus: Преобразование ?строчные букв?
MP_StringW& MP_StringW::toLower(void)
{
	wcslwr(theString);
	return *this;
}
#endif

// eng: Find substring in string
// rus: Поис?подстрок??строке
int MP_StringW::find(const wchar_t* subs)
{
	wchar_t* s=wcsstr(theString,subs);
	if (s==NULL)
		return -1;
    else
	{
		*s=0;
		int pos=(int)wcslen(theString);
		*s=*subs;
		return pos;
	}
}

int MP_StringW::find(const MP_StringW& so)
{
	return find(so.theString);
}

// eng: String pointer
// rus: Адре?строки
const wchar_t* MP_StringW::w_str()
{
	return (const wchar_t*)theString;
}

bool MP_StringW::empty()
{
	return (length()==0);
}

