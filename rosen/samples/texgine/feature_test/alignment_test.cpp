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
#include <sstream>

#include <texgine/any_span.h>
#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"
#include "my_any_span.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
constexpr const char *TEXT = "Hi 世界";

struct AlignmentTestData {
    AnySpanAlignment alignment;
    double offset = 0;
    TextBaseline baseline = TextBaseline::ALPHABETIC;
} g_testDatas[] = {
    { .alignment = AnySpanAlignment::ABOVE_BASELINE },
    { .alignment = AnySpanAlignment::ABOVE_BASELINE, .baseline = TextBaseline::IDEOGRAPHIC },
    { .alignment = AnySpanAlignment::OFFSET_AT_BASELINE },
    { .alignment = AnySpanAlignment::OFFSET_AT_BASELINE, .offset = 40 },
    { .alignment = AnySpanAlignment::BELOW_BASELINE },
    { .alignment = AnySpanAlignment::TOP_OF_ROW_BOX },
    { .alignment = AnySpanAlignment::CENTER_OF_ROW_BOX },
    { .alignment = AnySpanAlignment::BOTTOM_OF_ROW_BOX },
};

AnySpanAlignment g_onelineAlignments[] = {
    AnySpanAlignment::TOP_OF_ROW_BOX,
    AnySpanAlignment::TOP_OF_ROW_BOX,
    AnySpanAlignment::CENTER_OF_ROW_BOX,
    AnySpanAlignment::BOTTOM_OF_ROW_BOX,
    AnySpanAlignment::OFFSET_AT_BASELINE,
    AnySpanAlignment::ABOVE_BASELINE,
    AnySpanAlignment::BELOW_BASELINE,
};

std::map<AnySpanAlignment, std::string> g_alignmentToString = {
    {AnySpanAlignment::ABOVE_BASELINE, "AB"},
    {AnySpanAlignment::BELOW_BASELINE, "BB"},
    {AnySpanAlignment::OFFSET_AT_BASELINE, "BL"},
    {AnySpanAlignment::TOP_OF_ROW_BOX, "TO"},
    {AnySpanAlignment::BOTTOM_OF_ROW_BOX, "BO"},
    {AnySpanAlignment::CENTER_OF_ROW_BOX, "MI"},
};

std::map<TextBaseline, std::string> g_baselineToString = {
    {TextBaseline::ALPHABETIC, "Alpha"},
    {TextBaseline::IDEOGRAPHIC, "Ideog"},
};

class AlignmentTest : public TestFeature {
public:
    AlignmentTest() : TestFeature("AlignmentTest") {}

    void Layout() override
    {
        option_.needRainbowChar = true;
        TextStyle textStyle = {
            .fontSize = 20,
            .decoration = TextDecoration::BASELINE,
        };

        for (const auto &data : g_testDatas) {
            std::vector<double> heights = {20, 40, 60};
            double width = 20.0;
            for (const auto &height : heights) {
                auto builder = TypographyBuilder::Create();
                builder->PushStyle(textStyle);
                builder->AppendSpan(TEXT);

                auto anySpan = std::make_shared<MyAnySpan>(width,
                    height, data.alignment, data.baseline, data.offset);
                builder->AppendSpan(anySpan);

                auto typography = builder->Build();
                double widthLimit = 150.0;
                typography->Layout(widthLimit);

                std::stringstream ss;
                ss << g_alignmentToString[data.alignment];
                if (data.alignment == AnySpanAlignment::OFFSET_AT_BASELINE) {
                    ss << "(" << data.offset << ")";
                }
                ss << " " << g_baselineToString[data.baseline];
                ss << " " << height << "px";

                typographies_.push_back({
                    .typography = typography,
                    .comment = ss.str(),
                    .atNewline = (height == *heights.begin()),
                });
            }
        }

        LayoutOneLine(textStyle);
    }

    void LayoutOneLine(const TextStyle &textStyle)
    {
        auto builder = TypographyBuilder::Create();
        std::string title = "单行多个不同对齐方式";
        double anySpanWidth = 20.0;
        double anySpanHeight = 30.0;
        for (const auto &alignment : g_onelineAlignments) {
            auto anySpan = std::make_shared<MyAnySpan>(anySpanWidth, anySpanHeight, alignment);
            builder->AppendSpan(anySpan);
            builder->PushStyle(textStyle);
            builder->AppendSpan(TEXT);
            title += g_alignmentToString[alignment] + " ";
        }

        auto typography = builder->Build();
        double widthLimit = 800.0;
        typography->Layout(widthLimit);
        typographies_.push_back({
            .typography = typography,
            .comment = title,
        });
    }
} g_test;
} // namespace
