/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <functional>
#include <gtest/gtest.h>

#include "param_test_macros.h"
#include "char_groups.h"
#include "texgine_exception.h"
#include "text_converter.h"

using namespace testing;
using namespace testing::ext;

#define INVALID_CGS(...) \
    {.object = invalid1Cgs_, ##__VA_ARGS__, .exception = ExceptionType::ERROR_STATUS}, \
    {.object = invalid2Cgs_, ##__VA_ARGS__, .exception = ExceptionType::ERROR_STATUS}, \
    {.object = invalid3Cgs_, ##__VA_ARGS__, .exception = ExceptionType::ERROR_STATUS}, \
    {.object = invalid4Cgs_, ##__VA_ARGS__, .exception = ExceptionType::ERROR_STATUS}, \
    {.object = invalid5Cgs_, ##__VA_ARGS__, .exception = ExceptionType::ERROR_STATUS}

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class CharGroupsTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        // {0x013B, 13.664} is {codepoint, advanceX}
        normalCgs_.PushBack({.chars = TextConverter::ToUTF16("m"), .glyphs = {{0x013B, 13.664}}});
        normalCgs_.PushBack({.chars = TextConverter::ToUTF16("o"), .glyphs = {{0x0145, 9.456}}});
        normalCgs_.PushBack({.chars = TextConverter::ToUTF16("s"), .glyphs = {{0x0166, 7.28}}});
        normalCgs_.PushBack({.chars = TextConverter::ToUTF16("t"), .glyphs = {{0x016E, 5.88}}});
        glyphsCgs_.PushBack({.chars = TextConverter::ToUTF16("most"),
            .glyphs = {{0x013B, 13.664}, {0x0145, 9.456}, {0x0166, 7.28}, {0x016E, 5.88}}});
        normalSubCgs1_ = normalCgs_.GetSub(0, 1);
        normalSubCgs2_ = normalCgs_.GetSub(1, 2);   // 1, 2 means getting the range of sub chargrops {start, end}
    }

    static inline CharGroups normalCgs_ = CharGroups::CreateEmpty();
    static inline CharGroups glyphsCgs_ = CharGroups::CreateEmpty();
    static inline CharGroups emptyCgs_ = CharGroups::CreateEmpty();
    static inline CharGroups defaultCgs_ = {};
    static inline CharGroups normalSubCgs1_;
    static inline CharGroups normalSubCgs2_;
    // {6, 2} is chargrops range {start, end}
    static inline CharGroups invalid1Cgs_ = CharGroups::CreateWithInvalidRange({6, 2});
    static inline CharGroups invalid2Cgs_ = CharGroups::CreateWithInvalidRange({-1, 1});
    static inline CharGroups invalid3Cgs_ = CharGroups::CreateWithInvalidRange({-4, -1});
    static inline CharGroups invalid4Cgs_ = CharGroups::CreateWithInvalidRange({2, 5});
    static inline CharGroups invalid5Cgs_ = CharGroups::CreateWithInvalidRange({0x7ffffffe, 0x7fffffff});
    static inline CharGroup  cg_ = {.chars = TextConverter::ToUTF16("n"), .glyphs = {{0x013C, 13.664}}};
};

auto GetObjectRangeChecker(int t1, int t2)
{
    return [t1, t2](CharGroups &&ret) {
        ASSERT_EQ(t1, ret.GetRange().start);
        ASSERT_EQ(t2, ret.GetRange().end);
    };
}

auto GetIndexRangeChecker(int t1, int t2)
{
    return [t1, t2](IndexRange ret) {
        ASSERT_EQ(t1, ret.start);
        ASSERT_EQ(t2, ret.end);
    };
}

/**
 * @tc.name: GetNumberOfGlyph
 * @tc.desc: Verify the GetNumberOfGlyph
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, GetNumberOfGlyph, {
    {.object = defaultCgs_,  .exception = ExceptionType::INVALID_CHAR_GROUPS},
    // 0u, 4u: check the return value
    {.object = emptyCgs_,    .checkFunc = GetResultChecker(0u)},
    {.object = normalCgs_,   .checkFunc = GetResultChecker(4u)},
    {.object = glyphsCgs_,   .checkFunc = GetResultChecker(4u)},
});

/**
 * @tc.name: GetNumberOfCharGroup
 * @tc.desc: Verify the GetNumberOfCharGroup
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, GetNumberOfCharGroup, {
    {.object = defaultCgs_,  .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = emptyCgs_,    .checkFunc = GetResultChecker(0u)},
    {.object = normalCgs_,   .checkFunc = GetResultChecker(4u)},
    {.object = glyphsCgs_,   .checkFunc = GetResultChecker(1u)},
});

/**
 * @tc.name: GetRange
 * @tc.desc: Verify the GetRange
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, GetRange, {
    // (6, 2) check the IndexRange
    {.object = invalid1Cgs_, .checkFunc = GetIndexRangeChecker(6, 2)},
    {.object = invalid2Cgs_, .checkFunc = GetIndexRangeChecker(-1, 1)},
    {.object = invalid3Cgs_, .checkFunc = GetIndexRangeChecker(-4, -1)},
    {.object = invalid4Cgs_, .checkFunc = GetIndexRangeChecker(2, 5)},
    {.object = invalid5Cgs_, .checkFunc = GetIndexRangeChecker(0x7ffffffe, 0x7fffffff)},
    {.object = defaultCgs_,  .checkFunc = GetIndexRangeChecker(0, 0)},
    {.object = emptyCgs_,    .checkFunc = GetIndexRangeChecker(0, 0)},
    {.object = normalCgs_,   .checkFunc = GetIndexRangeChecker(0, 4)},
    {.object = glyphsCgs_,   .checkFunc = GetIndexRangeChecker(0, 1)},
});

/**
 * @tc.name: GetBack
 * @tc.desc: Verify the GetBack
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, GetBack, {
    {.object = defaultCgs_,  .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = emptyCgs_,    .exception = ExceptionType::OUT_OF_RANGE},
    {.object = normalCgs_,   .checkFunc = [](CharGroup cg) {
        ASSERT_EQ(TextConverter::ToStr(cg.chars), "t");
    }},
    {.object = glyphsCgs_,   .checkFunc = [](CharGroup cg) {
        ASSERT_EQ(TextConverter::ToStr(cg.chars), "most");
    }},
});

/**
 * @tc.name: GetSize
 * @tc.desc: Verify the GetSize
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, GetSize, {
    {.object = defaultCgs_,  .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = emptyCgs_,    .checkFunc = GetResultChecker(0u)},
    {.object = normalCgs_,   .checkFunc = GetResultChecker(4u)},
    {.object = glyphsCgs_,   .checkFunc = GetResultChecker(1u)},
});

/**
 * @tc.name: IsValid
 * @tc.desc: Verify the IsValid
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, IsValid, {
    {.object = defaultCgs_,  .checkFunc = GetResultChecker(false)},
    {.object = emptyCgs_,    .checkFunc = GetResultChecker(true)},
    {.object = normalCgs_,   .checkFunc = GetResultChecker(true)},
    {.object = glyphsCgs_,   .checkFunc = GetResultChecker(true)},
});

/**
 * @tc.name: IsSameCharGroups
 * @tc.desc: Verify the IsSameCharGroups
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST1(CharGroups, IsSameCharGroups, CharGroups, {
    {.object = normalCgs_,     .arg1 = emptyCgs_,      .checkFunc = GetResultChecker(false)},
    {.object = normalCgs_,     .arg1 = defaultCgs_,    .checkFunc = GetResultChecker(false)},
    {.object = normalCgs_,     .arg1 = normalSubCgs1_, .checkFunc = GetResultChecker(true)},
    {.object = normalCgs_,     .arg1 = normalSubCgs2_, .checkFunc = GetResultChecker(true)},
    {.object = normalSubCgs1_, .arg1 = normalSubCgs2_, .checkFunc = GetResultChecker(true)},
});

/**
 * @tc.name: IsIntersect
 * @tc.desc: Verify the IsIntersect
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST1(CharGroups, IsIntersect, CharGroups, {
    {.object = emptyCgs_,      .arg1 = invalid3Cgs_,   .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = defaultCgs_,    .arg1 = defaultCgs_,    .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = normalSubCgs1_, .arg1 = normalSubCgs2_, .checkFunc = GetResultChecker(false)},
    {.object = normalCgs_,     .arg1 = normalSubCgs1_, .checkFunc = GetResultChecker(true)},
    {.object = normalCgs_,     .arg1 = normalSubCgs2_, .checkFunc = GetResultChecker(true)},
    {.object = normalCgs_,     .arg1 = glyphsCgs_,     .checkFunc = GetResultChecker(false)},
});

/**
 * @tc.name: GetSplit
 * @tc.desc: Verify the GetSplit
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST1(CharGroups, GetSplit, int, {
    // arg1 is parameters of GetSplit
    {.object = normalCgs_,   .arg1 = -1, .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = defaultCgs_,  .arg1 = 1,  .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = normalCgs_,   .arg1 = 0,  .exception = ExceptionType::OUT_OF_RANGE},
    {.object = normalCgs_,   .arg1 = 4,  .exception = ExceptionType::OUT_OF_RANGE},
    {.object = normalCgs_,   .arg1 = 1,  .checkFunc = [](CharGroupsPair ret) {
        ASSERT_EQ(ret[0].GetRange().end, 1);
        ASSERT_EQ(ret[1].GetRange().start, 1);
    }},
    {.object = normalCgs_,   .arg1 = 3,  .checkFunc = [](CharGroupsPair ret) {
        ASSERT_EQ(ret[0].GetRange().end, 3);
        ASSERT_EQ(ret[1].GetRange().start, 3);
    }},
});

/**
 * @tc.name: GetSplitAll
 * @tc.desc: Verify the GetSplitAll
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST1(CharGroups, GetSplitAll, int, {
    // arg1 is parameters of GetSplitAll
    {.object = defaultCgs_,  .arg1 = 1, .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = normalCgs_,   .arg1 = 0, .exception = ExceptionType::OUT_OF_RANGE},
    {.object = normalCgs_,   .arg1 = 4, .exception = ExceptionType::OUT_OF_RANGE},
    {.object = normalCgs_,   .arg1 = 1, .checkFunc = [](CharGroupsPair ret) {
        ASSERT_EQ(ret[0].GetRange().end, 1);
        ASSERT_EQ(ret[1].GetRange().start, 1);
    }},
    {.object = normalCgs_,   .arg1 = 3, .checkFunc = [](CharGroupsPair ret) {
        ASSERT_EQ(ret[0].GetRange().end, 3);
        ASSERT_EQ(ret[1].GetRange().start, 3);
    }},
});

/**
 * @tc.name: GetSub
 * @tc.desc: Verify the GetSub
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST2(CharGroups, GetSub, int, int, {
    // arg1 and arg2 are parameters of GetSub, arg1 is start, arg2 is end
    {.object = normalCgs_,     .arg1 = -1, .arg2 = 2, .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalCgs_,     .arg1 = 2,  .arg2 = 1, .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalSubCgs2_, .arg1 = -1, .arg2 = 1, .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalCgs_,     .arg1 = 2,  .arg2 = 6, .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalCgs_,     .arg1 = 0,  .arg2 = 0, .checkFunc = GetObjectRangeChecker(0, 0)},
    {.object = normalCgs_,     .arg1 = 1,  .arg2 = 2, .checkFunc = GetObjectRangeChecker(1, 2)},
    {.object = normalCgs_,     .arg1 = 4,  .arg2 = 4, .checkFunc = GetObjectRangeChecker(4, 4)},
});

/**
 * @tc.name: GetSubAll
 * @tc.desc: Verify the GetSubAll
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST2(CharGroups, GetSubAll, int, int, {
    // arg1 and arg2 are parameters of GetSubAll, arg1 is start, arg2 is end
    {.object = defaultCgs_,  .arg1 = 0,  .arg2 = 0, .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = normalCgs_,   .arg1 = -1, .arg2 = 2, .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalCgs_,   .arg1 = 2,  .arg2 = 1, .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalCgs_,   .arg1 = 1,  .arg2 = 9, .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalCgs_,   .arg1 = 0,  .arg2 = 0, .checkFunc = GetObjectRangeChecker(0, 0)},
    {.object = normalCgs_,   .arg1 = 0,  .arg2 = 4, .checkFunc = GetObjectRangeChecker(0, 4)},
    {.object = normalCgs_,   .arg1 = 2,  .arg2 = 3, .checkFunc = GetObjectRangeChecker(2, 3)},
});

/**
 * @tc.name: GetSubFromU16RangeAll
 * @tc.desc: Verify the GetSubFromU16RangeAll
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST2(CharGroups, GetSubFromU16RangeAll, int, int, {
    // arg1 and arg2 are parameters of GetSubFromU16RangeAll, arg1 is start, arg2 is end
    {.object = defaultCgs_,  .arg1 = 1,  .arg2 = 2,  .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = normalCgs_,   .arg1 = -1, .arg2 = 2,  .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalCgs_,   .arg1 = 2,  .arg2 = 1,  .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalCgs_,   .arg1 = 10, .arg2 = 20, .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalCgs_,   .arg1 = 0,  .arg2 = 0,  .checkFunc = GetObjectRangeChecker(0, 0)},
    {.object = normalCgs_,   .arg1 = 1,  .arg2 = 2,  .checkFunc = GetObjectRangeChecker(1, 2)},
    {.object = normalCgs_,   .arg1 = 4,  .arg2 = 4,  .checkFunc = GetObjectRangeChecker(4, 4)},
    {.object = glyphsCgs_,   .arg1 = 0,  .arg2 = 1,  .checkFunc = GetObjectRangeChecker(0, 1)},
    {.object = glyphsCgs_,   .arg1 = 0,  .arg2 = 2,  .checkFunc = GetObjectRangeChecker(0, 1)},
    {.object = glyphsCgs_,   .arg1 = 0,  .arg2 = 3,  .checkFunc = GetObjectRangeChecker(0, 1)},
    {.object = glyphsCgs_,   .arg1 = 0,  .arg2 = 4,  .checkFunc = GetObjectRangeChecker(0, 1)},
});

/**
 * @tc.name: GetIntersect
 * @tc.desc: Verify the GetIntersect
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST1(CharGroups, GetIntersect, CharGroups, {
    {.object = normalCgs_,     .arg1 = invalid1Cgs_,   .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = defaultCgs_,    .arg1 = normalCgs_,     .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = normalCgs_,     .arg1 = defaultCgs_,    .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalSubCgs1_, .arg1 = normalSubCgs2_, .exception = ExceptionType::CUSTOM},
    {.object = normalCgs_,     .arg1 = normalSubCgs1_, .checkFunc = GetObjectRangeChecker(0, 1)},
    {.object = normalCgs_,     .arg1 = normalSubCgs2_, .checkFunc = GetObjectRangeChecker(1, 2)},
});

/**
 * @tc.name: Get
 * @tc.desc: Verify the Get
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST1(CharGroups, Get, int32_t, {
    {.object = normalSubCgs2_, .arg1 = -1, .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalSubCgs1_, .arg1 = 0,
     .checkFunc = [](struct CharGroup ret) {
         ASSERT_EQ(TextConverter::ToStr(ret.chars), "m");
    }},
    {.object = normalSubCgs2_, .arg1 = 0,
     .checkFunc = [](struct CharGroup ret) {
         ASSERT_EQ(TextConverter::ToStr(ret.chars), "o");
    }},
});

/**
 * @tc.name: GetAll
 * @tc.desc: Verify the GetAll
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST1(CharGroups, GetAll, int32_t, {
    {.object = defaultCgs_,    .arg1 = 0,  .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = normalCgs_,     .arg1 = -1, .exception = ExceptionType::OUT_OF_RANGE},
    {.object = normalCgs_,     .arg1 = 4,  .exception = ExceptionType::OUT_OF_RANGE},
    {.object = normalCgs_,     .arg1 = 0,
     .checkFunc = [](struct CharGroup ret) {
         ASSERT_EQ(TextConverter::ToStr(ret.chars), "m");
    }},
    {.object = normalSubCgs2_, .arg1 = 0,
     .checkFunc = [](struct CharGroup ret) {
         ASSERT_EQ(TextConverter::ToStr(ret.chars), "m");
    }},
});

/**
 * @tc.name: ToUTF16
 * @tc.desc: Verify the ToUTF16
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, ToUTF16, {
    {.object = defaultCgs_,    .exception = ExceptionType::INVALID_CHAR_GROUPS},
    // 0 is the return size
    {.object = emptyCgs_,      .checkFunc = GetVecSizeChecker<uint16_t>(0)},
    {.object = normalCgs_,     .checkFunc = GetVecSizeChecker<uint16_t>(4)},
    {.object = normalSubCgs1_, .checkFunc = GetVecSizeChecker<uint16_t>(1)},
    {.object = normalSubCgs2_, .checkFunc = GetVecSizeChecker<uint16_t>(1)},
    {.object = glyphsCgs_,     .checkFunc = GetVecSizeChecker<uint16_t>(4)},
});

/**
 * @tc.name: ToUTF16All
 * @tc.desc: Verify the ToUTF16All
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, ToUTF16All, {
    {.object = defaultCgs_,    .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = emptyCgs_,      .checkFunc = GetVecSizeChecker<uint16_t>(0)},
    {.object = normalCgs_,     .checkFunc = GetVecSizeChecker<uint16_t>(4)},
    {.object = normalSubCgs1_, .checkFunc = GetVecSizeChecker<uint16_t>(4)},
    {.object = normalSubCgs2_, .checkFunc = GetVecSizeChecker<uint16_t>(4)},
    {.object = glyphsCgs_,     .checkFunc = GetVecSizeChecker<uint16_t>(4)},
});

/**
 * @tc.name: begin
 * @tc.desc: Verify the begin
 * @tc.type:FUNC
 * @tc.require: issueI6XQ27
 */
DEFINE_PARAM_TEST0(CharGroups, begin, {
    {.object = defaultCgs_,  .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = emptyCgs_,    .checkFunc = [&](std::vector<struct CharGroup>::iterator ret) {
        ASSERT_EQ(ret, emptyCgs_.end());
    }},
    {.object = normalCgs_,   .checkFunc = [](std::vector<struct CharGroup>::iterator ret) {
        ASSERT_EQ(TextConverter::ToStr(ret->chars), "m");
    }},
});

/**
 * @tc.name: end
 * @tc.desc: Verify the end
 * @tc.type:FUNC
 * @tc.require: issueI6XQ27
 */
DEFINE_PARAM_TEST0(CharGroups, end, {
    {.object = defaultCgs_,  .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = emptyCgs_,    .checkFunc = [](std::vector<struct CharGroup>::iterator ret) {
        ASSERT_EQ(ret, emptyCgs_.begin());
    }},
    {.object = normalCgs_,   .checkFunc = [](std::vector<struct CharGroup>::iterator ret) {
        ASSERT_EQ(TextConverter::ToStr((ret - 1)->chars), "t");
    }},
});

auto GetCharGroupsEqualChecker(const CharGroups &cgs)
{
    return [cgs](CharGroups &&obj) {
        ASSERT_FALSE(obj.IsSameCharGroups(cgs));
        ASSERT_EQ(obj.GetRange().start, cgs.GetRange().start);
        ASSERT_EQ(obj.GetRange().end, cgs.GetRange().end);
    };
}

/**
 * @tc.name: Clone
 * @tc.desc: Verify the Clone
 * @tc.type:FUNC
 * @tc.require: issueI6XQ27
 */
DEFINE_PARAM_TEST0(CharGroups, Clone, {
    {.object = defaultCgs_,    .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = emptyCgs_,      .checkFunc = GetCharGroupsEqualChecker(emptyCgs_)},
    {.object = normalCgs_,     .checkFunc = GetCharGroupsEqualChecker(normalCgs_)},
    {.object = normalSubCgs1_, .checkFunc = GetCharGroupsEqualChecker(normalSubCgs1_)},
    {.object = normalSubCgs2_, .checkFunc = GetCharGroupsEqualChecker(normalSubCgs2_)},
    {.object = glyphsCgs_,     .checkFunc = GetCharGroupsEqualChecker(glyphsCgs_)},
});

/**
 * @tc.name: Merge
 * @tc.desc: Verify the Merge
 * @tc.type:FUNC
 * @tc.require: issueI6XQ27
 */
DEFINE_VOID_PARAM_TEST1(CharGroups, Merge, CharGroups, {
    {.object = defaultCgs_,    .arg1 = normalSubCgs1_, .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = normalSubCgs1_, .arg1 = defaultCgs_,    .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalSubCgs1_, .arg1 = invalid1Cgs_,   .exception = ExceptionType::INVALID_ARGUMENT},
    {.object = normalSubCgs1_, .arg1 = normalSubCgs2_, .checkFunc = [](CharGroups &arg1, CharGroups &obj) {
        ASSERT_EQ(obj.GetRange().end, 2);
    }},
});

/**
 * @tc.name: PushBack
 * @tc.desc: Verify the PushBack
 * @tc.type:FUNC
 * @tc.require: issueI6XQ27
 */
DEFINE_VOID_PARAM_TEST1(CharGroups, PushBack, CharGroup, {
    {.object = defaultCgs_,            .arg1 = cg_, .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = emptyCgs_.Clone(),      .arg1 = cg_, .checkFunc = [](CharGroup &arg1, CharGroups &obj) {
        ASSERT_EQ(obj.GetSize(), 1);
        ASSERT_EQ(TextConverter::ToStr(obj.GetAll(0).chars), "n");
    }},
    {.object = normalCgs_.Clone(),     .arg1 = cg_, .checkFunc = [](CharGroup &arg1, CharGroups &obj) {
        ASSERT_EQ(obj.GetSize(), 5);
        ASSERT_EQ(TextConverter::ToStr(obj.GetAll(4).chars), "n");
    }},
    {.object = normalSubCgs1_.Clone(), .arg1 = cg_, .exception = ExceptionType::CUSTOM},
    {.object = normalSubCgs2_.Clone(), .arg1 = cg_, .exception = ExceptionType::CUSTOM},
    {.object = glyphsCgs_.Clone(),     .arg1 = cg_, .checkFunc = [](CharGroup &arg1, CharGroups &obj) {
        ASSERT_EQ(obj.GetSize(), 2);
        ASSERT_EQ(TextConverter::ToStr(obj.GetAll(1).chars), "n");
    }},
});

/**
 * @tc.name: ReverseAll
 * @tc.desc: Verify the ReverseAll
 * @tc.type:FUNC
 * @tc.require: issueI6XQ27
 */
DEFINE_VOID_PARAM_TEST0(CharGroups, ReverseAll, {
    {.object = defaultCgs_,            .exception = ExceptionType::INVALID_CHAR_GROUPS},
    {.object = emptyCgs_.Clone(),      .checkFunc = [](CharGroups &obj) {}},
    {.object = normalCgs_.Clone(),     .checkFunc = [](CharGroups &obj) {
        ASSERT_EQ(TextConverter::ToStr((obj.begin())->chars), "t");
        ASSERT_EQ(TextConverter::ToStr((obj.end() - 1)->chars), "m");
    }},
    {.object = normalSubCgs1_.Clone(), .exception = ExceptionType::CUSTOM},
    {.object = normalSubCgs2_.Clone(), .exception = ExceptionType::CUSTOM},
    {.object = glyphsCgs_.Clone(),     .checkFunc = [](CharGroups &obj) {
        ASSERT_EQ(TextConverter::ToStr((obj.begin())->chars), "most");
    }},
});
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
