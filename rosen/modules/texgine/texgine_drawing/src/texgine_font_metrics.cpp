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

#include "texgine_font_metrics.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
TexgineFontMetrics::TexgineFontMetrics()
{
    fFlags_ = &fontMetrics_->fFlags;
    fTop_ = &fontMetrics_->fTop;
    fAscent_ = &fontMetrics_->fAscent;
    fDescent_ = &fontMetrics_->fDescent;
    fBottom_ = &fontMetrics_->fBottom;
    fLeading_ = &fontMetrics_->fLeading;
    fAvgCharWidth_ = &fontMetrics_->fAvgCharWidth;
    fMaxCharWidth_ = &fontMetrics_->fMaxCharWidth;
    fXMin_ = &fontMetrics_->fXMin;
    fXMax_ = &fontMetrics_->fXMax;
    fXHeight_ = &fontMetrics_->fXHeight;
    fCapHeight_ = &fontMetrics_->fCapHeight;
    fUnderlineThickness_ = &fontMetrics_->fUnderlineThickness;
    fUnderlinePosition_ = &fontMetrics_->fUnderlinePosition;
    fStrikeoutThickness_ = &fontMetrics_->fStrikeoutThickness;
    fStrikeoutPosition_ = &fontMetrics_->fStrikeoutPosition;
}
std::shared_ptr<RSFontMetrics> TexgineFontMetrics::GetFontMetrics() const
{
    return fontMetrics_;
}

void TexgineFontMetrics::SetFontMetrics(const std::shared_ptr<RSFontMetrics> metrics)
{
    if (metrics == nullptr) {
        return;
    }

    fontMetrics_ = metrics;
    fFlags_ = &fontMetrics_->fFlags;
    fTop_ = &fontMetrics_->fTop;
    fAscent_ = &fontMetrics_->fAscent;
    fDescent_ = &fontMetrics_->fDescent;
    fBottom_ = &fontMetrics_->fBottom;
    fLeading_ = &fontMetrics_->fLeading;
    fAvgCharWidth_ = &fontMetrics_->fAvgCharWidth;
    fMaxCharWidth_ = &fontMetrics_->fMaxCharWidth;
    fXMin_ = &fontMetrics_->fXMin;
    fXMax_ = &fontMetrics_->fXMax;
    fXHeight_ = &fontMetrics_->fXHeight;
    fCapHeight_ = &fontMetrics_->fCapHeight;
    fUnderlineThickness_ = &fontMetrics_->fUnderlineThickness;
    fUnderlinePosition_ = &fontMetrics_->fUnderlinePosition;
    fStrikeoutThickness_ = &fontMetrics_->fStrikeoutThickness;
    fStrikeoutPosition_ = &fontMetrics_->fStrikeoutPosition;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
