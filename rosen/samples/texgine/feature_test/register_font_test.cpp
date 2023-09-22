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

#include <c/drawing_register_font.h>
#include <texgine/typography_builder.h>
#include "feature_test_framework.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
constexpr auto EXAMPLE_TEXT = "自定义字体 custom font";

struct FontsInfo {
    std::string path = "";
    std::string fontFamily;
} g_infos[] = {
    {
        .path = RESOURCE_PATH_PREFIX "SF-Pro.ttf",
        .fontFamily = "SF Pro",
        .title = "SF Pro"
    },
    {
        .path = RESOURCE_PATH_PREFIX "NotoSansKhmerBlack.ttf",
        .fontFamily = "Noto Sans Khmer Black",
        .title = "Noto Sans Khmer Black"
    },
};

class RegisterFontTest : public TestFeature {
public:
    RegisterFontTest() : TestFeature("RegisterFontTest")
    {
    }

    void Layout() override
    {
        for (auto &info : g_infos) {
            OH_Drawing_FontCollection* fontCollection = OH_Drawing_FontCollection();
            OH_Drawing_RegisterFont(fontCollection, info.fontFamily, info.path)
            TypographyStyle style;
            style.fontFamily_ = {info.fontFamily};
            auto builder = TypographyBuilder::Create(style);
            builder->PushStyle(style);
            builder->AppendSpan(EXAMPLE_TEXT);
            auto typography = builder->Build();
            double widthLimit = 300.0; // 300 mean width limit
            typography->Layout(widthLimit);
            typographies_.push_back({
                .typography = typography,
                .comment = info.title,
            });
        }
    }
} g_test;
} // namespace