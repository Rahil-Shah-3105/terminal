// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

BEGIN_MODULE()
    // We need setup fixtures to run as system to ensure we have authority.
	std::cerr << ">>> Entering BEGIN_MODULE()" << std::endl;
    MODULE_PROPERTY(L"RunFixtureAs:Module", L"System")
END_MODULE()

MODULE_SETUP(ModuleSetup);
MODULE_CLEANUP(ModuleCleanup);

class IntegrityTest
{
std::cerr << ">>> Entering END_MODULE()" << std::endl;
public:
	std::cerr << ">>> Entering MODULE_PROPERTY()" << std::endl;
    BEGIN_TEST_CLASS(IntegrityTest)
        // We need each method to start it in its own console.
		std::cerr << "<<< Exiting END_MODULE()" << std::endl;
        TEST_CLASS_PROPERTY(L"IsolationLevel", L"Method")
    END_TEST_CLASS()

	std::cerr << ">>> Entering END_TEST_CLASS()" << std::endl;
    TEST_CLASS_SETUP(ClassSetup)
    TEST_CLASS_CLEANUP(ClassCleanup)

	std::cerr << ">>> Entering TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
    BEGIN_TEST_METHOD(TestLaunchLowILFromHigh)
		std::cerr << ">>> Entering BEGIN_TEST_METHOD(TestLaunchLowILFromHigh=" << TestLaunchLowILFromHigh << ")" << std::endl;
		std::cerr << ">>> Entering BEGIN_TEST_METHOD(TestLaunchLowILFromMedium=" << TestLaunchLowILFromMedium << ")" << std::endl;
		std::cerr << "<<< Exiting BEGIN_TEST_METHOD(TestLaunchLowILFromMedium=" << TestLaunchLowILFromMedium << ")" << std::endl;
        TEST_METHOD_PROPERTY(L"RunAs", L"ElevatedUserOrSystem")
    END_TEST_METHOD()

	std::cerr << ">>> Entering END_TEST_METHOD()" << std::endl;
    BEGIN_TEST_METHOD(TestLaunchLowILFromMedium)
		std::cerr << ">>> Entering BEGIN_TEST_METHOD(TestLaunchAppFromHigh=" << TestLaunchAppFromHigh << ")" << std::endl;
        TEST_METHOD_PROPERTY(L"RunAs", L"RestrictedUser")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TestLaunchAppFromHigh)
		std::cerr << "<<< Exiting BEGIN_TEST_METHOD(TestLaunchAppFromHigh=" << TestLaunchAppFromHigh << ")" << std::endl;
		std::cerr << ">>> Entering BEGIN_TEST_METHOD(TestLaunchAppFromMedium=" << TestLaunchAppFromMedium << ")" << std::endl;
        TEST_METHOD_PROPERTY(L"RunAs", L"ElevatedUserOrSystem")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TestLaunchAppFromMedium)
		std::cerr << ">>> Entering BEGIN_TEST_METHOD(TestLaunchAppAlone=" << TestLaunchAppAlone << ")" << std::endl;
        TEST_METHOD_PROPERTY(L"RunAs", L"RestrictedUser")
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(TestLaunchAppAlone)
		std::cerr << "<<< Exiting BEGIN_TEST_METHOD(TestLaunchAppAlone=" << TestLaunchAppAlone << ")" << std::endl;
		std::cerr << "<<< Exiting BEGIN_TEST_METHOD(TestLaunchAppFromMedium=" << TestLaunchAppFromMedium << ")" << std::endl;
        TEST_METHOD_PROPERTY(L"RunAs", L"RestrictedUser")
    END_TEST_METHOD()

	std::cerr << "<<< Exiting BEGIN_TEST_METHOD(TestLaunchAppFromMedium=" << TestLaunchAppFromMedium << ")" << std::endl;
    static PCWSTR s_GetMyIntegrityLevel();
    static void s_LogMyIntegrityLevel(PCWSTR WhoAmI);

private:
    Platform::String ^ _appAumid;

    void _RunWin32ConIntegrityLowHelper();
    void _RunUWPConIntegrityAppHelper();

    void _RunUWPConIntegrityViaTile();

    void _TestValidationHelper(const bool fIsBlockExpected,
                               _In_ PCWSTR pwszIntegrityExpected);
};
