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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_MATERIAL_SHADER_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_MATERIAL_SHADER_DRAWABLE_H

#include "drawable/rs_drawable.h"
#include "property/rs_properties_def.h"

namespace OHOS::Rosen {
class RSNGRenderShaderBase;
namespace Drawing {
class GEVisualEffectContainer;
}

namespace DrawableV2 {

class RSMaterialShaderDrawable : public RSDrawable {
public:
    RSMaterialShaderDrawable() = default;
    ~RSMaterialShaderDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

private:
    bool needSync_ = false;
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer_;
    std::shared_ptr<RSNGRenderShaderBase> stagingShader_;
    float cornerRadius_ = 0.f;
    float stagingCornerRadius_ = 0.f;
    RectF stagingDrawRect_;
    RectF drawRect_;
    NodeId stagingNodeId_ = INVALID_NODEID;
    NodeId nodeId_ = INVALID_NODEID;
};

} // namespace DrawableV2
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_MATERIAL_SHADER_DRAWABLE_H