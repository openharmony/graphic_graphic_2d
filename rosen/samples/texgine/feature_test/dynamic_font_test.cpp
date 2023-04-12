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

#include <texgine/dynamic_file_font_provider.h>
#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
constexpr auto exampleText = "Hello World 1234";

struct FontsInfo {
    std::string path_ = "";
    std::string fontFamily_;
    std::string title_ = "";
} infos[] = {
    {
        .title_ = "default",
    },
    {
        .path_ = RESOURCE_PATH_PREFIX "Roboto-Black.ttf",
        .fontFamily_ = "Roboto",
        .title_ = "Roboto",
    },
    {
        .path_ = RESOURCE_PATH_PREFIX "NotExists.ttf",
        .fontFamily_ = "NotExists",
        .title_ = "NotExists",
    },
};

class DynamicFontTest : public TestFeature {
public:
    DynamicFontTest() : TestFeature("DynamicFontTest")
    {
    }

    void Layout()
    {
        for (auto &info : infos) {
            TypographyStyle tystyle;
            auto dfprovider = DynamicFileFontProvider::Create();
            dfprovider->LoadFont(info.fontFamily_, info.path_);
            auto fps = FontProviders::Create();
            fps->AppendFontProvider(dfprovider);
            fps->AppendFontProvider(SystemFontProvider::GetInstance());
            auto builder = TypographyBuilder::Create(tystyle, std::move(fps));

            TextStyle style;
            style.fontFamilies_ = {info.fontFamily_};
            builder->PushStyle(style);
            builder->AppendSpan(exampleText);
            builder->PopStyle();

            auto typography = builder->Build();
            typography->Layout(300);
            typographies_.push_back({
                .typography = typography,
                .comment = info.title_,
            });
        }
    }
} g_test;
} // namespace
