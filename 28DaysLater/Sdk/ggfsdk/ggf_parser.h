/*
#pragma once



namespace ggf
{
namespace parser
{
typedef std::vector<std::string> VectorString;
typedef std::vector<int> VectorInt;
typedef std::vector<float> VectorFloat;

//기능확장 함수구현

inline void GetAttrib(const char * szBuf,int &i)
{	
	i = atoi(szBuf);
}

inline void GetAttrib(const char * szBuf,double &db)
{	
	db = atof(szBuf);
}

inline void GetAttrib(const char * szBuf,float &db)
{	
	db = (float)atof(szBuf);
}


inline void GetAttrib(const char * szBuf,VectorString &vs)
{
	//std::vector<std::string>::iterator it;

	std::string delimiter = ",\t"; 
	std::string keep_delim = ";:";  
	std::string quote = "\'\""; 
	std::string esc = "\\#";  
	std::string input;
	VectorString tokens;
	VectorString::iterator itTemp;

	//input = GetAttrib(szAttrib);
	input = szBuf;

	vs.clear();
	tokenize(input,tokens,delimiter,keep_delim,esc);

	for(itTemp = tokens.begin();itTemp != tokens.end();itTemp++)
	{
		vs.push_back(*itTemp);
	}

}
inline void GetAttrib(const char * szBuf,VectorInt &vi)
{
	std::string delimiter = ",\t"; 
	std::string keep_delim = ";:";  
	std::string quote = "\'\""; 
	std::string esc = "\\#";  
	std::string input;
	VectorString tokens;
	VectorString::iterator itTemp;

	//input = GetAttrib(szAttrib);
	input = szBuf;

	tokenize(input,tokens,delimiter,keep_delim,esc);
	
	vi.clear();
	for(itTemp = tokens.begin();itTemp != tokens.end();itTemp++)
	{
		int nTemp;
		nTemp = atoi(itTemp->c_str());
		vi.push_back(nTemp);		
		//vi.push_back(*itTemp);
	}


	//std::vector<int>::iterator
}
inline void GetAttrib(const char * szBuf,VectorFloat &vf)
{
	std::string delimiter = ",\t"; 
	std::string keep_delim = ";:";  
	std::string quote = "\'\""; 
	std::string esc = "\\#";  
	std::string input;
	VectorString tokens;
	VectorString::iterator itTemp;

	//input = GetAttrib(szAttrib);
	input = szBuf;

	tokenize(input,tokens,delimiter,keep_delim,esc);
	vf.clear();
	for(itTemp = tokens.begin();itTemp != tokens.end();itTemp++)
	{
		float fTemp;
		fTemp = (float)atof(itTemp->c_str());
		vf.push_back(fTemp);				
	}
	
}

inline void GetAttrib(const char * szBuf,bool &bIs)
{
	std::string str;
	//str = GetAttrib(szAttrib);
	str = szBuf;

	if(str == "true")
	{
		bIs = true;		
	}
	else
	{
		bIs = false;
	}
}


}
}
*/



#pragma once

//update log
//2008. 8. 21 : irrlicht extension add



