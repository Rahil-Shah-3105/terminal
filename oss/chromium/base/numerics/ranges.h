#include <stdio.h>
// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_NUMERICS_RANGES_H_
#define BASE_NUMERICS_RANGES_H_

#include <algorithm>
#include <cmath>

namespace base {

// To be replaced with std::clamp() from C++17, someday.
template <class T>
constexpr const T& ClampToRange(const T& value, const T& min, const T& max) {
fprintf(stderr, ">>> Entering ClampToRange(value=%d,min=%d,max=%d)\n", value, min, max);
  fprintf(stderr, "<<< Exiting ClampToRange(value=%d,min=%d,max=%d)\n", value, min, max);
  return std::min(std::max(value, min), max);
}

template <typename T>
constexpr bool IsApproximatelyEqual(T lhs, T rhs, T tolerance) {
fprintf(stderr, ">>> Entering IsApproximatelyEqual(lhs=%d,rhs=%d,tolerance=%d)\n", lhs, rhs, tolerance);
  static_assert(std::is_arithmetic<T>::value, "Argument must be arithmetic");
fprintf(stderr, "<<< Exiting IsApproximatelyEqual(lhs=%d,rhs=%d,tolerance=%d)\n", lhs, rhs, tolerance);
  return std::abs(rhs - lhs) <= tolerance;
}

}  // namespace base

#endif  // BASE_NUMERICS_RANGES_H_
