#pragma once

#include <map>

template<class Key, class Value>
class Map : public std::map<Key, Value>
{
public:
	Map<Key, Value>() : std::map<Key, Value>() {}

	Value Find(Key key)
	{
		typename std::map<Key, Value>::iterator it;

		it = this->find(key);
		if (it != this->end())
		{
			return it->second;
		}

		return nullptr;
	}

	//Value& operator [] (Key key)
	//{
	//	return this->[key];
	//};

};