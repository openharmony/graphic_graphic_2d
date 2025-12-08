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

#ifndef RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_MANAGER_H
#define RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_MANAGER_H

#include <atomic>
#include <memory>

#include "draw/canvas.h"
#include "draw/color.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_def.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
class RSColorPickerManager : public std::enable_shared_from_this<RSColorPickerManager> {
public:
    RSColorPickerManager() = default;
    ~RSColorPickerManager() noexcept = default;
    Drawing::ColorQuad GetColorPicked(RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, uint64_t nodeId,
        ColorPickStrategyType strategy, uint64_t interval);

private:
    std::atomic<Drawing::ColorQuad> colorPicked_ = Drawing::Color::COLOR_TRANSPARENT;
    uint64_t lastUpdateTime_ = 0;
};
} // Rosen
} // OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_MANAGER_H
