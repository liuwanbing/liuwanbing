#include "StdAfx.h"
#include "UserManager.h"

CUserManager::CUserManager(void)
{
	m_dataPloor.reserve(3000);
}

CUserManager::~CUserManager(void)
{
}


void CUserManager::Add(int index, LPUSER data)
{
	map<int, LPUSER>::iterator it = m_Users.find(index);
	if (it == m_Users.end())
	{
		for (int i = 0; i < m_iSize; i++)
		{
			if (!m_dataPloor[i].bIsValid) m_dataPloor[i] = *data;
			m_Users[index] = &m_dataPloor[i];
		}
	}
}

void CUserManager::Del(int index)
{
	map<int, LPUSER>::iterator it = m_Users.find(index);
	if (it != m_Users.end())
	{
		//it.second->bIsValid = false;
	}
}

int CUserManager::Find(int index)
{
	return 0;
}
