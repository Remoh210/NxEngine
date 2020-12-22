#pragma once
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "Common/Common.h"
#include "Common/dataStructs/Array.h"
#include "rttr/registration"

class vec3f
{
public:
#pragma region constructors
	inline vec3f(float x, float y, float z)
	{
		data = glm::vec3(x, y, z);
	}

	inline vec3f(float value)
	{
		data = glm::vec3(value, value, value);
	}

	inline vec3f(glm::vec3 value)
	{
		data = value;
	}

	inline vec3f()
	{
		data = glm::vec3();
	}

	inline glm::vec3& ToVec()
	{
		return data;
	}
#pragma endregion

#pragma region operators
	// Operators
	float operator [] (int index) { return data[index]; }

	inline vec3f operator-() const { return vec3f(-data); };

	inline vec3f operator + (float valueIn) const { return vec3f(data + valueIn); }
	inline vec3f operator - (float valueIn) const { return vec3f(data - valueIn); }
	inline vec3f operator * (float valueIn) const { return vec3f(data * valueIn); }
	inline vec3f operator / (float valueIn) const { return vec3f(data / valueIn); }

	inline vec3f operator + (const vec3f& other) const { return vec3f(data + other.data); }
	inline vec3f operator - (const vec3f& other) const { return vec3f(data - other.data); }
	inline vec3f operator * (const vec3f& other) const { return vec3f(data * other.data); }
	inline vec3f operator / (const vec3f& other) const { return vec3f(data / other.data); }

	inline vec3f operator += (float valueIn) { data += valueIn; return *this; }
	inline vec3f operator -= (float valueIn) { data -= valueIn; return *this; }
	inline vec3f operator *= (float valueIn) { data *= valueIn; return *this; }
	inline vec3f operator /= (float valueIn) { data /= valueIn; return *this; }

	inline vec3f operator += (const vec3f& other) { data += other.data; return *this; }
	inline vec3f operator -= (const vec3f& other) { data -= other.data; return *this; }
	inline vec3f operator *= (const vec3f& other) { data *= other.data; return *this; }
	inline vec3f operator /= (const vec3f& other) { data /= other.data; return *this; }

	void operator = (glm::vec3 vecIn) { data = vecIn; }
	void operator = (vec3f vecIn) { data = vecIn.ToVec(); }
#pragma endregion

#pragma region reflection
	//This function is used for reflection
	inline std::vector<float> Get()
	{
		std::vector<float> arrayOut;
		float* value = glm::value_ptr(data);
		arrayOut.assign(value, value + 3);
		return arrayOut;
	}

	//This function is used for reflection
	inline void Set(std::vector<float> arrayIn)
	{
		int size = arrayIn.size();
		if (size < 3)
		{
			DEBUG_LOG("vec3", "Error", "Can't create vec3 from array of size = %d", size);
			return;
		}
		data = glm::make_vec3(&arrayIn[0]);
	}

	inline std::vector<float> GetTest() 
	{
		return data2;
	}

	inline void SetTest(std::vector<float> arrayIn)
	{
		data2 = arrayIn;
	}
	std::vector<float> data2;
	RTTR_ENABLE();
#pragma endregion

private:
	glm::vec3 data;
	
};

struct wtf
{
	int tst;
};