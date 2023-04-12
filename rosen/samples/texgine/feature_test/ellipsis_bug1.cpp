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
constexpr auto exampleText = "宽度限制过小导致省略号填充的bug";

struct WidthInfo {
    double widthLimit_ = 0;
    std::string title_ = "";
} infos[] = {
    {
        .widthLimit_ = 400,
        .title_ = "宽度限制可以容纳整个文本",
    },
    {
        .widthLimit_ = 50,
        .title_ = "容纳部分文本",
    },
    {
        .widthLimit_ = 5,
        .title_ = "小于一个字符的宽度",
    },
};

class EllipsisBug1 : public TestFeature {
public:
    EllipsisBug1() : TestFeature("EllipsisBug1")
    {
    }

    void Layout()
    {
        for (auto &info : infos) {
            TypographyStyle ys = { .maxLines_ = 1, .ellipsis_ = u"..." };

            auto builder = TypographyBuilder::Create(ys);
            TextStyle tstyle;
            builder->PushStyle(tstyle);
            builder->AppendSpan(exampleText);
            auto typography = builder->Build();
            typography->Layout(info.widthLimit_);

            typographies_.push_back({
                .typography = typography,
                .comment = info.title_,
            });
        }
    }
} g_test;
} // namespace
