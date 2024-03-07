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

#include <sstream>

#include <texgine/dynamic_file_font_provider.h>
#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"
#include "text_define.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
constexpr auto EXAMPLE_TEXT = "hello" WATCH PERSON " wolrd تحول " WOMAN_SPORT_VARIANT
    " كبير! hello 123 آخر الأخبار من محطة سكة حديد شيان هنا!" WATCH;

class DirectionTest : public TestFeature {
public:
    DirectionTest() : TestFeature("DirectionTest")
    {
    }

    void Layout() override
    {
        for (const auto &dir : {TextDirection::LTR, TextDirection::RTL}) {
            TypographyStyle tystyle = {
                .direction = dir,
            };

            auto dfProvider = DynamicFileFontProvider::Create();
            dfProvider->LoadFont("Segoe UI Emoji", RESOURCE_PATH_PREFIX "seguiemj.ttf");
            auto fps = FontProviders::Create();
            fps->AppendFontProvider(dfProvider);
            fps->AppendFontProvider(SystemFontProvider::GetInstance());
            auto builder = TypographyBuilder::Create(tystyle, std::move(fps));

            TextStyle style = {
                .fontFamilies = {"Segoe UI Emoji"},
                .fontSize = 24,
            };

            builder->PushStyle(style);
            builder->AppendSpan(EXAMPLE_TEXT);
            builder->PopStyle();

            std::stringstream ss;
            ss << "typography direction: " << (dir == TextDirection::LTR ? "LTR" : "RTL");

            auto typography = builder->Build();
            double widthLimit = 600.0;
            typography->Layout(widthLimit);
            typographies_.push_back({
                .typography = typography,
                .comment = ss.str(),
            });
        }
    }
} g_test;
} // namespace
