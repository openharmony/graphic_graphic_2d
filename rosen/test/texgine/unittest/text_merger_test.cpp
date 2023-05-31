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

#include <gtest/gtest.h>
#include <unicode/ubidi.h>

#include "mock/mock_any_span.h"
#include "param_test_macros.h"
#include "texgine_exception.h"
#include "text_span.h"
#include "texgine/typography_types.h"
#include "text_converter.h"
#include "text_merger.h"
#include "my_any_span.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
int g_tempCount = 0;

std::vector<int32_t> tempIdeographic = {0};
U_STABLE int32_t U_EXPORT2 u_getIntPropertyValue(UChar32 c, UProperty which)
{
    auto ret = tempIdeographic[g_tempCount];
    return ret;
}

std::vector<UBool> isWhitespace = {0};
U_STABLE UBool U_EXPORT2 u_isWhitespace(UChar32 c)
{
    auto ret = isWhitespace[g_tempCount];
    g_tempCount++;
    return ret;
}

struct TextSpanInfo {
    bool rtl = false;
    CharGroups cgs = CharGroups::CreateEmpty();
};

class ControllerForTest {
public:
    static std::shared_ptr<TextSpan> GenerateTestSpan(TextSpanInfo info)
    {
        auto ts = std::make_shared<TextSpan>();
        ts->rtl_ = info.rtl;
        ts->cgs_ = info.cgs;
        return ts;
    }

    static CharGroups GetCharGroups(const std::shared_ptr<TextSpan> &span)
    {
        return span->cgs_;
    }
};

auto GenTestSpan(TextSpanInfo info)
{
    return ControllerForTest::GenerateTestSpan(info);
}

auto InitMockArgs(std::vector<int32_t> ideographics, std::vector<UBool> whitespaces, int count = 0)
{
    return [ideographics, whitespaces, count]() {
        g_tempCount = count;
        tempIdeographic = ideographics;
        isWhitespace = whitespaces;
    };
}

auto GetMergerResultChecker(const MergeResult result, const CharGroups &spanCgs = {},
                            bool isMerged = false, IndexRange range = {})
{
    return [result, spanCgs, isMerged, range](MergeResult &&actual, const VariantSpan &span,
        std::optional<bool> &rtl, CharGroups &cgs, TextMerger &object) {
            if (result != actual) {
                return false;
            }
            if (result != MergeResult::IGNORE && result != MergeResult::REJECTED) {
                if (isMerged) {
                    return cgs.GetRange() == range;
                } else {
                    return spanCgs == cgs;
                }
            }
            return true;
    };
}

struct MergedSpanCheck {
    IndexRange range_ = {0, 0};
    bool isAnySpan_ = false;
};

auto MergedSpanChecker(const std::vector<struct MergedSpanCheck> &result)
{
    return [result](std::vector<VariantSpan> sss) {
        if (result.size() != sss.size()) {
            return false;
        }
        for (auto i = 0u; i < sss.size(); i++) {
            if (result[i].isAnySpan_) {
                if (sss[i].TryToAnySpan() == nullptr) {
                    return false;
                }
            } else {
                auto ts = sss[i].TryToTextSpan();
                if (ts == nullptr) {
                    return false;
                }
                if (!(ControllerForTest::GetCharGroups(ts).GetRange() == result[i].range_)) {
                    return false;
                }
            }
        }
        return true;
    };
}

