#pragma once

#include <vector>
#include <algorithm>
#include "rttr/registration"

#define NxArray std::vector

//template<typename T>
//class Array : public std::vector<T>
//{
//public:
//	Array<T>() : std::vector<T>() {}
//	Array<T>(size_t n) : std::vector<T>(n) {}
//
//	void swap_remove(size_t index)
//	{
//		std::swap((*this)[index], (*this)[this->size()-1]);
//		this->pop_back();
//	}
//
//	void Add(T value)
//	{
//		this->push_back(value);
//	}
//
//	bool Find(T value)
//	{
//		return std::find(this->begin(), this->end(), value) != this->end();
//	}
//
//	void Remove(T value)
//	{
//		if(this->end() != this->erase(std::remove(this->begin(), this->end(), value), this->end()));
//	}
//
//	std::vector<T> Get()
//	{
//		return static_cast<std::vector<T>>(this);
//	}
//	void Set(std::vector<T> ArrayIn)
//	{
//		this->assign(ArrayIn);
//	}
//};


struct ArrayFuncs
{

	//static inline bool Find(T value)
	//{
	//	return std::find(this->begin(), this->end(), value) != this->end();
	//}

	template <class T>
	static inline void Remove(NxArray<T>& arrayIn, T& value)
	{
		if (arrayIn.end() != arrayIn.erase(std::remove(arrayIn.begin(), arrayIn.end(), value), arrayIn.end()));
	}

	//template <class T>
	//static inline void swap_remove()
	//{
	//	std::swap((*this)[index], (*this)[this->size()-1]);
	//	this->pop_back();
	//}
};