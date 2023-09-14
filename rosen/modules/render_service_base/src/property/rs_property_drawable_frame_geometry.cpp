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

#include "property/rs_property_drawable_frame_geometry.h"

#include "property/rs_properties.h"

namespace OHOS::Rosen {

RSFrameGeometryDrawable::RSFrameGeometryDrawable(float frameOffsetX, float frameOffsetY)
    : frameOffsetX_(frameOffsetX), frameOffsetY_(frameOffsetY)
{}
void RSFrameGeometryDrawable::Draw(RSModifierContext& context)
{
    context.canvas_->translate(frameOffsetX_, frameOffsetY_);
}
std::unique_ptr<RSPropertyDrawable> RSFrameGeometryDrawable::Generate(const RSProperties& properties)
{
    auto frameOffsetX = properties.GetFrameOffsetX();
    auto frameOffsetY = properties.GetFrameOffsetY();
    if (frameOffsetX == 0 && frameOffsetY == 0) {
        return nullptr;
    }
    return std::make_unique<RSFrameGeometryDrawable>(frameOffsetX, frameOffsetY);
}

} // namespace OHOS::Rosen
