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

#include <utility>

#include <gtest/gtest.h>

#include "line_breaker.h"
#include "mock/mock_any_span.h"
#include "my_any_span.h"
#include "param_test_macros.h"
#include "text_converter.h"
#include "text_span.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::vector<ScoredSpan> GenScoredSpansByBreaks(const std::vector<std::pair<double, double>> &breaks)
{
    std::vector<ScoredSpan> sss;
    for (const auto &breakPair : breaks) {
        sss.push_back({.preBreak = breakPair.first, .postBreak = breakPair.second});
    }
    return sss;
}

struct ScoredSpanCheck {
    double preBreak = 0;
    double postBreak = 0;
    bool isAnySpan = false;
};

auto ScoredSpansChecker(const std::vector<struct ScoredSpanCheck> &cls)
{
    return [cls](std::vector<struct ScoredSpan> sss) {
        ASSERT_EQ(cls.size(), sss.size());
        for (auto i = 0u; i < sss.size(); i++) {
            ASSERT_EQ(sss[i].preBreak, cls[i].preBreak);
            ASSERT_EQ(sss[i].postBreak, cls[i].postBreak);
            if (cls[i].isAnySpan) {
                ASSERT_NE(sss[i].span.TryToAnySpan(), nullptr);
            } else {
                ASSERT_NE(sss[i].span.TryToTextSpan(), nullptr);
            }
        }
    };
}

auto DoBreakLinesChecker(const std::vector<int> &arg)
{
    return [arg](std::vector<struct ScoredSpan> &arg1,
        double &arg2, TypographyStyle &arg3, std::vector<float> &arg4, LineBreaker &object) {
        if (arg.size() != arg1.size()) {
            return false;
        }
        for (auto i = 0u; i < arg1.size(); i++) {
            if (arg1[i].prev != arg[i]) {
                return false;
            }
        }
        return true;
    };
}

// Setting the flag of break lines
std::vector<ScoredSpan> GenScoredSpansByPrevs(const std::vector<int> &prevs)
{
    std::vector<ScoredSpan> sss;
    for (const auto &prev : prevs) {
        sss.push_back({.prev = prev});
    }
    return sss;
}

auto LineMetricsSizesChecker(const std::vector<size_t> &sizes)
{
    return [sizes](std::vector<LineMetrics> &&lineMetrics) {
        if (lineMetrics.size() != sizes.size()) {
            return false;
        }
        for (auto i = 0u; i < lineMetrics.size(); i++) {
            if (lineMetrics[i].lineSpans.size() != sizes[i]) {
                return false;
            }
        }
        return true;
    };
}

class ControllerForTest {
public:
    struct TextSpanInfo {
        double preBreak = 0;
        double postBreak = 0;
        CharGroups cgs_ = {};
    };

    static std::shared_ptr<TextSpan> GenerateTestSpan(const TextSpanInfo &info)
    {
        auto ts = std::make_shared<TextSpan>();
        ts->preBreak_ = info.preBreak;
        ts->postBreak_ = info.postBreak;
        ts->cgs_ = info.cgs_;
        return ts;
    }
};

#define PARAMCLASS LineBreaker
class LineBreakerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        CharGroups cgs1 = CharGroups::CreateEmpty();
        // {0x013B, 13.664}: {glyph codepont, davanceX}
        cgs1.PushBack({.chars = TextConverter::ToUTF16("m"), .glyphs = {{0x013B, 13.664}}});
        cgs1.PushBack({.chars = TextConverter::ToUTF16("o"), .glyphs = {{0x0145, 9.456}}});
        cgs1.PushBack({.chars = TextConverter::ToUTF16("s"), .glyphs = {{0x0166, 7.28}}});
        cgs1.PushBack({.chars = TextConverter::ToUTF16("t"), .glyphs = {{0x016E, 5.88}}});
        ts10_ = ControllerForTest::GenerateTestSpan({.preBreak = 0, .postBreak = 10, .cgs_ = cgs1});

        CharGroups cgs2 = CharGroups::CreateEmpty();
        cgs2.PushBack({.chars = TextConverter::ToUTF16("m"), .glyphs = {{0x013B, 13.664}}});
        ts20_ = ControllerForTest::GenerateTestSpan({.preBreak = 20, .postBreak = 20, .cgs_ = cgs2});

        ts11_ = ControllerForTest::GenerateTestSpan({.preBreak = 10, .postBreak = 10, .cgs_ = cgs1.GetSub(0, 1)});
        ts12_ = ControllerForTest::GenerateTestSpan({.preBreak = 10, .postBreak = 20, .cgs_ = cgs1.GetSub(1, 2)});

        gStyle_.breakStrategy = BreakStrategy::GREEDY;
        hStyle_.breakStrategy = BreakStrategy::HIGH_QUALITY;
    }

    static inline std::shared_ptr<TextSpan> ts10_ = nullptr;
    static inline std::shared_ptr<TextSpan> ts20_ = nullptr;
    static inline std::shared_ptr<TextSpan> ts11_ = nullptr;
    static inline std::shared_ptr<TextSpan> ts12_ = nullptr;
    static inline TypographyStyle gStyle_;
    static inline TypographyStyle hStyle_;
};

//(10, 0): (width, height)
auto as10 = std::make_shared<MyAnySpan>(10, 0);
auto as20 = std::make_shared<MyAnySpan>(20, 0);
std::shared_ptr<AnySpan> asNullptr = nullptr;
std::shared_ptr<TextSpan> tsNullptr = nullptr;
// {0, 10}: {preBreak, postBreak}
auto longChecker1 = ScoredSpansChecker({{0, 10, false}, {20, 20, true}, {40, 40, false}, {60, 60, true}});
auto longChecker2 = ScoredSpansChecker(
    {{0, 10, false}, {20, 20, true}, {30, 30, false}, {30, 40, false}, {60, 60, true}});
