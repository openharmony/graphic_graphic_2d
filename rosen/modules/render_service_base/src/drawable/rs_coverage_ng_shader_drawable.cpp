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

#include "drawable/rs_coverage_ng_shader_drawable.h"

#include "ge_render.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "memory/rs_tag_tracker.h"
#include "platform/common/rs_log.h"
#include "property/rs_point_light_manager.h"
#include "render/rs_effect_luminance_manager.h"
namespace OHOS::Rosen {
namespace DrawableV2 {

RSDrawable::Ptr RSCoverageNGShaderDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSCoverageNGShaderDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

std::shared_ptr<RSNGRenderShaderBase> RSCoverageNGShaderDrawable::GetShader(const RSProperties& properties) const
{
    return properties.GetCoverageNGShader();
}

void RSCoverageNGShaderDrawable::DfxOnDraw() const
{
    RS_OPTIONAL_TRACE_FMT("RSCoverageNGShaderDrawable, nodeId:%{public}" PRIu64
                          ", drawRect:%{public}s, cornerRadius:%{public}f",
        nodeId_, drawRect_.ToString().c_str(), cornerRadius_);
}
} // namespace DrawableV2
} // namespace OHOS::Rosen