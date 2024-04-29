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

#ifndef BLUR_DRAW_LOOPER_H
#define BLUR_DRAW_LOOPER_H

#include <memory>

#include "common/rs_macros.h"
#include "draw/color.h"
#include "effect/mask_filter.h"
#include "utils/drawing_macros.h"
#include "utils/scalar.h"
#include "utils/data.h"
#include "utils/point.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class DRAWING_API BlurDrawLooper {
public:
    /**
     * @brief Create a blur drawLooper, it will generate two draw operations, which may affect performance.
     * @param radius blur radius.
     * @param dx x offset.
     * @param dy y offset.
     * @param color offset drawlooper's color.
     * @return A shared pointer to BlurDrawLooper
     */
    static std::shared_ptr<BlurDrawLooper> CreateBlurDrawLooper(float blurRadius,
        scalar dx, scalar dy, const Color& color);

    BlurDrawLooper(float blurRadius, scalar dx, scalar dy, const Color& color) noexcept;

    float GetRadius() const { return blurDrawLooperPriv_.blurRadius; }
    scalar GetXOffset() const { return blurDrawLooperPriv_.dx; }
    scalar GetYOffset() const { return blurDrawLooperPriv_.dy; }
    const Color& GetColor() const { return blurDrawLooperPriv_.color; }
    const std::shared_ptr<MaskFilter>& GetMaskFilter() const { return blurMaskFilter_; }

    ~BlurDrawLooper() = default;

    friend bool operator==(const BlurDrawLooper& lhs, const BlurDrawLooper& rhs);
    friend bool operator!=(const BlurDrawLooper& lhs, const BlurDrawLooper& rhs);

    std::shared_ptr<Data> Serialize() const;
    static std::shared_ptr<BlurDrawLooper> Deserialize(std::shared_ptr<Data> data);

private:
    struct BlurDrawLooperPriv {
        scalar blurRadius = 0.f;
        scalar dx = 0.f;
        scalar dy = 0.f;
        Color color = Color::COLOR_BLACK;
    };
    BlurDrawLooperPriv blurDrawLooperPriv_;
    std::shared_ptr<MaskFilter> blurMaskFilter_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif