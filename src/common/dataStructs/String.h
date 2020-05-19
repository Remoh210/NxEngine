#pragma once

#include <string>
#include <sstream>
#include "common/Common.h"
#include "common/dataStructs/Array.h"

#define String std::string

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

