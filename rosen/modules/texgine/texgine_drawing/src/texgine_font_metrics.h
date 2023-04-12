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

#include <include/core/SkFontMetrics.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontMetrics {
public:
    TexgineFontMetrics();
    std::shared_ptr<SkFontMetrics> GetFontMetrics();
    void SetFontMetrics(std::shared_ptr<SkFontMetrics> metrics);

    uint32_t *fFlags_;
    float *fTop_;
    float *fAscent_;
    float *fDescent_;
    float *fBottom_;
    float *fLeading_;
    float *fAvgCharWidth_;
    float *fMaxCharWidth_;
    float *fXMin_;
    float *fXMax_;
    float *fXHeight_;
    float *fCapHeight_;
    float *fUnderlineThickness_;
    float *fUnderlinePosition_;
    float *fStrikeoutThickness_;
    float *fStrikeoutPosition_;

private:
    std::shared_ptr<SkFontMetrics> fontMetrics_ = std::make_shared<SkFontMetrics>();
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_METRICS_H
