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
struct LineMetrics {
    std::vector<VariantSpan> lineSpans;
    double width = 0.0;
    double indent = 0.0;
    double lineHeight = 0.0;
    double lineY = 0.0;

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

    void ComputeLineHeightAndY()
    {
        if (lineSpans.size() == 0) {
            lineHeight = 0.0;
            lineY = 0.0;
        } else if (lineHeight == 0.0 || lineY == 0.0) {
            lineHeight = 0.0;
            lineY = INT_MAX;
            for (const auto &span : lineSpans) {
                lineHeight = std::max(lineHeight, span.GetHeight());
                if (auto textSpan = span.TryToTextSpan(); textSpan != nullptr) {
                    lineY = std::min(lineY, span.GetOffsetY() + *(textSpan->tmetrics_->fAscent_));
                } else {
                    lineY = std::min(lineY, span.GetOffsetY());
                }
            }
        }
    }
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_LINE_METRICS_H
