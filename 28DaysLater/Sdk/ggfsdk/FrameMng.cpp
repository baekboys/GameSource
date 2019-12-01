//#include "stdafx.h"
//#include "kogg5.h"

#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <mmsystem.h>
#include <comutil.h>


#include <stdio.h>

#include <irrlicht.h>

#include <string>
#include <vector>
#include <deque>

using namespace std;


#include "EsenFrame.h"
#include "EsenObject.h"
#include "FrameMng.h"

CFrameMng::~CFrameMng()
{
	vector<CEsenFrame *>::iterator it;

	for(it = m_vtpFrame.begin();it != m_vtpFrame.end();it++)
	{
		delete *it;
	}


	m_vtpFrame.clear();

}


void CFrameMng::Render()
{
	if(m_pActiveFrame != NULL)
	{
		if(m_pActiveFrame->m_bEnable)
			m_pActiveFrame->Render();		
	}
}


bool CFrameMng::Apply(irr::u32 timeMs)
{	
	
	if(m_pActiveFrame != NULL)
	{
		if(m_pActiveFrame->m_bEnable && !m_pActiveFrame->m_bPause )
		{
			if(m_pDevice && m_uFrameSyncDelay != 0) //정해진 프레임수만큼
			{

				irr::ITimer *pTimer = m_pDevice->getTimer();
				irr::u32 uTick = pTimer->getRealTime();
				
				if( (uTick - m_uSyncTick) > m_uFrameSyncDelay )
				{
					m_uSyncTick = uTick;
					m_pActiveFrame->Apply(timeMs);				
				}
			}
			else//시스템이허용하는한최대..
			{
				m_pActiveFrame->Apply(timeMs);
			}
		}		
		return true;
	}	

	return false;
}


void CFrameMng::ChangeFrame(CEsenFrame *pFrame)
{
	if(m_pActiveFrame != NULL)
	{
		m_pActiveFrame->Release();
	}

	m_pActiveFrame = pFrame;

	m_pActiveFrame->Init();
}

void CFrameMng::AddFrame(CEsenFrame *pFrame)
{
	//pFrame->LoadRes();	
	//if(pFrame->m_strName == "aaa");
	m_vtpFrame.push_back(pFrame);

	m_pActiveFrame = pFrame;

	//if(m_pActiveFrame->m_strName == "aaa");
	

}

bool CFrameMng::SetActiveFrame(LPCSTR strName)
{
	CEsenFrame *pFrame = GetFrame(strName);

	if(pFrame)
	{
		ChangeFrame(pFrame);
		return true;
	}

	return false;

}

CEsenFrame *CFrameMng::GetFrame(LPCSTR strName)
{
	vector<CEsenFrame *>::iterator it;	

	for(it=m_vtpFrame.begin();it != m_vtpFrame.end();it++)
	{	
		if( (*it)->m_strName == strName)
		{
			return (*it);
		}
	}

	return NULL;

}

bool CFrameMng::DeleteFrame(LPCSTR strName)
{	

	vector<CEsenFrame *>::iterator it;	

	for(it=m_vtpFrame.begin();it != m_vtpFrame.end();it++)
	{		
		
		if((*it)->m_strName == strName)
		{
			delete (*it);
			m_vtpFrame.erase(it);
			return true;
		}
	}
	return false;
}