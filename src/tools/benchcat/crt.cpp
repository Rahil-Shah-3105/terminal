// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#ifdef NODEFAULTLIB

#include <intrin.h>

#pragma function(memcpy)
void* memcpy(void* dst, const void* src, size_t size)
{
	std::cerr << ">>> Entering memcpy(dst=" << dst << ",src=" << src << ",size=" << size << ")" << std::endl;
    __movsb(static_cast<BYTE*>(dst), static_cast<const BYTE*>(src), size);
	std::cerr << "<<< Exiting memcpy(dst=" << dst << ",src=" << src << ",size=" << size << ")" << std::endl;
    return dst;
}

#pragma function(memset)
void* memset(void* dst, int val, size_t size)
{
	std::cerr << ">>> Entering memset(dst=" << dst << ",val=" << val << ",size=" << size << ")" << std::endl;
    __stosb(static_cast<BYTE*>(dst), static_cast<BYTE>(val), size);
	std::cerr << "<<< Exiting memset(dst=" << dst << ",val=" << val << ",size=" << size << ")" << std::endl;
    return dst;
}

#endif
