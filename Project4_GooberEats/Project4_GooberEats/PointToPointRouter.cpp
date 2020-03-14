#include "provided.h"
#include <utility> 
#include <list>
#include <set>
#include <queue> 
#include <unordered_map>
#include "ExpandableHashMap.h"
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;

private:
	const StreetMap* m_map;

	double calcF(GeoCoord startSeg, GeoCoord endSeg, GeoCoord end) const
	{
		double g = sqrt(((endSeg.latitude - startSeg.latitude) * (endSeg.latitude - startSeg.latitude)) + ((endSeg.longitude - startSeg.longitude) * (endSeg.longitude - startSeg.longitude)));
		double h = sqrt(((end.latitude - endSeg.latitude) * (end.latitude - endSeg.latitude)) + ((end.longitude - endSeg.longitude) * (end.longitude - endSeg.longitude)));
		double f = h + g;
		return f;
	}

	struct ginfo
	{
		ginfo(GeoCoord geo)
		{
			g = geo;
			f = 0.0;
		}

		GeoCoord g;
		GeoCoord prev;

		double f;
		double d;
		double h;

		string name;

		ginfo() {}
		~ginfo() {}
	};

	struct compareF
	{
		bool operator()(ginfo const& g1, ginfo const& g2)
		{
			return g1.f < g2.f;
		}
	};
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
	m_map = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
	m_map = nullptr;
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(const GeoCoord& start, const GeoCoord& end, list<StreetSegment>& route, double& totalDistanceTravelled) const
{
	vector<StreetSegment> segs;
	if (!m_map->getSegmentsThatStartWith(end, segs))
		return BAD_COORD;
	segs.clear();

	if(!m_map->getSegmentsThatStartWith(start, segs))
		return BAD_COORD;

	priority_queue<ginfo, vector<ginfo>, compareF> openList;
	ExpandableHashMap<GeoCoord, ginfo> visited;
	ExpandableHashMap<GeoCoord, ginfo> inList;
	ginfo first(start);
	first.prev = start;
	openList.push(first);
	inList.associate(start, first);

	while (!openList.empty())
	{
		ginfo q = openList.top();
		openList.pop();

		if (q.g == end)
		{
			//return path
			return DELIVERY_SUCCESS;
		}

		for (int i = 0; i < segs.size(); i++)
		{
			GeoCoord gSegEnd(segs[i].end);
			if (visited.find(gSegEnd) == nullptr)
			{
				ginfo r(gSegEnd);
				visited.associate(gSegEnd, r);
				r.f = calcF(q.g, gSegEnd, end);
				r.prev = q.g;
				r.name = segs[i].name;

				if (inList.find(r.g) == nullptr)
					openList.push(r);
				else
				{
					if (inList.find(r.g)->prev == q.g && inList.find(r.g)->f > r.f)
					{
						openList.push(r);
						inList.associate(r.g, r);
					}
				}
				
			}
			
		}//end for
	}//end while

	return NO_ROUTE;
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
