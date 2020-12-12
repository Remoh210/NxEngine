#pragma once

class NxTime
{
public:
	inline static void Update(float dt) 
	{
		deltaTime = dt;
	};

	inline static float GetDeltaTime()
	{
		return deltaTime;
	};

private:
	static float deltaTime;
};