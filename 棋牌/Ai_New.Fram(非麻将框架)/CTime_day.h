#include "stdafx.h"
#include "resource.h"


#pragma once

struct TTime_day
{
	int iHour;
	int iMinute;
	TTime_day()
	{
		iHour = 0;
		iMinute = 0;
	}
	TTime_day(int h, int m)
	{
		iHour = h;
		iMinute = m;
	}
	CString getTimeStr()
	{
		CString s;
		s.Format("%d ±%d∑÷", iHour, iMinute);
		return s;
	}
	bool GetValid()
	{
		return (0 <= iHour && iHour < 24 && 0 <= iMinute && iMinute < 60);
	}
};

static int CmpTime_dat(const TTime_day& t1, const TTime_day& t2)
{
	return t1.iHour * 60 + t1.iMinute - (t2.iHour * 60 + t2.iMinute);
}
