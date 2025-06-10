/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_INCLUDE_HPAE_FUSION_OPERATOR_H
#define RENDER_SERVICE_BASE_INCLUDE_HPAE_FUSION_OPERATOR_H

#include "utils/rect.h"
#include "render/rs_drawing_filter.h"
#include "hpae_base/rs_hpae_base_types.h"
#include "hpae_base/rs_hpae_hianimation_types.h"

namespace OHOS::Rosen {

class RSB_EXPORT RSHpaeFusionOperator {
public:
    static int ProcessGreyAndStretch(const Drawing::RectI &clipBounds, const std::shared_ptr<Drawing::Image> &image,
        const HpaeBufferInfo &targetBuffer, const std::shared_ptr<RSDrawingFilter> &filter, const Drawing::RectI &src);

    static void GetColorMatrixCoef(const std::shared_ptr<RSDrawingFilter> &filter,
        float (&colorMatrix)[HAE_COLOR_MATRIX_COEF_COUNT]);

    static HaePixel GetHaePixel(const std::shared_ptr<RSDrawingFilter> &filter);

    static Drawing::Matrix GetShaderTransform(const Drawing::Rect& blurRect, float scalW, float scaleH);
};

} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_BASE_INCLUDE_HPAE_FUSION_OPERATOR_H