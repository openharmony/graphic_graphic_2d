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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_COVERAGE_NG_SHADER_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_COVERAGE_NG_SHADER_DRAWABLE_H
#include "common/rs_rect.h"
#include "common/rs_vector4.h"
#include "drawable/rs_shader_drawable.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"

namespace OHOS::Rosen {
namespace Drawing {
class RuntimeShaderBuilder;
}
namespace DrawableV2 {
namespace {
constexpr int MAX_LIGHT_SOURCES = 12;
}
class RSCoverageNGShaderDrawable : public RSShaderDrawable {
public:
    RSCoverageNGShaderDrawable() = default;
    ~RSCoverageNGShaderDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);

protected:
    std::shared_ptr<RSNGRenderShaderBase> GetShader(const RSProperties& properties) const override;
    void DfxOnDraw() const override;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif