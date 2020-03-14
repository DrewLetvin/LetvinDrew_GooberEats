#ifndef PROVIDED_INCLUDED
#define PROVIDED_INCLUDED

// YOU MUST MAKE NO CHANGES TO THIS FILE!

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>

enum DeliveryResult
{
    DELIVERY_SUCCESS, NO_ROUTE, BAD_COORD
};

struct GeoCoord
{
    GeoCoord(std::string lat, std::string lon)
     : latitudeText(lat), longitudeText(lon), latitude(std::stod(lat)), longitude(std::stod(lon))
    {}

    GeoCoord()
     : latitudeText("0"), longitudeText("0"), latitude(0), longitude(0)
    {}

    std::string latitudeText;
    std::string longitudeText;
    double      latitude;
    double      longitude;
};

inline
bool operator==(const GeoCoord& lhs, const GeoCoord& rhs)
{
    return lhs.latitudeText == rhs.latitudeText  &&  lhs.longitudeText == rhs.longitudeText;
}

inline
bool operator!=(const GeoCoord& lhs, const GeoCoord& rhs)
{
    return !(lhs == rhs);
}

  // This would be needed only if something required some ordering relation
inline
bool operator<(const GeoCoord& lhs, const GeoCoord& rhs)
{
    if (lhs.latitudeText < rhs.latitudeText) return true;
    if (rhs.latitudeText < lhs.latitudeText) return false;
    return lhs.longitudeText < rhs.longitudeText;
}

struct StreetSegment
{
    StreetSegment(const GeoCoord& s, const GeoCoord& e, std::string streetName)
     : start(s), end(e), name(streetName)
    {}

    StreetSegment()
    {}

    GeoCoord start;
    GeoCoord end;
    std::string name;
};

inline
bool operator==(const StreetSegment& lhs, const StreetSegment& rhs)
{
    return lhs.start == rhs.start  &&  lhs.end == rhs.end;
}

class StreetMapImpl;

class StreetMap
{
public:
    StreetMap();
    ~StreetMap();
    bool load(std::string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, std::vector<StreetSegment>& segs) const;
      // We prevent a StreetMap object from being copied or assigned.
    StreetMap(const StreetMap&) = delete;
    StreetMap& operator=(const StreetMap&) = delete;
private:
    StreetMapImpl* m_impl;
};

class PointToPointRouterImpl;

class PointToPointRouter
{
public:
    PointToPointRouter(const StreetMap* sm);
    ~PointToPointRouter();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        std::list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
      // We prevent a PointToPointRouter object from being copied or assigned.
    PointToPointRouter(const PointToPointRouter&) = delete;
    PointToPointRouter& operator=(const PointToPointRouter&) = delete;
private:
    PointToPointRouterImpl* m_impl;
};

struct DeliveryRequest
{
    DeliveryRequest(std::string it, const GeoCoord& loc)
     : item(it), location(loc)
    {}
    std::string item;
    GeoCoord location;
};

class DeliveryOptimizerImpl;

class DeliveryOptimizer
{
public:
    DeliveryOptimizer(const StreetMap* sm);
    ~DeliveryOptimizer();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        std::vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
      // We prevent a DeliveryOptimizer object from being copied or assigned.
    DeliveryOptimizer(const DeliveryOptimizer&) = delete;
    DeliveryOptimizer& operator=(const DeliveryOptimizer&) = delete;
private:
    DeliveryOptimizerImpl* m_impl;
};

class DeliveryCommand
{
public:
    DeliveryCommand()
     : m_type(INVALID)
    {}

      // make this DeliveryCommand a Proceed command
    void initAsProceedCommand(std::string dir, std::string streetName, double dist)
    {
        m_type = PROCEED;
        m_streetName = streetName;
        m_direction = dir;
        m_distance = dist;
    }

      // make this DeliveryCommand a Turn command
    void initAsTurnCommand(std::string dir, std::string streetName)
    {
        m_type = TURN;
        m_streetName = streetName;
        m_direction = dir;
        m_distance = 0;
    }

