// Person detector using 2 lidar data
// Written by O. Aycard

#include <cmath>

#include "detector.hpp"

using namespace follow_the_drow;

#if defined ROS_ENVIRONMENT
#include "ros/ros.h"
#define LOG(...) ROS_INFO(__VA_ARGS__)
#else
#include <cstdio>
#define LOG(...) printf(__VA_ARGS__)
#endif


const std::vector<Cluster> AlgorithmicDetector::performClustering(const std::vector<Point>& storage) const {
    std::vector<Cluster> clusters;
    int clusterStart = 0;
    for (int loop = 1; loop < storage.size(); loop++)
        if (storage[loop-1].distanceTo(storage[loop]) >= clusterThreshold) {
            clusters.push_back(Cluster(clusterStart, loop - 1, storage));
            clusterStart = loop;
        }
    if (clusterStart != storage.size() - 1 && storage.size() > 0)
        clusters.push_back(Cluster(clusterStart, storage.size() - 1, storage));
    return clusters;
}

const std::vector<Cluster> AlgorithmicDetector::detectLegs(const std::vector<Cluster>& clusters) const {
    std::vector<Cluster> legClusters;
    for (int loop = 0; loop < clusters.size(); loop++)
        if ((clusters[loop].size() < legSizeMax) && (clusters[loop].size() > legSizeMin))
            legClusters.push_back(Cluster(clusters[loop]));
    if (verbose && legClusters.size() > 0) LOG("%ld legs have been detected\n", legClusters.size());
    return legClusters;
}

const std::vector<Cluster> AlgorithmicDetector::detectChests(const std::vector<Cluster>& clusters) const {
    std::vector<Cluster> chestClusters;
    for (int loop = 0; loop < clusters.size(); loop++)
        if ((clusters[loop].size() < chestSizeMax) && (clusters[loop].size() > chestSizeMin))
            chestClusters.push_back(Cluster(clusters[loop]));
    if (verbose && chestClusters.size() > 0) LOG("%ld chests have been detected\n", chestClusters.size());
    return chestClusters;
}

const std::vector<Point> AlgorithmicDetector::detectPeople(const std::vector<Cluster>& legClusters, const std::vector<Cluster>& chestClusters, bool topScanReceived) const {
    std::vector<Point> detection;

    for (int loopLeftLeg = 0; loopLeftLeg < legClusters.size(); loopLeftLeg++)
        for (int loopRightLeg = loopLeftLeg + 1; loopRightLeg < legClusters.size(); loopRightLeg++) {
            float twoLegsDistance = legClusters[loopLeftLeg].distanceTo(legClusters[loopRightLeg]);
            bool personLegsDetected = (twoLegsDistance > legsDistanceMin && twoLegsDistance < legsDistanceMax);
            if (!personLegsDetected) continue;

            if (!topScanReceived) {
                detection.push_back(legClusters[loopLeftLeg].middleBetween(legClusters[loopRightLeg]));
                continue;
            }

            for (int loopChest = 0; loopChest < chestClusters.size(); loopChest++) {
                float leftChestDistance = legClusters[loopLeftLeg].distanceTo(chestClusters[loopChest]);
                float rightChestDistance = legClusters[loopRightLeg].distanceTo(chestClusters[loopChest]);
                bool personChestDetected = (leftChestDistance < distanceLevel) && (rightChestDistance < distanceLevel);
                if (!personChestDetected) continue;

                detection.push_back(chestClusters[loopChest].betweenPoint());
            }
        }
    if (verbose && detection.size() > 0) LOG("%ld persons have been detected\n", detection.size());
    return detection;
}

const std::vector<Tracked> AlgorithmicDetector::estimatePreviousPeople(const Point& newOdometry) {
    std::vector<Tracked> tracked(previousPeople.size());
    for (int loop = 0; loop < previousPeople.size(); loop++) {
        Tracked previous = previousPeople[loop];
        float angle = newOdometry.z - previousPosition.z;
        float sinus = sin(angle);
        float cosinus = cos(angle);
        float xRot = previous.x * cosinus - previous.y * sinus;
        float yRot = previous.x * sinus + previous.y * cosinus;
        float xTra = xRot + (newOdometry.x - previousPosition.x);
        float yTra = yRot + (newOdometry.y - previousPosition.y);
        tracked[loop] = Tracked(xTra, yTra, previous.z, previous.frequency, previous.uncertainty);
    }
    return tracked;
}

