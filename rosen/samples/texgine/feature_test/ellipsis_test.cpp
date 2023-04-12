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

constexpr const char *text = "hello world hello world hello world hello 123";

namespace {
struct EllipsisInfo {
    std::string span_;
    TypographyStyle tpStyle_;
    TextStyle style_;
    std::string title_ = "";
} infos[] = {
    {
        .span_ = text,
        .title_ = "default",
    },
    {
        .span_ = text,
        .tpStyle_ = {
            .maxLines_ = 1,
        },
        .title_ = "maxline: 1, ellipse: default",
    },
    {
        .span_ = text,
        .tpStyle_ = {
            .ellipsis_ = u"",
        },
        .title_ = "maxline: defult, ellipse: \"\"",
    },
    {
        .span_ = text,
        .tpStyle_ = {
            .maxLines_ = 1,
            .ellipsis_ = u"",
        },
        .title_ = "maxline: 1, ellipse: \"\"",
    },
};

class EllipsisTest : public TestFeature {
public:
    EllipsisTest() : TestFeature("EllipsisTest")
    {
    }

    void Layout()
    {
        for (auto &info : infos) {
            auto builder = TypographyBuilder::Create(info.tpStyle_);
            builder->PushStyle(info.style_);
            builder->AppendSpan(info.span_);
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
