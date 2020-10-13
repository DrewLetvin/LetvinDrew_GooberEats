#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = 0;
    newCrowDistance = 0;

	//vector<DeliveryRequest> toVisit;
	//vector<DeliveryRequest> visited;

	//for (int j = 0; j < deliveries.size(); j++)
	//{
	//	toVisit.push_back(deliveries[j]);
	//}

	oldCrowDistance += distanceEarthMiles(depot, deliveries[0].location);
	for (int i = 0; i < deliveries.size() - 1; i++)
	{
		oldCrowDistance += distanceEarthMiles(deliveries[i].location, deliveries[i + 1].location);
	}
	oldCrowDistance += distanceEarthMiles(deliveries[deliveries.size() - 1].location, depot);


	//DeliveryRequest current("current", depot);
	//current = toVisit[0];
	//for (int k = 0; k < toVisit.size(); k++)
	//{

	//}

	newCrowDistance = oldCrowDistance;
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
