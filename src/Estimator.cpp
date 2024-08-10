#include "Estimator.hpp"
#include <tuple>
#include <algorithm>

double Estimator::estimate(std::unordered_map<Arc, LUT> viewPerArc, const Arc& arc, double x, double y, bool isRising, bool extrapolate) {
    auto xIdxValues = viewPerArc.at(arc).xIdxValues;
    auto yIdxValues = viewPerArc.at(arc).yIdxValues;
    auto zMatrix = isRising ? viewPerArc.at(arc).riseZMatrix : viewPerArc.at(arc).fallZMatrix;

    return bilinearInterpolate(x, y, xIdxValues, yIdxValues, zMatrix, extrapolate);
}

double Estimator::bilinearInterpolate(double x, double y, const std::vector<double>& xIdxValues, const std::vector<double>& yIdxValues, const std::vector<std::vector<double>>& zMatrix, bool extrapolate) {
    auto [xLowerIdx, xUpperIdx] = neighboringIdxs(x, xIdxValues, extrapolate);
    auto xLower = xIdxValues.at(xLowerIdx);
    auto xUpper = xIdxValues.at(xUpperIdx);

    auto [yLowerIdx, yUpperIdx] = neighboringIdxs(y, yIdxValues, extrapolate);
    auto yLower = yIdxValues.at(yLowerIdx);
    auto yUpper = yIdxValues.at(yUpperIdx);

    double zLowerXLowerY = zMatrix.at(xLowerIdx).at(yLowerIdx);
    double zLowerXUpperY = zMatrix.at(xLowerIdx).at(yUpperIdx);
    double zUpperXLowerY = zMatrix.at(xUpperIdx).at(yLowerIdx);
    double zUpperXUpperY = zMatrix.at(xUpperIdx).at(yUpperIdx);

    double zLowerX = interpolate(y, yLower, yUpper, zLowerXLowerY, zLowerXUpperY);
    double zUpperX = interpolate(y, yLower, yUpper, zUpperXLowerY, zUpperXUpperY);
    double zItp = interpolate(x, xLower, xUpper, zLowerX, zUpperX);

    return zItp;
}

std::tuple<std::vector<double>::size_type, std::vector<double>::size_type> Estimator::neighboringIdxs(double value, const std::vector<double>& values, bool extrapolate) {
    auto endIt = std::find_if(values.begin(), values.end(), [value](double v) { return value > v; });
    decltype(endIt) startIt;

    if (endIt == values.end()) {
        if (!extrapolate) {
            throw std::runtime_error("cannot find neighboring value " + std::to_string(value) + " (out of upper bound)");
        }
        endIt--;
    }
    else if (endIt == values.begin()) {
        if (!extrapolate) {
            throw std::runtime_error("cannot find neighboring value " + std::to_string(value) + " (out of lower bound)");
        }
        endIt++;
    }
    startIt = endIt - 1;

    return { std::distance(values.begin(), startIt), std::distance(values.begin(), endIt) };
}

double Estimator::interpolate(double x, double x1, double x2, double y1, double y2) {
    return y1 + ((y2-y1)/(x2-x1))*(x-x1);
}