#include <stdio.h>
#pragma once

namespace winrt::TerminalApp::implementation
{
    template<typename D, typename... I>
    struct App_baseWithProvider : public App_base<D, ::winrt::Windows::UI::Xaml::Markup::IXamlMetadataProvider>
    {
        using IXamlType = ::winrt::Windows::UI::Xaml::Markup::IXamlType;

        IXamlType GetXamlType(const ::winrt::Windows::UI::Xaml::Interop::TypeName& type)
        {
			fprintf(stderr, ">>> Entering GetXamlType(type=%d)\n", type);
            fprintf(stderr, "<<< Exiting GetXamlType(type=%d)\n", type);
            return AppProvider()->GetXamlType(type);
        }

        IXamlType GetXamlType(const ::winrt::hstring& fullName)
        {
            return AppProvider()->GetXamlType(fullName);
        }

        ::winrt::com_array<::winrt::Windows::UI::Xaml::Markup::XmlnsDefinition> GetXmlnsDefinitions()
        {
            return AppProvider()->GetXmlnsDefinitions();
        }

        void AddOtherProvider(const ::winrt::Windows::UI::Xaml::Markup::IXamlMetadataProvider& provider)
        {
			fprintf(stderr, ">>> Entering AddOtherProvider(provider=%d)\n", provider);
            AppProvider()->AddOtherProvider(provider);
			fprintf(stderr, "<<< Exiting AddOtherProvider(provider=%d)\n", provider);
        }

    private:
        bool _contentLoaded{ false };
        winrt::com_ptr<XamlMetaDataProvider> _appProvider;
        winrt::com_ptr<XamlMetaDataProvider> AppProvider()
        {
			fprintf(stderr, ">>> Entering AppProvider()\n");
            if (!_appProvider)
            {
                _appProvider = winrt::make_self<XamlMetaDataProvider>();
				fprintf(stderr, "AppProvider(): _appProvider → %d\n", _appProvider);
            fprintf(stderr, "<<< Exiting AppProvider()\n");
            }
            return _appProvider;
        }
    };

    template<typename D, typename... I>
    using AppT2 = App_baseWithProvider<D, I...>;
}
