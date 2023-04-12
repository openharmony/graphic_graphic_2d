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
} texts[] = {
    {
        .text = "Decoration对照",
        .style = {},
    },
    {
        .text = "Decoration上划线",
        .style = {
            .decoration_ = TextDecoration::OVERLINE,
        },
    },
    {
        .text = "Decoration删除线",
        .style = {
            .decoration_ = TextDecoration::LINETHROUGH,
        },
    },
    {
        .text = "Decoration下划线",
        .style = {
            .decoration_ = TextDecoration::UNDERLINE,
        },
    },
    {
        .text = "Decoration两两混合1",
        .style = {
            .decoration_ = TextDecoration::OVERLINE |
                TextDecoration::LINETHROUGH,
        },
    },
    {
        .text = "Decoration两两混合2",
        .style = {
            .decoration_ = TextDecoration::OVERLINE |
                TextDecoration::UNDERLINE,
        },
    },
    {
        .text = "Decoration两两混合3",
        .style = {
            .decoration_ = TextDecoration::LINETHROUGH |
                TextDecoration::UNDERLINE,
        },
    },
    {
        .text = "Decoration全混合",
        .style = {
            .decoration_ = TextDecoration::OVERLINE |
                TextDecoration::LINETHROUGH |
                TextDecoration::UNDERLINE,
        },
    },
    {
        .text = "Decoration实下滑线",
        .style = {
            .decoration_ = TextDecoration::UNDERLINE,
            .decorationStyle_ = TextDecorationStyle::SOLID,
        },
    },
    {
        .text = "Decoration虚下滑线",
        .style = {
            .decoration_ = TextDecoration::UNDERLINE,
            .decorationStyle_ = TextDecorationStyle::DASHED,
        },
    },
    {
        .text = "Decoration点下滑线",
        .style = {
            .decoration_ = TextDecoration::UNDERLINE,
            .decorationStyle_ = TextDecorationStyle::DOTTED,
        },
    },
    {
        .text = "Decoration波浪下滑线",
        .style = {
            .decoration_ = TextDecoration::UNDERLINE,
            .decorationStyle_ = TextDecorationStyle::WAVY,
        },
    },
    {
        .text = "Decoration双实下滑线",
        .style = {
            .decoration_ = TextDecoration::UNDERLINE,
            .decorationStyle_ = TextDecorationStyle::DOUBLE,
        },
    },
    {
        .text = "Decoration粗下滑线",
        .style = {
            .decoration_ = TextDecoration::UNDERLINE,
            .decorationThicknessScale_ = 5.0f,
        },
    },
    {
        .text = "Decoration红下划线",
        .style = {
            .decoration_ = TextDecoration::UNDERLINE,
            .decorationColor_ = 0xffff0000,
        },
    },
};

class DecorationTest : public TestFeature {
public:
    DecorationTest() : TestFeature("DecorationTest")
    {
    }

    void Layout()
    {
        option_.needBorder = false;

        TypographyStyle tstyle;
        for (auto &[text, style] : texts) {
            auto builder = TypographyBuilder::Create(tstyle);
            builder->PushStyle(style);
            builder->AppendSpan(text);
            auto typography = builder->Build();
            typography->Layout(200);
            typographies_.push_back({
                .typography = typography,
            });
        }
    }
} g_test;
} // namespace