#define PARAMCLASS TextMerger
class TextMergerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        // {0x013B, 13.664}: {glyph codepoint, glyph advanceX}
        cgs1_.PushBack({.chars = TextConverter::ToUTF16("m"), .glyphs = {{0x013B, 13.664}}});
        cgs1_.PushBack({.chars = TextConverter::ToUTF16("o"), .glyphs = {{0x0145, 9.456}}});
        cgs1_.PushBack({.chars = TextConverter::ToUTF16("s"), .glyphs = {{0x0166, 7.28}}});
        cgs1_.PushBack({.chars = TextConverter::ToUTF16("t"), .glyphs = {{0x016E, 5.88}}});
        cgs1_.PushBack({.chars = TextConverter::ToUTF16(" "), .glyphs = {{0x0002, 4.32}}});

        cgs2_.PushBack({.chars = TextConverter::ToUTF16("m"), .glyphs = {{0x013B, 13.664}}});
        cgs2_.PushBack({.chars = TextConverter::ToUTF16("o"), .glyphs = {{0x0145, 9.456}}});

        tsCgs1_ = GenTestSpan({.rtl = false, .cgs = cgs1_});
        // (1, 2): (sart, end)
        tsSubCgs12_ = GenTestSpan({.rtl = false, .cgs = cgs1_.GetSub(1, 2)});

        seqT_ = {GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(0, 1)})};

        seqTT_ = { GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(0, 1)}),
                   GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(1, 2)}) };

        seqTTST_ = { GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(0, 1)}),
                     GenTestSpan({.rtl = false, .cgs = cgs1_.GetSub(1, 5)}),
                     GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(0, 1)}) };

        seqTiTi_ = { GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(0, 1)}),
                     GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(1, 2)}) };

        seqTA_ = { GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(0, 1)}), std::make_shared<MyAnySpan>(0, 0) };

        seqTTATT_ = { GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(0, 1)}),
                      GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(1, 2)}),
                      std::make_shared<MyAnySpan>(0, 0),
                      GenTestSpan({.rtl = false, .cgs = cgs1_.GetSub(2, 3)}),
                      GenTestSpan({.rtl = false, .cgs = cgs1_.GetSub(3, 4)}) };

        seqRL_ = { GenTestSpan({.rtl = true, .cgs  = cgs1_.GetSub(0, 1)}),
                   GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(1, 2)}) };

        seqLR_ = { GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(0, 1)}),
                   GenTestSpan({.rtl = true, .cgs  = cgs1_.GetSub(1, 2)}) };

        seqRLR_ = { GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(0, 1)}),
                    GenTestSpan({.rtl = true, .cgs  = cgs1_.GetSub(1, 2)}),
                    GenTestSpan({.rtl = false, .cgs = cgs1_.GetSub(2, 3)}) };

        seqTTtt_ = { GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(0, 1)}),
                     GenTestSpan({.rtl = false, .cgs  = cgs1_.GetSub(1, 2)}),
                     GenTestSpan({.rtl = false, .cgs = cgs2_.GetSub(0, 1)}),
                     GenTestSpan({.rtl = false, .cgs = cgs2_.GetSub(1, 2)}) };
    }
    static inline std::shared_ptr<TextSpan> tsCgs1_ = nullptr;
    static inline std::shared_ptr<TextSpan> tsSubCgs12_ = nullptr;
    static inline std::vector<VariantSpan> seqT_ =  {};
    static inline std::vector<VariantSpan> seqTT_ =  {};
    static inline std::vector<VariantSpan> seqTTST_ =  {};
    static inline std::vector<VariantSpan> seqTiTi_ =  {};
    static inline std::vector<VariantSpan> seqTA_ =  {};
    static inline std::vector<VariantSpan> seqTTATT_ =  {};
    static inline std::vector<VariantSpan> seqRL_ =  {};
    static inline std::vector<VariantSpan> seqLR_ =  {};
    static inline std::vector<VariantSpan> seqRLR_ =  {};
    static inline std::vector<VariantSpan> seqTTtt_ =  {};

    static inline CharGroups cgs1_ = CharGroups::CreateEmpty();
    static inline CharGroups cgs2_ = CharGroups::CreateEmpty();
};

#define PARAMFUNC MergeSpan
/**
 * @tc.name: MergeSpan
 * @tc.desc: Verify the MergeSpan
 * @tc.type:FUNC
 */
