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
struct TestInfo {
    std::string title;
    std::string text = "华为鸿蒙系统是一款全新的面向全场景的分布式操作系统，创造一个超级虚拟终端互联的世界。";
    TextStyle style;
} g_testInfos[] = {
    {
        .title = "default",
        .style = {
        }
    },
    {
        .title = "italic",
        .style = {
            .fontStyle = FontStyle::ITALIC,
        }
    },
    {
        .title = "W100 thin",
        .style = {
            .fontWeight = FontWeight::W100,
        }
    },
    {
        .title = "W200",
        .style = {
            .fontWeight = FontWeight::W200,
        }
    },
    {
        .title = "W300",
        .style = {
            .fontWeight = FontWeight::W300,
        }
    },
    {
        .title = "W400 normal",
        .style = {
            .fontWeight = FontWeight::W400,
        }
    },
    {
        .title = "W500",
        .style = {
            .fontWeight = FontWeight::W500,
        }
    },
    {
        .title = "W600",
        .style = {
            .fontWeight = FontWeight::W600,
        }
    },
    {
        .title = "W700 bold",
        .style = {
            .fontWeight = FontWeight::W700,
        }
    },
    {
        .title = "W800",
        .style = {
            .fontWeight = FontWeight::W800,
        }
    },
    {
        .title = "W900",
        .style = {
            .fontWeight = FontWeight::W900,
        }
    },
    {
        .title = "W700 bold",
        .style = {
            .fontWeight = FontWeight::W700,
            .fontStyle = FontStyle::ITALIC,
        }
    },
};

class WeightStyleTest : public TestFeature {
public:
    WeightStyleTest() : TestFeature("WeightStyleTest")
    {
    }

    void Layout() override
    {
        for (auto &[title, text, style] : g_testInfos) {
            auto builder = TypographyBuilder::Create();
            builder->PushStyle(style);
            builder->AppendSpan(text);

            auto typography = builder->Build();
            typography->Layout(200);    // 200 means layout width
            typographies_.push_back({
                .typography = typography,
                .comment = title,
            });
        }
    }
} g_test;
} // namespace
