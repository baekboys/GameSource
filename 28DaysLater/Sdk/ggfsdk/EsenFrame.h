#pragma once

class CEsenFrame
{
public:

	BYTE m_bufParam[4096];
	//static void *m_pFrameMng;

	bool m_bEnable;
	bool m_bPause;
	int m_nPhase; //상태구분
	int m_nStep;  //상태안에서의 절차또는순서구분
	std::string m_strName; //프레임이름

	CEsenFrame(LPCSTR name)
	{		
		m_bPause = m_bEnable = false;
		m_nPhase = 0;
		m_nStep = 0;		
		m_strName = name;
	}

	CEsenFrame()
	{		
		m_bEnable = false;
		m_nPhase = 0;
		m_nStep = 0;
		m_strName = "noname";
	}
	
public:
	virtual ~CEsenFrame(void)
	{
		//ggf::irr_util::DebugOutputFmt(NULL,"%s delete\n",m_strName.c_str());
	}
	
	virtual void Init() = 0;
	virtual void Release() =0;
	virtual void Apply(irr::u32 timeMs)=0;
	virtual void Render()=0;
	virtual void LoadRes()=0;
	virtual bool OnEvent(irr::SEvent ev) = 0;	
	virtual void DecodePacket(irr::s32 nCode,void *pBuf) = 0;	

	virtual void SetPhase(int nPhase,void *pParam)
	{
		m_nPhase = nPhase;
		m_nStep = 0;
	}

	int GetPhase()
	{
		return m_nPhase;
	}

	
};
