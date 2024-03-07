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

#include "texgine/system_font_provider.h"
#include "texgine/typography_builder.h"

#include <sstream>

#include "feature_test_framework.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
constexpr auto EXAMPLE_TEXT = "你好世界 Landscape engineering. Landscape engineering. Landscape engineering. ";
struct TestInfo {
    TextStyle xs_;
    TypographyStyle ys_;
} g_datas[] = {
    { },
    { .xs_ = { .letterSpacing = -1, }, },
    { .xs_ = { .letterSpacing = 5, }, },
    { .xs_ = { .letterSpacing = 20, }, },
    { .xs_ = { .wordSpacing = -5, }, },
    { .xs_ = { .wordSpacing = 5, }, },
    { .xs_ = { .wordSpacing = 20, }, },
    {
        .xs_ = { .letterSpacing = 20, .wordSpacing = 20, },
        .ys_ = { .wordBreakType = WordBreakType::BREAK_ALL, },
    },
};

class SpacingTest : public TestFeature {
public:
    SpacingTest() : TestFeature("SpacingTest")
    {
    }

    void Layout() override
    {
        option_.needRainbowChar = true;

        for (auto &[xs, ys] : g_datas) {
            auto builder = TypographyBuilder::Create(ys);
            builder->PushStyle(xs);
            builder->AppendSpan(EXAMPLE_TEXT);
            builder->PopStyle();
            auto typography = builder->Build();
            typography->Layout(250);    // 250 means layout width

            std::stringstream ss;
            if (xs.letterSpacing) {
                ss << "LetterSpacing: " << xs.letterSpacing << " ";
            }
            if (xs.wordSpacing) {
                ss << "WordSpacing: " << xs.wordSpacing << " ";
            }
            if (ys.wordBreakType == WordBreakType::BREAK_ALL) {
                ss << "BreakAll";
            }

            typographies_.push_back({
                .typography = typography,
                .comment = ss.str(),
            });
        }
    }
} g_test;
} // namespace
