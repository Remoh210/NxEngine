#pragma once

#include <map>

#define NxMap std::map

//template<class Key, class Value>
//class Map : public std::map<Key, Value>
//{
//public:
//	NxMap<Key, Value>() : std::map<Key, Value>() {}
//
//	Value Find(Key key)
//	{
//		typename std::map<Key, Value>::iterator it;
//
//		it = this->find(key);
//		if (it != this->end())
//		{
//			return it->second;
//		}
//
//		return nullptr;
//	}
//};


struct MapFuncs
{
	template <class Key, class Value>
	static inline Value Find(NxMap<Key, Value>& MapIn, Key& KeyIn)
	{
		std::map<Key, Value>::iterator it;

		it = MapIn.find(KeyIn);
		if (it != MapIn.end())
		{
			return it->second;
		}

		return nullptr;
	}
};