#include "detector.hpp"

using namespace follow_the_drow;


Cluster::Cluster(int start, int end, const std::vector<Point>& reference):
    dSize(reference[start].distanceTo(reference[end])),
    iStart(start),
    iEnd(end),
    iMiddle((start + end) / 2),
    pStart(reference[start]),
    pMiddle(reference[iMiddle]),
    pEnd(reference[end]),
    pBetween(pStart.middleBetween(pEnd)) {}

Cluster::Cluster(const Cluster& cluster):
    dSize(cluster.dSize),
    iStart(cluster.iStart),
    iEnd(cluster.iEnd),
    iMiddle(cluster.iMiddle),
    pStart(cluster.pStart),
    pMiddle(cluster.pMiddle),
    pEnd(cluster.pEnd),
    pBetween(cluster.pBetween) {}


float Cluster::size() const {
    return dSize;
}

int Cluster::start() const {
    return iStart;
}

int Cluster::middle() const {
    return iMiddle;
}

int Cluster::end() const {
    return iEnd;
}

const Point& Cluster::startPoint() const {
    return pStart;
}

const Point& Cluster::middlePoint() const {
    return pMiddle;
}

const Point& Cluster::betweenPoint() const {
    return pBetween;
}

const Point& Cluster::endPoint() const {
    return pEnd;
}


float Cluster::distanceTo(const Cluster& cluster) const {
    return this->pBetween.distanceTo(cluster.pBetween);
}

Point Cluster::middleBetween(const Cluster& cluster) const {
    return this->betweenPoint().middleBetween(cluster.betweenPoint());
}
