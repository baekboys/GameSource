#pragma once

/*
frame manager v1.0.0
수정:
2007.7.4 : 최초 버전 등록
*/

class CFrameMng
{
	irr::u32 m_uSyncTick;
	irr::IrrlichtDevice *m_pDevice;
	irr::u32 m_uFrameSyncDelay;
public:	
	CFrameMng(void)
	{
		m_uSyncTick = 0;
		m_uFrameSyncDelay = 0;
		m_pActiveFrame = NULL;
		m_pDevice = NULL;
	}

	CFrameMng(irr::IrrlichtDevice *pDevice,irr::u32 uFpsHz)
	{
		m_uSyncTick = 0;
		m_uFrameSyncDelay = 0;
		m_pActiveFrame = NULL;
		m_pDevice = pDevice;
		SetStaticFrameRate(uFpsHz);
		
	}

	CFrameMng(CEsenFrame *pFrame)
	{		
		m_uFrameSyncDelay = 0;
		m_pActiveFrame = pFrame;
		m_pActiveFrame->Init();
		m_pDevice = NULL;
		AddFrame(pFrame);
	}

	CFrameMng(CEsenFrame *pFrame,irr::IrrlichtDevice *pDevice)
	{		
		m_pActiveFrame = pFrame;
		m_pActiveFrame->Init();
		AddFrame(pFrame);
		m_pDevice = pDevice;
	}

public:
	~CFrameMng(void);	

	CEsenFrame *m_pActiveFrame;
	std::vector<CEsenFrame *> m_vtpFrame;	

	virtual void Render();
	virtual bool Apply(irr::u32 timeMs);
	
	void ChangeFrame(CEsenFrame *pFrame);	

	void AddFrame(CEsenFrame *pFrame);
	bool SetActiveFrame(LPCSTR strName);
	CEsenFrame *GetFrame(LPCSTR strName);
	bool DeleteFrame(LPCSTR strName);

	void SetStaticFrameRate(irr::u32 uHz)
	{
		if(uHz > 0)
			m_uFrameSyncDelay = 1000/uHz; //1khz 타이머
		else
			m_uFrameSyncDelay = 0;
	}

	CEsenFrame *GetActiveFrame()
	{
		return m_pActiveFrame;
	}
	
};
