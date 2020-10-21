#pragma once

#include <string>
#include <sstream>
#include "Common/Common.h"
#include "Common/dataStructs/Array.h"

#define String std::string

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
	static inline String toString(T val)
	{
		std::ostringstream convert;
		convert << val;
		return convert.str();
	}

	static Array<String> split(const String& s, char delim);
	static String getFilePath(const String& fileName);
	static bool loadTextFileWithIncludes(String& output, const String& fileName,
	const String& includeKeyword);
};

