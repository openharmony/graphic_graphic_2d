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

#include "texgine/any_span.h"
#include "texgine/system_font_provider.h"
#include "texgine/typography_builder.h"

#include "feature_test_framework.h"
#include "my_any_span.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
constexpr auto EXAMPLE_TEXT = "hello world, hello openharmony!";
constexpr auto EXAMPLE_TEXT2 = "你 好 世 界 你好鸿蒙！";

struct IntrinsicTestData {
    TypographyStyle ys;
    std::string title;
    double widthLimit = 150;
    const char *text = EXAMPLE_TEXT;
    std::shared_ptr<MyAnySpan> as = std::make_shared<MyAnySpan>(20, 80);
} g_datas[] = {
    {
        .ys = {
            .ellipsis = u"",
        },
        .title = "不换行的对照组",
        .widthLimit = 700,
    },
    {
        .ys = {
            .ellipsis = u"",
            .wordBreakType = WordBreakType::BREAK_WORD,
        },
        .title = "breakWord",
    },
    {
        .ys = {
            .ellipsis = u"",
            .wordBreakType = WordBreakType::BREAK_ALL,
        },
        .title = "breakAll",
    },
    {
        .ys = {
            .maxLines = 1,
        },
        .title = "maxline = 1",
    },
    {
        .ys = {
            .ellipsis = u"",
            .wordBreakType = WordBreakType::BREAK_ALL,
        },
        .title = "breakAll，中文",
        .widthLimit = 120,
        .text = EXAMPLE_TEXT2,
    },
};

class IntrinsicWidthTest : public TestFeature {
public:
    IntrinsicWidthTest() : TestFeature("IntrinsicWidthTest")
    {
    }

    void Layout()
    {
        for (auto &[ys, title, limit, text, as] : g_datas) {
            auto builder = TypographyBuilder::Create(ys);
            builder->PushStyle({});
            builder->AppendSpan(text);
            builder->AppendSpan(as);
            builder->PopStyle();

            auto typography = builder->Build();
            typography->Layout(limit);
            auto onPaint = [&](const struct TypographyData &tyData, TexgineCanvas &canvas, double x, double y) {
                // typography
                tyData.typography->Paint(canvas, x, y);

                TexginePaint paint;
                paint.SetAntiAlias(true);
                paint.SetStyle(TexginePaint::FILL);

                // max
                paint.SetColor(0x5500FF00);
                auto rect1 = TexgineRect::MakeXYWH(x, y, tyData.typography->GetMaxIntrinsicWidth(), 10);
                canvas.DrawRect(rect1, paint);

                // min
                paint.SetColor(0x5500FFCC);
                auto rect2 = TexgineRect::MakeXYWH(x, y + 10, tyData.typography->GetMinIntrinsicWidth(), 10);
                canvas.DrawRect(rect2, paint);

                // placeholder
                auto rects = tyData.typography->GetTextRectsOfPlaceholders();
                for (auto &[rect, _] : rects) {
                    rect.GetRect()->Offset(x, y);
                    canvas.DrawRect(rect, paint);
                }
            };

            typographies_.push_back({
                .typography = typography,
                .comment = title,
                .atNewline = true,
                .onPaint = onPaint,
            });
        }
    }
} g_test;
} // namespace
