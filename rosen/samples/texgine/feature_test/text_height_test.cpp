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

#include <texgine/typography_builder.h>

#include "feature_test_framework.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
struct TextHeightTestData {
    std::string text;
    TextStyle style;
} g_datas[] = {
    {
        .text = "Default text height.",
        .style = {
            .heightOnly = true,
        },
    },
    {
        .text = "Text height test1.",
        .style = {
            .heightOnly = true,
            .fontSize = 10.0, // 10.0 means font size
            .heightScale = 1.0, // 1.0 means height scale value
        },
    },
    {
        .text = "Text height test2.",
        .style = {
            .heightOnly = true,
            .fontSize = 10.0, // 10.0 means font size
            .heightScale = 1.5, // 1.5 means height scale value
        },
    },
    {
        .text = "Text height test3.",
        .style = {
            .heightOnly = true,
            .fontSize = 10.0, // 10.0 means font size
            .heightScale = 2.0, // 2.0 means height scale value
        },
    },
    {
        .text = "Text height test4.",
        .style = {
            .heightOnly = true,
            .fontSize = 15.0, // 15.0 means font size
            .heightScale = 2.0, // 2.0 means height scale value
        },
    },
    {
        .text = "Text height test5.",
        .style = {
            .heightOnly = true,
            .fontSize = 20.0, // 20.0 means font size
            .heightScale = 2.0, // 2.0 means height scale value
        },
    },
    {
        .text = "Text height test6.",
        .style = {
            .heightOnly = true,
            .fontSize = 20.0, // 20.0 means font size
            .heightScale = 2.5, // 2.5 means height scale value
        },
    },
};

class TextHeightTest : public TestFeature {
public:
    TextHeightTest() : TestFeature("TextHeightTest")
    {
    }

    void Layout()
    {
        for (auto &[text, style] : g_datas) {
            auto builder = TypographyBuilder::Create();
            builder->PushStyle(style);
            builder->AppendSpan(text);
            auto typography = builder->Build();
            typography->Layout(300); // 300 means width limit
            typographies_.push_back({
                .typography = typography,
            });
        }
    }
} g_test;
} // namespace
