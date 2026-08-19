#include <stdio.h>
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

namespace til
{
    template<ContiguousBytes Target>
    constexpr void bytes_advance(Target& target, size_t count)
    {
		fprintf(stderr, ">>> Entering bytes_advance(target=%d,count=%zu)\n", target, count);
        if (count > target.size())
        {
			fprintf(stderr, "Throwing std from bytes_advance(target=%d,count=%zu)\n", target, count);
            throw std::length_error{ "insufficient space left" };
        }

        target = { target.data() + count, target.size() - count };
		fprintf(stderr, "bytes_advance(target=%d,count=%zu): target → %d\n", target, count, target);
    fprintf(stderr, "<<< Exiting bytes_advance(target=%d,count=%zu)\n", target, count);
    }

    template<TriviallyCopyable T, ContiguousBytes Target>
    constexpr bool bytes_can_put(const Target& target)
    {
		fprintf(stderr, ">>> Entering bytes_can_put(target=%d)\n", target);
        fprintf(stderr, "<<< Exiting bytes_can_put(target=%d)\n", target);
        return target.size() >= sizeof(T);
    }

    template<TriviallyCopyable T, ContiguousBytes Target>
    constexpr void bytes_put(Target& target, const T& value)
    {
		fprintf(stderr, ">>> Entering bytes_put(target=%d,value=%d)\n", target, value);
        using TargetType = typename Target::value_type;
        constexpr auto size = sizeof(value);

        if (size > target.size())
        {
			fprintf(stderr, "Throwing std from bytes_put(target=%d,value=%d)\n", target, value);
            throw std::length_error{ "insufficient space left" };
        }

        std::copy_n(reinterpret_cast<const TargetType*>(&value), size, target.data());
        target = { target.data() + size, target.size() - size };
		fprintf(stderr, "bytes_put(target=%d,value=%d): target → %d\n", target, value, target);
    fprintf(stderr, "<<< Exiting bytes_put(target=%d,value=%d)\n", target, value);
    }

    template<ContiguousBytes Target, ContiguousView Source>
        requires TriviallyCopyable<typename Source::value_type>
    constexpr void bytes_transfer(Target& target, Source& source)
    {
		fprintf(stderr, ">>> Entering bytes_transfer(target=%d,source=%d)\n", target, source);
        using TargetType = typename Target::value_type;
        constexpr auto elementSize = sizeof(typename Source::value_type);

        const auto sourceCount = std::min(source.size(), target.size() / elementSize);
        const auto targetCount = sourceCount * elementSize;

        std::copy_n(reinterpret_cast<const TargetType*>(source.data()), targetCount, target.data());

        target = { target.data() + targetCount, target.size() - targetCount };
		fprintf(stderr, "bytes_transfer(target=%d,source=%d): target → %d\n", target, source, target);
        source = { source.data() + sourceCount, source.size() - sourceCount };
		fprintf(stderr, "bytes_transfer(target=%d,source=%d): source → %d\n", target, source, source);
    fprintf(stderr, "<<< Exiting bytes_transfer(target=%d,source=%d)\n", target, source);
    }
}
