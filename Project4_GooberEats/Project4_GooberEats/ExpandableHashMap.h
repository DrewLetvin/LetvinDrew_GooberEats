// ExpandableHashMap.h

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
	std::vector<std::list<Node>> m_vals;

	unsigned int getBucketNumber(const KeyType& key) const;
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<typename KeyType, typename ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
	m_numItems = 0;
	m_numBuckets = 8;

	for (int i = 0; i < m_numBuckets; i++)
	{
		std::list<Node> tempList;
		m_vals.push_back(tempList);
	}

	if (maximumLoadFactor > 0)
		maxLoad = maximumLoadFactor;
	else
		maxLoad = 0.5;
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<typename KeyType, typename ValueType>::~ExpandableHashMap()
{
}

template<typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<typename KeyType, typename ValueType>::getBucketNumber(const KeyType& key) const
{
	unsigned int hasher(const KeyType & k); // prototype
	unsigned int h = hasher(key);
	h = h % m_numBuckets;
	return h;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<typename KeyType, typename ValueType>::reset()  
{
	std::vector<std::list<Node>> temp;
	for (int i = 0; i < m_numBuckets; i++)
	{
		std::list<Node> tempList;
		m_vals.push_back(tempList);
	}

	m_vals = temp;
	m_numBuckets = 8;
	m_numItems = 0;

}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<typename KeyType, typename ValueType>::size() const
{
	return m_numItems;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<typename KeyType, typename ValueType>::associate(const KeyType& key, const ValueType& value)
{
	int h = getBucketNumber(key);
	bool contained = false;

	std::list<Node> tempList = m_vals[h];
	typename std::list<Node>::iterator it;

	for (it = tempList.begin(); it != tempList.end(); it++)
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
				std::list<Node> tempList2 = m_vals[k];
				typename std::list<Node>::iterator it2;
				for (it2 = tempList2.begin(); it2 != tempList2.end(); it2++)
				{
					int h2 = getBucketNumber(it2->key);
					tempMap[h2].push_back(*it2);
				}
			}
			m_vals = tempMap;
		}
		
		h = getBucketNumber(key);
		Node toInsert;
		toInsert.val = value;
		toInsert.key = key;
		m_vals[h].push_back(toInsert);
		m_numItems++;
		
	}
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<typename KeyType, typename ValueType>::find(const KeyType& key) const
{
	int h = getBucketNumber(key);
	
	std::list<Node> tempList = m_vals[h];
	const ValueType* v;
	typename std::list<Node>::iterator it;

	for (it = tempList.begin(); it != tempList.end(); it++)
	{
		if (it->key == key)
		{
			v =  &(it->val);
			return v;
		}
	}
	
	return nullptr;
}

#endif