// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "WexTestClass.h"
#include "../../inc/consoletaeftemplates.hpp"

#include "CommonState.hpp"

#include "globals.h"
#include "../buffer/out/textBuffer.hpp"
#include "../buffer/out/textBufferCellIterator.hpp"
#include "../buffer/out/textBufferTextIterator.hpp"

#include "input.h"

#include "../interactivity/inc/ServiceLocator.hpp"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;
using Microsoft::Console::Interactivity::ServiceLocator;

template<typename T>
T GetIterator();

template<typename T>
T GetIteratorAt(til::point at);

template<typename T>
T GetIteratorWithAdvance();

template<>
TextBufferCellIterator GetIteratorAt<TextBufferCellIterator>(til::point at)
{
    const auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
    const auto& outputBuffer = gci.GetActiveOutputBuffer();
    return outputBuffer.GetCellDataAt(at);
}

template<>
TextBufferCellIterator GetIterator<TextBufferCellIterator>()
{
    return GetIteratorAt<TextBufferCellIterator>({});
}

template<>
TextBufferCellIterator GetIteratorWithAdvance<TextBufferCellIterator>()
{
    return GetIteratorAt<TextBufferCellIterator>({ 5, 5 });
}

template<>
TextBufferTextIterator GetIteratorAt<TextBufferTextIterator>(til::point at)
{
    const auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
    const auto& outputBuffer = gci.GetActiveOutputBuffer();
    return outputBuffer.GetTextDataAt(at);
}

template<>
TextBufferTextIterator GetIterator<TextBufferTextIterator>()
{
    return GetIteratorAt<TextBufferTextIterator>({});
}

template<>
TextBufferTextIterator GetIteratorWithAdvance<TextBufferTextIterator>()
{
    return GetIteratorAt<TextBufferTextIterator>({ 5, 5 });
}

class TextBufferIteratorTests
{
    CommonState* m_state;

    TEST_CLASS(TextBufferIteratorTests);

