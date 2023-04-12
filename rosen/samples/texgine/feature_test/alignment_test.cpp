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

#include "texgine/any_span.h"

#include <sstream>

#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"
#include "my_any_span.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
constexpr const char *text = "Hi 世界";

struct AlignmentTestData {
    AnySpanAlignment alignment_;
    double offset_ = 0;
    TextBaseline baseline_ = TextBaseline::ALPHABETIC;
} testDatas[] = {
    { .alignment_ = AnySpanAlignment::ABOVE_BASELINE },
    { .alignment_ = AnySpanAlignment::ABOVE_BASELINE, .baseline_ = TextBaseline::IDEOGRAPHIC },
    { .alignment_ = AnySpanAlignment::OFFSET_AT_BASELINE },
    { .alignment_ = AnySpanAlignment::OFFSET_AT_BASELINE, .offset_ = 40 },
    { .alignment_ = AnySpanAlignment::BELOW_BASELINE },
    { .alignment_ = AnySpanAlignment::TOP_OF_ROW_BOX },
    { .alignment_ = AnySpanAlignment::CENTER_OF_ROW_BOX },
    { .alignment_ = AnySpanAlignment::BOTTOM_OF_ROW_BOX },
};

AnySpanAlignment onelineAlignments[] = {
    AnySpanAlignment::TOP_OF_ROW_BOX,
    AnySpanAlignment::TOP_OF_ROW_BOX,
    AnySpanAlignment::CENTER_OF_ROW_BOX,
    AnySpanAlignment::BOTTOM_OF_ROW_BOX,
    AnySpanAlignment::OFFSET_AT_BASELINE,
    AnySpanAlignment::ABOVE_BASELINE,
    AnySpanAlignment::BELOW_BASELINE,
};

std::map<AnySpanAlignment, std::string> alignmentToString = {
    {AnySpanAlignment::ABOVE_BASELINE, "AB"},
    {AnySpanAlignment::BELOW_BASELINE, "BB"},
    {AnySpanAlignment::OFFSET_AT_BASELINE, "BL"},
    {AnySpanAlignment::TOP_OF_ROW_BOX, "TO"},
    {AnySpanAlignment::BOTTOM_OF_ROW_BOX, "BO"},
    {AnySpanAlignment::CENTER_OF_ROW_BOX, "MI"},
};

std::map<TextBaseline, std::string> baselineToString = {
    {TextBaseline::ALPHABETIC, "Alpha"},
    {TextBaseline::IDEOGRAPHIC, "Ideog"},
};

class AlignmentTest : public TestFeature {
public:
    AlignmentTest() : TestFeature("AlignmentTest")
    {
    }

    void Layout()
    {
        option_.needRainbowChar = true;

        TextStyle textStyle = {
            .fontSize_ = 20,
            .decoration_ = TextDecoration::BASELINE,
        };

        for (const auto &data : testDatas) {
            auto heights = {20, 40, 60};
            for (const auto &height : heights) {
                auto builder = TypographyBuilder::Create();
                builder->PushStyle(textStyle);
                builder->AppendSpan(text);

                auto anySpan = std::make_shared<MyAnySpan>(20,
                    height, data.alignment_, data.baseline_, data.offset_);
                builder->AppendSpan(anySpan);

                auto typography = builder->Build();
                typography->Layout(150);

                std::stringstream ss;
                ss << alignmentToString[data.alignment_];
                if (data.alignment_ == AnySpanAlignment::OFFSET_AT_BASELINE) {
                    ss << "(" << data.offset_ << ")";
                }
                ss << " " << baselineToString[data.baseline_];
                ss << " " << height << "px";

                typographies_.push_back({
                    .typography = typography,
                    .comment = ss.str(),
                    .atNewline = (height == *heights.begin()),
                });
            }
        }

        auto builder = TypographyBuilder::Create();
        std::string title = "单行多个不同对齐方式";
        for (const auto &alignment : onelineAlignments) {
            auto anySpan = std::make_shared<MyAnySpan>(20, 30, alignment);
            builder->AppendSpan(anySpan);
            builder->PushStyle(textStyle);
            builder->AppendSpan(text);
            title += alignmentToString[alignment] + " ";
        }

        auto typography = builder->Build();
        typography->Layout(800);
        typographies_.push_back({
            .typography = typography,
            .comment = title,
        });
    }
} g_test;
} // namespace