namespace ggf
{
namespace parser
{
typedef std::vector<std::string> VectorString;
typedef std::vector<int> VectorInt;
typedef std::vector<float> VectorFloat;

//기능확장 함수구현

inline void GetAttrib(const char * szBuf,std::string &str)
{	
	str = szBuf;
}

inline void GetAttrib(const char * szBuf,int &i)
{	
	i = atoi(szBuf);
}

inline void GetAttrib(const char * szBuf,double &db)
{	
	db = atof(szBuf);
}

inline void GetAttrib(const char * szBuf,float &db)
{	
	db = (float)atof(szBuf);
}

inline void GetAttrib(const char * szBuf,VectorString &vs)
{
	//std::vector<std::string>::iterator it;

	std::string delimiter = ",\t"; 
	std::string keep_delim = ";:";  
	std::string quote = "\'\""; 
	std::string esc = "\\#";  
	std::string input;
	VectorString tokens;
	VectorString::iterator itTemp;

	//input = GetAttrib(szAttrib);
	input = szBuf;

	vs.clear();
	tokenize(input,tokens,delimiter,keep_delim,quote,esc);

	for(itTemp = tokens.begin();itTemp != tokens.end();itTemp++)
	{
		vs.push_back(*itTemp);
	}

}
inline void GetAttrib(const char * szBuf,VectorInt &vi)
{
	std::string delimiter = ",\t"; 
	std::string keep_delim = ";:";  
	std::string quote = "\'\""; 
	std::string esc = "\\#";  
	std::string input;
	VectorString tokens;
	VectorString::iterator itTemp;

	//input = GetAttrib(szAttrib);
	input = szBuf;

	tokenize(input,tokens,delimiter,keep_delim,quote,esc);
	
	vi.clear();
	for(itTemp = tokens.begin();itTemp != tokens.end();itTemp++)
	{
		int nTemp;
		nTemp = atoi(itTemp->c_str());
		vi.push_back(nTemp);		
		//vi.push_back(*itTemp);
	}


	//std::vector<int>::iterator
}
inline void GetAttrib(const char * szBuf,VectorFloat &vf)
{
	std::string delimiter = ",\t"; 
	std::string keep_delim = ";:";  
	std::string quote = "\'\""; 
	std::string esc = "\\#";  
	std::string input;
	VectorString tokens;
	VectorString::iterator itTemp;

	//input = GetAttrib(szAttrib);
	input = szBuf;

	tokenize(input,tokens,delimiter,keep_delim,quote,esc);
	vf.clear();
	for(itTemp = tokens.begin();itTemp != tokens.end();itTemp++)
	{
		float fTemp;
		fTemp = (float)atof(itTemp->c_str());
		vf.push_back(fTemp);				
	}
	
}

inline void GetAttrib(const char * szBuf,bool &bIs)
{
	std::string str;
	//str = GetAttrib(szAttrib);
	str = szBuf;

	if(str == "true")
	{
		bIs = true;		
	}
	else
	{
		bIs = false;
	}
}

inline bool GetAttrib(const char * szBuf)
{
	std::string str;	
	str = szBuf;

	if(str == "true")
	{
		return true;		
	}
	else
	{
		return false;
	}
}


//일리히트 확장
//#ifdef _IRR_WINDOWS_
//
//inline void GetAttrib(const char * szBuf,irr::core::vector3df &v3df)
//{	
//	VectorFloat vf;
//	GetAttrib(szBuf,vf);
//
//	v3df = irr::core::vector3df(vf[0],vf[1],vf[2]);	
//}
//
//inline void GetAttrib(const char * szBuf,irr::core::vector3di &v3di)
//{	
//	VectorInt vi;
//	GetAttrib(szBuf,vi);
//
//	v3di = irr::core::vector3di(vi[0],vi[1],vi[2]);	
//
//	
//}
//
//inline void GetAttrib(const char * szBuf,irr::core::vector2di &v2di)
//{	
//	VectorInt vi;
//	GetAttrib(szBuf,vi);
//
//	v2di = irr::core::vector2di(vi[0],vi[1]);	
//}
//
//inline irr::core::position2di GetAttrib(const char * szBuf,irr::core::position2di &pos2di)
//{	
//	VectorInt vi;
//	GetAttrib(szBuf,vi);
//
//	pos2di = irr::core::position2di(vi[0],vi[1]);	
//}
//
//inline void GetAttrib(const char * szBuf,irr::core::vector2df &v2df)
//{	
//	VectorFloat vf;
//	GetAttrib(szBuf,vf);
//
//	v2df = irr::core::vector2df(vf[0],vf[1]);	
//}
//
//
//
//inline void GetAttrib(const char * szBuf,irr::core::position2df &pos2df)
//{	
//	VectorFloat vf;
//	GetAttrib(szBuf,vf);
//
//	pos2df = irr::core::position2df(vf[0],vf[1]);	
//}
//
//inline void GetAttrib(const char * szBuf,irr::core::rect<irr::s32> &rt)
//{	
//	VectorInt vi;
//	GetAttrib(szBuf,vi);
//
//	rt = irr::core::rect<irr::s32>(vi[0],vi[1],vi[2],vi[3]);	
//}
//
//#endif


}
}