	std::cerr << ">>> Entering TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
	std::cerr << "TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << "): m_state → " << m_state << std::endl;
	std::cerr << "<<< Exiting TEST_CLASS_SETUP(ClassSetup=" << ClassSetup << ")" << std::endl;
    TEST_CLASS_SETUP(ClassSetup)
    {
        m_state = new CommonState();

        m_state->PrepareGlobalScreenBuffer();

        return true;
    }

	std::cerr << ">>> Entering TEST_CLASS_CLEANUP(ClassCleanup=" << ClassCleanup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_CLASS_CLEANUP(ClassCleanup=" << ClassCleanup << ")" << std::endl;
    TEST_CLASS_CLEANUP(ClassCleanup)
    {
        m_state->CleanupGlobalScreenBuffer();

        delete m_state;

        return true;
    }

	std::cerr << ">>> Entering TEST_METHOD_SETUP(MethodSetup=" << MethodSetup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD_SETUP(MethodSetup=" << MethodSetup << ")" << std::endl;
    TEST_METHOD_SETUP(MethodSetup)
    {
        m_state->PrepareNewTextBufferInfo();

        return true;
    }

	std::cerr << ">>> Entering TEST_METHOD_CLEANUP(MethodCleanup=" << MethodCleanup << ")" << std::endl;
	std::cerr << "<<< Exiting TEST_METHOD_CLEANUP(MethodCleanup=" << MethodCleanup << ")" << std::endl;
    TEST_METHOD_CLEANUP(MethodCleanup)
    {
        m_state->CleanupNewTextBufferInfo();

        return true;
    }

    template<typename T>
    void BoolOperatorTestHelper()
    {
		std::cerr << ">>> Entering BoolOperatorTestHelper()" << std::endl;
        const auto it = GetIterator<T>();
        VERIFY_IS_TRUE(it);
		std::cerr << "BoolOperatorTestHelper(): it → " << it << std::endl;

        const auto& outputBuffer = ServiceLocator::LocateGlobals().getConsoleInformation().GetActiveOutputBuffer();
        const auto size = outputBuffer.GetBufferSize().Dimensions();
		std::cerr << "BoolOperatorTestHelper(): size → " << size << std::endl;
        T itInvalidPos(it);
        itInvalidPos._exceeded = true;
		std::cerr << "BoolOperatorTestHelper(): itInvalidPos._exceeded → " << itInvalidPos._exceeded << std::endl;
        VERIFY_IS_FALSE(itInvalidPos);
		std::cerr << "<<< Exiting BoolOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(BoolOperatorText);
    TEST_METHOD(BoolOperatorCell);

    template<typename T>
    void EqualsOperatorTestHelper()
    {
		std::cerr << ">>> Entering EqualsOperatorTestHelper()" << std::endl;
        const auto it = GetIterator<T>();
        const auto it2 = GetIterator<T>();
		std::cerr << "EqualsOperatorTestHelper(): it2 → " << it2 << std::endl;
		std::cerr << "EqualsOperatorTestHelper(): it → " << it << std::endl;

        VERIFY_ARE_EQUAL(it, it2);
		std::cerr << "<<< Exiting EqualsOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(EqualsOperatorText);
    TEST_METHOD(EqualsOperatorCell);

    template<typename T>
    void NotEqualsOperatorTestHelper()
    {
		std::cerr << ">>> Entering NotEqualsOperatorTestHelper()" << std::endl;
        const auto it = GetIterator<T>();

		std::cerr << "NotEqualsOperatorTestHelper(): it → " << it << std::endl;
        auto oneOff = it._pos;
        oneOff.x++;
        const auto it2 = GetIteratorAt<T>(oneOff);
		std::cerr << "NotEqualsOperatorTestHelper(): it2 → " << it2 << std::endl;

        VERIFY_ARE_NOT_EQUAL(it, it2);
		std::cerr << "<<< Exiting NotEqualsOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(NotEqualsOperatorText);
    TEST_METHOD(NotEqualsOperatorCell);

    template<typename T>
    void PlusEqualsOperatorTestHelper()
    {
		std::cerr << ">>> Entering PlusEqualsOperatorTestHelper()" << std::endl;
        auto it = GetIterator<T>();

		std::cerr << "PlusEqualsOperatorTestHelper(): it → " << it << std::endl;
        ptrdiff_t diffUnit = 3;
        auto expectedPos = it._pos;
        expectedPos.x += gsl::narrow<til::CoordType>(diffUnit);
        const auto itExpected = GetIteratorAt<T>(expectedPos);
		std::cerr << "PlusEqualsOperatorTestHelper(): itExpected → " << itExpected << std::endl;

        it += diffUnit;

        VERIFY_ARE_EQUAL(itExpected, it);
		std::cerr << "<<< Exiting PlusEqualsOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(PlusEqualsOperatorText);
    TEST_METHOD(PlusEqualsOperatorCell);

    template<typename T>
    void MinusEqualsOperatorTestHelper()
    {
		std::cerr << ">>> Entering MinusEqualsOperatorTestHelper()" << std::endl;
        auto itExpected = GetIteratorWithAdvance<T>();

		std::cerr << "MinusEqualsOperatorTestHelper(): itExpected → " << itExpected << std::endl;
        ptrdiff_t diffUnit = 3;
        til::point pos = itExpected._pos;
        pos.x += gsl::narrow<til::CoordType>(diffUnit);
        auto itOffset = GetIteratorAt<T>(pos);
		std::cerr << "MinusEqualsOperatorTestHelper(): itOffset → " << itOffset << std::endl;

        itOffset -= diffUnit;

        VERIFY_ARE_EQUAL(itExpected, itOffset);
		std::cerr << "<<< Exiting MinusEqualsOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(MinusEqualsOperatorText);
    TEST_METHOD(MinusEqualsOperatorCell);

    template<typename T>
    void PrefixPlusPlusOperatorTestHelper()
    {
		std::cerr << ">>> Entering PrefixPlusPlusOperatorTestHelper()" << std::endl;
        auto itActual = GetIterator<T>();

		std::cerr << "PrefixPlusPlusOperatorTestHelper(): itActual → " << itActual << std::endl;
        til::point expectedPos = itActual._pos;
        expectedPos.x++;
        const auto itExpected = GetIteratorAt<T>(expectedPos);
		std::cerr << "PrefixPlusPlusOperatorTestHelper(): itExpected → " << itExpected << std::endl;

        ++itActual;

        VERIFY_ARE_EQUAL(itExpected, itActual);
		std::cerr << "<<< Exiting PrefixPlusPlusOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(PrefixPlusPlusOperatorText);
    TEST_METHOD(PrefixPlusPlusOperatorCell);

    template<typename T>
    void PrefixMinusMinusOperatorTestHelper()
    {
		std::cerr << ">>> Entering PrefixMinusMinusOperatorTestHelper()" << std::endl;
        const auto itExpected = GetIteratorWithAdvance<T>();

		std::cerr << "PrefixMinusMinusOperatorTestHelper(): itExpected → " << itExpected << std::endl;
        til::point pos = itExpected._pos;
        pos.x++;
        auto itActual = GetIteratorAt<T>(pos);
		std::cerr << "PrefixMinusMinusOperatorTestHelper(): itActual → " << itActual << std::endl;

        --itActual;

        VERIFY_ARE_EQUAL(itExpected, itActual);
		std::cerr << "<<< Exiting PrefixMinusMinusOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(PrefixMinusMinusOperatorText);
    TEST_METHOD(PrefixMinusMinusOperatorCell);

    template<typename T>
    void PostfixPlusPlusOperatorTestHelper()
    {
		std::cerr << ">>> Entering PostfixPlusPlusOperatorTestHelper()" << std::endl;
        auto it = GetIterator<T>();

		std::cerr << "PostfixPlusPlusOperatorTestHelper(): it → " << it << std::endl;
        auto expectedPos = it._pos;
        expectedPos.x++;
        const auto itExpected = GetIteratorAt<T>(expectedPos);
		std::cerr << "PostfixPlusPlusOperatorTestHelper(): itExpected → " << itExpected << std::endl;

        ++it;

        VERIFY_ARE_EQUAL(itExpected, it);
		std::cerr << "<<< Exiting PostfixPlusPlusOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(PostfixPlusPlusOperatorText);
    TEST_METHOD(PostfixPlusPlusOperatorCell);

    template<typename T>
    void PostfixMinusMinusOperatorTestHelper()
    {
		std::cerr << ">>> Entering PostfixMinusMinusOperatorTestHelper()" << std::endl;
        const auto itExpected = GetIteratorWithAdvance<T>();

		std::cerr << "PostfixMinusMinusOperatorTestHelper(): itExpected → " << itExpected << std::endl;
        til::point pos = itExpected._pos;
        pos.x++;
        auto itActual = GetIteratorAt<T>(pos);
		std::cerr << "PostfixMinusMinusOperatorTestHelper(): itActual → " << itActual << std::endl;

        itActual--;

        VERIFY_ARE_EQUAL(itExpected, itActual);
		std::cerr << "<<< Exiting PostfixMinusMinusOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(PostfixMinusMinusOperatorText);
    TEST_METHOD(PostfixMinusMinusOperatorCell);

    template<typename T>
    void PlusOperatorTestHelper()
    {
		std::cerr << ">>> Entering PlusOperatorTestHelper()" << std::endl;
        auto it = GetIterator<T>();

		std::cerr << "PlusOperatorTestHelper(): it → " << it << std::endl;
        ptrdiff_t diffUnit = 3;
        auto expectedPos = it._pos;
        expectedPos.x += gsl::narrow<til::CoordType>(diffUnit);
        const auto itExpected = GetIteratorAt<T>(expectedPos);
		std::cerr << "PlusOperatorTestHelper(): itExpected → " << itExpected << std::endl;

        const auto itActual = it + diffUnit;

        VERIFY_ARE_EQUAL(itExpected, itActual);
		std::cerr << "<<< Exiting PlusOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(PlusOperatorText);
    TEST_METHOD(PlusOperatorCell);

    template<typename T>
    void MinusOperatorTestHelper()
    {
		std::cerr << ">>> Entering MinusOperatorTestHelper()" << std::endl;
        auto itExpected = GetIteratorWithAdvance<T>();

		std::cerr << "MinusOperatorTestHelper(): itExpected → " << itExpected << std::endl;
        ptrdiff_t diffUnit = 3;
        til::point pos = itExpected._pos;
        pos.x += gsl::narrow<til::CoordType>(diffUnit);
        auto itOffset = GetIteratorAt<T>(pos);
		std::cerr << "MinusOperatorTestHelper(): itOffset → " << itOffset << std::endl;

        const auto itActual = itOffset - diffUnit;

        VERIFY_ARE_EQUAL(itExpected, itActual);
		std::cerr << "<<< Exiting MinusOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(MinusOperatorText);
    TEST_METHOD(MinusOperatorCell);

    template<typename T>
    void DifferenceOperatorTestHelper()
    {
		std::cerr << ">>> Entering DifferenceOperatorTestHelper()" << std::endl;
        const ptrdiff_t expected(3);
        auto it = GetIterator<T>();
		std::cerr << "DifferenceOperatorTestHelper(): it → " << it << std::endl;
        auto it2 = it + expected;

        const ptrdiff_t actual = it2 - it;
        VERIFY_ARE_EQUAL(expected, actual);
		std::cerr << "<<< Exiting DifferenceOperatorTestHelper()" << std::endl;
    }

    TEST_METHOD(DifferenceOperatorText);
    TEST_METHOD(DifferenceOperatorCell);

    TEST_METHOD(AsCharInfoCell);

    TEST_METHOD(DereferenceOperatorText);
    TEST_METHOD(DereferenceOperatorCell);

    TEST_METHOD(ConstructedNoLimit);
    TEST_METHOD(ConstructedLimits);
};

void TextBufferIteratorTests::BoolOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::BoolOperatorText()" << std::endl;
    BoolOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::BoolOperatorText()" << std::endl;
}

void TextBufferIteratorTests::BoolOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::BoolOperatorCell()" << std::endl;
    BoolOperatorTestHelper<TextBufferCellIterator>();

    Log::Comment(L"For cells, also check incrementing past the end.");
    const auto& outputBuffer = ServiceLocator::LocateGlobals().getConsoleInformation().GetActiveOutputBuffer();
    const auto size = outputBuffer.GetBufferSize().Dimensions();
	std::cerr << "TextBufferIteratorTests::BoolOperatorCell(): size → " << size << std::endl;
    TextBufferCellIterator it(outputBuffer.GetTextBuffer(), { size.width - 1, size.height - 1 });
    VERIFY_IS_TRUE(it);
    it++;
    VERIFY_IS_FALSE(it);
	std::cerr << "<<< Exiting TextBufferIteratorTests::BoolOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::EqualsOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::EqualsOperatorText()" << std::endl;
    EqualsOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::EqualsOperatorText()" << std::endl;
}

void TextBufferIteratorTests::EqualsOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::EqualsOperatorCell()" << std::endl;
    EqualsOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::EqualsOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::NotEqualsOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::NotEqualsOperatorText()" << std::endl;
    NotEqualsOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::NotEqualsOperatorText()" << std::endl;
}

void TextBufferIteratorTests::NotEqualsOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::NotEqualsOperatorCell()" << std::endl;
    NotEqualsOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::NotEqualsOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::PlusEqualsOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PlusEqualsOperatorText()" << std::endl;
    PlusEqualsOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PlusEqualsOperatorText()" << std::endl;
}

void TextBufferIteratorTests::PlusEqualsOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PlusEqualsOperatorCell()" << std::endl;
    PlusEqualsOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PlusEqualsOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::MinusEqualsOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::MinusEqualsOperatorText()" << std::endl;
    MinusEqualsOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::MinusEqualsOperatorText()" << std::endl;
}

void TextBufferIteratorTests::MinusEqualsOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::MinusEqualsOperatorCell()" << std::endl;
    MinusEqualsOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::MinusEqualsOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::PrefixPlusPlusOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PrefixPlusPlusOperatorText()" << std::endl;
    PrefixPlusPlusOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PrefixPlusPlusOperatorText()" << std::endl;
}

void TextBufferIteratorTests::PrefixPlusPlusOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PrefixPlusPlusOperatorCell()" << std::endl;
    PrefixPlusPlusOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PrefixPlusPlusOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::PrefixMinusMinusOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PrefixMinusMinusOperatorText()" << std::endl;
    PrefixMinusMinusOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PrefixMinusMinusOperatorText()" << std::endl;
}

void TextBufferIteratorTests::PrefixMinusMinusOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PrefixMinusMinusOperatorCell()" << std::endl;
    PrefixMinusMinusOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PrefixMinusMinusOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::PostfixPlusPlusOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PostfixPlusPlusOperatorText()" << std::endl;
    PostfixPlusPlusOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PostfixPlusPlusOperatorText()" << std::endl;
}

void TextBufferIteratorTests::PostfixPlusPlusOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PostfixPlusPlusOperatorCell()" << std::endl;
    PostfixPlusPlusOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PostfixPlusPlusOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::PostfixMinusMinusOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PostfixMinusMinusOperatorText()" << std::endl;
    PostfixMinusMinusOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PostfixMinusMinusOperatorText()" << std::endl;
}

void TextBufferIteratorTests::PostfixMinusMinusOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PostfixMinusMinusOperatorCell()" << std::endl;
    PostfixMinusMinusOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PostfixMinusMinusOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::PlusOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PlusOperatorText()" << std::endl;
    PlusOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PlusOperatorText()" << std::endl;
}

