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

#ifndef GRADIENT_BLUR_UTILS_H
#define GRADIENT_BLUR_UTILS_H

#include <array>
#include <iostream>

#include "drawing/engine_adapter/impl_interface/gradient_blur_impl.h"
#include "draw/canvas.h"
#include "gradient_blur_param.h"
#include "image/image.h"
#include "rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class DRAWING_API GradientBlurUtils {
public:
    GradientBlurUtils();

    virtual ~GradientBlurUtils() {}

    void DrawGradientBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst, Drawing::GradientBlurParam& param);

private:

    std::shared_ptr<GradientBlurImpl> gradientBlurImplPtr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // GRADIENT_BLUR_UTILS_H