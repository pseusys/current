#pragma once

#include <pybind11/numpy.h>

#include "detector.hpp"

namespace py = pybind11;


namespace follow_the_drow {
    class PythonDetectorFactory {
        private:
            float minAngle, incAngle;
            AlgorithmicDetector detector;

            const std::vector<Point> toPointVector(std::vector<float>& measures);
            const Point toPoint(std::vector<float>& measures);
            float* toRawFloats(std::vector<Point>& points);

        public:
            PythonDetectorFactory(float minAngle, float incAngle, int frequencyInit, int frequencyMax, float uncertaintyMax, float uncertaintyMin, float uncertaintyInc, float clusterThreshold, float distanceLevel, float legSizeMin, float legSizeMax, float chestSizeMin, float chestSizeMax, float legsDistanceMin, float legsDistanceMax, bool logging);

            const py::array_t<float> forwardOne(const py::array_t<float>& latestBottomScan, const py::array_t<float>& latestOdometry);
            const py::array_t<float> forwardBoth(const py::array_t<float>& latestBottomScan, const py::array_t<float>& latestTopScan, const py::array_t<float>& latestOdometry);
    };
}