void TextBufferIteratorTests::PlusOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::PlusOperatorCell()" << std::endl;
    PlusOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::PlusOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::MinusOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::MinusOperatorText()" << std::endl;
    MinusOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::MinusOperatorText()" << std::endl;
}

void TextBufferIteratorTests::MinusOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::MinusOperatorCell()" << std::endl;
    MinusOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::MinusOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::DifferenceOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::DifferenceOperatorText()" << std::endl;
    DifferenceOperatorTestHelper<TextBufferTextIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::DifferenceOperatorText()" << std::endl;
}

void TextBufferIteratorTests::DifferenceOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::DifferenceOperatorCell()" << std::endl;
    DifferenceOperatorTestHelper<TextBufferCellIterator>();
	std::cerr << "<<< Exiting TextBufferIteratorTests::DifferenceOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::AsCharInfoCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::AsCharInfoCell()" << std::endl;
    m_state->FillTextBuffer();
    const auto it = GetIterator<TextBufferCellIterator>();
	std::cerr << "TextBufferIteratorTests::AsCharInfoCell(): it → " << it << std::endl;
    const auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();

    const auto& outputBuffer = gci.GetActiveOutputBuffer();

    const auto& row = outputBuffer._textBuffer->GetRowByOffset(it._pos.y);

    const auto wcharExpected = *row.GlyphAt(it._pos.x).begin();
	std::cerr << "TextBufferIteratorTests::AsCharInfoCell(): wcharExpected → " << wcharExpected << std::endl;
	std::cerr << "TextBufferIteratorTests::AsCharInfoCell(): attrExpected → " << attrExpected << std::endl;
    const auto attrExpected = row.GetAttrByColumn(it._pos.x);

    const auto cellActual = gci.AsCharInfo(*it);
	std::cerr << "TextBufferIteratorTests::AsCharInfoCell(): cellActual → " << cellActual << std::endl;
    const auto wcharActual = cellActual.Char.UnicodeChar;
    const auto attrActual = it->TextAttr();
	std::cerr << "TextBufferIteratorTests::AsCharInfoCell(): attrActual → " << attrActual << std::endl;

    VERIFY_ARE_EQUAL(wcharExpected, wcharActual);
    VERIFY_ARE_EQUAL(attrExpected, attrActual);
	std::cerr << "<<< Exiting TextBufferIteratorTests::AsCharInfoCell()" << std::endl;
}

