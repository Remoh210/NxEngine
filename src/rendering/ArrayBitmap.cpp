#include "ArrayBitmap.h"
#include "stb_image.h"

#define LOG_TYPE_ArrayBitmap "ArrayBitmap"

ArrayBitmap::ArrayBitmap(const string& fileName) 
{
    Load(fileName);
}

bool ArrayBitmap::Load(const string& fileName) 
{
	bool bResult = false;
	int width, height, nrComponents;
	data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 4);
	if (data)
	{
		// GLenum format;
		// if (nrComponents == 1)
		// 	format = GL_RED;
		// else if (nrComponents == 3)
		// 	format = GL_RGB;
		// else if (nrComponents == 4)
		// 	format = GL_RGBA;
		bResult = true;
	}
	else
	{
		DEBUG_LOG(LOG_TYPE_ArrayBitmap, LOG_ERROR, "Failed to load ArrayBitmap: %s", fileName.c_str());
	}
	stbi_image_free(data);

	return bResult;
}

ArrayBitmap::~ArrayBitmap() 
{
    delete data;
}
