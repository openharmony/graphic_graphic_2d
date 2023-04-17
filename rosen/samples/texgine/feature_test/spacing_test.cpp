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
    { .xs_ = { .letterSpacing_ = -1, }, },
    { .xs_ = { .letterSpacing_ = 5, }, },
    { .xs_ = { .letterSpacing_ = 20, }, },
    { .xs_ = { .wordSpacing_ = -5, }, },
    { .xs_ = { .wordSpacing_ = 5, }, },
    { .xs_ = { .wordSpacing_ = 20, }, },
    {
        .xs_ = { .letterSpacing_ = 20, .wordSpacing_ = 20, },
        .ys_ = { .wordBreakType_ = WordBreakType::BREAKALL, },
    },
};

class SpacingTest : public TestFeature {
public:
    SpacingTest() : TestFeature("SpacingTest")
    {
    }

    void Layout()
    {
        option_.needRainbowChar = true;

        for (auto &[xs, ys] : g_datas) {
            auto builder = TypographyBuilder::Create(ys);
            builder->PushStyle(xs);
            builder->AppendSpan(EXAMPLE_TEXT);
            builder->PopStyle();
            auto typography = builder->Build();
            typography->Layout(250);

            std::stringstream ss;
            if (xs.letterSpacing_) {
                ss << "LetterSpacing: " << xs.letterSpacing_ << " ";
            }
            if (xs.wordSpacing_) {
                ss << "WordSpacing: " << xs.wordSpacing_ << " ";
            }
            if (ys.wordBreakType_ == WordBreakType::BREAKALL) {
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
