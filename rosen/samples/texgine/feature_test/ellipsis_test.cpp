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

constexpr const char *TEXT = "hello world hello world hello world hello 123";

namespace {
struct EllipsisInfo {
    std::string span;
    TypographyStyle tpStyle;
    TextStyle style;
    std::string title = "";
} g_infos[] = {
    {
        .span = TEXT,
        .title = "default",
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
        },
        .title = "maxline: 1, ellipse: default",
    },
    {
        .span = TEXT,
        .tpStyle = {
            .ellipsis = u"",
        },
        .title = "maxline: defult, ellipse: \"\"",
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = u"",
        },
        .title = "maxline: 1, ellipse: \"\"",
    },
};

class EllipsisTest : public TestFeature {
public:
    EllipsisTest() : TestFeature("EllipsisTest")
    {
    }

    void Layout() override
    {
        for (auto &info : g_infos) {
            auto builder = TypographyBuilder::Create(info.tpStyle);
            builder->PushStyle(info.style);
            builder->AppendSpan(info.span);
            auto typography = builder->Build();
            double widthLimit = 300.0;
            typography->Layout(widthLimit);
            typographies_.push_back({
                .typography = typography,
                .comment = info.title,
            });
        }
    }
} g_test;
} // namespace
