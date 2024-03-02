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

#ifndef RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_HARDWARE_COMPOSER_H
#define RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_HARDWARE_COMPOSER_H
#include <cstdint>
#include <map>
#include <memory>
#include <vector>

#include "utils/round_rect.h"

#include "common/rs_vector4.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"

namespace OHOS {
namespace Rosen {
class RSParallelSelfDrawingSurfaceShape {
public:
    RSParallelSelfDrawingSurfaceShape(bool isRRect, RectF rect, Vector4f cornerRadius);
    ~RSParallelSelfDrawingSurfaceShape() = default;
    bool IsRRect() const
    {
        return isRRect_;
    }
    Drawing::Rect GetRect()
    {
        return Drawing::Rect(rect_.GetLeft(), rect_.GetTop(),
            rect_.GetWidth() + rect_.GetLeft(), rect_.GetHeight() + rect_.GetTop());
    }
    Drawing::RoundRect GetRRect()
    {
        if (isRRect_) {
            RRect absClipRRect = RRect(rect_, cornerRadius_);
            return RSPropertiesPainter::RRect2DrawingRRect(absClipRRect);
        }
        Vector4f radius = {0.f, 0.f, 0.f, 0.f};
        RRect absClipRect = RRect(rect_, radius);
        return RSPropertiesPainter::RRect2DrawingRRect(absClipRect);
    }
private:
    bool isRRect_;
    RectF rect_;
    Vector4f cornerRadius_;
};

class RSParallelHardwareComposer {
public:
    RSParallelHardwareComposer() = default;
    ~RSParallelHardwareComposer() = default;
    void Init(uint32_t threadNum);
    void ClearTransparentColor(RSPaintFilterCanvas& canvas, unsigned int surfaceIndex);
    void AddTransparentColorArea(unsigned int surfaceIndex,
        std::unique_ptr<RSParallelSelfDrawingSurfaceShape> &&shape);
private:
    using Holes = std::vector<std::unique_ptr<RSParallelSelfDrawingSurfaceShape>>;
    std::map<unsigned int, Holes> surfaceAndHolesMap_;
};

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_HARDWARE_COMPOSER_H