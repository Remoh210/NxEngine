#pragma once

#include <string>
#include <sstream>
#include "Common/Common.h"
#include "Common/dataStructs/NxArray.h"

#define NString std::string

//class String : public std::string
//{
//public:
//
//	String() : std::string()
//	{
//	}
//
//	String(const std::string& str) : std::string(str)
//	{
//	}
//
//	String(const char* s): std::string(s)
//	{
//	}
//
//	const char* operator * () 
//	{
//		return this->c_str();
//	}
//
//};

struct StringFuncs
{
	template<typename T>
	static inline NString toString(T val)
	{
		std::ostringstream convert;
		convert << val;
		return convert.str();
	}

	static NxArray<NString> split(const NString& s, char delim);
	static NString getFilePath(const NString& fileName);
	static bool loadTextFileWithIncludes(NString& output, const NString& fileName, const NString& includeKeyword);
	static bool StartWith(const NString& stringIn, NString prefix);
};

