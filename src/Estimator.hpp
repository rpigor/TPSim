#pragma once

#include "Cell.hpp"
#include "Power.hpp"
#include <unordered_map>
#include <string>

namespace Estimator {

    unsigned long estimateEndTime(unsigned long startTick, double slope, const std::string& timeUnit, const std::string& tickUnit);
    double estimate(std::unordered_map<Arc, LUT> viewPerArc, const Arc& arc, double x, double y, bool isRising, bool extrapolate);
    double bilinearInterpolate(double x, double y, const std::vector<double>& xIdxValues, const std::vector<double>& yIdxValues, const std::vector<std::vector<double>>& zMatrix, bool extrapolate);
    std::tuple<std::vector<double>::size_type, std::vector<double>::size_type> neighboringIdxs(double value, const std::vector<double>& values, bool extrapolate);
    double interpolate(double x, double x1, double x2, double y1, double y2);

}