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
constexpr auto EXAMPLE_TEXT = "    Landscape engineering quality management, the most likely "
    "problem is not clear responsible person, responsible unit, resulting "
    "in problems again and again, therefore, should establish a strict quality "
    "management responsibility system and norms, through the system to ensure the quality "
    "of     Landscape engineering construction.";

struct AlignTestData {
    std::string title;
    TypographyStyle tStyle;
} g_datas[] = {
    {
        .title = "Start对齐布局",
        .tStyle = {
            .align = TextAlign::START,
        },
    },
    {
        .title = "左对齐布局",
        .tStyle = {
            .align = TextAlign::LEFT,
        },
    },
    {
        .title = "右对齐布局",
        .tStyle = {
            .align = TextAlign::RIGHT,
        },
    },
    {
        .title = "居中对齐布局",
        .tStyle = {
            .align = TextAlign::CENTER,
        },
    },
    {
        .title = "End对齐布局",
        .tStyle = {
            .align = TextAlign::END,
        },
    },
};

class AlignTest : public TestFeature {
public:
    AlignTest() : TestFeature("AlignTest") {}

    void Layout() override
    {
        TexginePaint background;
        uint32_t color = 0xff00ff00;
        double strokeWidth = 2.0;
        background.SetColor(color);
        background.SetStrokeWidth(strokeWidth);
        background.SetStyle(TexginePaint::STROKE);
        TextStyle style = {
            .fontSize = 16,
            .background = background,
        };

        for (auto &[title, tstyle] : g_datas) {
            for (auto dir : {TextDirection::LTR, TextDirection::RTL}) {
                tstyle.direction = dir;
                auto builder = TypographyBuilder::Create(tstyle);
                builder->PushStyle(style);
                builder->AppendSpan(EXAMPLE_TEXT);
                auto typography = builder->Build();
                double widthLimit = 325.0;
                typography->Layout(widthLimit);
                typographies_.push_back({
                    .typography = typography,
                    .comment = (dir == TextDirection::LTR ? "LTR " : "RTL ") + title,
                });
            }
        }
    }
} g_test;
} // namespace
