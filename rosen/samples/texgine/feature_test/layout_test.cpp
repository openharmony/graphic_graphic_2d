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

#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
#define EXAMPLE 5

#if EXAMPLE == 0
#elif EXAMPLE == 1
constexpr auto EXAMPLE_TEXT = "Typography is the art and technique of arranging type to make written "
    "language legible, readable and appealing when displayed. The "
    "arrangement of type involves selecting typefaces, point sizes, "
    "line lengths, line-spacing (leading), and letter-spacing "
    "(tracking), as well as adjusting the space between pairs of "
    "letters (kerning).";
constexpr auto EXAMPLE_TEXT_HALF = "Typography is the art and technique of arranging type to make written "
    "language legible, readable and appealing when displayed.";
#elif EXAMPLE == 2
constexpr auto EXAMPLE_TEXT = "مرحباً،";
constexpr auto EXAMPLE_TEXT_HALF = "";
#elif EXAMPLE == 3
constexpr auto EXAMPLE_TEXT = "مرحباً، العالم";
constexpr auto EXAMPLE_TEXT_HALF = "";
#elif EXAMPLE == 4
constexpr auto EXAMPLE_TEXT = "مرحباً، العالم جاهز";
constexpr auto EXAMPLE_TEXT_HALF = "";
#elif EXAMPLE == 5
constexpr auto EXAMPLE_TEXT = "12345678 مرحباً، العالم جاهز "
    "22345678 مرحباً، العالم جاهز "
    "32345678 مرحباً، العالم جاهز "
    "42345678 مرحباً، العالم جاهز "
    "52345678 مرحباً، العالم جاهز "
    "62345678 مرحباً، العالم جاهز "
    "72345678 مرحباً، العالم جاهز ";
constexpr auto EXAMPLE_TEXT_HALF = "12345678 مرحباً، العالم جاهز "
    "22345678 مرحباً، العالم جاهز "
    "32345678 مرحباً، العالم جاهز ";
#elif EXAMPLE == 6
constexpr auto EXAMPLE_TEXT = "مرحباً، العالم جاهز 1234567890";
constexpr auto EXAMPLE_TEXT_HALF = "مرحباً،";
#endif

class LayoutTest : public TestFeature {
public:
    LayoutTest() : TestFeature("LayoutTest")
    {
    }

    void Layout() override
    {
        option_.needRainbowChar = true;

        TypographyStyle ys = {
            .ellipsis = u"",
            .breakStrategy = BreakStrategy::GREEDY,
            .wordBreakType = WordBreakType::BREAKALL,
        };
        auto builder = TypographyBuilder::Create(ys);

        TextStyle style = {
            .fontSize = 16,
        };

        builder->PushStyle(style);
        builder->AppendSpan(EXAMPLE_TEXT_HALF);
        builder->PopStyle();

        TextStyle style2 = {
            .fontSize = 24,
        };

        builder->PushStyle(style2);
        builder->AppendSpan((EXAMPLE_TEXT + strlen(EXAMPLE_TEXT_HALF)));
        builder->PopStyle();

        auto typography = builder->Build();
        double widthLimit = 300.0;
        typography->Layout(widthLimit);
        typographies_.push_back({
            .typography = typography
        });
    }
} g_test;
} // namespace
