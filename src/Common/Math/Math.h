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
	
	inline vec3f()
	{
		data = glm::vec3(0);
	}
	
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

	void operator = (glm::vec3& vecIn)
	{
		data = vecIn;
	}

	void operator = (const glm::vec3& vecIn)
	{
		data = vecIn;
	}
	
	void operator = (vec3f& vecIn)
	{
		InternalData = vecIn.InternalData;
		
		data.x = vecIn.data.x;
		data.y = vecIn.data.y;
		data.z = vecIn.data.z;
		
	}

	void operator = (const vec3f& vecIn)
	{
		InternalData = vecIn.InternalData;
		
		data.x = vecIn.data.x;
		data.y = vecIn.data.y;
		data.z = vecIn.data.z;
	}
#pragma endregion

#pragma region reflection
	inline std::vector<float>& GetInternal()
	{
		InternalData = {data[0], data[1], data[2] };
		return InternalData;
	}
	
	inline void SetInternal(std::vector<float>& VectorIn)
	{
		if (VectorIn.size() >= 3)
		{
			InternalData = VectorIn;
			data = glm::vec3(InternalData[0], InternalData[1], InternalData[2]);
		}
		else
		{
			DEBUG_LOG_TEMP("Can't set internal data vector");
		}
	}
#pragma endregion

	
private:
	glm::vec3 data;
	// Used only for serialization
	std::vector<float> InternalData;
	
	RTTR_ENABLE()
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

	inline vec4f operator-() const { return vec4f(-data); };

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
	
	void operator = (vec4f& vecIn)
	{
		InternalData = vecIn.InternalData;
		
		data.x = vecIn.data.x;
		data.y = vecIn.data.y;
		data.z = vecIn.data.z;
		data.w = vecIn.data.w;
	}

	void operator = (const vec4f& vecIn)
	{
		InternalData = vecIn.InternalData;
		
		data.x = vecIn.data.x;
		data.y = vecIn.data.y;
		data.z = vecIn.data.z;
		data.w = vecIn.data.w;
	}
#pragma endregion

#pragma region reflection
	inline std::vector<float>& GetInternal()
	{
		InternalData = { data[0], data[1], data[2], data[3] };
		return InternalData;
	}
	
	inline void SetInternal(std::vector<float>& VectorIn)
	{
		if (VectorIn.size() >= 4)
		{
			InternalData = VectorIn;
			data = glm::vec4(InternalData[0], InternalData[1], InternalData[2], InternalData[3]);
		}
		else
		{
			DEBUG_LOG_TEMP("Can't set internal data vector");
		}
	}
#pragma endregion
	
private:
	
	glm::vec4 data;
	// Used only for serialization
	std::vector<float> InternalData;
	
	RTTR_ENABLE()
};