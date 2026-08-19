// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "WexTestClass.h"
#include "../../inc/consoletaeftemplates.hpp"

#include "CommonState.hpp"

#include "../../types/inc/Viewport.hpp"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

class ViewportTests
{
    TEST_CLASS(ViewportTests);

	std::cerr << ">>> Entering TEST_METHOD(CreateEmpty=" << CreateEmpty << ")" << std::endl;
	std::cerr << "TEST_METHOD(CreateEmpty=" << CreateEmpty << "): v → " << v << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CreateEmpty=" << CreateEmpty << ")" << std::endl;
    TEST_METHOD(CreateEmpty)
    {
		std::cerr << ">>> Entering TEST_METHOD(CreateEmpty=" << CreateEmpty << ")" << std::endl;
        const auto v = Viewport::Empty();

        VERIFY_ARE_EQUAL(0, v.Left());
        VERIFY_ARE_EQUAL(-1, v.RightInclusive());
        VERIFY_ARE_EQUAL(0, v.RightExclusive());
        VERIFY_ARE_EQUAL(0, v.Top());
        VERIFY_ARE_EQUAL(-1, v.BottomInclusive());
        VERIFY_ARE_EQUAL(0, v.BottomExclusive());
        VERIFY_ARE_EQUAL(0, v.Height());
        VERIFY_ARE_EQUAL(0, v.Width());
        VERIFY_ARE_EQUAL(til::point{}, v.Origin());
        VERIFY_ARE_EQUAL(til::size{}, v.Dimensions());
    }

	std::cerr << ">>> Entering TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << ")" << std::endl;
	std::cerr << "TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << "): rect.top → " << rect.top << std::endl;
	std::cerr << "TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << "): rect.bottom → " << rect.bottom << std::endl;
	std::cerr << "TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << "): rect.left → " << rect.left << std::endl;
	std::cerr << "TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << "): rect.right → " << rect.right << std::endl;
	std::cerr << "TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << "): origin.x → " << origin.x << std::endl;
	std::cerr << "TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << "): origin.y → " << origin.y << std::endl;
	std::cerr << "TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << "): dimensions.width → " << dimensions.width << std::endl;
	std::cerr << "TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << "): dimensions.height → " << dimensions.height << std::endl;
	std::cerr << "TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << "): v → " << v << std::endl;
    TEST_METHOD(CreateFromInclusive)
    {
		std::cerr << ">>> Entering TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << ")" << std::endl;
        til::inclusive_rect rect;
		std::cerr << "<<< Exiting TEST_METHOD(CreateEmpty=" << CreateEmpty << ")" << std::endl;
        rect.top = 3;
        rect.bottom = 5;
        rect.left = 10;
        rect.right = 20;

        til::point origin;
        origin.x = rect.left;
        origin.y = rect.top;

        til::size dimensions;
        dimensions.width = rect.right - rect.left + 1;
        dimensions.height = rect.bottom - rect.top + 1;

        const auto v = Viewport::FromInclusive(rect);

        VERIFY_ARE_EQUAL(rect.left, v.Left());
        VERIFY_ARE_EQUAL(rect.right, v.RightInclusive());
        VERIFY_ARE_EQUAL(rect.right + 1, v.RightExclusive());
        VERIFY_ARE_EQUAL(rect.top, v.Top());
        VERIFY_ARE_EQUAL(rect.bottom, v.BottomInclusive());
        VERIFY_ARE_EQUAL(rect.bottom + 1, v.BottomExclusive());
        VERIFY_ARE_EQUAL(dimensions.height, v.Height());
        VERIFY_ARE_EQUAL(dimensions.width, v.Width());
        VERIFY_ARE_EQUAL(origin, v.Origin());
        VERIFY_ARE_EQUAL(dimensions, v.Dimensions());
    }

	std::cerr << ">>> Entering TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << ")" << std::endl;
	std::cerr << "TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << "): rect.top → " << rect.top << std::endl;
	std::cerr << "TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << "): rect.bottom → " << rect.bottom << std::endl;
	std::cerr << "TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << "): rect.left → " << rect.left << std::endl;
	std::cerr << "TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << "): rect.right → " << rect.right << std::endl;
	std::cerr << "TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << "): origin.x → " << origin.x << std::endl;
	std::cerr << "TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << "): origin.y → " << origin.y << std::endl;
	std::cerr << "TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << "): dimensions.width → " << dimensions.width << std::endl;
	std::cerr << "TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << "): dimensions.height → " << dimensions.height << std::endl;
	std::cerr << "TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << "): v → " << v << std::endl;
    TEST_METHOD(CreateFromExclusive)
    {
		std::cerr << ">>> Entering TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << ")" << std::endl;
        til::rect rect;
		std::cerr << "<<< Exiting TEST_METHOD(CreateFromInclusive=" << CreateFromInclusive << ")" << std::endl;
        rect.top = 3;
        rect.bottom = 5;
        rect.left = 10;
        rect.right = 20;

        til::point origin;
        origin.x = rect.left;
        origin.y = rect.top;

        til::size dimensions;
        dimensions.width = rect.right - rect.left;
        dimensions.height = rect.bottom - rect.top;

        const auto v = Viewport::FromExclusive(rect);

        VERIFY_ARE_EQUAL(rect.left, v.Left());
        VERIFY_ARE_EQUAL(rect.right - 1, v.RightInclusive());
        VERIFY_ARE_EQUAL(rect.right, v.RightExclusive());
        VERIFY_ARE_EQUAL(rect.top, v.Top());
        VERIFY_ARE_EQUAL(rect.bottom - 1, v.BottomInclusive());
        VERIFY_ARE_EQUAL(rect.bottom, v.BottomExclusive());
        VERIFY_ARE_EQUAL(dimensions.height, v.Height());
        VERIFY_ARE_EQUAL(dimensions.width, v.Width());
        VERIFY_ARE_EQUAL(origin, v.Origin());
        VERIFY_ARE_EQUAL(dimensions, v.Dimensions());
    }

	std::cerr << ">>> Entering TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << ")" << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << "): rect.top → " << rect.top << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << "): rect.bottom → " << rect.bottom << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << "): rect.left → " << rect.left << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << "): rect.right → " << rect.right << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << "): origin.x → " << origin.x << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << "): origin.y → " << origin.y << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << "): dimensions.width → " << dimensions.width << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << "): dimensions.height → " << dimensions.height << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << "): v → " << v << std::endl;
    TEST_METHOD(CreateFromDimensionsWidthHeight)
    {
		std::cerr << ">>> Entering TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << ")" << std::endl;
        til::inclusive_rect rect;
		std::cerr << "<<< Exiting TEST_METHOD(CreateFromExclusive=" << CreateFromExclusive << ")" << std::endl;
        rect.top = 3;
        rect.bottom = 5;
        rect.left = 10;
        rect.right = 20;

        til::point origin;
        origin.x = rect.left;
        origin.y = rect.top;

        til::size dimensions;
        dimensions.width = rect.right - rect.left + 1;
        dimensions.height = rect.bottom - rect.top + 1;

        const auto v = Viewport::FromDimensions(origin, dimensions);

        VERIFY_ARE_EQUAL(rect.left, v.Left());
        VERIFY_ARE_EQUAL(rect.right, v.RightInclusive());
        VERIFY_ARE_EQUAL(rect.right + 1, v.RightExclusive());
        VERIFY_ARE_EQUAL(rect.top, v.Top());
        VERIFY_ARE_EQUAL(rect.bottom, v.BottomInclusive());
        VERIFY_ARE_EQUAL(rect.bottom + 1, v.BottomExclusive());
        VERIFY_ARE_EQUAL(dimensions.height, v.Height());
        VERIFY_ARE_EQUAL(dimensions.width, v.Width());
        VERIFY_ARE_EQUAL(origin, v.Origin());
        VERIFY_ARE_EQUAL(dimensions, v.Dimensions());
    }

	std::cerr << ">>> Entering TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << ")" << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << "): rect.top → " << rect.top << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << "): rect.bottom → " << rect.bottom << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << "): rect.left → " << rect.left << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << "): rect.right → " << rect.right << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << "): origin.x → " << origin.x << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << "): origin.y → " << origin.y << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << "): dimensions.width → " << dimensions.width << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << "): dimensions.height → " << dimensions.height << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << "): v → " << v << std::endl;
    TEST_METHOD(CreateFromDimensions)
    {
		std::cerr << ">>> Entering TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << ")" << std::endl;
        til::inclusive_rect rect;
		std::cerr << "<<< Exiting TEST_METHOD(CreateFromDimensionsWidthHeight=" << CreateFromDimensionsWidthHeight << ")" << std::endl;
        rect.top = 3;
        rect.bottom = 5;
        rect.left = 10;
        rect.right = 20;

        til::point origin;
        origin.x = rect.left;
        origin.y = rect.top;

        til::size dimensions;
        dimensions.width = rect.right - rect.left + 1;
        dimensions.height = rect.bottom - rect.top + 1;

        const auto v = Viewport::FromDimensions(origin, dimensions);

        VERIFY_ARE_EQUAL(rect.left, v.Left());
        VERIFY_ARE_EQUAL(rect.right, v.RightInclusive());
        VERIFY_ARE_EQUAL(rect.right + 1, v.RightExclusive());
        VERIFY_ARE_EQUAL(rect.top, v.Top());
        VERIFY_ARE_EQUAL(rect.bottom, v.BottomInclusive());
        VERIFY_ARE_EQUAL(rect.bottom + 1, v.BottomExclusive());
        VERIFY_ARE_EQUAL(dimensions.height, v.Height());
        VERIFY_ARE_EQUAL(dimensions.width, v.Width());
        VERIFY_ARE_EQUAL(origin, v.Origin());
        VERIFY_ARE_EQUAL(dimensions, v.Dimensions());
    }

	std::cerr << ">>> Entering TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << ")" << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << "): rect.top → " << rect.top << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << "): rect.left → " << rect.left << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << "): rect.bottom → " << rect.bottom << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << "): rect.right → " << rect.right << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << "): origin.x → " << origin.x << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << "): origin.y → " << origin.y << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << "): dimensions.width → " << dimensions.width << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << "): dimensions.height → " << dimensions.height << std::endl;
	std::cerr << "TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << "): v → " << v << std::endl;
    TEST_METHOD(CreateFromDimensionsNoOrigin)
    {
		std::cerr << ">>> Entering TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << ")" << std::endl;
        til::inclusive_rect rect;
		std::cerr << "<<< Exiting TEST_METHOD(CreateFromDimensions=" << CreateFromDimensions << ")" << std::endl;
        rect.top = 0;
        rect.left = 0;
        rect.bottom = 5;
        rect.right = 20;

        til::point origin;
        origin.x = rect.left;
        origin.y = rect.top;

        til::size dimensions;
        dimensions.width = rect.right - rect.left + 1;
        dimensions.height = rect.bottom - rect.top + 1;

        const auto v = Viewport::FromDimensions({}, dimensions);

        VERIFY_ARE_EQUAL(rect.left, v.Left());
        VERIFY_ARE_EQUAL(rect.right, v.RightInclusive());
        VERIFY_ARE_EQUAL(rect.right + 1, v.RightExclusive());
        VERIFY_ARE_EQUAL(rect.top, v.Top());
        VERIFY_ARE_EQUAL(rect.bottom, v.BottomInclusive());
        VERIFY_ARE_EQUAL(rect.bottom + 1, v.BottomExclusive());
        VERIFY_ARE_EQUAL(dimensions.height, v.Height());
        VERIFY_ARE_EQUAL(dimensions.width, v.Width());
        VERIFY_ARE_EQUAL(origin, v.Origin());
        VERIFY_ARE_EQUAL(dimensions, v.Dimensions());
    }

	std::cerr << ">>> Entering TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << ")" << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << "): r.top → " << r.top << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << "): r.bottom → " << r.bottom << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << "): r.left → " << r.left << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << "): r.right → " << r.right << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << "): v → " << v << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << "): c.x → " << c.x << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << "): c.y → " << c.y << std::endl;
    TEST_METHOD(IsInBoundsCoord)
    {
		std::cerr << ">>> Entering TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << ")" << std::endl;
        til::inclusive_rect r;
		std::cerr << "<<< Exiting TEST_METHOD(CreateFromDimensionsNoOrigin=" << CreateFromDimensionsNoOrigin << ")" << std::endl;
        r.top = 3;
        r.bottom = 5;
        r.left = 10;
        r.right = 20;

        const auto v = Viewport::FromInclusive(r);

        til::point c;
        c.x = r.left;
        c.y = r.top;
        VERIFY_IS_TRUE(v.IsInBounds(c), L"Top left corner in bounds.");

        c.y = r.bottom;
        VERIFY_IS_TRUE(v.IsInBounds(c), L"Bottom left corner in bounds.");

        c.x = r.right;
        VERIFY_IS_TRUE(v.IsInBounds(c), L"Bottom right corner in bounds.");

        c.y = r.top;
        VERIFY_IS_TRUE(v.IsInBounds(c), L"Top right corner in bounds.");

        c.x++;
        VERIFY_IS_FALSE(v.IsInBounds(c), L"One right out the top right is out of bounds.");

        c.x--;
        c.y--;
        VERIFY_IS_FALSE(v.IsInBounds(c), L"One up out the top right is out of bounds.");

        c.x = r.left;
        c.y = r.top;
        c.x--;
        VERIFY_IS_FALSE(v.IsInBounds(c), L"One left out the top left is out of bounds.");

        c.x++;
        c.y--;
        VERIFY_IS_FALSE(v.IsInBounds(c), L"One up out the top left is out of bounds.");

        c.x = r.left;
        c.y = r.bottom;
        c.x--;
        VERIFY_IS_FALSE(v.IsInBounds(c), L"One left out the bottom left is out of bounds.");

        c.x++;
        c.y++;
        VERIFY_IS_FALSE(v.IsInBounds(c), L"One down out the bottom left is out of bounds.");

        c.x = r.right;
        c.y = r.bottom;
        c.x++;
        VERIFY_IS_FALSE(v.IsInBounds(c), L"One right out the bottom right is out of bounds.");

        c.x--;
        c.y++;
        VERIFY_IS_FALSE(v.IsInBounds(c), L"One down out the bottom right is out of bounds.");
    }

	std::cerr << ">>> Entering TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << ")" << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << "): rect.top → " << rect.top << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << "): rect.bottom → " << rect.bottom << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << "): rect.left → " << rect.left << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << "): rect.right → " << rect.right << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << "): view → " << view << std::endl;
	std::cerr << "TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << "): rect → " << rect << std::endl;
    TEST_METHOD(IsInBoundsViewport)
    {
		std::cerr << ">>> Entering TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << ")" << std::endl;
        til::inclusive_rect rect;
		std::cerr << "<<< Exiting TEST_METHOD(IsInBoundsCoord=" << IsInBoundsCoord << ")" << std::endl;
        rect.top = 3;
        rect.bottom = 5;
        rect.left = 10;
        rect.right = 20;

        const auto original = rect;

        const auto view = Viewport::FromInclusive(rect);

        auto test = Viewport::FromInclusive(rect);
		std::cerr << "TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << "): test → " << test << std::endl;
        VERIFY_IS_TRUE(view.IsInBounds(test), L"Same size/position viewport is in bounds.");

        rect.top++;
        rect.bottom--;
        rect.left++;
        rect.right--;
        test = Viewport::FromInclusive(rect);
        VERIFY_IS_TRUE(view.IsInBounds(test), L"Viewport inscribed inside viewport is in bounds.");

        rect = original;
        rect.top--;
        test = Viewport::FromInclusive(rect);
        VERIFY_IS_FALSE(view.IsInBounds(test), L"Viewport that is one taller upwards is out of bounds.");

        rect = original;
        rect.bottom++;
        test = Viewport::FromInclusive(rect);
        VERIFY_IS_FALSE(view.IsInBounds(test), L"Viewport that is one taller downwards is out of bounds.");

        rect = original;
        rect.left--;
        test = Viewport::FromInclusive(rect);
        VERIFY_IS_FALSE(view.IsInBounds(test), L"Viewport that is one wider leftwards is out of bounds.");

        rect = original;
        rect.right++;
        test = Viewport::FromInclusive(rect);
        VERIFY_IS_FALSE(view.IsInBounds(test), L"Viewport that is one wider rightwards is out of bounds.");

        rect = original;
        rect.left++;
        rect.right++;
        rect.top++;
        rect.bottom++;
        test = Viewport::FromInclusive(rect);
        VERIFY_IS_FALSE(view.IsInBounds(test), L"Viewport offset at the origin but same size is out of bounds.");
    }

	std::cerr << ">>> Entering TEST_METHOD(ClampCoord=" << ClampCoord << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << ")" << std::endl;
	std::cerr << "TEST_METHOD(ClampCoord=" << ClampCoord << "): rect.top → " << rect.top << std::endl;
	std::cerr << "TEST_METHOD(ClampCoord=" << ClampCoord << "): rect.bottom → " << rect.bottom << std::endl;
	std::cerr << "TEST_METHOD(ClampCoord=" << ClampCoord << "): rect.left → " << rect.left << std::endl;
	std::cerr << "TEST_METHOD(ClampCoord=" << ClampCoord << "): rect.right → " << rect.right << std::endl;
	std::cerr << "TEST_METHOD(ClampCoord=" << ClampCoord << "): view → " << view << std::endl;
	std::cerr << "TEST_METHOD(ClampCoord=" << ClampCoord << "): pos.x → " << pos.x << std::endl;
	std::cerr << "TEST_METHOD(ClampCoord=" << ClampCoord << "): pos.y → " << pos.y << std::endl;
	std::cerr << "TEST_METHOD(ClampCoord=" << ClampCoord << "): pos → " << pos << std::endl;
	std::cerr << "TEST_METHOD(ClampCoord=" << ClampCoord << "): invalidView → " << invalidView << std::endl;
    TEST_METHOD(ClampCoord)
    {
		std::cerr << ">>> Entering TEST_METHOD(ClampCoord=" << ClampCoord << ")" << std::endl;
        til::inclusive_rect rect;
		std::cerr << "<<< Exiting TEST_METHOD(IsInBoundsViewport=" << IsInBoundsViewport << ")" << std::endl;
        rect.top = 3;
        rect.bottom = 5;
        rect.left = 10;
        rect.right = 20;

        const auto view = Viewport::FromInclusive(rect);

        til::point pos;
        pos.x = rect.left;
        pos.y = rect.top;

        auto before = pos;
		std::cerr << "TEST_METHOD(ClampCoord=" << ClampCoord << "): before → " << before << std::endl;
        view.Clamp(pos);
        VERIFY_ARE_EQUAL(before, pos, L"Verify clamp did nothing for position in top left corner.");

        pos.x = rect.left;
        pos.y = rect.bottom;
        before = pos;
        view.Clamp(pos);
        VERIFY_ARE_EQUAL(before, pos, L"Verify clamp did nothing for position in bottom left corner.");

        pos.x = rect.right;
        pos.y = rect.bottom;
        before = pos;
        view.Clamp(pos);
        VERIFY_ARE_EQUAL(before, pos, L"Verify clamp did nothing for position in bottom right corner.");

        pos.x = rect.right;
        pos.y = rect.top;
        before = pos;
        view.Clamp(pos);
        VERIFY_ARE_EQUAL(before, pos, L"Verify clamp did nothing for position in top right corner.");

        til::point expected;
        expected.x = rect.right;
        expected.y = rect.top;

        pos = expected;
        pos.x++;
        pos.y--;
        before = pos;

        view.Clamp(pos);
        VERIFY_ARE_NOT_EQUAL(before, pos, L"Verify clamp modified position out the top right corner back.");
        VERIFY_ARE_EQUAL(expected, pos, L"Verify position was clamped into the top right corner.");

        expected.x = rect.left;
        expected.y = rect.top;

        pos = expected;
        pos.x--;
        pos.y--;
        before = pos;

        view.Clamp(pos);
        VERIFY_ARE_NOT_EQUAL(before, pos, L"Verify clamp modified position out the top left corner back.");
        VERIFY_ARE_EQUAL(expected, pos, L"Verify position was clamped into the top left corner.");

        expected.x = rect.left;
        expected.y = rect.bottom;

        pos = expected;
        pos.x--;
        pos.y++;
        before = pos;

        view.Clamp(pos);
        VERIFY_ARE_NOT_EQUAL(before, pos, L"Verify clamp modified position out the bottom left corner back.");
        VERIFY_ARE_EQUAL(expected, pos, L"Verify position was clamped into the bottom left corner.");

        expected.x = rect.right;
        expected.y = rect.bottom;

        pos = expected;
        pos.x++;
        pos.y++;
        before = pos;

        view.Clamp(pos);
        VERIFY_ARE_NOT_EQUAL(before, pos, L"Verify clamp modified position out the bottom right corner back.");
        VERIFY_ARE_EQUAL(expected, pos, L"Verify position was clamped into the bottom right corner.");

        auto invalidView = Viewport::Empty();
        VERIFY_THROWS_SPECIFIC(invalidView.Clamp(pos),
                               wil::ResultException,
                               [](wil::ResultException& e) { return e.GetErrorCode() == E_NOT_VALID_STATE; });
    }

	std::cerr << ">>> Entering TEST_METHOD(ClampViewport=" << ClampViewport << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(ClampCoord=" << ClampCoord << ")" << std::endl;
	std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): rect.top → " << rect.top << std::endl;
	std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): rect.bottom → " << rect.bottom << std::endl;
	std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): rect.left → " << rect.left << std::endl;
	std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): rect.right → " << rect.right << std::endl;
	std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): view → " << view << std::endl;
	std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): testRect.top → " << testRect.top << std::endl;
	std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): testRect.bottom → " << testRect.bottom << std::endl;
	std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): testRect.left → " << testRect.left << std::endl;
	std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): testRect.right → " << testRect.right << std::endl;
	std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): expectedView → " << expectedView << std::endl;
    TEST_METHOD(ClampViewport)
    {
		std::cerr << ">>> Entering TEST_METHOD(ClampViewport=" << ClampViewport << ")" << std::endl;
        // Create the rectangle/view we will clamp to.
		std::cerr << "<<< Exiting TEST_METHOD(ClampCoord=" << ClampCoord << ")" << std::endl;
        til::inclusive_rect rect;
        rect.top = 3;
        rect.bottom = 5;
        rect.left = 10;
        rect.right = 20;

        const auto view = Viewport::FromInclusive(rect);

        Log::Comment(L"Make a rectangle that is larger than and fully encompasses our clamping rectangle.");
        til::inclusive_rect testRect;
        testRect.top = rect.top - 3;
        testRect.bottom = rect.bottom + 3;
        testRect.left = rect.left - 3;
        testRect.right = rect.right + 3;

        auto testView = Viewport::FromInclusive(testRect);
		std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): testView → " << testView << std::endl;

        auto actual = view.Clamp(testView);
		std::cerr << "TEST_METHOD(ClampViewport=" << ClampViewport << "): actual → " << actual << std::endl;
        VERIFY_ARE_EQUAL(view, actual, L"All sides should get reduced down to the size of the given rect.");

        Log::Comment(L"Make a rectangle that is fully inscribed inside our clamping rectangle.");
        testRect.top = rect.top + 1;
        testRect.bottom = rect.bottom - 1;
        testRect.left = rect.left + 1;
        testRect.right = rect.right - 1;
        testView = Viewport::FromInclusive(testRect);

        actual = view.Clamp(testView);
        VERIFY_ARE_EQUAL(testView, actual, L"Verify that nothing changed because this rectangle already sat fully inside the clamping rectangle.");

        Log::Comment(L"Craft a rectangle where the left is outside the right, right is outside the left, top is outside the bottom, and bottom is outside the top.");
        testRect.top = rect.bottom + 10;
        testRect.bottom = rect.top - 10;
        testRect.left = rect.right + 10;
        testRect.right = rect.left - 10;
        testView = Viewport::FromInclusive(testRect);

        Log::Comment(L"We expect it to be pulled back so each coordinate is in bounds, but the rectangle is still invalid (since left will be > right).");
        const auto expectedView = Viewport::Empty();

        actual = view.Clamp(testView);
        VERIFY_ARE_EQUAL(expectedView, actual, L"Every dimension should be pulled just inside the clamping rectangle.");
    }

	std::cerr << ">>> Entering TEST_METHOD(IncrementInBounds=" << IncrementInBounds << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(ClampViewport=" << ClampViewport << ")" << std::endl;
	std::cerr << "TEST_METHOD(IncrementInBounds=" << IncrementInBounds << "): edges.left → " << edges.left << std::endl;
	std::cerr << "TEST_METHOD(IncrementInBounds=" << IncrementInBounds << "): edges.right → " << edges.right << std::endl;
	std::cerr << "TEST_METHOD(IncrementInBounds=" << IncrementInBounds << "): edges.top → " << edges.top << std::endl;
	std::cerr << "TEST_METHOD(IncrementInBounds=" << IncrementInBounds << "): edges.bottom → " << edges.bottom << std::endl;
	std::cerr << "TEST_METHOD(IncrementInBounds=" << IncrementInBounds << "): v → " << v << std::endl;
	std::cerr << "TEST_METHOD(IncrementInBounds=" << IncrementInBounds << "): original.x → " << original.x << std::endl;
	std::cerr << "TEST_METHOD(IncrementInBounds=" << IncrementInBounds << "): original.y → " << original.y << std::endl;
    TEST_METHOD(IncrementInBounds)
    {
		std::cerr << ">>> Entering TEST_METHOD(IncrementInBounds=" << IncrementInBounds << ")" << std::endl;
        auto success = false;

		std::cerr << "TEST_METHOD(IncrementInBounds=" << IncrementInBounds << "): success → " << success << std::endl;
        til::inclusive_rect edges;
		std::cerr << "<<< Exiting TEST_METHOD(ClampViewport=" << ClampViewport << ")" << std::endl;
        edges.left = 10;
        edges.right = 19;
        edges.top = 20;
        edges.bottom = 29;

        const auto v = Viewport::FromInclusive(edges);
        til::point original;
        til::point screen;

        // #1 coord inside region
        original.x = screen.x = 15;
        original.y = screen.y = 25;

        success = v.IncrementInBounds(screen);

        VERIFY_IS_TRUE(success);
        VERIFY_ARE_EQUAL(screen.x, original.x + 1);
        VERIFY_ARE_EQUAL(screen.y, original.y);

        // #2 coord right edge, not bottom
        original.x = screen.x = edges.right;
        original.y = screen.y = 25;

        success = v.IncrementInBounds(screen);

        VERIFY_IS_TRUE(success);
        VERIFY_ARE_EQUAL(screen.x, edges.left);
        VERIFY_ARE_EQUAL(screen.y, original.y + 1);

        // #3 coord right edge, bottom
        original.x = screen.x = edges.right;
        original.y = screen.y = edges.bottom;

        success = v.IncrementInBounds(screen);

        VERIFY_IS_FALSE(success);
        VERIFY_ARE_EQUAL(screen.x, edges.right);
        VERIFY_ARE_EQUAL(screen.y, edges.bottom);
    }

	std::cerr << ">>> Entering TEST_METHOD(DecrementInBounds=" << DecrementInBounds << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(IncrementInBounds=" << IncrementInBounds << ")" << std::endl;
	std::cerr << "TEST_METHOD(DecrementInBounds=" << DecrementInBounds << "): edges.left → " << edges.left << std::endl;
	std::cerr << "TEST_METHOD(DecrementInBounds=" << DecrementInBounds << "): edges.right → " << edges.right << std::endl;
	std::cerr << "TEST_METHOD(DecrementInBounds=" << DecrementInBounds << "): edges.top → " << edges.top << std::endl;
	std::cerr << "TEST_METHOD(DecrementInBounds=" << DecrementInBounds << "): edges.bottom → " << edges.bottom << std::endl;
	std::cerr << "TEST_METHOD(DecrementInBounds=" << DecrementInBounds << "): v → " << v << std::endl;
	std::cerr << "TEST_METHOD(DecrementInBounds=" << DecrementInBounds << "): original.x → " << original.x << std::endl;
	std::cerr << "TEST_METHOD(DecrementInBounds=" << DecrementInBounds << "): original.y → " << original.y << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(DecrementInBounds=" << DecrementInBounds << ")" << std::endl;
    TEST_METHOD(DecrementInBounds)
    {
        auto success = false;

		std::cerr << "TEST_METHOD(DecrementInBounds=" << DecrementInBounds << "): success → " << success << std::endl;
        til::inclusive_rect edges;
		std::cerr << "<<< Exiting TEST_METHOD(IncrementInBounds=" << IncrementInBounds << ")" << std::endl;
        edges.left = 10;
        edges.right = 19;
        edges.top = 20;
        edges.bottom = 29;

        const auto v = Viewport::FromInclusive(edges);
        til::point original;
        til::point screen;

        // #1 coord inside region
        original.x = screen.x = 15;
        original.y = screen.y = 25;

        success = v.DecrementInBounds(screen);

        VERIFY_IS_TRUE(success);
        VERIFY_ARE_EQUAL(screen.x, original.x - 1);
        VERIFY_ARE_EQUAL(screen.y, original.y);

        // #2 coord left edge, not top
        original.x = screen.x = edges.left;
        original.y = screen.y = 25;

        success = v.DecrementInBounds(screen);

        VERIFY_IS_TRUE(success);
        VERIFY_ARE_EQUAL(screen.x, edges.right);
        VERIFY_ARE_EQUAL(screen.y, original.y - 1);

        // #3 coord left edge, top
        original.x = screen.x = edges.left;
        original.y = screen.y = edges.top;

        success = v.DecrementInBounds(screen);

        VERIFY_IS_FALSE(success);
        VERIFY_ARE_EQUAL(screen.x, edges.left);
        VERIFY_ARE_EQUAL(screen.y, edges.top);
    }

    static til::CoordType RandomCoord()
    {
		std::cerr << ">>> Entering RandomCoord()" << std::endl;
        til::CoordType s;

        do
        {
            s = (til::CoordType)rand() % SHORT_MAX;
			std::cerr << "RandomCoord(): s → " << s << std::endl;
        } while (s == 0);

		std::cerr << "<<< Exiting RandomCoord()" << std::endl;
	std::cerr << "TEST_METHOD(MoveInBounds=" << MoveInBounds << "): srectEdges.top → " << srectEdges.top << std::endl;
	std::cerr << "TEST_METHOD(MoveInBounds=" << MoveInBounds << "): srectEdges.bottom → " << srectEdges.bottom << std::endl;
	std::cerr << "TEST_METHOD(MoveInBounds=" << MoveInBounds << "): v → " << v << std::endl;
	std::cerr << "TEST_METHOD(MoveInBounds=" << MoveInBounds << "): sAddAmount → " << sAddAmount << std::endl;
	std::cerr << "TEST_METHOD(MoveInBounds=" << MoveInBounds << "): coordClamped → " << coordClamped << std::endl;
	std::cerr << "TEST_METHOD(MoveInBounds=" << MoveInBounds << "): fActualResult → " << fActualResult << std::endl;
        return s;
    }

    TEST_METHOD(MoveInBounds)
    {
        const auto cTestLoopInstances = 100;

        const auto sRowWidth = 20;
        VERIFY_IS_TRUE(sRowWidth > 0);

        // 20x20 box
        til::inclusive_rect srectEdges;
        srectEdges.top = srectEdges.left = 0;
        srectEdges.bottom = srectEdges.right = sRowWidth - 1;

        const auto v = Viewport::FromInclusive(srectEdges);

        // repeat test
        for (UINT i = 0; i < cTestLoopInstances; i++)
        {
            til::point coordPos;
            coordPos.x = RandomCoord() % 20;
            coordPos.y = RandomCoord() % 20;

            auto sAddAmount = RandomCoord() % (sRowWidth * sRowWidth);

            const til::point coord{
                (coordPos.x + sAddAmount) % sRowWidth,
                coordPos.y + ((coordPos.x + sAddAmount) / sRowWidth),
            };
            const auto coordClamped = std::clamp(coord, v.Origin(), v.BottomRightInclusive());
            const auto fExpectedResult = coord == coordClamped;
            const bool fActualResult = v.WalkInBounds(coordPos, sAddAmount);

            VERIFY_ARE_EQUAL(fExpectedResult, fActualResult);
            VERIFY_ARE_EQUAL(coordPos, coordClamped);
        }
    }

	std::cerr << "TEST_METHOD(CompareInBounds=" << CompareInBounds << "): edges.left → " << edges.left << std::endl;
	std::cerr << "TEST_METHOD(CompareInBounds=" << CompareInBounds << "): edges.right → " << edges.right << std::endl;
	std::cerr << "TEST_METHOD(CompareInBounds=" << CompareInBounds << "): edges.top → " << edges.top << std::endl;
	std::cerr << "TEST_METHOD(CompareInBounds=" << CompareInBounds << "): edges.bottom → " << edges.bottom << std::endl;
	std::cerr << "TEST_METHOD(CompareInBounds=" << CompareInBounds << "): v → " << v << std::endl;
	std::cerr << "TEST_METHOD(CompareInBounds=" << CompareInBounds << "): first.x → " << first.x << std::endl;
	std::cerr << "TEST_METHOD(CompareInBounds=" << CompareInBounds << "): first.y → " << first.y << std::endl;
	std::cerr << "TEST_METHOD(CompareInBounds=" << CompareInBounds << "): second → " << second << std::endl;
	std::cerr << "TEST_METHOD(CompareInBounds=" << CompareInBounds << "): second.x → " << second.x << std::endl;
	std::cerr << "TEST_METHOD(CompareInBounds=" << CompareInBounds << "): second.y → " << second.y << std::endl;
    TEST_METHOD(CompareInBounds)
    {
        til::inclusive_rect edges;
        edges.left = 10;
        edges.right = 19;
        edges.top = 20;
        edges.bottom = 29;

        const auto v = Viewport::FromInclusive(edges);

        til::point first, second;
        first.x = 12;
        first.y = 24;
        second = first;
        second.x += 2;

        VERIFY_ARE_EQUAL(-2, v.CompareInBounds(first, second), L"Second and first on same row. Second is right of first.");
        VERIFY_ARE_EQUAL(2, v.CompareInBounds(second, first), L"Reverse params, should get opposite direction, same magnitude.");

        first.x = edges.left;
        first.y = 24;

        second.x = edges.right;
        second.y = first.y - 1;

        VERIFY_ARE_EQUAL(1, v.CompareInBounds(first, second), L"Second is up a line at the right edge from first at the line below on the left edge.");
        VERIFY_ARE_EQUAL(-1, v.CompareInBounds(second, first), L"Reverse params, should get opposite direction, same magnitude.");
    }

	std::cerr << "TEST_METHOD(Offset=" << Offset << "): edges.top → " << edges.top << std::endl;
	std::cerr << "TEST_METHOD(Offset=" << Offset << "): edges.left → " << edges.left << std::endl;
	std::cerr << "TEST_METHOD(Offset=" << Offset << "): edges.right → " << edges.right << std::endl;
	std::cerr << "TEST_METHOD(Offset=" << Offset << "): edges.bottom → " << edges.bottom << std::endl;
	std::cerr << "TEST_METHOD(Offset=" << Offset << "): original → " << original << std::endl;
	std::cerr << "TEST_METHOD(Offset=" << Offset << "): expectedEdges.top → " << expectedEdges.top << std::endl;
	std::cerr << "TEST_METHOD(Offset=" << Offset << "): expectedEdges.bottom → " << expectedEdges.bottom << std::endl;
	std::cerr << "TEST_METHOD(Offset=" << Offset << "): expectedEdges.left → " << expectedEdges.left << std::endl;
	std::cerr << "TEST_METHOD(Offset=" << Offset << "): expectedEdges.right → " << expectedEdges.right << std::endl;
	std::cerr << "TEST_METHOD(Offset=" << Offset << "): adjust → " << adjust << std::endl;
    TEST_METHOD(Offset)
    {
        til::inclusive_rect edges;
        edges.top = 0;
        edges.left = 0;
        edges.right = 10;
        edges.bottom = 10;

        const auto original = Viewport::FromInclusive(edges);

        Log::Comment(L"Move down and to the right first.");
        til::point adjust{ 7, 2 };
        til::inclusive_rect expectedEdges;
        expectedEdges.top = edges.top + adjust.y;
        expectedEdges.bottom = edges.bottom + adjust.y;
        expectedEdges.left = edges.left + adjust.x;
        expectedEdges.right = edges.right + adjust.x;

        auto expected = Viewport::FromInclusive(expectedEdges);
		std::cerr << "TEST_METHOD(Offset=" << Offset << "): expected → " << expected << std::endl;

        auto actual = Viewport::Offset(original, adjust);
		std::cerr << "TEST_METHOD(Offset=" << Offset << "): actual → " << actual << std::endl;
        VERIFY_ARE_EQUAL(expected, actual);

        Log::Comment(L"Now try moving up and to the left.");
        adjust = { -3, -5 };

        expectedEdges.top = edges.top + adjust.y;
        expectedEdges.bottom = edges.bottom + adjust.y;
        expectedEdges.left = edges.left + adjust.x;
        expectedEdges.right = edges.right + adjust.x;

        expected = Viewport::FromInclusive(expectedEdges);
        actual = Viewport::Offset(original, adjust);
        VERIFY_ARE_EQUAL(expected, actual);
    }

	std::cerr << "TEST_METHOD(Union=" << Union << "): srOne.left → " << srOne.left << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srOne.right → " << srOne.right << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srOne.top → " << srOne.top << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srOne.bottom → " << srOne.bottom << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): one → " << one << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srTwo.left → " << srTwo.left << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srTwo.right → " << srTwo.right << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srTwo.top → " << srTwo.top << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srTwo.bottom → " << srTwo.bottom << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): two → " << two << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srExpected.left → " << srExpected.left << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srExpected.right → " << srExpected.right << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srExpected.top → " << srExpected.top << std::endl;
	std::cerr << "TEST_METHOD(Union=" << Union << "): srExpected.bottom → " << srExpected.bottom << std::endl;
    TEST_METHOD(Union)
    {
        til::inclusive_rect srOne;
        srOne.left = 4;
        srOne.right = 10;
        srOne.top = 6;
        srOne.bottom = 14;
        const auto one = Viewport::FromInclusive(srOne);

        til::inclusive_rect srTwo;
        srTwo.left = 5;
        srTwo.right = 13;
        srTwo.top = 2;
        srTwo.bottom = 10;
        const auto two = Viewport::FromInclusive(srTwo);

        til::inclusive_rect srExpected;
        srExpected.left = srOne.left < srTwo.left ? srOne.left : srTwo.left;
        srExpected.right = srOne.right > srTwo.right ? srOne.right : srTwo.right;
        srExpected.top = srOne.top < srTwo.top ? srOne.top : srTwo.top;
        srExpected.bottom = srOne.bottom > srTwo.bottom ? srOne.bottom : srTwo.bottom;

        const auto expected = Viewport::FromInclusive(srExpected);
		std::cerr << "TEST_METHOD(Union=" << Union << "): expected → " << expected << std::endl;

        const auto actual = Viewport::Union(one, two);
		std::cerr << "TEST_METHOD(Union=" << Union << "): actual → " << actual << std::endl;
        VERIFY_ARE_EQUAL(expected, actual);
    }

	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srOne.left → " << srOne.left << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srOne.right → " << srOne.right << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srOne.top → " << srOne.top << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srOne.bottom → " << srOne.bottom << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): one → " << one << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srTwo.left → " << srTwo.left << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srTwo.right → " << srTwo.right << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srTwo.top → " << srTwo.top << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srTwo.bottom → " << srTwo.bottom << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): two → " << two << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srExpected.left → " << srExpected.left << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srExpected.right → " << srExpected.right << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srExpected.top → " << srExpected.top << std::endl;
	std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): srExpected.bottom → " << srExpected.bottom << std::endl;
    TEST_METHOD(Intersect)
    {
        til::inclusive_rect srOne;
        srOne.left = 4;
        srOne.right = 10;
        srOne.top = 6;
        srOne.bottom = 14;
        const auto one = Viewport::FromInclusive(srOne);

        til::inclusive_rect srTwo;
        srTwo.left = 5;
        srTwo.right = 13;
        srTwo.top = 2;
        srTwo.bottom = 10;
        const auto two = Viewport::FromInclusive(srTwo);

        til::inclusive_rect srExpected;
        srExpected.left = srOne.left > srTwo.left ? srOne.left : srTwo.left;
        srExpected.right = srOne.right < srTwo.right ? srOne.right : srTwo.right;
        srExpected.top = srOne.top > srTwo.top ? srOne.top : srTwo.top;
        srExpected.bottom = srOne.bottom < srTwo.bottom ? srOne.bottom : srTwo.bottom;

        const auto expected = Viewport::FromInclusive(srExpected);
		std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): expected → " << expected << std::endl;

        const auto actual = Viewport::Intersect(one, two);
		std::cerr << "TEST_METHOD(Intersect=" << Intersect << "): actual → " << actual << std::endl;
        VERIFY_ARE_EQUAL(expected, actual);
    }

	std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): srOriginal.top → " << srOriginal.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): srOriginal.left → " << srOriginal.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): srOriginal.bottom → " << srOriginal.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): srOriginal.right → " << srOriginal.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): original → " << original << std::endl;
	std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): srRemove.top → " << srRemove.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): srRemove.left → " << srRemove.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): srRemove.bottom → " << srRemove.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): srRemove.right → " << srRemove.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): remove → " << remove << std::endl;
    TEST_METHOD(SubtractFour)
    {
        til::inclusive_rect srOriginal;
        srOriginal.top = 0;
        srOriginal.left = 0;
        srOriginal.bottom = 10;
        srOriginal.right = 10;
        const auto original = Viewport::FromInclusive(srOriginal);

        til::inclusive_rect srRemove;
        srRemove.top = 3;
        srRemove.left = 3;
        srRemove.bottom = 6;
        srRemove.right = 6;
        const auto remove = Viewport::FromInclusive(srRemove);

        std::vector<Viewport> expected;
        // til::inclusive_rect constructed as: Left, Top, Right, Bottom
        // Top View
        expected.emplace_back(Viewport::FromInclusive({ srOriginal.left, srOriginal.top, srOriginal.right, srRemove.top - 1 }));
        // Bottom View
        expected.emplace_back(Viewport::FromInclusive({ srOriginal.left, srRemove.bottom + 1, srOriginal.right, srOriginal.bottom }));
        // Left View
        expected.emplace_back(Viewport::FromInclusive({ srOriginal.left, srRemove.top, srRemove.left - 1, srRemove.bottom }));
        // Right View
        expected.emplace_back(Viewport::FromInclusive({ srRemove.right + 1, srRemove.top, srOriginal.right, srRemove.bottom }));

        const auto actual = Viewport::Subtract(original, remove);
		std::cerr << "TEST_METHOD(SubtractFour=" << SubtractFour << "): actual → " << actual << std::endl;

        VERIFY_ARE_EQUAL(expected.size(), actual.size(), L"Same number of viewports in expected and actual");
        Log::Comment(L"Now validate that each viewport has the expected area.");
        for (size_t i = 0; i < expected.size(); i++)
        {
            const auto& exp = expected.at(i);
            const auto& act = actual.at(i);
            VERIFY_ARE_EQUAL(exp, act);
        }
    }

	std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): srOriginal.top → " << srOriginal.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): srOriginal.left → " << srOriginal.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): srOriginal.bottom → " << srOriginal.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): srOriginal.right → " << srOriginal.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): original → " << original << std::endl;
	std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): srRemove.top → " << srRemove.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): srRemove.left → " << srRemove.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): srRemove.bottom → " << srRemove.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): srRemove.right → " << srRemove.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): remove → " << remove << std::endl;
    TEST_METHOD(SubtractThree)
    {
        til::inclusive_rect srOriginal;
        srOriginal.top = 0;
        srOriginal.left = 0;
        srOriginal.bottom = 10;
        srOriginal.right = 10;
        const auto original = Viewport::FromInclusive(srOriginal);

        til::inclusive_rect srRemove;
        srRemove.top = 3;
        srRemove.left = 3;
        srRemove.bottom = 6;
        srRemove.right = 15;
        const auto remove = Viewport::FromInclusive(srRemove);

        std::vector<Viewport> expected;
        // til::inclusive_rect constructed as: Left, Top, Right, Bottom
        // Top View
        expected.emplace_back(Viewport::FromInclusive({ srOriginal.left, srOriginal.top, srOriginal.right, srRemove.top - 1 }));
        // Bottom View
        expected.emplace_back(Viewport::FromInclusive({ srOriginal.left, srRemove.bottom + 1, srOriginal.right, srOriginal.bottom }));
        // Left View
        expected.emplace_back(Viewport::FromInclusive({ srOriginal.left, srRemove.top, srRemove.left - 1, srRemove.bottom }));

        const auto actual = Viewport::Subtract(original, remove);
		std::cerr << "TEST_METHOD(SubtractThree=" << SubtractThree << "): actual → " << actual << std::endl;

        VERIFY_ARE_EQUAL(expected.size(), actual.size(), L"Same number of viewports in expected and actual");
        Log::Comment(L"Now validate that each viewport has the expected area.");
        for (size_t i = 0; i < expected.size(); i++)
        {
            const auto& exp = expected.at(i);
            const auto& act = actual.at(i);
            VERIFY_ARE_EQUAL(exp, act);
        }
    }

	std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): srOriginal.top → " << srOriginal.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): srOriginal.left → " << srOriginal.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): srOriginal.bottom → " << srOriginal.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): srOriginal.right → " << srOriginal.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): original → " << original << std::endl;
	std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): srRemove.top → " << srRemove.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): srRemove.left → " << srRemove.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): srRemove.bottom → " << srRemove.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): srRemove.right → " << srRemove.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): remove → " << remove << std::endl;
    TEST_METHOD(SubtractTwo)
    {
        til::inclusive_rect srOriginal;
        srOriginal.top = 0;
        srOriginal.left = 0;
        srOriginal.bottom = 10;
        srOriginal.right = 10;
        const auto original = Viewport::FromInclusive(srOriginal);

        til::inclusive_rect srRemove;
        srRemove.top = 3;
        srRemove.left = 3;
        srRemove.bottom = 15;
        srRemove.right = 15;
        const auto remove = Viewport::FromInclusive(srRemove);

        std::vector<Viewport> expected;
        // til::inclusive_rect constructed as: Left, Top, Right, Bottom
        // Top View
        expected.emplace_back(Viewport::FromInclusive({ srOriginal.left, srOriginal.top, srOriginal.right, srRemove.top - 1 }));
        // Left View
        expected.emplace_back(Viewport::FromInclusive({ srOriginal.left, srRemove.top, srRemove.left - 1, srOriginal.bottom }));

        const auto actual = Viewport::Subtract(original, remove);
		std::cerr << "TEST_METHOD(SubtractTwo=" << SubtractTwo << "): actual → " << actual << std::endl;

        VERIFY_ARE_EQUAL(expected.size(), actual.size(), L"Same number of viewports in expected and actual");
        Log::Comment(L"Now validate that each viewport has the expected area.");
        for (size_t i = 0; i < expected.size(); i++)
        {
            const auto& exp = expected.at(i);
            const auto& act = actual.at(i);
            VERIFY_ARE_EQUAL(exp, act);
        }
    }

	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): srOriginal.top → " << srOriginal.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): srOriginal.left → " << srOriginal.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): srOriginal.bottom → " << srOriginal.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): srOriginal.right → " << srOriginal.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): original → " << original << std::endl;
	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): srRemove.top → " << srRemove.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): srRemove.left → " << srRemove.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): srRemove.bottom → " << srRemove.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): srRemove.right → " << srRemove.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): remove → " << remove << std::endl;
	std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): foo → " << foo << std::endl;
    TEST_METHOD(SubtractOne)
    {
        til::inclusive_rect srOriginal;
        srOriginal.top = 0;
        srOriginal.left = 0;
        srOriginal.bottom = 10;
        srOriginal.right = 10;
        const auto original = Viewport::FromInclusive(srOriginal);

        til::inclusive_rect srRemove;
        srRemove.top = 3;
        srRemove.left = -12;
        srRemove.bottom = 15;
        srRemove.right = 15;
        const auto remove = Viewport::FromInclusive(srRemove);

        std::vector<Viewport> expected;
        // til::inclusive_rect constructed as: Left, Top, Right, Bottom
        // Top View
        expected.emplace_back(Viewport::FromInclusive({ srOriginal.left, srOriginal.top, srOriginal.right, srRemove.top - 1 }));

        const auto foo = expected.cbegin();

        const auto actual = Viewport::Subtract(original, remove);
		std::cerr << "TEST_METHOD(SubtractOne=" << SubtractOne << "): actual → " << actual << std::endl;

        VERIFY_ARE_EQUAL(expected.size(), actual.size(), L"Same number of viewports in expected and actual");
        Log::Comment(L"Now validate that each viewport has the expected area.");
        for (size_t i = 0; i < expected.size(); i++)
        {
            const auto& exp = expected.at(i);
            const auto& act = actual.at(i);
            VERIFY_ARE_EQUAL(exp, act);
        }
    }

	std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): srOriginal.top → " << srOriginal.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): srOriginal.left → " << srOriginal.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): srOriginal.bottom → " << srOriginal.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): srOriginal.right → " << srOriginal.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): original → " << original << std::endl;
	std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): srRemove.top → " << srRemove.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): srRemove.left → " << srRemove.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): srRemove.bottom → " << srRemove.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): srRemove.right → " << srRemove.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): remove → " << remove << std::endl;
    TEST_METHOD(SubtractZero)
    {
        til::inclusive_rect srOriginal;
        srOriginal.top = 0;
        srOriginal.left = 0;
        srOriginal.bottom = 10;
        srOriginal.right = 10;
        const auto original = Viewport::FromInclusive(srOriginal);

        til::inclusive_rect srRemove;
        srRemove.top = 12;
        srRemove.left = 12;
        srRemove.bottom = 15;
        srRemove.right = 15;
        const auto remove = Viewport::FromInclusive(srRemove);

        std::vector<Viewport> expected;
        expected.emplace_back(original);

        const auto actual = Viewport::Subtract(original, remove);
		std::cerr << "TEST_METHOD(SubtractZero=" << SubtractZero << "): actual → " << actual << std::endl;

        VERIFY_ARE_EQUAL(expected.size(), actual.size(), L"Same number of viewports in expected and actual");
        Log::Comment(L"Now validate that each viewport has the expected area.");
        for (size_t i = 0; i < expected.size(); i++)
        {
            const auto& exp = expected.at(i);
            const auto& act = actual.at(i);
            VERIFY_ARE_EQUAL(exp, act);
        }
    }

	std::cerr << "TEST_METHOD(SubtractSame=" << SubtractSame << "): srOriginal.top → " << srOriginal.top << std::endl;
	std::cerr << "TEST_METHOD(SubtractSame=" << SubtractSame << "): srOriginal.left → " << srOriginal.left << std::endl;
	std::cerr << "TEST_METHOD(SubtractSame=" << SubtractSame << "): srOriginal.bottom → " << srOriginal.bottom << std::endl;
	std::cerr << "TEST_METHOD(SubtractSame=" << SubtractSame << "): srOriginal.right → " << srOriginal.right << std::endl;
	std::cerr << "TEST_METHOD(SubtractSame=" << SubtractSame << "): original → " << original << std::endl;
    TEST_METHOD(SubtractSame)
    {
        til::inclusive_rect srOriginal;
        srOriginal.top = 0;
        srOriginal.left = 0;
        srOriginal.bottom = 10;
        srOriginal.right = 10;
        const auto original = Viewport::FromInclusive(srOriginal);
        const auto remove = original;

        const auto actual = Viewport::Subtract(original, remove);
		std::cerr << "TEST_METHOD(SubtractSame=" << SubtractSame << "): actual → " << actual << std::endl;

        VERIFY_ARE_EQUAL(0u, actual.size(), L"There should be no viewports returned");
    }
};
