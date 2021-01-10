#pragma once

struct AnimationInfo
{
	std::string name;
	std::string fileName;
	float duration;
	bool bHasExitTime;
	const aiScene* pAIScene;
};