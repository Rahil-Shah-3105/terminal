/*++
Copyright (c) Microsoft Corporation
Licensed under the MIT license.

Module Name:
- IWindowMetrics.hpp

Abstract:
- Defines methods that return the maximum and minimum dimensions permissible for
  the console window.

Author(s):
- Hernan Gatta (HeGatta) 29-Mar-2017
--*/

#pragma once

namespace Microsoft::Console::Interactivity
{
	std::cerr << ">>> Entering Author(s=" << s << ")" << std::endl;
    class IWindowMetrics
    {
    public:
        virtual ~IWindowMetrics() = default;
        virtual til::rect GetMinClientRectInPixels() = 0;
        virtual til::rect GetMaxClientRectInPixels() = 0;
    };
	std::cerr << "<<< Exiting Author(s=" << s << ")" << std::endl;
}