void TextBufferIteratorTests::DereferenceOperatorText()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::DereferenceOperatorText()" << std::endl;
    m_state->FillTextBuffer();
    const auto it = GetIterator<TextBufferTextIterator>();
	std::cerr << "TextBufferIteratorTests::DereferenceOperatorText(): it → " << it << std::endl;

    const auto& outputBuffer = ServiceLocator::LocateGlobals().getConsoleInformation().GetActiveOutputBuffer();

    const auto& row = outputBuffer._textBuffer->GetRowByOffset(it._pos.y);

    const auto wcharExpected = row.GlyphAt(it._pos.x);
	std::cerr << "TextBufferIteratorTests::DereferenceOperatorText(): wcharExpected → " << wcharExpected << std::endl;
    const auto wcharActual = *it;

    VERIFY_ARE_EQUAL(*wcharExpected.begin(), *wcharActual.begin());
	std::cerr << "<<< Exiting TextBufferIteratorTests::DereferenceOperatorText()" << std::endl;
}

void TextBufferIteratorTests::DereferenceOperatorCell()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::DereferenceOperatorCell()" << std::endl;
    m_state->FillTextBuffer();
    const auto it = GetIterator<TextBufferCellIterator>();
	std::cerr << "TextBufferIteratorTests::DereferenceOperatorCell(): it → " << it << std::endl;

    const auto& outputBuffer = ServiceLocator::LocateGlobals().getConsoleInformation().GetActiveOutputBuffer();

    const auto& row = outputBuffer._textBuffer->GetRowByOffset(it._pos.y);

    const auto textExpected = (std::wstring_view)row.GlyphAt(it._pos.x);
	std::cerr << "TextBufferIteratorTests::DereferenceOperatorCell(): textExpected → " << textExpected << std::endl;
	std::cerr << "TextBufferIteratorTests::DereferenceOperatorCell(): dbcsExpected → " << dbcsExpected << std::endl;
	std::cerr << "TextBufferIteratorTests::DereferenceOperatorCell(): attrExpected → " << attrExpected << std::endl;
    const auto dbcsExpected = row.DbcsAttrAt(it._pos.x);
    const auto attrExpected = row.GetAttrByColumn(it._pos.x);

    const auto cellActual = *it;
    const auto textActual = cellActual.Chars();
	std::cerr << "TextBufferIteratorTests::DereferenceOperatorCell(): textActual → " << textActual << std::endl;
	std::cerr << "TextBufferIteratorTests::DereferenceOperatorCell(): dbcsActual → " << dbcsActual << std::endl;
	std::cerr << "TextBufferIteratorTests::DereferenceOperatorCell(): attrActual → " << attrActual << std::endl;
    const auto dbcsActual = cellActual.DbcsAttr();
    const auto attrActual = cellActual.TextAttr();

    VERIFY_ARE_EQUAL(String(textExpected.data(), (int)textExpected.size()), String(textActual.data(), (int)textActual.size()));
    VERIFY_ARE_EQUAL(dbcsExpected, dbcsActual);
    VERIFY_ARE_EQUAL(attrExpected, attrActual);
	std::cerr << "<<< Exiting TextBufferIteratorTests::DereferenceOperatorCell()" << std::endl;
}

