// ExpandableHashMap.h
#include<list>
#include<vector>
// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

#ifndef ExpandableHashMap_h
#define ExpandableHashMap_h

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void reset();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
	struct Node
	{
		KeyType key;
		ValueType val;
	};

	double m_numItems;
	int m_numBuckets;
	double maxLoad;
	std::vector<std::list<Node>>* m_vals;

	unsigned int getBucketNumber(const KeyType& key) const;
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
	m_vals = new std::vector<std::list<Node>>;

	m_numItems = 0;
	m_numBuckets = 8;

	for (int i = 0; i < m_numBuckets; i++)
	{
		std::list<Node> tempList;
		m_vals->push_back(tempList);
	}

	if (maximumLoadFactor > 0)
		maxLoad = maximumLoadFactor;
	else
		maxLoad = 0.5;
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
	delete m_vals;
}

template<typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::getBucketNumber(const KeyType& key) const
{
	unsigned int hasher(const KeyType & k); // prototype
	unsigned int h = hasher(key);
	h = h % m_numBuckets;
	return h;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()  
{
	delete m_vals;

	m_vals = new std::vector<std::list<Node>>;
	for (int i = 0; i < m_numBuckets; i++)
	{
		std::list<Node> tempList;
		m_vals->push_back(tempList);
	}

	m_numBuckets = 8;
	m_numItems = 0;

}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
	return m_numItems;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	int h = getBucketNumber(key);
	bool contained = false;

	typename std::list<Node>::iterator it;
	for (it = (*m_vals)[h].begin(); it != (*m_vals)[h].end(); it++)
	{
		if (it->key == key)
		{
			contained = true;
			it->val = value;
		}
	}

	if (contained == false)
	{
		if ((m_numItems + 1) / m_numBuckets > maxLoad)
		{
			m_numBuckets *= 2;
			std::vector<std::list<Node>> tempMap;
			for (int i = 0; i < m_numBuckets; i++)
			{
				std::list<Node> temp;
				tempMap.push_back(temp);
			}

			for (int k = 0; k < (m_numBuckets / 2); k++)
			{
				std::list<Node> tempList2 = (*m_vals)[k];
				typename std::list<Node>::iterator it2;
				for (auto it2 = tempList2.begin(); it2 != tempList2.end(); it2++)
				{
					int h2 = getBucketNumber(it2->key);
					tempMap[h2].push_back(*it2);
				}
			}
			delete m_vals;
			m_vals = new std::vector<std::list<Node>>;
			*m_vals = tempMap;
		}
		
		h = getBucketNumber(key);
		Node toInsert;
		toInsert.val = value;
		toInsert.key = key;
		(*m_vals)[h].push_back(toInsert);
		m_numItems++;
		
	}
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
	int h = getBucketNumber(key);
	
	typename std::list<Node>::const_iterator it;

	for (auto it = (*m_vals)[h].begin(); it != (*m_vals)[h].end(); it++)
	{
		if (it->key == key)
		{
			return &(it->val);
		}
	}
	
	return nullptr;
}

#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//// Substitute ExpandableHashMap.h
//
/////////////////////////////////////////////////////////////////////////////////
//// This file is a fallback for people who can't get their own ExpandableHashMap
//// to work.  If you turn in this as your ExpandableHashMap.h, you won't get any
//// points for the ExpandableHashMap part of the correctness score, because it
//// uses <unordered_map>, but it doesn't interfere with your earning correctness
//// points for other parts of this project.
/////////////////////////////////////////////////////////////////////////////////
//
//#include <unordered_map>
//
//template<typename KeyType>
//struct Hasher
//{
//	std::size_t operator()(const KeyType& key) const
//	{
//		unsigned int hasher(const KeyType & key);
//		return hasher(key);
//	}
//};
//
//template<typename KeyType, typename ValueType>
//class ExpandableHashMap
//{
//public:
//	ExpandableHashMap(double maximumLoadFactor = 0.5);
//	void reset();
//	int size() const;
//	void associate(const KeyType& key, const ValueType& value);
//
//	// for a map that can't be modified, return a pointer to const ValueType
//	const ValueType* find(const KeyType& key) const;
//
//	// for a modifiable map, return a pointer to modifiable ValueType
//	ValueType* find(const KeyType& key)
//	{
//		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
//	}
//
//	// C++11 syntax for preventing copying and assignment
//	ExpandableHashMap(const ExpandableHashMap&) = delete;
//	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;
//
//private:
//	std::unordered_map<KeyType, ValueType, Hasher<KeyType>> m_map;
//};
//
//template<typename KeyType, typename ValueType>
//ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double /* maximumLoadFactor */)
//{
//}
//
//template<typename KeyType, typename ValueType>
//void ExpandableHashMap<KeyType, ValueType>::reset()
//{
//	m_map.clear();
//}
//
//template<typename KeyType, typename ValueType>
//int ExpandableHashMap<KeyType, ValueType>::size() const
//{
//	return m_map.size();
//}
//
//template<typename KeyType, typename ValueType>
//void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
//{
//	m_map[key] = value;
//}
//
//template<typename KeyType, typename ValueType>
//const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
//{
//	auto p = m_map.find(key);
//	return p == m_map.end() ? nullptr : &p->second;
//}