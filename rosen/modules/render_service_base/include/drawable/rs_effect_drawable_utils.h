/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_EFFECT_DRAWABLE_UTILS_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_EFFECT_DRAWABLE_UTILS_H

#include <memory>
#include <string>

#include "common/rs_common_def.h"
#include "property/rs_properties_def.h"

namespace OHOS::Rosen {
class RSNGRenderShaderBase;
class RSNGRenderShapeBase;
class RSProperties;
class RSRenderNode;
namespace Drawing {
class Canvas;
class GEVisualEffectContainer;
} // namespace Drawing

namespace DrawableV2 {

// Drawing helpers dedicated to NG shader drawables: SDF shape staging helpers are called on UI
// thread (OnUpdate paths), the DRM bypass is called on render thread (OnDraw paths).
class RSEffectDrawableUtils {
public:
    // Resolve the node SDF shape from properties and apply it to the shader, fall back to a
    // default rrect shape built from the node rrect when no explicit shape is set.
    static void ApplySDFShapeToEffect(
        const RSProperties& properties, const std::shared_ptr<RSNGRenderShaderBase>& shader, NodeId nodeId);
    // Calculate the effect draw rect: expanded for out-of-bounds effects, equals bounds for
    // effect types without an expansion rule; union nodes (gravity pull mode) have a bounding
    // box larger than bounds, the whole bounding box is preferred in that case.
    static RectF CalcShaderDrawRect(
        const RSRenderNode& node, const RSProperties& properties, const std::shared_ptr<RSNGRenderShaderBase>& shader);
    // DRM bypass: draw a nearly opaque SDF shape color instead of the blur effect when the
    // effect region intersects the DRM layer.
    static void DrawColorUsingSDFWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect, bool isDark,
        const std::shared_ptr<Drawing::GEVisualEffectContainer>& filterGEContainer, const std::string& filterTag,
        const std::string& shapeTag);

private:
    static std::shared_ptr<RSNGRenderShapeBase> CreateDefaultRRectShape(const RRect& sdfRRect, NodeId nodeId);
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_EFFECT_DRAWABLE_UTILS_H
