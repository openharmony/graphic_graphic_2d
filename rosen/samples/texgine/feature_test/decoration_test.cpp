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
struct StyledText {
    std::string text;
    TextStyle style;
} g_texts[] = {
    {
        .text = "Decoration对照",
        .style = {},
    },
    {
        .text = "Decoration上划线",
        .style = {
            .decoration = TextDecoration::OVERLINE,
        },
    },
    {
        .text = "Decoration删除线",
        .style = {
            .decoration = TextDecoration::LINE_THROUGH,
        },
    },
    {
        .text = "Decoration下划线",
        .style = {
            .decoration = TextDecoration::UNDERLINE,
        },
    },
    {
        .text = "Decoration两两混合1",
        .style = {
            .decoration = TextDecoration::OVERLINE |
                TextDecoration::LINE_THROUGH,
        },
    },
    {
        .text = "Decoration两两混合2",
        .style = {
            .decoration = TextDecoration::OVERLINE |
                TextDecoration::UNDERLINE,
        },
    },
    {
        .text = "Decoration两两混合3",
        .style = {
            .decoration = TextDecoration::LINE_THROUGH |
                TextDecoration::UNDERLINE,
        },
    },
    {
        .text = "Decoration全混合",
        .style = {
            .decoration = TextDecoration::OVERLINE |
                TextDecoration::LINE_THROUGH |
                TextDecoration::UNDERLINE,
        },
    },
    {
        .text = "Decoration实下滑线",
        .style = {
            .decoration = TextDecoration::UNDERLINE,
            .decorationStyle = TextDecorationStyle::SOLID,
        },
    },
    {
        .text = "Decoration虚下滑线",
        .style = {
            .decoration = TextDecoration::UNDERLINE,
            .decorationStyle = TextDecorationStyle::DASHED,
        },
    },
    {
        .text = "Decoration点下滑线",
        .style = {
            .decoration = TextDecoration::UNDERLINE,
            .decorationStyle = TextDecorationStyle::DOTTED,
        },
    },
    {
        .text = "Decoration波浪下滑线",
        .style = {
            .decoration = TextDecoration::UNDERLINE,
            .decorationStyle = TextDecorationStyle::WAVY,
        },
    },
    {
        .text = "Decoration双实下滑线",
        .style = {
            .decoration = TextDecoration::UNDERLINE,
            .decorationStyle = TextDecorationStyle::DOUBLE,
        },
    },
    {
        .text = "Decoration粗下滑线",
        .style = {
            .decoration = TextDecoration::UNDERLINE,
            .decorationThicknessScale = 5.0f,
        },
    },
    {
        .text = "Decoration红下划线",
        .style = {
            .decoration = TextDecoration::UNDERLINE,
            .decorationColor = 0xffff0000,
        },
    },
};

class DecorationTest : public TestFeature {
public:
    DecorationTest() : TestFeature("DecorationTest")
    {
    }

    void Layout() override
    {
        option_.needBorder = false;

        TypographyStyle tStyle;
        for (auto &[text, style] : g_texts) {
            auto builder = TypographyBuilder::Create(tStyle);
            builder->PushStyle(style);
            builder->AppendSpan(text);
            auto typography = builder->Build();
            double widthLimit = 200.0;
            typography->Layout(widthLimit);
            typographies_.push_back({
                .typography = typography,
            });
        }
    }
} g_test;
} // namespace
