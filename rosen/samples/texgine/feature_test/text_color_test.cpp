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
    TextStyle style;
    std::string textContent;
    std::string title;
} g_infos[] = {
    {
        .textContent = "0xff000000",
        .title = "default",
    },
    {
        .style = { .color = 0xffff0000 },
        .textContent = "0xffff0000",
        .title = "不透明 红色",
    },
    {
        .style = { .color = 0xff00ffff },
        .textContent = "0xff00ffff",
        .title = "不透明 绿色",
    },
    {
        .style = { .color = 0x77ff0000 },
        .textContent = "0x77ff0000",
        .title = "透明 红色",
    },
    {
        .style = { .color = 0x7700ff00 },
        .textContent = "0x7700ff00",
        .title = "透明 绿色",
    }
};

class TextColorTest : public TestFeature {
public:
    TextColorTest() : TestFeature("TextColorTest")
    {
    }

    void Layout() override
    {
        for (auto &info : g_infos) {
            auto builder = TypographyBuilder::Create();
            builder->PushStyle(info.style);
            builder->AppendSpan(info.textContent);

            auto typography = builder->Build();
            typography->Layout(200);    // 200 means layout width
            typographies_.push_back({
                .typography = typography
            });
        }
    }
} g_test;
} // namespace
