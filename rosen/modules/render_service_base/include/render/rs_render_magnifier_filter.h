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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MAGNIFIER_SHADER_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MAGNIFIER_SHADER_FILTER_H

#include "render/rs_magnifier_para.h"
#include "render/rs_render_filter_base.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSMagnifierShaderFilter : public RSRenderFilterParaBase {
public:
    RSMagnifierShaderFilter(const std::shared_ptr<RSMagnifierParams>& para);
    RSMagnifierShaderFilter(const RSMagnifierShaderFilter&) = delete;
    RSMagnifierShaderFilter operator=(const RSMagnifierShaderFilter&) = delete;
    ~RSMagnifierShaderFilter() override = default;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;
    void SetMagnifierOffset(Drawing::Matrix& mat);
    float GetMagnifierOffsetX() const { return offsetX_; }
    float GetMagnifierOffsetY() const { return offsetY_; }

private:
    friend class RSMarshallingHelper;
    std::shared_ptr<RSMagnifierParams> magnifierPara_ = nullptr;
    float offsetX_ = 0.f;
    float offsetY_ = 0.f;
    float scaleX_ = 1.f;
    float scaleY_ = 1.f;
    int32_t rotateDegree_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MAGNIFIER_SHADER_FILTER_H