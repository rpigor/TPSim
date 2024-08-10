#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/Estimator.hpp"
#include <vector>

TEST_CASE( "interpolation function works", "[estimator]" ) {

    double itp;
    SECTION( "positive interpolation" ) {
        itp = Estimator::interpolate(0.5, 0, 1, 50, 100);
        REQUIRE_THAT(itp, Catch::Matchers::WithinRel(75, 0.001));

        itp = Estimator::interpolate(0.5, 1, 0, 100, 50);
        REQUIRE_THAT(itp, Catch::Matchers::WithinRel(75, 0.001));

        REQUIRE(Estimator::interpolate(0.5, 0, 1, 0, 0) == 0.0);
        REQUIRE(Estimator::interpolate(2849, 0, 1, 0, 0) == 0.0);
    }
    SECTION( "negative interpolation" ) {
        itp = Estimator::interpolate(-5.0, 0.0, -10.0, 0.5, 1.0);
        REQUIRE_THAT(itp, Catch::Matchers::WithinRel(0.75, 0.001));

        itp = Estimator::interpolate(-0.5, -1, 0, -100, -50);
        REQUIRE_THAT(itp, Catch::Matchers::WithinRel(-75, 0.001));

        REQUIRE(Estimator::interpolate(-0.000183, -0.249, -195.3, 0.0, 0.0) == 0.0);
    }
    SECTION( "edge intepolation" ) {
        itp = Estimator::interpolate(0.01, 0.01, 0.01735, 0.01414, 0.05208);
        REQUIRE_THAT(itp, Catch::Matchers::WithinRel(0.01414, 0.001));

        itp = Estimator::interpolate(0.01, 0.01735, 0.01, 0.05208, 0.01414);
        REQUIRE_THAT(itp, Catch::Matchers::WithinRel(0.01414, 0.001));

        itp = Estimator::interpolate(0.01735, 0.01, 0.01735, 0.01414, 0.05208);
        REQUIRE_THAT(itp, Catch::Matchers::WithinRel(0.05208, 0.001));
    }
    SECTION( "extrapolation" ) {
        itp = Estimator::interpolate(-0.5, 1, 0, 100, 50);
        REQUIRE_THAT(itp, Catch::Matchers::WithinRel(25, 0.001));

        itp = Estimator::interpolate(2, 0, 1, 50, 100);
        REQUIRE_THAT(itp, Catch::Matchers::WithinRel(150, 0.001));

        REQUIRE(Estimator::interpolate(0.5, 0, 0, 50, 100) == std::numeric_limits<double>::infinity());
        REQUIRE(Estimator::interpolate(-0.5, 0, 0, 50, 100) == -std::numeric_limits<double>::infinity());
        REQUIRE(Estimator::interpolate(0.5, 0, 0, 100, 50) == -std::numeric_limits<double>::infinity());
    }

}

TEST_CASE( "neighboringIdxs function works", "[estimator]" ) {

    std::vector<double> values = {0.01,0.01735,0.02602,0.03903,0.05855,0.08782,0.13172,0.19757,0.29634,0.44449,0.6667,1.0,1.5};
    SECTION( "indexes in the middle" ) {
        auto [x1, y1] = Estimator::neighboringIdxs(0.06, values, false);
        REQUIRE(x1 == 4);
        REQUIRE(y1 == 5);

        auto [x2, y2] = Estimator::neighboringIdxs(0.05855, values, false);
        REQUIRE(x2 == 4);
        REQUIRE(y2 == 5);
    }
    SECTION( "corner indexes" ) {
        auto [x1, y1] = Estimator::neighboringIdxs(0.01, values, false);
        REQUIRE(x1 == 0);
        REQUIRE(y1 == 1);

        auto [x2, y2] = Estimator::neighboringIdxs(1.5, values, false);
        REQUIRE(x2 == 11);
        REQUIRE(y2 == 12);
    }
    SECTION( "extrapolation" ) {
        REQUIRE_THROWS(Estimator::neighboringIdxs(0.005, values, false));
        REQUIRE_THROWS(Estimator::neighboringIdxs(2.15, values, false));

        auto [x1, y1] = Estimator::neighboringIdxs(2.15, values, true);
        REQUIRE(x1 == 11);
        REQUIRE(y1 == 12);

        auto [x2, y2] = Estimator::neighboringIdxs(0.005, values, true);
        REQUIRE(x2 == 0);
        REQUIRE(y2 == 1);
    }

    // should also check if function works for monotonically decreasing vectors

}

