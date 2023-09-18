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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_H

#include <utility>

#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS::Rosen {
class RSPaintFilterCanvas;
class RSDrawCmdList;

// Pure virtual base class
class RSPropertyDrawable {
public:
    RSPropertyDrawable() = default;
    virtual ~RSPropertyDrawable() = default;
    virtual void Draw(RSModifierContext& context) = 0;
    virtual void OnGeometryChange(const RSProperties& properties) {}

    using SaveRestorePair = std::pair<std::unique_ptr<RSPropertyDrawable>, std::unique_ptr<RSPropertyDrawable>>;
    static SaveRestorePair GenerateSaveRestore(RSPaintFilterCanvas::SaveType type = RSPaintFilterCanvas::kCanvas);

    // not copyable and moveable
    RSPropertyDrawable(const RSPropertyDrawable&) = delete;
    RSPropertyDrawable(const RSPropertyDrawable&&) = delete;
    RSPropertyDrawable& operator=(const RSPropertyDrawable&) = delete;
    RSPropertyDrawable& operator=(const RSPropertyDrawable&&) = delete;
};
}; // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_H
