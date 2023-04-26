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
    TextStyle style_;
    std::string textContent_;
    std::string title_;
} infos[] = {
    {
        .textContent_ = "0xff000000",
        .title_ = "default",
    },
    {
        .style_ = { .color_ = 0xffff0000 },
        .textContent_ = "0xffff0000",
        .title_ = "不透明 红色",
    },
    {
        .style_ = { .color_ = 0xff00ffff },
        .textContent_ = "0xff00ffff",
        .title_ = "不透明 绿色",
    },
    {
        .style_ = { .color_ = 0x77ff0000 },
        .textContent_ = "0x77ff0000",
        .title_ = "透明 红色",
    },
    {
        .style_ = { .color_ = 0x7700ff00 },
        .textContent_ = "0x7700ff00",
        .title_ = "透明 绿色",
    }
};

class TextColorTest : public TestFeature {
public:
    TextColorTest() : TestFeature("TextColorTest")
    {
    }

    void Layout()
    {
        for (auto &info : infos) {
            auto builder = TypographyBuilder::Create();
            builder->PushStyle(info.style_);
            builder->AppendSpan(info.textContent_);

            auto typography = builder->Build();
            typography->Layout(200);
            typographies_.push_back({
                .typography = typography
            });
        }
    }
} g_test;
} // namespace
