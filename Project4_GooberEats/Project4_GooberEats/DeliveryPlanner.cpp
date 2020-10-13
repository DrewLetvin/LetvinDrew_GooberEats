#include "provided.h"
#include <vector>
#include <utility>
#include <list>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;

private:
	const StreetMap* m_map;
	PointToPointRouter* router;

	struct streetInfo
	{
		streetInfo(StreetSegment s)
		{
			name = s.name;
			length = 0.0;
			start = s.start;
			end = s.end;
		}

		string name;
		double length;

		GeoCoord start;
		GeoCoord end;

		streetInfo() {}
		~streetInfo() {}
	};

	double getAngle(streetInfo currentS, streetInfo nextS) const
	{
		double deg = 0.0;
		StreetSegment current(currentS.start, currentS.end, "");
		StreetSegment next(nextS.start, nextS.end, "");

		deg = angleBetween2Lines(current, next);

		return deg;
	}

	double getAngle(streetInfo currentS) const
	{
		double deg = 0.0;
		StreetSegment current(currentS.start, currentS.end, "");

		deg = angleOfLine(current);

		return deg;
	}

	string getDirection(streetInfo street) const
	{
		double ang = getAngle(street);

		if (0 <= ang && ang < 22.5)
			return "east";
		else if (22.5 <= ang && ang < 67.5)
			return "northeast";
		else if (67.5 <= ang && ang < 112.5)
			return "north";
		else if (112.5 <= ang && ang < 157.5)
			return "northwest";
		else if (157.5 <= ang && ang < 202.5)
			return "west";
		else if (202.5 <= ang && ang < 247.5)
			return "southwest";
		else if (247.5 <= ang && ang < 292.5)
			return "south";
		else if (292.5 <= ang && ang < 337.5)
			return "southeast";
		else if (ang >= 337.5)
			return "east";

		return "";
	}

};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
	m_map = sm;
	router = new PointToPointRouter(sm);
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
	m_map = nullptr;
	delete router;
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
	totalDistanceTravelled = 0;
	DeliveryRequest ends("", depot);
	if (deliveries.size() == 0)
		return NO_ROUTE;


	for (int i = 0; i < deliveries.size() + 1; i++)
	{
		list<StreetSegment> segs;
		DeliveryCommand cmd;
		double distance = 0.0;

		if (i == 0)//generates list of street segments
		{
			if (router->generatePointToPointRoute(depot, deliveries[i].location, segs, distance) != DELIVERY_SUCCESS)
				return NO_ROUTE;
		}
		else if(i == (deliveries.size()))
		{
			if (router->generatePointToPointRoute(deliveries[i - 1].location, depot, segs, distance) != DELIVERY_SUCCESS)
				return NO_ROUTE;
		}
		else
		{
			if (router->generatePointToPointRoute(deliveries[i - 1].location, deliveries[i].location, segs, distance) != DELIVERY_SUCCESS)
				return NO_ROUTE;
		}

		totalDistanceTravelled += distance;

		if (segs.empty())
		{
			//make delivery at location
			cmd.initAsDeliverCommand(deliveries[i].item);
			commands.push_back(cmd);
		}
		else
		{
			list<streetInfo> streetList;
			double streetLength = 0.0;

			list<StreetSegment>::iterator it = segs.begin();
			for (; it != segs.end(); it++) //not loading correctly for some reason
			{
				streetLength = distanceEarthMiles(it->start, it->end);
				streetInfo street(*it);
				street.length = streetLength;
				streetList.push_back(street);
			}

			list<streetInfo>::iterator iter = streetList.begin();
			list<streetInfo>::iterator iterPre = streetList.begin();
			advance(iter, 1);
			for (; iter != streetList.end(); iter++) 
			{
				if (iter->name == iterPre->name)
				{
					iterPre->length += iter->length;
					iterPre->end = iter->end;
					streetList.erase(iter);
					iter = iterPre;
				}
				else
				{
					iterPre++;
				}
			}

			list<streetInfo>::iterator iter2 = streetList.begin();
			list<streetInfo>::iterator iterNext = streetList.begin();
			advance(iterNext, 1);
			string dir;
			double angle = 0.0;
			for (; iter2 != streetList.end(); iter2++)
			{
				if (iterNext == streetList.end())
				{
					//one proceeds cmd then one delivery cmd
					dir = getDirection(*iter2);//finds direction of road
					cmd.initAsProceedCommand(dir, iter2->name, iter2->length);
					commands.push_back(cmd);
					if (i < deliveries.size())
					{
						cmd.initAsDeliverCommand(deliveries[i].item);
						commands.push_back(cmd);
					}
					break;
				}
				else
				{
					//one proceeds cmd then one turn cmd
					dir = getDirection(*iter2);
					cmd.initAsProceedCommand(dir, iter2->name, iter2->length);
					commands.push_back(cmd);

					angle = getAngle(*iter2, *iterNext);
					if(angle < 1 || angle > 359) {}
					else if(angle >= 1 && angle < 180)
						cmd.initAsTurnCommand("left", iterNext->name);
					else if(angle >= 180 && angle <= 359)
						cmd.initAsTurnCommand("right", iterNext->name);
					
					commands.push_back(cmd);
				}
				iterNext++;
			}



		}

    }

	return DELIVERY_SUCCESS;
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
