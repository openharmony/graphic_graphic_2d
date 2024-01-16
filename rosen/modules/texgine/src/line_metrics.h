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

#ifndef ROSEN_MODULES_TEXGINE_SRC_LINE_METRICS_H
#define ROSEN_MODULES_TEXGINE_SRC_LINE_METRICS_H

#include <vector>

#include "variant_span.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define MAX_INT_VALUE 0x7FFFFFFF

struct LineMetrics {
    std::vector<VariantSpan> lineSpans;
    double width = 0.0;
    double indent = 0.0;

    void AddSpanAndUpdateMetrics(const VariantSpan &span);

    double GetAllSpanWidth() const
    {
        double allSpanWidth = 0.0;
        for (const auto &span : lineSpans) {
            allSpanWidth += span.GetWidth();
        }
        return allSpanWidth;
    }

    double GetMaxHeight() const
    {
        double maxHeight = 0.0;
        for (const auto &span : lineSpans) {
            if (span.GetHeight() > maxHeight) {
                maxHeight = span.GetHeight();
            }
        }
        return maxHeight;
    }
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_LINE_METRICS_H
