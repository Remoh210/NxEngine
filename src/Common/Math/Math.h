#pragma once
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "Common/Common.h"
#include "Common/dataStructs/NxArray.h"
#include "rttr/registration"
#include <cmath>

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
		std::vector<float> NxArrayOut;
		float* value = glm::value_ptr(data);
		NxArrayOut.assign(value, value + 3);
		return NxArrayOut;
	}

	//This function is used for reflection
	inline void Set(std::vector<float> NxArrayIn)
	{
		int size = NxArrayIn.size();
		if (size < 3)
		{
			DEBUG_LOG("vec3", "Error", "Can't create vec3 from NxArray of size = %d", size);
			return;
		}
		data = glm::make_vec3(&NxArrayIn[0]);
	}

	RTTR_ENABLE();
#pragma endregion

private:
	glm::vec3 data;
	
};






class vec4f
{
public:
#pragma region constructors
	inline vec4f(float x, float y, float z, float w)
	{
		data = glm::vec4(x, y, z, w);
	}

	inline vec4f(float value)
	{
		data = glm::vec4(value, value, value, value);
	}

	inline vec4f(glm::vec4 value)
	{
		data = value;
	}

	inline vec4f()
	{
		data = glm::vec4();
	}

	inline glm::vec4& ToVec()
	{
		return data;
	}
#pragma endregion

#pragma region operators
	// Operators
	float operator [] (int index) { return data[index]; }

	inline vec3f operator-() const { return vec3f(-data); };

	inline vec4f operator + (float valueIn) const { return vec4f(data + valueIn); }
	inline vec4f operator - (float valueIn) const { return vec4f(data - valueIn); }
	inline vec4f operator * (float valueIn) const { return vec4f(data * valueIn); }
	inline vec4f operator / (float valueIn) const { return vec4f(data / valueIn); }

	inline vec4f operator + (const vec4f& other) const { return vec4f(data + other.data); }
	inline vec4f operator - (const vec4f& other) const { return vec4f(data - other.data); }
	inline vec4f operator * (const vec4f& other) const { return vec4f(data * other.data); }
	inline vec4f operator / (const vec4f& other) const { return vec4f(data / other.data); }

	inline vec4f operator += (float valueIn) { data += valueIn; return *this; }
	inline vec4f operator -= (float valueIn) { data -= valueIn; return *this; }
	inline vec4f operator *= (float valueIn) { data *= valueIn; return *this; }
	inline vec4f operator /= (float valueIn) { data /= valueIn; return *this; }

	inline vec4f operator += (const vec4f& other) { data += other.data; return *this; }
	inline vec4f operator -= (const vec4f& other) { data -= other.data; return *this; }
	inline vec4f operator *= (const vec4f& other) { data *= other.data; return *this; }
	inline vec4f operator /= (const vec4f& other) { data /= other.data; return *this; }

	void operator = (glm::vec4 vecIn) { data = vecIn; }
	void operator = (vec4f vecIn) { data = vecIn.ToVec(); }
#pragma endregion

#pragma region reflection
	//This function is used for reflection
	inline std::vector<float> Get()
	{
		std::vector<float> NxArrayOut;
		float* value = glm::value_ptr(data);
		NxArrayOut.assign(value, value + 4);
		return NxArrayOut;
	}

	//This function is used for reflection
	inline void Set(std::vector<float> NxArrayIn)
	{
		int size = NxArrayIn.size();
		if (size < 4)
		{
			DEBUG_LOG("vec3", "Error", "Can't create vec3 from NxArray of size = %d", size);
			return;
		}
		data = glm::make_vec4(&NxArrayIn[0]);
	}

	RTTR_ENABLE();
#pragma endregion

private:
	glm::vec4 data;

};