const std::vector<Tracked> AlgorithmicDetector::pullAssociatedPeople(std::vector<Point>& detectedPeople, const std::vector<Tracked>& estimatedPrevious) {
    std::vector<Tracked> associated;
    for (int prevLoop = 0; prevLoop < estimatedPrevious.size(); prevLoop++) {
        int associatedIndex = -1;
        float minDistance = uncertaintyMax;
        for (int detLoop = 0; detLoop < detectedPeople.size(); detLoop++) {
            float distance = estimatedPrevious[prevLoop].distanceTo(detectedPeople[detLoop]);
            if (distance < minDistance) {
                associatedIndex = detLoop;
                minDistance = distance;
            }
        }
        if (associatedIndex != -1) {
            associated.push_back(Tracked(detectedPeople[associatedIndex], frequencyInit, uncertaintyMin));
            detectedPeople.erase(detectedPeople.begin() + associatedIndex);
        } else {
            float uncertainty = estimatedPrevious[prevLoop].uncertainty + uncertaintyInc;
            int frequency = estimatedPrevious[prevLoop].frequency + 1;
            if (frequency < frequencyMax) associated.push_back(Tracked(estimatedPrevious[prevLoop], frequency, uncertainty));
        }
    }
    return associated;
}

const std::vector<Tracked> AlgorithmicDetector::trackDetectedPeople(const std::vector<Point>& detectedPeople) {
    std::vector<Tracked> tracked = previousPeople;
    for (int loop = 0; loop < detectedPeople.size(); loop++) tracked.push_back(Tracked(detectedPeople[loop], frequencyInit, uncertaintyMin));
    return tracked;
}

const std::vector<Point> AlgorithmicDetector::getCurrentlyDetectedPeople() {
    std::vector<Point> current;
    for (int loop = 0; loop < previousPeople.size(); loop++)
        if (previousPeople[loop].frequency == frequencyInit)
            current.push_back(previousPeople[loop]);
    return current;
}

const std::vector<Point> AlgorithmicDetector::forward(const std::vector<Point>& latestBottomScan, const std::vector<Point>& latestTopScan, bool topScanReceived, const Point& odometry, bool odometryReceived) {
    std::vector<Cluster> legs = detectLegs(performClustering(latestBottomScan));
    std::vector<Cluster> chests;
    if (topScanReceived) chests = detectChests(performClustering(latestTopScan));

    std::vector<Point> detected = detectPeople(legs, chests, topScanReceived);
    if (previousPeople.size() > 0) {
        std::vector<Tracked> estimatedPrevious = previousPeople;
        if (odometryReceived) estimatedPrevious = estimatePreviousPeople(odometry);
        previousPeople = pullAssociatedPeople(detected, estimatedPrevious);
    }

    previousPeople = trackDetectedPeople(detected);
    return getCurrentlyDetectedPeople();
}

const std::vector<Point> AlgorithmicDetector::forward(const std::vector<Point>& latestBottomScan) {
    Point odometry;
    std::vector<Point> latestTopScan;
    return forward(latestBottomScan, latestTopScan, false, odometry, false);
}

const std::vector<Point> AlgorithmicDetector::forward(const std::vector<Point>& latestBottomScan, const Point& odometry) {
    std::vector<Point> latestTopScan;
    return forward(latestBottomScan, latestTopScan, false, odometry, true);
}

const std::vector<Point> AlgorithmicDetector::forward(const std::vector<Point>& latestBottomScan, const std::vector<Point>& latestTopScan) {
    Point odometry;
    bool topScanExists = latestTopScan.size() > 0;
    return forward(latestBottomScan, latestTopScan, topScanExists, odometry, false);
}

const std::vector<Point> AlgorithmicDetector::forward(const std::vector<Point>& latestBottomScan, const std::vector<Point>& latestTopScan, const Point& odometry) {
    bool topScanExists = latestTopScan.size() > 0;
    return forward(latestBottomScan, latestTopScan, topScanExists, odometry, true);
}

AlgorithmicDetector::AlgorithmicDetector(bool logging, int freqInit, int freqMax, float uncertMax, float uncertMin, float uncertInc, float clustThresh, float distLevel, float legSizMin, float legSizMax, float chestSizMin, float chestSizMax, float legsDistMin, float legsDistMax): verbose(logging), frequencyInit(freqInit), frequencyMax(freqMax), uncertaintyMax(uncertMax), uncertaintyMin(uncertMin), uncertaintyInc(uncertInc), clusterThreshold(clustThresh), distanceLevel(distLevel), legSizeMin(legSizMin), legSizeMax(legSizMax), chestSizeMin(chestSizMin), chestSizeMax(chestSizMax), legsDistanceMin(legsDistMin), legsDistanceMax(legsDistMax) {}
