/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef GRADIENT_BLUR_IMPL_H
#define GRADIENT_BLUR_IMPL_H

#include <memory>
#include <vector>

#include "base_impl.h"
#include "draw/canvas.h"
#include "image/image.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct GradientBlurParam;
class GradientBlurImpl : public BaseImpl {
public:
    GradientBlurImpl() noexcept {}
    ~GradientBlurImpl() override {}

    virtual void DrawGradientBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst, Drawing::GradientBlurParam& param) = 0;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif