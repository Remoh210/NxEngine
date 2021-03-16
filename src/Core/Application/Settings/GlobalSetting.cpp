#include "GlobalSettings.h"

#include "Core/FileSystem/FileSystem.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

int GlobalSettings::windowWidth     = 1600;
int GlobalSettings::windowHeight    = 1000;

int GlobalSettings::APIVersionMajor = -1;
int GlobalSettings::APIVersionMinor = -1;
int GlobalSettings::APIVersionConc  = -1;

bool GlobalSettings::LoadSettings(NString configFile)
{
	std::string fileToLoadFullPath = Nx::FileSystem::GetRoot() + "/Config/Settings/" + configFile;

	rapidjson::Document doc;
	FILE* fp = fopen(fileToLoadFullPath.c_str(), "rb"); // non-Windows use "r"
	if (!fp) { DEBUG_LOG("InitSetting", "ERROR", "No Config file: %s", fileToLoadFullPath.c_str()); return false; }
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	doc.ParseStream(is);
	fclose(fp);

	windowWidth = doc["Window"]["Width"].GetInt();
	windowHeight = doc["Window"]["Height"].GetInt();

	APIVersionMajor = doc["API"]["Version Major"].GetInt();
	APIVersionMinor = doc["API"]["Version Minor"].GetInt();

	// Convert both the integers to string 
	NString s1 = std::to_string(APIVersionMajor);
	NString s2 = std::to_string(APIVersionMinor);
	NString s_conc = s1 + s2;
	APIVersionConc = std::stoi(s_conc);

	return true;
}