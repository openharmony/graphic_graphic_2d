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
struct Info {
    std::string span;
    TextStyle style;
} g_infos[] = {
    {
        .span = " hello",
        .style = {
            .fontSize = 16,
            .decoration = TextDecoration::BASELINE,
        },
    },
    {
        .span = " world",
        .style = {
            .fontSize = 32,
            .decoration = TextDecoration::BASELINE,
        },
    },
    {
        .span = " hello",
        .style = {
            .fontSize = 64,
            .decoration = TextDecoration::BASELINE,
        },
    },
    {
        .span = " world",
        .style = {
            .fontSize = 32,
            .decoration = TextDecoration::BASELINE,
        },
    },
    {
        .span = " HELLO",
        .style = {
            .fontSize = 16,
            .decoration = TextDecoration::BASELINE,
        },
    },
};

struct StyleData {
    TextRectHeightStyle heightstyle;
    TextRectWidthStyle widthstyle;
    std::string title;
} g_styleDatas [] = {
    {
        .heightstyle = TextRectHeightStyle::TIGHT,
        .widthstyle = TextRectWidthStyle::TIGHT,
        .title = "字体大小不同，最终的布局显示基线位置相同",
    },
    {
        .heightstyle = TextRectHeightStyle::COVER_TOP_AND_BOTTOM,
        .widthstyle = TextRectWidthStyle::TIGHT,
        .title = "复现MaxHeight现象",
    }
};

class CalibratePositionTest : public TestFeature {
public:
    CalibratePositionTest() : TestFeature("CalibratePositionTest")
    {
    }

    void Layout() override
    {
        option_.needRainbowChar = true;

        auto builder = TypographyBuilder::Create();
        for (const auto &[span, style] : g_infos) {
            builder->PushStyle(style);
            builder->AppendSpan(span);
            builder->PopStyle();
        }

        auto typography = builder->Build();
        double widthLimit = 300.0;
        typography->Layout(widthLimit);

        for (const auto &style : g_styleDatas) {
            typographies_.push_back({
                .typography = typography,
                .comment = style.title,
                .ws = style.widthstyle,
                .hs = style.heightstyle,
            });
        }
    }
} g_test;
} // namespace