      // make this DeliveryCommand a Deliver command
    void initAsDeliverCommand(std::string item)
    {
        m_type = DELIVER;
        m_item = item;
    }

    void increaseDistance(double byThisMuch)
    {
        m_distance += byThisMuch;
    }

    std::string streetName() const
    {
        return m_streetName;
    }

    std::string description() const
    {
        std::ostringstream oss;
        switch (m_type)
        {
          case INVALID:
            oss << "<invalid>";
            break;
          case TURN:
            oss << "Turn " << m_direction << " on " << m_streetName;
            break;
          case PROCEED:
            oss.setf(std::ios::fixed);
            oss.precision(2);
            oss << "Proceed " << m_direction << " on " << m_streetName << " for " << m_distance << " miles";
            break;
          case DELIVER:
            oss << "DELIVER " << m_item;
            break;
        }
        return oss.str();
    }

private:
    enum CommandType { INVALID, PROCEED, TURN, DELIVER };
    CommandType m_type;        // turn left, turn right, proceed
    std::string  m_streetName;  // Westwood Blvd
    std::string  m_direction;   // "left" for turn or "northeast" for proceed
    std::string  m_item;        // Item to deliver
    double       m_distance;    // 1.92 (in miles)
};

class DeliveryPlannerImpl;

class DeliveryPlanner
{
public:
    DeliveryPlanner(const StreetMap* sm);
    ~DeliveryPlanner();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const std::vector<DeliveryRequest>& deliveries,
        std::vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
      // We prevent a DeliveryPlanner object from being copied or assigned.
    DeliveryPlanner(const DeliveryPlanner&) = delete;
    DeliveryPlanner& operator=(const DeliveryPlanner&) = delete;
private:
    DeliveryPlannerImpl* m_impl;
};

// Tools for computing distance between GeoCoords, angle of a StreetSegment,
// and angle between two StreetSegments 

#include <cmath> 

  // decimal degrees to radians
inline double deg2rad(double deg) {
    static const double PI = 4 * std::atan(1.0);
    return deg * PI / 180;
}

  // radians to decimal degrees
inline double rad2deg(double rad) {
    static const double PI = 4 * std::atan(1.0);
    return rad * 180 / PI;
}

/**
* Returns the distance between two points on the Earth.
* Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
* @param lat1d Latitude of the first point in degrees
* @param lon1d Longitude of the first point in degrees
* @param lat2d Latitude of the second point in degrees
* @param lon2d Longitude of the second point in degrees
* @return The distance between the two points in kilometers
*/
inline double distanceEarthKM(const GeoCoord& g1, const GeoCoord& g2) {
    static const double earthRadiusKm = 6371.0;
    double lat1r = deg2rad(g1.latitude);
    double lon1r = deg2rad(g1.longitude);
    double lat2r = deg2rad(g2.latitude);
    double lon2r = deg2rad(g2.longitude);
    double u = std::sin((lat2r - lat1r) / 2);
    double v = std::sin((lon2r - lon1r) / 2);
    return 2.0 * earthRadiusKm * std::asin(std::sqrt(u * u + std::cos(lat1r) * std::cos(lat2r) * v * v));
}

inline double distanceEarthMiles(const GeoCoord& g1, const GeoCoord& g2) {
    const double milesPerKm = 1 / 1.609344;
    return distanceEarthKM(g1, g2) * milesPerKm;
}

inline double angleBetween2Lines(const StreetSegment& line1, const StreetSegment& line2)
{
    double angle1 = atan2(line1.end.latitude - line1.start.latitude, line1.end.longitude - line1.start.longitude);
    double angle2 = atan2(line2.end.latitude - line2.start.latitude, line2.end.longitude - line2.start.longitude);

    double result = rad2deg(angle2 - angle1);
    if (result < 0)
        result += 360;

    return result;
}

inline double angleOfLine(const StreetSegment& line)
{
    double angle = atan2(line.end.latitude - line.start.latitude, line.end.longitude - line.start.longitude);
    double result = rad2deg(angle);
    if (result < 0)
        result += 360;

    return result;
}

#endif // PROVIDED_INCLUDED
