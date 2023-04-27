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

#include <texgine/any_span.h>
#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>
#include <texgine/utils/memory_usage_scope.h>

#include "feature_test_framework.h"
#include "my_any_span.h"

using namespace OHOS::Rosen::TextEngine;

namespace {
struct Res {
    std::string textContent = "";
    uint32_t color = 0x00000000;
};

std::vector<Res> g_shortRes = {
    {
        .textContent = "提示：十翅+堡堡分享桶=？",
        .color = 0xffffff00,
    },
};

std::vector<Res> g_longRes = {
    {
        .textContent = "【损友阿乐的投喂】",
        .color = 0xff00ff00,
    },
    {
        .textContent = "休息天也要和朋友们一起云享受生活！损友阿乐拎着美味出现了！来猜今天阿乐带来什么美味套餐？",
        .color = 0xffffff00,
    },
    {
        .textContent = "提示：十翅+堡堡分享桶=？",
        .color = 0xffff00ff,
    },
    {
        .textContent = "答案：十翅堡堡分享桶，疫情期间注意防护哦",
        .color = 0xff0000ff,
    },
    {
        .textContent = "【损友阿乐的投喂】",
        .color = 0xff00ff00,
    },
    {
        .textContent = "休息天也要和朋友们一起云享受生活！损友阿乐拎着美味出现了！来猜今天阿乐带来什么美味套餐？",
        .color = 0xffffff00,
    },
    {
        .textContent = "提示：十翅+堡堡分享桶=？",
        .color = 0xff0000ff,
    },
    {
        .textContent = "答案：十翅堡堡分享桶，疫情期间注意防护哦",
        .color = 0xff0000ff,
    }
};

std::vector<Res> g_manyImageRes = {
    { .textContent = "同行多个", },
    { .color = 0xffff0000, },
    { .color = 0xffffff00, },
    { .color = 0xff0000ff, },
    { .color = 0xff00ff00, },
    { .textContent = "同行多个", },
    { .color = 0xffff0000, },
    { .color = 0xffffff00, },
    { .color = 0xff0000ff, },
    { .color = 0xff00ff00, },
};

struct Info {
    std::vector<Res> res;
    TextStyle textStyle = {};
    double imageHeight = 20.0;
    TypographyStyle tpStyle;
    TextRectHeightStyle heightStyle = TextRectHeightStyle::TIGHT;
    TextRectWidthStyle widthStyle = TextRectWidthStyle::TIGHT;
    int widthLimit = 500;
    std::string title = "";
    bool rect = false;
} g_infos[] = {
    {
        .res = g_shortRes,
        .title = "default",
    },
    {
        .res = g_shortRes,
        .textStyle = { .fontSize = 32, },
        .imageHeight = 32,
        .title = "fontSize: 32, imageHeight: 32",
    },
    {
        .res = g_longRes,
        .imageHeight = 16,
        .title = "多行显示",
    },
    {
        .res = g_manyImageRes,
        .imageHeight = 16,
        .title = "同行多个",
    },
    {
        .res = g_longRes,
        .imageHeight = 16,
        .tpStyle = { .maxLines_ = 2 },
        .title = "maxLines: 2",
    },
    {
        .res = g_longRes,
        .imageHeight = 16,
        .tpStyle = { .maxLines = 2, .ellipsis = u"***" },
        .title = "maxLines: 2, ellipsis: '***'",
    },
    {
        .res = g_shortRes,
        .tpStyle = {
            .lineStyle = { .only = true },
        },
        .title = "RectHeightStyle: Tight, RectWidthStyle: Tight",
        .rect = true,
    },
    {
        .res = g_shortRes,
        .tpStyle = {
            .lineStyle = { .only = true },
        },
        .heightStyle = TextRectHeightStyle::COVER_TOP_AND_BOTTOM,
        .title = "RectHeightStyle: MaxHeight, RectWidthStyle: Tight",
        .rect = true,
    },
    {
        .res = g_longRes,
        .imageHeight = 16,
        .tpStyle = {
            .lineStyle = { .only = true },
        },
        .title = "RectHeightStyle: Tight, RectWidthStyle: Tight",
        .rect = true,
    },
    {
        .res = g_longRes,
        .imageHeight = 16,
        .tpStyle = {
            .lineStyle = { .only = true },
        },
        .widthStyle = TextRectWidthStyle::MAX_WIDTH,
        .title = "RectHeightStyle: Tight, RectWidthStyle: MaxWidth",
        .rect = true,
    }
};

class MixTest : public TestFeature {
public:
    MixTest() : TestFeature("MixTest")
    {
    }

    void Layout() override
    {
        for (auto &info : g_infos) {
            auto builder = TypographyBuilder::Create(info.tpStyle);

            for (auto &res : info.res) {
                if (res.color != 0x00000000) {
                    double imageWidth = 20.0;
                    builder->AppendSpan(std::make_shared<MyAnySpan>(
                        imageWidth, info.imageHeight,
                        AnySpanAlignment::ABOVE_BASELINE,
                        TextBaseline::ALPHABETIC,
                        0, res.color));
                }
                if (!res.textContent.empty()) {
                    builder->PushStyle(info.textStyle);
                    builder->AppendSpan(res.textContent);
                }
            }

            auto typography = builder->Build();
            typography->Layout(info.widthLimit);
            typographies_.push_back({
                .typography = typography,
                .needRainbowChar = info.rect,
            });
        }
    }
} g_test;
} // namespace
