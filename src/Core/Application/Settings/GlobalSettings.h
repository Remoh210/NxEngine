#pragma once
#include "Common/dataStructs/String.h"

class GlobalSettings
{
public:
	GlobalSettings() {};
	static bool LoadSettings(NString configFile);

	//Getters
	static int GetWindowWidth()	{ return windowWidth; };
	static int GetWindowHeight() { return windowHeight; };

	static int GetAPIVersionMajor() { return APIVersionMajor; };
	static int GetAPIVersionMinor() { return APIVersionMinor; };

	//Concatenated
	static int GetAPIVersionConc() { return APIVersionConc; };

private:
	
	static int windowWidth;
	static int windowHeight;
	static int APIVersionMajor;
	static int APIVersionMinor;
	static int APIVersionConc;
};