TEST_CASE( "bilinearInterpolate function works", "[estimator]" ) {

    std::vector<double> xs = {0.01,0.01735,0.02602,0.03903,0.05855,0.08782,0.13172,0.19757,0.29634,0.44449,0.6667,1.0,1.5};
    std::vector<double> ys = {0.0,0.00894,0.01073,0.01288,0.01545,0.01854,0.02225,0.0267,0.03204,0.03845,0.04614,0.05537,0.06644,0.07973,0.09568,0.11482,0.13779,0.16535,0.19842,0.23811};
    std::vector<std::vector<double>> zs = {
        {0.01414,0.05208,0.05945,0.06818,0.07869,0.09124,0.1062,0.1243,0.14589,0.1717,0.20295,0.24032,0.28517,0.33891,0.40337,0.48079,0.57359,0.68483,0.81818,0.97923},
        {0.01689,0.05454,0.06197,0.07069,0.08118,0.0938,0.10881,0.12696,0.14854,0.17454,0.20544,0.24277,0.28772,0.34114,0.40581,0.48331,0.57576,0.68755,0.82148,0.98179},
        {0.01975,0.05763,0.06492,0.07378,0.08423,0.09683,0.11183,0.12986,0.15157,0.17756,0.20858,0.24588,0.29084,0.34433,0.40913,0.48636,0.57937,0.69068,0.82437,0.98442},
        {0.02277,0.06239,0.06974,0.07855,0.08904,0.10157,0.11674,0.13473,0.15643,0.1823,0.21352,0.25084,0.29567,0.34945,0.41395,0.49145,0.58395,0.69573,0.82932,0.98947},
        {0.02625,0.06972,0.07702,0.0858,0.09624,0.10888,0.12403,0.14196,0.1637,0.18965,0.22068,0.25808,0.30274,0.35672,0.42113,0.49848,0.5914,0.70292,0.83661,0.99591},
        {0.03035,0.08109,0.08833,0.09705,0.10744,0.11994,0.13508,0.15304,0.17487,0.20069,0.23187,0.26914,0.31387,0.36742,0.43207,0.50934,0.60167,0.71289,0.84728,1.00762},
        {0.03544,0.09723,0.10532,0.11423,0.12454,0.13698,0.1519,0.16988,0.19157,0.21749,0.24863,0.2858,0.33069,0.38422,0.44884,0.52564,0.61893,0.73036,0.8637,1.02323},
        {0.04212,0.11648,0.12645,0.13746,0.14955,0.1629,0.17782,0.19559,0.2171,0.24278,0.27396,0.31129,0.35573,0.40952,0.47392,0.55093,0.64404,0.75465,0.88876,1.04916},
        {0.05108,0.13919,0.15143,0.16498,0.17988,0.19631,0.21438,0.23423,0.25605,0.28166,0.31248,0.34951,0.39394,0.44758,0.51208,0.58934,0.68182,0.79335,0.92605,1.08705},
        {0.06318,0.16607,0.18092,0.19746,0.21572,0.23593,0.25819,0.28264,0.30948,0.33892,0.37121,0.40795,0.4523,0.5056,0.56967,0.64639,0.73929,0.85039,0.98429,1.14309},
        {0.07971,0.19863,0.21625,0.23606,0.25813,0.28271,0.30992,0.33994,0.37296,0.40921,0.44892,0.49252,0.54012,0.59332,0.65712,0.73362,0.82572,0.93672,1.06982,1.22992},
        {0.10253,0.23947,0.25997,0.28317,0.30917,0.33847,0.37127,0.40767,0.44797,0.49237,0.54127,0.59487,0.65357,0.71787,0.78827,0.86577,0.95737,1.06767,1.20017,1.35937},
        {0.1346,0.2923,0.3158,0.3426,0.3727,0.4069,0.4454,0.4887,0.537,0.5908,0.6503,0.7159,0.7881,0.8672,0.9539,1.0486,1.1524,1.266,1.3975,1.5559}
    };
    double result;
    SECTION( "in the middle" ) {
        result = Estimator::bilinearInterpolate(0.01735, 0.01288, xs, ys, zs, false);
        REQUIRE_THAT(result, Catch::Matchers::WithinRel(0.07069, 0.001));

        result = Estimator::bilinearInterpolate(0.19757, 0.01545, xs, ys, zs, false);
        REQUIRE_THAT(result, Catch::Matchers::WithinRel(0.14955, 0.001));

        result = Estimator::bilinearInterpolate(0.015, 0.012, xs, ys, zs, false);
        REQUIRE_THAT(result, Catch::Matchers::WithinRel(0.06631705806043348, 0.001));
    }
    SECTION( "edges" ) {
        result = Estimator::bilinearInterpolate(0.01, 0.01288, xs, ys, zs, false);
        REQUIRE_THAT(result, Catch::Matchers::WithinRel(0.06818, 0.001));

        result = Estimator::bilinearInterpolate(0.19757, 0.23811, xs, ys, zs, false);
        REQUIRE_THAT(result, Catch::Matchers::WithinRel(1.04916, 0.001));

        result = Estimator::bilinearInterpolate(0.01, 0.0, xs, ys, zs, false);
        REQUIRE_THAT(result, Catch::Matchers::WithinRel(0.01414, 0.001));

        result = Estimator::bilinearInterpolate(1.5, 0.23811, xs, ys, zs, false);
        REQUIRE_THAT(result, Catch::Matchers::WithinRel(1.5559, 0.001));
    }
    SECTION( "extrapolation" ) {
        REQUIRE_THROWS(Estimator::bilinearInterpolate(0.005, 0.01288, xs, ys, zs, false));
        REQUIRE_THROWS(Estimator::bilinearInterpolate(0.025, 0.25, xs, ys, zs, false));

        result = Estimator::bilinearInterpolate(0.0, 0.004, xs, ys, zs, true);
        REQUIRE_THAT(result, Catch::Matchers::WithinRel(0.02755043083900227, 0.001));
    }

    // should also check if function works for monotonically decreasing x and y vectors

}