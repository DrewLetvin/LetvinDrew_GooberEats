#include "provided.h"
#include <utility> 
#include <list>
#include <queue> 
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

	struct ginfo
	{
		ginfo(GeoCoord geo)
		{
			g = geo;
			f = 0.0;
			distFromStart = 0.0;
			h = 0.0;
			popped = false;
		}

		GeoCoord g;
		GeoCoord prev;

		double f;
		double distFromStart;
		double h;

		string name;
		bool popped;

		ginfo() {}
		~ginfo() {}
	};

	struct compareF
	{
		bool operator()(ginfo const& g1, ginfo const& g2)
		{
			return g1.f > g2.f;
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

	if(!m_map->getSegmentsThatStartWith(start, segs))
		return BAD_COORD;

	priority_queue<ginfo, vector<ginfo>, compareF> openList;
	ExpandableHashMap<GeoCoord, ginfo> visited;
	ExpandableHashMap<GeoCoord, ginfo> inList;
	ginfo first(start);
	first.prev = start;
	openList.push(first);
	inList.associate(start, first);
	visited.associate(start, first);

	while (!openList.empty())
	{
		ginfo q = openList.top();
		m_map->getSegmentsThatStartWith(q.g, segs);
		openList.pop();
		q.popped = true;

		if (q.g == end)
		{
			totalDistanceTravelled = q.distFromStart;
			StreetSegment endpoint(q.prev, q.g, q.name);
			route.push_front(endpoint);
			//return path
			while (q.prev != q.g)
			{
				q = *visited.find(q.prev);
				if (q.prev != q.g)
				{
					StreetSegment toAdd(q.prev, q.g, q.name);
					route.push_front(toAdd);
				}
			}
			return DELIVERY_SUCCESS;
		}

		for (int i = 0; i < segs.size(); i++)
		{
			GeoCoord gSegEnd(segs[i].end);
			if (visited.find(gSegEnd) == nullptr)
			{
				ginfo r(gSegEnd);

				//double g = sqrt(((gSegEnd.latitude - q.g.latitude) * (gSegEnd.latitude - q.g.latitude)) + ((gSegEnd.longitude - q.g.longitude) * (gSegEnd.longitude - q.g.longitude)));
				//double h = sqrt(((end.latitude - gSegEnd.latitude) * (end.latitude - gSegEnd.latitude)) + ((end.longitude - gSegEnd.longitude) * (end.longitude - gSegEnd.longitude)));

				double g = distanceEarthMiles(q.g, gSegEnd);
				double h = distanceEarthMiles(gSegEnd, end);

				r.h = h;
				r.distFromStart = q.distFromStart + g;
				r.f = r.distFromStart + r.h;
				r.prev = q.g;
				r.name = segs[i].name;

				visited.associate(gSegEnd, r);

				if (inList.find(r.g) == nullptr)
					openList.push(r);
				else
				{
					if (inList.find(r.g)->f > r.f)
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


//int main()
//{
//	StreetMap* sm = new StreetMap();
//	sm->load("D:/UCLA/CS/CS32/LetvinDrew_GooberEats/Project4_GooberEats/Project4_GooberEats/testMap.txt");
//	PointToPointRouter* router = new PointToPointRouter(sm);
//
//	list<StreetSegment> segs;
//	double distance = 0.0;
//	GeoCoord a("3", "1");
//	GeoCoord b("1", "3");
//	router->generatePointToPointRoute(a, b, segs, distance);
//}