void TextBufferIteratorTests::ConstructedNoLimit()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::ConstructedNoLimit()" << std::endl;
    m_state->FillTextBuffer();

    const auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
    const auto& outputBuffer = gci.GetActiveOutputBuffer();
    const auto& textBuffer = outputBuffer.GetTextBuffer();
    const auto& bufferSize = textBuffer.GetSize();

    TextBufferCellIterator it(textBuffer, {});

    VERIFY_IS_TRUE(it, L"Iterator is valid.");
    VERIFY_ARE_EQUAL(bufferSize, it._bounds, L"Bounds match the bounds of the text buffer.");

    const auto totalBufferDistance = bufferSize.Width() * bufferSize.Height();
	std::cerr << "TextBufferIteratorTests::ConstructedNoLimit(): totalBufferDistance → " << totalBufferDistance << std::endl;

    // Advance buffer to one before the end.
    it += (totalBufferDistance - 1);
    VERIFY_IS_TRUE(it, L"Iterator is still valid.");

    // Advance over the end.
    it++;
    VERIFY_IS_FALSE(it, L"Iterator invalid now.");

    // Verify throws for out of range.
    VERIFY_THROWS_SPECIFIC(TextBufferCellIterator(textBuffer, { -1, -1 }), wil::ResultException, [](wil::ResultException& e) { return e.GetErrorCode() == E_INVALIDARG; });
	std::cerr << "<<< Exiting TextBufferIteratorTests::ConstructedNoLimit()" << std::endl;
}

