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

#include <texgine/typography_builder.h>
#include "feature_test_framework.h"

using namespace OHOS::Rosen::TextEngine;

constexpr const char *TEXT = "hello world1 hello world2 hello world3 hello 123";

namespace {
struct EllipsisModalInfo {
    std::string span;
    TypographyStyle tpStyle;
    TextStyle style;
    std::string title = "";
    double widthLimit = 300.0;
} g_infos[] = {
    {
        .span = TEXT,
        .title = "default",
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "maxL:1,head",
        .widthLimit = 100.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 2,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "maxL:2,head",
        .widthLimit = 150.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 3,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "maxL:3, head",
        .widthLimit = 70.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 4,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "maxL:4, head",
        .widthLimit = 60.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 7,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "maxL:7, head",
        .widthLimit = 30.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 10,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "maxL:10, head",
        .widthLimit = 30.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "maxL:1,mid",
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 2,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "maxL:2,mid",
        .widthLimit = 150.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 3,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "maxL:3,mid",
        .widthLimit = 70.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 4,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "maxL:4,mid",
        .widthLimit = 60.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 7,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "maxL:7,mid",
        .widthLimit = 30.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 10,
            .ellipsis = u"...",
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "maxL:10,mid",
        .widthLimit = 30.0,
    },
};

class EllipsisModalTest : public TestFeature {
public:
    EllipsisModalTest() : TestFeature("EllipsisModalTest")
    {
    }

    void Layout()
    {
        int loopNum = 2;
        for (int i = 0; i < loopNum; i++) {
            for (auto &info : g_infos) {
                if (i) {
                    info.tpStyle.wordBreakType = WordBreakType::BREAK_ALL;
                }
                auto builder = TypographyBuilder::Create(info.tpStyle);
                builder->PushStyle(info.style);
                builder->AppendSpan(info.span);
                auto typography = builder->Build();
                typography->Layout(info.widthLimit);
                typographies_.push_back({
                    .typography = typography,
                    .comment = info.title,
                });
            }
        }
    }
} g_test;
} // namespace