HWTEST_F(TextMergerTest, MergeSpan, TestSize.Level1)
{
    DEFINE_ALL_TESTINFO3(VariantSpan, std::optional<bool>, CharGroups);

    std::shared_ptr<TextSpan> tsNull = nullptr;
    std::shared_ptr<MyAnySpan> asNull = nullptr;

    TextMerger tm;
    RUN_ALL_TESTINFO3(tm, {.arg1 = tsNull, .arg2 = false, .arg3 = {}, .exception = ExceptionType::INVALID_ARGUMENT });
    RUN_ALL_TESTINFO3(tm, {.arg1 = tsNull, .arg2 = false, .arg3 = {}, .exception = ExceptionType::INVALID_ARGUMENT });
    RUN_ALL_TESTINFO3(tm, {.arg1 = asNull, .arg2 = false, .arg3 = {}, .exception = ExceptionType::INVALID_ARGUMENT });
    // (0, 0): (width, height)
    RUN_ALL_TESTINFO3(tm, {.arg1 = std::make_shared<MyAnySpan>(0, 0), .arg2 = false, .arg3 = CharGroups::CreateEmpty(),
     .checkFunc = GetMergerResultChecker(MergeResult::REJECTED) });
    RUN_ALL_TESTINFO3(tm, {.arg1 = std::make_shared<MyAnySpan>(0, 0), .arg2 = false, .arg3 = {},
     .checkFunc = GetMergerResultChecker(MergeResult::IGNORE) });
    RUN_ALL_TESTINFO3(tm, {.arg1 = GenTestSpan({.rtl = true, .cgs = {}}), .arg2 = false, .arg3 = {},
     .checkFunc = GetMergerResultChecker(MergeResult::REJECTED) });
    RUN_ALL_TESTINFO3(tm, {.arg1 = GenTestSpan({.rtl = false, .cgs = {}}), .arg2 = false, .arg3 = {},
     .exception = ExceptionType::ERROR_STATUS });
    RUN_ALL_TESTINFO3(tm, {.arg1 = GenTestSpan({.rtl = false, .cgs = CharGroups::CreateEmpty()}),
     .arg2 = false, .arg3 = {}, .exception = ExceptionType::ERROR_STATUS });
    RUN_ALL_TESTINFO3(tm, {.arg1 = GenTestSpan({.rtl = false, .cgs = cgs1_}),
     .arg2 = false, .arg3 = CharGroups::CreateEmpty(), .checkFunc = GetMergerResultChecker(MergeResult::REJECTED) });
    RUN_ALL_TESTINFO3(tm, {.init = InitMockArgs({1}, {0}), .arg1 = tsCgs1_, .arg2 = false, .arg3 = {},
     .checkFunc = GetMergerResultChecker(MergeResult::ACCEPTED, ControllerForTest::GetCharGroups(tsCgs1_)) });
    RUN_ALL_TESTINFO3(tm, {.init = InitMockArgs({0}, {1}), .arg1 = tsCgs1_, .arg2 = false, .arg3 = {},
     .checkFunc = GetMergerResultChecker(MergeResult::BREAKED, ControllerForTest::GetCharGroups(tsCgs1_)) });
    RUN_ALL_TESTINFO3(tm, {.init = InitMockArgs({0}, {0}), .arg1 = tsCgs1_, .arg2 = false, .arg3 = {},
     .checkFunc = GetMergerResultChecker(MergeResult::ACCEPTED, ControllerForTest::GetCharGroups(tsCgs1_)) });
    RUN_ALL_TESTINFO3(tm, {.init = InitMockArgs({1}, {0}), .arg1 = tsSubCgs12_,
     .arg2 = false, .arg3 = cgs1_.GetSub(0, 1),
     // {0, 2}: check the IndexRange
     .checkFunc = GetMergerResultChecker(MergeResult::ACCEPTED, {}, true, {0, 2}) });
    RUN_ALL_TESTINFO3(tm, {.init = InitMockArgs({0}, {1}), .arg1 = tsSubCgs12_,
     .arg2 = false, .arg3 = cgs1_.GetSub(0, 1),
     .checkFunc = GetMergerResultChecker(MergeResult::BREAKED, {}, true, {0, 2}) });
    RUN_ALL_TESTINFO3(tm, {.init = InitMockArgs({0}, {0}), .arg1 = tsSubCgs12_,
     .arg2 = false, .arg3 = cgs1_.GetSub(0, 1),
     .checkFunc = GetMergerResultChecker(MergeResult::ACCEPTED, {}, true, {0, 2}) });
}
#undef PARAMFUNC

#define PARAMFUNC MergeSpans
/**
 * @tc.name: MergeSpans
 * @tc.desc: Verify the MergeSpans
 * @tc.type:FUNC
 */
HWTEST_F(TextMergerTest, MergeSpans, TestSize.Level1)
{
    DEFINE_TESTINFO1(std::vector<VariantSpan>);

    // A->AnySpan T/t->TextSpan S->Space L->LeftToRight R->RightToLeft
    std::vector<VariantSpan> seqA = {std::make_shared<MyAnySpan>(0, 0)};
    std::vector<VariantSpan> seqAA = {std::make_shared<MyAnySpan>(0, 0), std::make_shared<MyAnySpan>(0, 0)};
    std::shared_ptr<TextSpan> tsNull = nullptr;
    std::shared_ptr<MyAnySpan> asNull = nullptr;

    TextMerger tm;
    RUN_TESTINFO1(tm, { .arg1 = {tsNull}, .exception = ExceptionType::INVALID_ARGUMENT });
    RUN_TESTINFO1(tm, { .arg1 = {asNull}, .exception = ExceptionType::INVALID_ARGUMENT });
    RUN_TESTINFO1(tm, { .arg1 = {}, .checkFunc = CreateVecSizeChecker<VariantSpan>(0) });
    RUN_TESTINFO1(tm, { .arg1 = seqA, .checkFunc = MergedSpanChecker({{.isAnySpan_ = true}}) });
    RUN_TESTINFO1(tm, { .arg1 = seqAA, .checkFunc = MergedSpanChecker({{.isAnySpan_ = true}, {.isAnySpan_ = true}}) });
    RUN_TESTINFO1(tm, { .init = InitMockArgs({0}, {0}), .arg1 = seqT_,
     .checkFunc = MergedSpanChecker({{.range_ = {0, 1}}}) });
    RUN_TESTINFO1(tm, { .init = InitMockArgs({0, 0}, {0, 0}), .arg1 = seqTT_,
     .checkFunc = MergedSpanChecker({{.range_ = {0, 2}}}) });
    RUN_TESTINFO1(tm, { .init = InitMockArgs({0, 0, 0}, {0, 1, 0}), .arg1 = seqTTST_,
     .checkFunc = MergedSpanChecker({{.range_ = {0, 5}}, {.range_ = {0, 1}}}) });
    RUN_TESTINFO1(tm, { .init = InitMockArgs({1, 1}, {0, 0}), .arg1 = seqTiTi_,
     .checkFunc = MergedSpanChecker({{.range_ = {0, 2}}}) });
    RUN_TESTINFO1(tm, { .init = InitMockArgs({0}, {0}), .arg1 = seqTA_,
     .checkFunc = MergedSpanChecker({{.range_ = {0, 1}}, {.isAnySpan_ = true}}) });
    RUN_TESTINFO1(tm, { .init = InitMockArgs({0, 0, 0, 0}, {0, 0, 0, 0}), .arg1 = seqTTATT_,
     .checkFunc = MergedSpanChecker({{.range_ = {0, 2}}, {.isAnySpan_ = true}, {.range_ = {2, 4}}}) });
    RUN_TESTINFO1(tm, { .init = InitMockArgs({0, 0}, {0, 0}), .arg1 = seqRL_,
     .checkFunc = MergedSpanChecker({{.range_ = {0, 1}}, {.range_ = {1, 2}}}) });
    RUN_TESTINFO1(tm, { .init = InitMockArgs({0, 0}, {0, 0}), .arg1 = seqLR_,
     .checkFunc = MergedSpanChecker({{.range_ = {0, 1}}, {.range_ = {1, 2}}}) });
    RUN_TESTINFO1(tm, { .init = InitMockArgs({0, 0, 0}, {0, 0, 0}), .arg1 = seqRLR_,
     .checkFunc = MergedSpanChecker({{.range_ = {0, 1}}, {.range_ = {1, 2}}, {.range_ = {2, 3}}}) });
    RUN_TESTINFO1(tm, { .init = InitMockArgs({0, 0, 0, 0}, {0, 0, 0, 0}), .arg1 = seqTTtt_,
     .checkFunc = MergedSpanChecker({{.range_ = {0, 2}}, {.range_ = {0, 2}}}) });
}
#undef PARAMFUNC
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
