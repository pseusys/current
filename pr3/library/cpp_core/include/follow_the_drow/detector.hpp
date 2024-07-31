#pragma once

#include <vector>

#include "tracked.hpp"

#define _FREQUENCY_INIT 5
#define _FREQUENCY_MAX 25

#define _UNCERTAINTY_MIN 1
#define _UNCERTAINTY_MAX 3
#define _UNCERTAINTY_INC 0.05

#define _CLUSTER_THRESHOLD 0.1

#define _LEG_SIZE_MIN 0.05
#define _LEG_SIZE_MAX 0.25
#define _LEGS_DISTANCE_MIN 0.0
#define _LEGS_DISTANCE_MAX 0.7

#define _CHEST_SIZE_MIN 0.3
#define _CHEST_SIZE_MAX 0.8

#define _DISTANCE_LEVEL 0.6


namespace follow_the_drow {
    class Cluster {
        private:
            float dSize;
            int iStart, iMiddle, iEnd;
            Point pStart, pMiddle, pEnd, pBetween;

        public:
            Cluster(const Cluster& cluster);
            Cluster(int start, int end, const std::vector<Point>& reference);

            float size() const;
            int start() const;
            int middle() const;
            int end() const;

            const Point& startPoint() const;
            const Point& middlePoint() const;
            const Point& betweenPoint() const;
            const Point& endPoint() const;

            float distanceTo(const Cluster& cluster) const;
            Point middleBetween(const Cluster& cluster) const;
    };


    class AlgorithmicDetector {
        private:
            const int frequencyInit, frequencyMax;
            const float uncertaintyMax, uncertaintyMin, uncertaintyInc, clusterThreshold, distanceLevel;
            const float legSizeMin, legSizeMax, chestSizeMin, chestSizeMax, legsDistanceMin, legsDistanceMax;
            const bool verbose = true;

            Point previousPosition;
            std::vector<Tracked> previousPeople;

            const std::vector<Cluster> performClustering(const std::vector<Point>& storage) const;
            const std::vector<Cluster> detectLegs(const std::vector<Cluster>& clusters) const;
            const std::vector<Cluster> detectChests(const std::vector<Cluster>& clusters) const;
            const std::vector<Point> detectPeople(const std::vector<Cluster>& legClusters, const std::vector<Cluster>& chestClusters, bool topScanReceived) const;

            const std::vector<Tracked> estimatePreviousPeople(const Point& newOdometry);
            const std::vector<Tracked> pullAssociatedPeople(std::vector<Point>& detectedPeople, const std::vector<Tracked>& estimatedPrevious);
            const std::vector<Tracked> trackDetectedPeople(const std::vector<Point>& detectedPeople);
            const std::vector<Point> getCurrentlyDetectedPeople();

            const std::vector<Point> forward(const std::vector<Point>& latestBottomScan, const std::vector<Point>& latestTopScan, bool topScanReceived, const Point& odometry, bool odometryReceived);

        public:
            AlgorithmicDetector(bool logging, int freqInit = _FREQUENCY_INIT, int freqMax = _FREQUENCY_MAX, float uncertMax = _UNCERTAINTY_MAX, float uncertMin = _UNCERTAINTY_MIN, float uncertInc = _UNCERTAINTY_INC, float clustThresh = _CLUSTER_THRESHOLD, float distLevel = _DISTANCE_LEVEL, float legSizMin = _LEG_SIZE_MIN, float legSizMax = _LEG_SIZE_MAX, float chestSizMin = _CHEST_SIZE_MIN, float chestSizMax = _CHEST_SIZE_MAX, float legsDistMin = _LEGS_DISTANCE_MIN, float legsDistMax = _LEGS_DISTANCE_MAX);

            const std::vector<Point> forward(const std::vector<Point>& latestBottomScan);
            const std::vector<Point> forward(const std::vector<Point>& latestBottomScan, const Point& odometry);
            const std::vector<Point> forward(const std::vector<Point>& latestBottomScan, const std::vector<Point>& latestTopScan);
            const std::vector<Point> forward(const std::vector<Point>& latestBottomScan, const std::vector<Point>& latestTopScan, const Point& odometry);
    };
}
