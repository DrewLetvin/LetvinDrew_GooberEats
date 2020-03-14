#include "provided.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include "ExpandableHashMap.h"
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;

private:
	ExpandableHashMap<GeoCoord, vector<StreetSegment>>* m_data;
};

StreetMapImpl::StreetMapImpl()
{
	m_data = new ExpandableHashMap<GeoCoord, vector<StreetSegment>>();
}

StreetMapImpl::~StreetMapImpl() 
{
	delete m_data;
}

bool StreetMapImpl::load(string mapFile)
{
	ifstream infile(mapFile);
	if (!infile)
		return false;

	string str, name;
	while (getline(infile, str))
	{
		StreetSegment segment;

		int numSegments = 1;
		infile >> numSegments;
		infile.ignore(10000, '\n');

		for (int i = 0; i < numSegments; i++)
		{
			getline(infile, str);
			string g1Lat, g1Long, g2Lat, g2Long;

			g1Lat = str.substr(0, str.find(' '));
			str = str.substr(str.find(' ') + 1);
			g1Long = str.substr(0, str.find(' '));
			str = str.substr(str.find(' ') + 1);

			g2Lat = str.substr(0, str.find(' '));
			str = str.substr(str.find(' ') + 1);
			g2Long = str.substr(0, str.find(' '));
		
			GeoCoord g1(g1Lat, g1Long);
			GeoCoord g2(g2Lat, g2Long);

			StreetSegment tempSegment(g1, g2, name);
			segment = tempSegment;
			segment.name = name;
			if (m_data->find(g1) == nullptr)
			{
				vector<StreetSegment> streetVec;
				streetVec.push_back(segment);
				m_data->associate(g1, streetVec);
			}
			else 
			{
				vector<StreetSegment> vec;
				vec = *(m_data->find(g1));
				vec.push_back(segment);
				m_data->associate(g1, vec);
			}

			StreetSegment revTempSegment(g2, g1, name);
			segment = revTempSegment;
			segment.name = name;
			if (m_data->find(g2) == nullptr)
			{
				vector<StreetSegment> streetVec;
				streetVec.push_back(segment);
				m_data->associate(g2, streetVec);
			}
			else
			{
				vector<StreetSegment> vec;
				vec = *(m_data->find(g2));
				vec.push_back(segment);
				m_data->associate(g2, vec);
			}

		}

	}

	return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
	if (m_data->find(gc) == nullptr)
		return false;

	segs = *(m_data->find(gc));
	return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
