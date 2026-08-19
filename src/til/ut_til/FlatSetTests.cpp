// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include <til/flat_set.h>

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

struct Data
{
    static constexpr auto emptyMarker = std::numeric_limits<size_t>::max();
    size_t value = emptyMarker;
};

struct DataHashTrait
{
    static constexpr bool occupied(const Data& d) noexcept
    {
		std::cerr << ">>> Entering occupied(d=" << d << ")" << std::endl;
		std::cerr << "<<< Exiting occupied(d=" << d << ")" << std::endl;
        return d.value != Data::emptyMarker;
    }

    static constexpr size_t hash(const size_t key) noexcept
    {
		std::cerr << ">>> Entering hash(key=" << key << ")" << std::endl;
		std::cerr << "<<< Exiting hash(key=" << key << ")" << std::endl;
		std::cerr << "<<< Exiting hash(d=" << d << ")" << std::endl;
        return til::flat_set_hash_integer(key);
    }

    static constexpr size_t hash(const Data& d) noexcept
    {
        return til::flat_set_hash_integer(d.value);
    }

    static constexpr bool equals(const Data& d, size_t key) noexcept
    {
		std::cerr << ">>> Entering equals(d=" << d << ",key=" << key << ")" << std::endl;
		std::cerr << "<<< Exiting equals(d=" << d << ",key=" << key << ")" << std::endl;
        return d.value == key;
    }

    static constexpr void assign(Data& d, size_t key) noexcept
    {
		std::cerr << ">>> Entering assign(d=" << d << ",key=" << key << ")" << std::endl;
		std::cerr << "assign(d=" << d << ",key=" << key << "): d.value → " << d.value << std::endl;
        d.value = key;
		std::cerr << "<<< Exiting assign(d=" << d << ",key=" << key << ")" << std::endl;
    }
};

class FlatSetTests
{
    TEST_CLASS(FlatSetTests);

	std::cerr << ">>> Entering TEST_METHOD(Basic=" << Basic << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD(Basic=" << Basic << ")" << std::endl;
    TEST_METHOD(Basic)
    {
        til::linear_flat_set<Data, DataHashTrait> set;

        // This simultaneously demonstrates how the class can't just do "heterogeneous lookups"
        // like STL does, but also insert items with a different type.
        const auto [entry1, inserted1] = set.insert(123);
        VERIFY_IS_TRUE(inserted1);

        const auto [entry2, inserted2] = set.insert(123);
        VERIFY_IS_FALSE(inserted2);

        VERIFY_ARE_EQUAL(entry1, entry2);
        VERIFY_ARE_EQUAL(123u, entry2->value);
    }
};