void TextBufferIteratorTests::ConstructedLimits()
{
	std::cerr << ">>> Entering TextBufferIteratorTests::ConstructedLimits()" << std::endl;
    m_state->FillTextBuffer();

    const auto& gci = ServiceLocator::LocateGlobals().getConsoleInformation();
    const auto& outputBuffer = gci.GetActiveOutputBuffer();
    const auto& textBuffer = outputBuffer.GetTextBuffer();

    til::inclusive_rect limits;
    limits.top = 1;
	std::cerr << "TextBufferIteratorTests::ConstructedLimits(): limits.top → " << limits.top << std::endl;
	std::cerr << "TextBufferIteratorTests::ConstructedLimits(): limits.bottom → " << limits.bottom << std::endl;
	std::cerr << "TextBufferIteratorTests::ConstructedLimits(): limits.left → " << limits.left << std::endl;
	std::cerr << "TextBufferIteratorTests::ConstructedLimits(): limits.right → " << limits.right << std::endl;
	std::cerr << "TextBufferIteratorTests::ConstructedLimits(): viewport → " << viewport << std::endl;
    limits.bottom = 1;
    limits.left = 3;
    limits.right = 5;
    const auto viewport = Microsoft::Console::Types::Viewport::FromInclusive(limits);

    til::point pos;
    pos.x = limits.left;
    pos.y = limits.top;

    TextBufferCellIterator it(textBuffer, pos, viewport);

    VERIFY_IS_TRUE(it, L"Iterator is valid.");
    VERIFY_ARE_EQUAL(viewport, it._bounds, L"Bounds match the bounds given.");

    const auto totalBufferDistance = viewport.Width() * viewport.Height();
	std::cerr << "TextBufferIteratorTests::ConstructedLimits(): totalBufferDistance → " << totalBufferDistance << std::endl;

    // Advance buffer to one before the end.
    it += (totalBufferDistance - 1);
    VERIFY_IS_TRUE(it, L"Iterator is still valid.");

    // Advance over the end.
    it++;
    VERIFY_IS_FALSE(it, L"Iterator invalid now.");

    // Verify throws for out of range.
    VERIFY_THROWS_SPECIFIC(TextBufferCellIterator(textBuffer,
                                                  {},
                                                  viewport),
                           wil::ResultException,
                           [](wil::ResultException& e) { return e.GetErrorCode() == E_INVALIDARG; });

    // Verify throws for limit not inside buffer
    const auto bufferSize = textBuffer.GetSize();
	std::cerr << "TextBufferIteratorTests::ConstructedLimits(): bufferSize → " << bufferSize << std::endl;
	std::cerr << "TextBufferIteratorTests::ConstructedLimits(): invalidViewport → " << invalidViewport << std::endl;
    const auto invalidViewport = Viewport::FromInclusive({ bufferSize.Left(), bufferSize.Top(), bufferSize.RightInclusive() + 1, bufferSize.BottomInclusive() + 1 });
    VERIFY_THROWS_SPECIFIC(TextBufferCellIterator(textBuffer,
                                                  pos,
                                                  invalidViewport),
                           wil::ResultException,
                           [](wil::ResultException& e) { return e.GetErrorCode() == E_INVALIDARG; });
						std::cerr << "<<< Exiting TextBufferIteratorTests::ConstructedLimits()" << std::endl;
}
