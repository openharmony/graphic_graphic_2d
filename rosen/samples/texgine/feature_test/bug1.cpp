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
class Bug1 : public TestFeature {
public:
    Bug1() : TestFeature("Bug1")
    {
    }

    void Layout() override
    {
        TypographyStyle ys = { .maxLines = 1, .ellipsis = u"\u2026" };

        auto builder = TypographyBuilder::Create(ys);
        TextStyle tstyle;
        builder->PushStyle(tstyle);
        builder->AppendSpan("相机");
        auto typography = builder->Build();
        double widthLimit = 111.0;
        typography->Layout(widthLimit);

        auto actualWidth = typography->GetActualWidth();
        typography->Layout(actualWidth);

        typographies_.push_back({ .typography = typography });
    }
} g_test;
} // namespace
