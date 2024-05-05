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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_METRICS_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_METRICS_H

#include <memory>

#include "drawing.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontMetrics {
public:
    TexgineFontMetrics();
    std::shared_ptr<RSFontMetrics> GetFontMetrics() const;

    void SetFontMetrics(const std::shared_ptr<RSFontMetrics> metrics);
    uint32_t *fFlags_ = nullptr;
    float *fTop_ = nullptr;
    float *fAscent_ = nullptr;
    float *fDescent_ = nullptr;
    float *fBottom_ = nullptr;
    float *fLeading_ = nullptr;
    float *fAvgCharWidth_ = nullptr;
    float *fMaxCharWidth_ = nullptr;
    float *fXMin_ = nullptr;
    float *fXMax_ = nullptr;
    float *fXHeight_ = nullptr;
    float *fCapHeight_ = nullptr;
    float *fUnderlineThickness_ = nullptr;
    float *fUnderlinePosition_ = nullptr;
    float *fStrikeoutThickness_ = nullptr;
    float *fStrikeoutPosition_ = nullptr;

private:
    std::shared_ptr<RSFontMetrics> fontMetrics_ = std::make_shared<RSFontMetrics>();
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_METRICS_H
