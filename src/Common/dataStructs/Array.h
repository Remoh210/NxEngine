#pragma once

#include <vector>
#include <algorithm>

template<typename T>
class Array : public std::vector<T>
{
public:
	Array<T>() : std::vector<T>() {}
	Array<T>(size_t n) : std::vector<T>(n) {}

	void swap_remove(size_t index)
	{
		std::swap((*this)[index], (*this)[this->size()-1]);
		this->pop_back();
	}

	void Add(T value)
	{
		this->push_back(value);
	}

	bool Find(T value)
	{
		return std::find(this->begin(), this->end(), value) != this->end();
	}

	void Remove(T value)
	{
		if(this->end() != this->erase(std::remove(this->begin(), this->end(), value), this->end()));
	}
};