DEFINE_PARAM_TEST1(LineBreaker, GenerateScoreSpans, std::vector<VariantSpan>, {
    { .arg1 = {tsNullptr}, .exception = ExceptionType::INVALID_ARGUMENT },
    { .arg1 = {asNullptr}, .exception = ExceptionType::INVALID_ARGUMENT },
    { .arg1 = {ts10_}, .checkFunc = ScoredSpansChecker({{0, 10, false}}) },
    { .arg1 = {as10}, .checkFunc = ScoredSpansChecker({{10, 10, true}}) },
    { .arg1 = {ts10_, as10, ts20_, as20}, .checkFunc = longChecker1 },
    { .arg1 = {ts10_, as10, ts11_, ts12_, as20}, .checkFunc = longChecker2 },
});

// {100, 100}: {preBreak, postBreak}
auto ss1 = GenScoredSpansByBreaks({{100, 100}, {200, 200}, {300, 300}});
auto ss2 = GenScoredSpansByBreaks({{90, 90}, {100, 100}, {180, 180}});
auto ss3 = GenScoredSpansByBreaks({{90, 90}, {100, 1100}, {1180, 1180}});

// These vectors is the flag of break lines
std::vector<int> prevs000 = {0, 0, 0};
std::vector<int> prevs001 = {0, 0, 1};
std::vector<int> prevs002 = {0, 0, 2};
std::vector<int> prevs012 = {0, 1, 2};

/**
 * @tc.name: DoBreakLines
 * @tc.desc: Verify the DoBreakLines
 * @tc.type:FUNC
 */
#define PARAMFUNC DoBreakLines
HWTEST_F(LineBreakerTest, DoBreakLines, TestSize.Level1) {
    DEFINE_VOID_TESTINFO4(std::vector<struct ScoredSpan>, double, TypographyStyle, std::vector<float>);
    LineBreaker breaker;
    for (auto i = 0; i < static_cast<int>(ss1.size()); i++) {
        ss1[i].span = ts11_;
        ss2[i].span = ts11_;
        ss3[i].span = ts11_;
    }

    // arg1~3 is the parameters of DoBreakLines
    RUN_VOID_TESTINFO4(breaker, { .arg1 = ss1, .arg2 = 100, .arg3 = gStyle_, .arg4 = {},
        .checkFunc = DoBreakLinesChecker(prevs012) });
    RUN_VOID_TESTINFO4(breaker, { .arg1 = ss1, .arg2 = 1e9, .arg3 = gStyle_, .arg4 = {},
        .checkFunc = DoBreakLinesChecker(prevs000) });
    RUN_VOID_TESTINFO4(breaker, { .arg1 = ss1, .arg2 = 1e9, .arg3 = hStyle_, .arg4 = {},
        .checkFunc = DoBreakLinesChecker(prevs000) });
    RUN_VOID_TESTINFO4(breaker, { .arg1 = ss2, .arg2 = 100, .arg3 = gStyle_, .arg4 = {},
        .checkFunc = DoBreakLinesChecker(prevs002) });
    RUN_VOID_TESTINFO4(breaker, { .arg1 = ss2, .arg2 = 100, .arg3 = hStyle_, .arg4 = {},
        .checkFunc = DoBreakLinesChecker(prevs001) });
    RUN_VOID_TESTINFO4(breaker, { .arg1 = ss3, .arg2 = 100, .arg3 = gStyle_, .arg4 = {},
        .checkFunc = DoBreakLinesChecker(prevs002) });
    RUN_VOID_TESTINFO4(breaker, { .arg1 = ss3, .arg2 = 100, .arg3 = hStyle_, .arg4 = {},
        .checkFunc = DoBreakLinesChecker(prevs002) });
}
#undef PARAMFUNC

/**
 * @tc.name: GenerateBreaks
 * @tc.desc: Verify the GenerateBreaks
 * @tc.type:FUNC
 */
DEFINE_PARAM_TEST2(LineBreaker, GenerateBreaks, std::vector<VariantSpan>, std::vector<ScoredSpan>, {
    { .arg1 = {ts10_}, .arg2 = GenScoredSpansByPrevs({0, 2}), .exception = ExceptionType::ERROR_STATUS },
    { .arg1 = {ts10_}, .arg2 = GenScoredSpansByPrevs({0, 3, 2}), .exception = ExceptionType::ERROR_STATUS },
    { .arg1 = {ts10_}, .arg2 = GenScoredSpansByPrevs({0, 2, 5}), .exception = ExceptionType::ERROR_STATUS },
    { .arg1 = {ts10_}, .arg2 = GenScoredSpansByPrevs({0, 0, 0, 2, 2, 4, 4, 4, 7}),
      .checkFunc = GetResultChecker(std::vector<int>{2, 4, 7, 9}) },
});

/**
 * @tc.name: GenerateLineMetrics
 * @tc.desc: Verify the GenerateLineMetrics
 * @tc.type:FUNC
 */
#define PARAMFUNC GenerateLineMetrics
HWTEST_F(LineBreakerTest, GenerateLineMetrics, TestSize.Level1)
{
    DEFINE_TESTINFO4(double, std::vector<VariantSpan>, std::vector<int32_t>, std::vector<float>);
    LineBreaker breaker;
    RUN_TESTINFO4(breaker, { .arg1 = 100.0, .arg2 = {3, VariantSpan{}}, .arg3 = {2, 10}, .arg4 = {},
        .exception = ExceptionType::OUT_OF_RANGE });
}
#undef PARAMFUNC
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
