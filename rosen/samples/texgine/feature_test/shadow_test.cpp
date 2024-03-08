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
struct ShadowTestData {
    std::string text;
    TextStyle style;
} g_datas[] = {
    {
        .text = "阴影对照.",
        .style = {},
    },
    {
        .text = "阴影数量1.",
        .style = {
            .shadows = { { .offsetX = 4, .offsetY = 4, }, },
        },
    },
    {
        .text = "阴影数量4.",
        .style = {
            .shadows = {
                { .offsetX = +4, .offsetY = +4, },
                { .offsetX = +4, .offsetY = -4, },
                { .offsetX = -4, .offsetY = +4, },
                { .offsetX = -4, .offsetY = -4, },
            },
        },
    },
    {
        .text = "阴影X偏移量0.",
        .style = { .shadows = { { .offsetX = 0, }, }, },
    },
    {
        .text = "阴影X偏移量15.",
        .style = { .shadows = { { .offsetX = 15, }, }, },
    },
    {
        .text = "阴影X偏移量-15.",
        .style = { .shadows = { { .offsetX = -15, }, }, },
    },
    {
        .text = "阴影Y偏移量0.",
        .style = { .shadows = { { .offsetY = 0, }, }, },
    },
    {
        .text = "阴影Y偏移量15.",
        .style = { .shadows = { { .offsetY = 15, }, }, },
    },
    {
        .text = "阴影Y偏移量-15.",
        .style = { .shadows = { { .offsetY = -15, }, }, },
    },
    {
        .text = "阴影模糊半径0.",
        .style = {
            .shadows = {
                { .offsetX = 2, .offsetY = 2, .color = SK_ColorBLACK, .blurLeave = 0, },
            },
        },
    },
    {
        .text = "阴影模糊半径2.",
        .style = {
            .shadows = {
                { .offsetX = 2, .offsetY = 2, .color = SK_ColorBLACK, .blurLeave = 2, },
            },
        },
    },
    {
        .text = "阴影模糊半径8.",
        .style = {
            .shadows = {
                { .offsetX = 2, .offsetY = 2, .color = SK_ColorBLACK, .blurLeave = 8, },
            },
        },
    },
    {
        .text = "阴影模糊半径32.",
        .style = {
            .shadows = {
                { .offsetX = 2, .offsetY = 2, .color = SK_ColorBLACK, .blurLeave = 32, },
            },
        },
    },
    {
        .text = "阴影颜色红色.",
        .style = {
            .shadows = {
                { .offsetX = 2, .offsetY = 2, .color = SK_ColorRED, },
            },
        },
    },
};

class ShadowTest : public TestFeature {
public:
    ShadowTest() : TestFeature("ShadowTest")
    {
    }

    void Layout() override
    {
        for (auto &[text, style] : g_datas) {
            auto builder = TypographyBuilder::Create();
            builder->PushStyle(style);
            builder->AppendSpan(text);
            auto typography = builder->Build();
            typography->Layout(300);    // 300 means layout width
            typographies_.push_back({
                .typography = typography,
            });
        }
    }
} g_test;
} // namespace
