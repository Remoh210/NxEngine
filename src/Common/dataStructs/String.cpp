#include "String.h"
#include <fstream>

Array<NString> StringFuncs::split(const NString& s, char delim)
{
	Array<NString> elems;
        
    const char* cstr = s.c_str();
    unsigned int strLength = (unsigned int)s.length();
    unsigned int start = 0;
    unsigned int end = 0;
        
    while(end <= strLength) {
        while(end <= strLength) {
            if(cstr[end] == delim) {
                break;
			}
            end++;
        }
            
        elems.push_back(s.substr(start, end - start));
        start = end + 1;
        end = start;
    }
        
    return elems;
}

NString StringFuncs::getFilePath(const NString& fileName)
{
	const char* cstr = fileName.c_str();
	unsigned int strLength = (unsigned int)fileName.length();
	unsigned int end = strLength - 1;
	
	while(end != 0) {
		if(cstr[end] == '/') {
			break;
		}
		end--;
	}

	if(end == 0) {
		return fileName;
	} else {
		unsigned int start = 0;
		end = end + 1;
		return fileName.substr(start, end - start);
	}
}

bool StringFuncs::loadTextFileWithIncludes(NString& output, const NString& fileName,
		const NString& includeKeyword)
{
	std::ifstream file;
	file.open(fileName.c_str());

	NString filePath = getFilePath(fileName);
	std::stringstream ss;
	NString line;

	if(file.is_open()) {
		while(file.good()) {
			getline(file, line);
			
			if(line.find(includeKeyword) == NString::npos) {
				ss << line << "\n";
			} else {
				NString includeFileName = StringFuncs::split(line, ' ')[1];
				includeFileName =
					includeFileName.substr(1,includeFileName.length() - 2);

				NString toAppend;
				loadTextFileWithIncludes(toAppend, filePath + includeFileName,
						includeKeyword);
				ss << toAppend << "\n";
			}
		}
	} else {
		DEBUG_LOG(LOG_TYPE_IO, LOG_ERROR, "Unable to load shader: %s",
				fileName.c_str());
		return false;
	}

	output = ss.str();
	return true;
}

