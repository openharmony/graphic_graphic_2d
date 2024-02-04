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
constexpr const char *TEXT1 = "this is a test text from OpenHarmony operation system";
constexpr const char *TEXT2 = "honorificabilitudinitatibus";

namespace {
// default ellipsis \u2026 corresponds to ...
const std::u16string ELLIPSIS = u"\u2026";
struct EllipsisModalInfo {
    std::string span;
    TypographyStyle tpStyle;
    TextStyle style;
    std::string title = "";
    double widthLimit = 300.0;
} g_infos[] = {
    {
        .span = TEXT,
        .title = "default,300",
    },
    {
        .span = TEXT,
        .tpStyle = {
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "default,H,100",
        .widthLimit = 100.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 2,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "L:2,H,100",
        .widthLimit = 100.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "L:1,H,100",
        .widthLimit = 100.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "L:1,H,150",
        .widthLimit = 150.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "L:1,H,70",
        .widthLimit = 70.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "L:1,H,60",
        .widthLimit = 60.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "L:1,H,30",
        .widthLimit = 30.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "L:1,H,25",
        .widthLimit = 25.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::HEAD,
        },
        .title = "L:1,H,15",
        .widthLimit = 15.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 2,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "L:2,M,100",
        .widthLimit = 100.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "L:1,M,300",
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "L:1,M,150",
        .widthLimit = 150.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "L:1,M,70",
        .widthLimit = 70.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "L:1,M,60",
        .widthLimit = 60.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "L:1,M,30",
        .widthLimit = 30.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "L:1,M,25",
        .widthLimit = 25.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::MIDDLE,
        },
        .title = "L:1,M,15",
        .widthLimit = 15.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "default,T,100",
        .widthLimit = 100.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "L:1,T,100",
        .widthLimit = 100.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 2,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "L:2,T,100",
        .widthLimit = 100.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "L:1,T,70",
        .widthLimit = 70.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 2,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "L:2,T,70",
        .widthLimit = 70.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "L:1,T,60",
        .widthLimit = 60.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 2,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "L:2,T,60",
        .widthLimit = 60.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "L:1,T,30",
        .widthLimit = 30.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 2,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "L:2,T,30",
        .widthLimit = 30.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 1,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "L:1,T,15",
        .widthLimit = 15.0,
    },
    {
        .span = TEXT,
        .tpStyle = {
            .maxLines = 2,
            .ellipsis = ELLIPSIS,
            .ellipsisModal = EllipsisModal::TAIL,
        },
        .title = "L:2,T,15",
        .widthLimit = 15.0,
    },
};

class EllipsisModalTest : public TestFeature {
public:
    EllipsisModalTest() : TestFeature("EllipsisModalTest L means maxline, H means HEAD, M means MIDDLE, T means TAIL")
    {
    }

    void Layout() override
    {
        int loopNum = 3;
        for (int i = 0; i < loopNum; i++) {
            for (auto &info : g_infos) {
                auto builder = TypographyBuilder::Create(info.tpStyle);
                builder->PushStyle(info.style);
                switch (i) {
                    case 1: // 1 means test TEXT1
                        info.span = TEXT1;
                        break;
                    case 2: // 2 means test TEXT2
                        info.span = TEXT2;
                        break;
                    default:
                        break;
                };
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
