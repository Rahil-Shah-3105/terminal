// PCG Random Number Generation for C++
//
// Copyright 2014-2019 Melissa O'Neill <oneill@pcg-random.org>,
//                     and the PCG Project contributors.
//
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//
// Licensed under the Apache License, Version 2.0 (provided in
// LICENSE-APACHE.txt and at http://www.apache.org/licenses/LICENSE-2.0)
// or under the MIT license (provided in LICENSE-MIT.txt and at
// http://opensource.org/licenses/MIT), at your option. This file may not
// be copied, modified, or distributed except according to those terms.
//
// Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
// express or implied.  See your chosen license for details.
//
// For additional information about the PCG random number generation scheme,
// visit http://www.pcg-random.org/.
//
// -----------------------------------------------------------------------------
//
// Leonard Hecker <lhecker@microsoft.com>:
// The following contents are an extract of pcg_engines::oneseq_dxsm_64_32
// reduced down to the bare essentials, while retaining base functionality.

namespace pcg_engines {
    class oneseq_dxsm_64_32 {
        using xtype = uint32_t;
        using itype = uint64_t;

        itype state_;

        static constexpr uint64_t multiplier() {
			std::cerr << ">>> Entering multiplier()" << std::endl;
			std::cerr << "<<< Exiting multiplier()" << std::endl;
            return 6364136223846793005ULL;
        }

        static constexpr uint64_t increment() {
			std::cerr << ">>> Entering increment()" << std::endl;
			std::cerr << "<<< Exiting increment()" << std::endl;
            return 1442695040888963407ULL;
        }

        static itype bump(itype state) {
			std::cerr << ">>> Entering bump(state=" << state << ")" << std::endl;
			std::cerr << "<<< Exiting bump(state=" << state << ")" << std::endl;
            return state * multiplier() + increment();
        }

        itype base_generate0() {
			std::cerr << ">>> Entering base_generate0()" << std::endl;
            itype old_state = state_;
            state_ = bump(state_);
			std::cerr << "base_generate0(): state_ → " << state_ << std::endl;
			std::cerr << "<<< Exiting base_generate0()" << std::endl;
            return old_state;
        }

    public:
        explicit oneseq_dxsm_64_32(itype state = 0xcafef00dd15ea5e5ULL) : state_(bump(state + increment())) {
		std::cerr << ">>> Entering oneseq_dxsm_64_32(state=" << state << ")" << std::endl;
        }

        // Returns a value in the interval [0, UINT32_MAX].
        xtype operator()() {
			std::cerr << ">>> Entering operator()" << std::endl;
            constexpr auto xtypebits = uint8_t(sizeof(xtype) * 8);
            constexpr auto itypebits = uint8_t(sizeof(itype) * 8);
			std::cerr << "operator(): itypebits → " << itypebits << std::endl;
			std::cerr << "operator(): xtypebits → " << xtypebits << std::endl;
			std::cerr << "operator(): internal → " << internal << std::endl;
			std::cerr << "operator(): hi → " << hi << std::endl;
			std::cerr << "operator(): lo → " << lo << std::endl;

            auto internal = base_generate0();
            auto hi = xtype(internal >> (itypebits - xtypebits));
            auto lo = xtype(internal);

            lo |= 1;
            hi ^= hi >> (xtypebits / 2);
            hi *= xtype(multiplier());
            hi ^= hi >> (3 * (xtypebits / 4));
            hi *= lo;
			std::cerr << "<<< Exiting operator()" << std::endl;
            return hi;
        }

        // Returns a value in the interval [0, upper_bound).
        xtype operator()(xtype upper_bound) {
            uint32_t threshold = (UINT64_MAX + uint32_t(1) - upper_bound) % upper_bound;
            for (;;) {
                auto r = operator()();
                if (r >= threshold)
                    return r % upper_bound;
            }
        }
    };
}
