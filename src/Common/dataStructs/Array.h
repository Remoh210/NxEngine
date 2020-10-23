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

	template <class P>
	bool Find(P value)
	{
		return std::find(this->begin(), this->end(), value) != this->end();
	}

	template <class P>
	void Remove(P value)
	{
		if(this->end() != this->erase(std::remove(this->begin(), this->end(), value), this->end()));
	}
};

