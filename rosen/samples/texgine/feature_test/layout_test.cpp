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
constexpr auto exampleText = "Typography is the art and technique of arranging type to make written "
                              "language legible, readable and appealing when displayed. The "
                              "arrangement of type involves selecting typefaces, point sizes, "
                              "line lengths, line-spacing (leading), and letter-spacing "
                              "(tracking), as well as adjusting the space between pairs of "
                              "letters (kerning).";
constexpr auto exampleTextHalf = "Typography is the art and technique of arranging type to make written "
                                  "language legible, readable and appealing when displayed.";
#elif EXAMPLE == 2
constexpr auto exampleText = "مرحباً،";
constexpr auto exampleTextHalf = "";
#elif EXAMPLE == 3
constexpr auto exampleText = "مرحباً، العالم";
constexpr auto exampleTextHalf = "";
#elif EXAMPLE == 4
constexpr auto exampleText = "مرحباً، العالم جاهز";
constexpr auto exampleTextHalf = "";
#elif EXAMPLE == 5
constexpr auto exampleText = "12345678 مرحباً، العالم جاهز "
                             "22345678 مرحباً، العالم جاهز "
                             "32345678 مرحباً، العالم جاهز "
                             "42345678 مرحباً، العالم جاهز "
                             "52345678 مرحباً، العالم جاهز "
                             "62345678 مرحباً، العالم جاهز "
                             "72345678 مرحباً، العالم جاهز ";
constexpr auto exampleTextHalf = "12345678 مرحباً، العالم جاهز "
                                 "22345678 مرحباً، العالم جاهز "
                                 "32345678 مرحباً، العالم جاهز ";
#elif EXAMPLE == 6
constexpr auto exampleText = "مرحباً، العالم جاهز 1234567890";
constexpr auto exampleTextHalf = "مرحباً،";
#endif

class LayoutTest : public TestFeature {
public:
    LayoutTest() : TestFeature("LayoutTest")
    {
    }

    void Layout()
    {
        option_.needRainbowChar = true;

        TypographyStyle ys = {
            .ellipsis_ = u"",
            .breakStrategy_ = BreakStrategy::GREEDY,
            .wordBreakType_ = WordBreakType::BREAKALL,
        };
        auto builder = TypographyBuilder::Create(ys);

        TextStyle style = {
            .fontSize_ = 16,
        };

        builder->PushStyle(style);
        builder->AppendSpan(exampleTextHalf);
        builder->PopStyle();

        TextStyle style2 = {
            .fontSize_ = 24,
        };

        builder->PushStyle(style2);
        builder->AppendSpan((exampleText + strlen(exampleTextHalf)));
        builder->PopStyle();

        auto typography = builder->Build();
        typography->Layout(300);
        typographies_.push_back({
            .typography = typography
        });
    }
} g_test;
} // namespace
