/**
 * Test setup depends on Catch2, available at https://github.com/catchorg/Catch2.
 * Simply take the header-only download and place the header in this directory with name "catch.hpp".
 * Recommended to run following command _once_ (takes a while) to precompile main for the test setup:
 *   $ clang++ catch-main.cpp -c
 *
 * Afterwards, you can compile and run the actual tests as follows:
 *   $ clang++ geotests.cpp geo.cpp -std=c++17 -Wall -pedantic -c && clang++ -std=c++17 -Wall -o
 *          geotests geo.o catch-main.o geotests.o
 *   $ ./geotests
 */

#include "geo.hpp"
#include <cassert>
#include <iostream>

#include "catch.hpp"

using namespace geo;

TEST_CASE("angle arithmetic", "[angle]")
{
    Angle a1{1.5};
    Angle a2{0};
    Angle a3{-1.5};
    Angle a4{PI};
    Angle a5{-PI};
    Angle a6{2 * PI};

    Angle small_pos{0.1};
    Angle large_pos{-0.1};

    SECTION("constructor")
    {
        REQUIRE(a1.theta == Approx{1.5});
        REQUIRE(a2.theta == Approx{0});
        REQUIRE(a3.theta == Approx{2 * PI - 1.5});

        REQUIRE(a4.theta == Approx{PI});
        REQUIRE(a5.theta == Approx{PI});
        REQUIRE(a6.theta == Approx{0});
    }

    SECTION("arithmetic")
    {
        REQUIRE((a4 + a4).theta == Approx{0});
        REQUIRE((a2 + a2).theta == Approx{0});

        REQUIRE((small_pos - large_pos).theta == Approx{0.2});
        REQUIRE((a4 - small_pos).theta == Approx{3.0415926535});
        REQUIRE((a5 - Angle{0}).theta == Approx(PI));

        REQUIRE((Angle{PI} - Angle{-PI}).theta == Approx{0});
        REQUIRE((Angle{PI} + Angle{-PI}).theta == Approx{0});
    }

    SECTION("negate")
    {
        REQUIRE((-a1).theta == Approx{2 * PI - 1.5});
        REQUIRE((-Angle{0}).theta == Approx{0});
    }
}

TEST_CASE("point operations", "[point]")
{
    GlobalPoint orig{0, 0};
    GlobalPoint p1{1, 0};
    GlobalPoint p2{0, 1};
    GlobalPoint p3{-1, 0};
    GlobalPoint p4{0, -1};

    SECTION("angle of line")
    {
        REQUIRE(angle_of_line(orig, p2).theta == Approx{PI / 2});
        REQUIRE(angle_of_line(orig, p1).theta == Approx{0});
        REQUIRE(angle_of_line(orig, p3).theta == Approx{PI});
        REQUIRE(angle_of_line(orig, p4).theta == Approx{3 * PI / 2});
    }
    SECTION("addition")
    {
        auto p12 = p1 + p2;
        REQUIRE(p12.x == 1);
        REQUIRE(p12.y == 1);
    }
}

TEST_CASE("coordinate translations", "[point,angle]")
{
    SECTION("rotation 90 deg")
    {
        auto rotated = to_global_coordinates(GlobalPoint{0, 0}, Angle{PI / 2}, RelPoint{0, 1});
        REQUIRE(rotated.x == Approx{-1});
        // shhhhh don't worry about it
        REQUIRE(std::abs(rotated.y) < 0.0001);
    }

    SECTION("rotation")
    {
        auto rotated = to_global_coordinates(GlobalPoint{0, 0}, Angle{PI / 4}, RelPoint{1, 1});
        REQUIRE(std::abs(rotated.x) < 0.0001);
        REQUIRE(rotated.y == Approx{std::sqrt(2)});
    }

    SECTION("translation")
    {
        GlobalPoint orig{4, 2};
        RelPoint relpoint{-1, 2};
        auto moved = to_global_coordinates(orig, Angle{0}, relpoint);
        REQUIRE(moved.x == Approx{3});
        REQUIRE(moved.y == Approx{4});
    }

    SECTION("rotate and translate")
    {
        GlobalPoint orig{1, 0};
        Angle angle{PI / 2};
        RelPoint relpoint{1, 0};
        auto translated = to_global_coordinates(orig, angle, relpoint);
        REQUIRE(translated.x == Approx{1});
        REQUIRE(translated.y == Approx{1});
    }
}
