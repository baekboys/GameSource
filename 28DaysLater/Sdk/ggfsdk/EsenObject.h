
class CEsenObject
{
private:
	irr::u32 m_uWorkTick;

public:
	bool m_bEnable;
	int m_nStatus;
	int m_nWork;
	

	CEsenObject()
	{
		m_bEnable = false;
	}
	virtual ~CEsenObject()
	{
	}

	int GetStatus()
	{
		return m_nStatus;
	}

	void resetDeltaTick(irr::u32 uTick)
	{
		m_uWorkTick = uTick;		
	}

	irr::u32 getDeltaTick(irr::u32 uTick)
	{
		if(uTick > m_uWorkTick)
			return (uTick - m_uWorkTick);
		else
		{
			m_uWorkTick = uTick;
			return 0;
		}
	}


	virtual void SetStatus(int nStatus,void *pParam=NULL)
	{
		m_nStatus = nStatus;
		m_nWork = 0;
	}

	virtual void  Apply(irr::u32 timeMS) = NULL;	
	virtual void  Render() = NULL;
};
