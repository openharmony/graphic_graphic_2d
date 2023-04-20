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
    {.object = invalid1Cgs_, ##__VA_ARGS__, .exception = ExceptionType::ErrorStatus}, \
    {.object = invalid2Cgs_, ##__VA_ARGS__, .exception = ExceptionType::ErrorStatus}, \
    {.object = invalid3Cgs_, ##__VA_ARGS__, .exception = ExceptionType::ErrorStatus}, \
    {.object = invalid4Cgs_, ##__VA_ARGS__, .exception = ExceptionType::ErrorStatus}, \
    {.object = invalid5Cgs_, ##__VA_ARGS__, .exception = ExceptionType::ErrorStatus}

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class CharGroupsTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        // {0x013B, 13.664} is {codepoint, advanceX}
        normalCgs_.PushBack({.chars_ = TextConverter::ToUTF16("m"), .glyphs_ = {{0x013B, 13.664}}});
        normalCgs_.PushBack({.chars_ = TextConverter::ToUTF16("o"), .glyphs_ = {{0x0145, 9.456}}});
        normalCgs_.PushBack({.chars_ = TextConverter::ToUTF16("s"), .glyphs_ = {{0x0166, 7.28}}});
        normalCgs_.PushBack({.chars_ = TextConverter::ToUTF16("t"), .glyphs_ = {{0x016E, 5.88}}});
        glyphsCgs_.PushBack({.chars_ = TextConverter::ToUTF16("most"),
            .glyphs_ = {{0x013B, 13.664}, {0x0145, 9.456}, {0x0166, 7.28}, {0x016E, 5.88}}});
        normalSubCgs1_ = normalCgs_.GetSub(0, 1);
        normalSubCgs2_ = normalCgs_.GetSub(1, 2);
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
    static inline CharGroup  cg_ = {.chars_ = TextConverter::ToUTF16("n"), .glyphs_ = {{0x013C, 13.664}}};
};

auto GetObjectRangeChecker(int t1, int t2)
{
    return [t1, t2](CharGroups &&ret) {
        ASSERT_EQ(t1, ret.GetRange().start_);
        ASSERT_EQ(t2, ret.GetRange().end_);
    };
}

auto GetIndexRangeChecker(int t1, int t2)
{
    return [t1, t2](IndexRange ret) {
        ASSERT_EQ(t1, ret.start_);
        ASSERT_EQ(t2, ret.end_);
    };
}

/**
 * @tc.name: GetNumberOfGlyph
 * @tc.desc: Verify the GetNumberOfGlyph
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, GetNumberOfGlyph, {
    INVALID_CGS(),
    {.object = defaultCgs_,  .exception = ExceptionType::InvalidCharGroups},
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
    INVALID_CGS(),
    {.object = defaultCgs_,  .exception = ExceptionType::InvalidCharGroups},
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
    INVALID_CGS(),
    {.object = defaultCgs_,  .exception = ExceptionType::InvalidCharGroups},
    {.object = emptyCgs_,    .exception = ExceptionType::OutOfRange},
    {.object = normalCgs_,   .checkFunc = [](CharGroup cg) {
        ASSERT_EQ(TextConverter::ToStr(cg.chars_), "t");
    }},
    {.object = glyphsCgs_,   .checkFunc = [](CharGroup cg) {
        ASSERT_EQ(TextConverter::ToStr(cg.chars_), "most");
    }},
});

/**
 * @tc.name: GetSize
 * @tc.desc: Verify the GetSize
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, GetSize, {
    INVALID_CGS(),
    {.object = defaultCgs_,  .exception = ExceptionType::InvalidCharGroups},
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
    INVALID_CGS(),
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
    INVALID_CGS(.arg1 = {}),
    {.object = emptyCgs_,      .arg1 = invalid3Cgs_,   .exception = ExceptionType::InvalidArgument},
    {.object = defaultCgs_,    .arg1 = defaultCgs_,    .exception = ExceptionType::InvalidCharGroups},
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
    {.object = normalCgs_,   .arg1 = -1, .exception = ExceptionType::InvalidArgument},
    INVALID_CGS(.arg1 = 1),
    {.object = defaultCgs_,  .arg1 = 1,  .exception = ExceptionType::InvalidCharGroups},
    {.object = normalCgs_,   .arg1 = 0,  .exception = ExceptionType::OutOfRange},
    {.object = normalCgs_,   .arg1 = 4,  .exception = ExceptionType::OutOfRange},
    {.object = normalCgs_,   .arg1 = 1,  .checkFunc = [](CharGroupsPair ret) {
        ASSERT_EQ(ret[0].GetRange().end_, 1);
        ASSERT_EQ(ret[1].GetRange().start_, 1);
    }},
    {.object = normalCgs_,   .arg1 = 3,  .checkFunc = [](CharGroupsPair ret) {
        ASSERT_EQ(ret[0].GetRange().end_, 3);
        ASSERT_EQ(ret[1].GetRange().start_, 3);
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
    INVALID_CGS(.arg1 = 1),
    {.object = defaultCgs_,  .arg1 = 1, .exception = ExceptionType::InvalidCharGroups},
    {.object = normalCgs_,   .arg1 = 0, .exception = ExceptionType::OutOfRange},
    {.object = normalCgs_,   .arg1 = 4, .exception = ExceptionType::OutOfRange},
    {.object = normalCgs_,   .arg1 = 1, .checkFunc = [](CharGroupsPair ret) {
        ASSERT_EQ(ret[0].GetRange().end_, 1);
        ASSERT_EQ(ret[1].GetRange().start_, 1);
    }},
    {.object = normalCgs_,   .arg1 = 3, .checkFunc = [](CharGroupsPair ret) {
        ASSERT_EQ(ret[0].GetRange().end_, 3);
        ASSERT_EQ(ret[1].GetRange().start_, 3);
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
    {.object = normalCgs_,     .arg1 = -1, .arg2 = 2, .exception = ExceptionType::InvalidArgument},
    {.object = normalCgs_,     .arg1 = 2,  .arg2 = 1, .exception = ExceptionType::InvalidArgument},
    {.object = normalSubCgs2_, .arg1 = -1, .arg2 = 1, .exception = ExceptionType::InvalidArgument},
    {.object = normalCgs_,     .arg1 = 2,  .arg2 = 6, .exception = ExceptionType::InvalidArgument},
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
    INVALID_CGS(.arg1 = 0, .arg2 = 0),
    {.object = defaultCgs_,  .arg1 = 0,  .arg2 = 0, .exception = ExceptionType::InvalidCharGroups},
    {.object = normalCgs_,   .arg1 = -1, .arg2 = 2, .exception = ExceptionType::InvalidArgument},
    {.object = normalCgs_,   .arg1 = 2,  .arg2 = 1, .exception = ExceptionType::InvalidArgument},
    {.object = normalCgs_,   .arg1 = 1,  .arg2 = 9, .exception = ExceptionType::InvalidArgument},
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
    INVALID_CGS(.arg1 = 1, .arg2 = 2),
    {.object = defaultCgs_,  .arg1 = 1,  .arg2 = 2,  .exception = ExceptionType::InvalidCharGroups},
    {.object = normalCgs_,   .arg1 = -1, .arg2 = 2,  .exception = ExceptionType::InvalidArgument},
    {.object = normalCgs_,   .arg1 = 2,  .arg2 = 1,  .exception = ExceptionType::InvalidArgument},
    {.object = normalCgs_,   .arg1 = 10, .arg2 = 20, .exception = ExceptionType::InvalidArgument},
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
    INVALID_CGS(.arg1 = {}),
    {.object = normalCgs_,     .arg1 = invalid1Cgs_,   .exception = ExceptionType::InvalidArgument},
    {.object = defaultCgs_,    .arg1 = normalCgs_,     .exception = ExceptionType::InvalidCharGroups},
    {.object = normalCgs_,     .arg1 = defaultCgs_,    .exception = ExceptionType::InvalidArgument},
    {.object = normalSubCgs1_, .arg1 = normalSubCgs2_, .exception = ExceptionType::Custom},
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
    {.object = normalSubCgs2_, .arg1 = -1, .exception = ExceptionType::InvalidArgument},
    {.object = normalSubCgs1_, .arg1 = 0,
     .checkFunc = [](struct CharGroup ret) {
         ASSERT_EQ(TextConverter::ToStr(ret.chars_), "m");
    }},
    {.object = normalSubCgs2_, .arg1 = 0,
     .checkFunc = [](struct CharGroup ret) {
         ASSERT_EQ(TextConverter::ToStr(ret.chars_), "o");
    }},
});

/**
 * @tc.name: GetAll
 * @tc.desc: Verify the GetAll
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST1(CharGroups, GetAll, int32_t, {
    INVALID_CGS(.arg1 = 0),
    {.object = defaultCgs_,    .arg1 = 0,  .exception = ExceptionType::InvalidCharGroups},
    {.object = normalCgs_,     .arg1 = -1, .exception = ExceptionType::OutOfRange},
    {.object = normalCgs_,     .arg1 = 4,  .exception = ExceptionType::OutOfRange},
    {.object = normalCgs_,     .arg1 = 0,
     .checkFunc = [](struct CharGroup ret) {
         ASSERT_EQ(TextConverter::ToStr(ret.chars_), "m");
    }},
    {.object = normalSubCgs2_, .arg1 = 0,
     .checkFunc = [](struct CharGroup ret) {
         ASSERT_EQ(TextConverter::ToStr(ret.chars_), "m");
    }},
});

/**
 * @tc.name: ToUTF16
 * @tc.desc: Verify the ToUTF16
 * @tc.type:FUNC
 * @tc.require: issueI6TIIF
 */
DEFINE_PARAM_TEST0(CharGroups, ToUTF16, {
    INVALID_CGS(),
    {.object = defaultCgs_,    .exception = ExceptionType::InvalidCharGroups},
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
    INVALID_CGS(),
    {.object = defaultCgs_,    .exception = ExceptionType::InvalidCharGroups},
    {.object = emptyCgs_,      .checkFunc = GetVecSizeChecker<uint16_t>(0)},
    {.object = normalCgs_,     .checkFunc = GetVecSizeChecker<uint16_t>(4)},
    {.object = normalSubCgs1_, .checkFunc = GetVecSizeChecker<uint16_t>(4)},
    {.object = normalSubCgs2_, .checkFunc = GetVecSizeChecker<uint16_t>(4)},
    {.object = glyphsCgs_,     .checkFunc = GetVecSizeChecker<uint16_t>(4)},
});
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
