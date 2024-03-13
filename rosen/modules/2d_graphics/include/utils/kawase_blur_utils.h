/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef KAWASE_BLUR_UTILS_H
#define KAWASE_BLUR_UTILS_H

#include "draw/canvas.h"
#include "drawing/engine_adapter/impl_interface/kawase_blur_impl.h"
#include "effect/color_filter.h"
#include "image/image.h"
#include "utils/kawase_blur_param.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API KawaseBlurUtils {
public:
    KawaseBlurUtils() noexcept;
    ~KawaseBlurUtils();
    /*
     * @brief             Apply Kawase blur effect on current canvas.
     * @param canvas      Canvas to be drawn.
     * @param brush       Current brush.
     * @param image       Input image need to apply blur effect.
     * @param blurParam   Blur related params.
     * @return            If apply kawase blur success, return true.
     */
    bool ApplyKawaseBlur(Drawing::Canvas& canvas, Drawing::Brush& brush,
        const std::shared_ptr<Drawing::Image>& image, const KawaseParameters& blurParams,
        const KawaseProperties& properties);
private:
    std::unique_ptr<KawaseBlurImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // KAWASE_BLUR_UTILS_H