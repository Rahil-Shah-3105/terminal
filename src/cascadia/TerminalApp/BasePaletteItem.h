#include <stdio.h>
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

namespace winrt::TerminalApp::implementation
{
    template<typename T, winrt::TerminalApp::PaletteItemType Ty>
    struct BasePaletteItem
    {
    public:
        winrt::TerminalApp::PaletteItemType Type() { return Ty; }

        Windows::UI::Xaml::Controls::IconElement ResolvedIcon()
        {
			fprintf(stderr, ">>> Entering ResolvedIcon()\n");
            const auto icon{ static_cast<T*>(this)->Icon() };
            if (!icon.empty())
            {
                const auto resolvedIcon{ Microsoft::Terminal::UI::IconPathConverter::IconWUX(icon) };
                resolvedIcon.Width(16);
                resolvedIcon.Height(16);
			fprintf(stderr, "<<< Exiting ResolvedIcon()\n");
                return resolvedIcon;
            }
            return nullptr;
        }

        til::property_changed_event PropertyChanged;

    protected:
        void BaseRaisePropertyChanged(wil::zwstring_view property)
        {
			fprintf(stderr, ">>> Entering BaseRaisePropertyChanged(property=%d)\n", property);
            PropertyChanged.raise(*static_cast<T*>(this), winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ property });
			fprintf(stderr, "<<< Exiting BaseRaisePropertyChanged(property=%d)\n", property);
        }

        void InvalidateResolvedIcon()
        {
			fprintf(stderr, ">>> Entering InvalidateResolvedIcon()\n");
            BaseRaisePropertyChanged(L"ResolvedIcon");
			fprintf(stderr, "<<< Exiting InvalidateResolvedIcon()\n");
        }
    };
}
