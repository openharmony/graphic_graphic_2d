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
} infos[] = {
    {
        .span = " hello",
        .style = {
            .fontSize_ = 16,
            .decoration_ = TextDecoration::BASELINE,
        },
    },
    {
        .span = " world",
        .style = {
            .fontSize_ = 32,
            .decoration_ = TextDecoration::BASELINE,
        },
    },
    {
        .span = " hello",
        .style = {
            .fontSize_ = 64,
            .decoration_ = TextDecoration::BASELINE,
        },
    },
    {
        .span = " world",
        .style = {
            .fontSize_ = 32,
            .decoration_ = TextDecoration::BASELINE,
        },
    },
     {
        .span = " HELLO",
        .style = {
            .fontSize_ = 16,
            .decoration_ = TextDecoration::BASELINE,
        },
    },
};

struct StyleData {
    TextRectHeightStyle heightStyle;
    TextRectWidthStyle widthStyle;
    std::string title;
} styleDatas [] = {
    {
        .heightStyle = TextRectHeightStyle::TIGHT,
        .widthStyle = TextRectWidthStyle::TIGHT,
        .title = "字体大小不同，最终的布局显示基线位置相同",
    },
    {
        .heightStyle = TextRectHeightStyle::COVER_TOP_AND_BOTTOM,
        .widthStyle = TextRectWidthStyle::TIGHT,
        .title = "复现MaxHeight现象",
    }
};

class CalibratePositionTest : public TestFeature {
public:
    CalibratePositionTest() : TestFeature("CalibratePositionTest")
    {
    }

    void Layout()
    {
        option_.needRainbowChar = true;

        auto builder = TypographyBuilder::Create();
        for (const auto &[span, style] : infos) {
            builder->PushStyle(style);
            builder->AppendSpan(span);
            builder->PopStyle();
        }

        auto typography = builder->Build();
        typography->Layout(300);

        for (const auto &style : styleDatas) {
            typographies_.push_back({
                .typography = typography,
                .comment = style.title,
                .ws = style.widthStyle,
                .hs = style.heightStyle,
            });
        }
    }
} g_test;
} // namespace
