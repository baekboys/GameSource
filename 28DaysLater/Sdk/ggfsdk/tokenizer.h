/************************************************************************
The zlib/libpng License

Copyright (c) 2006 Joerg Wiedenmann

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
	you must not claim that you wrote the original software.
	If you use this software in a product, an acknowledgment
	in the product documentation would be appreciated but is
	not required.

2. Altered source versions must be plainly marked as such,
	and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

***********************************************************************/

/********************************************************************
	created:	2006-01-28
	filename: 	tokenizer.cpp
	author:		J?g Wiedenmann

	purpose:	A tokenizer function which provides a very
				customizable way of breaking up strings.
*********************************************************************/

/*

최종수정 2009.2.19
내용 :  
vs2005 이상 버전에서 tokenize함수 밑으로 인탤리젼트 기능이 잘안먹히는문제점때문에
(디폴트인자 값때문인거같음...>
디폴트인자를 제거하고 그대신 오버로드를 사용함
*/

#pragma once

//#include "stdafx.h"

#include <vector>
#include <string>
using namespace std;

// Function to break up a string into tokens
//
// Parameters:
//-----------
// str = the input string that will be tokenized
// result = the tokens for str
// delimiters = the delimiter characters
// delimiters preserve = same as above, but the delimiter characters
//		will be put into the result as a token
// quote = characters to protect the enclosed characters
// esc = characters to protect a single character
//

void tokenize (
			   const string& str, 
			   vector<string>& result,
			   const string& delimiters, 
			   const string& delimiters_preserve,// = "",
			   const string& quote,// = "\"", 
			   const string& esc// = "\\" 
			);

void tokenize(
			  const string& str, 
			  vector<string>& result,
			  const string& delimiters			  
			  );
			  
