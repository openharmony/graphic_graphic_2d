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

#include "common/rs_vector2.h"
#include "render/rs_shader_filter.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSMagnifierShaderFilter : public RSShaderFilter {
public:
    RSMagnifierShaderFilter(float offsetX, float offsetY);
    RSMagnifierShaderFilter(const RSMagnifierShaderFilter&) = delete;
    RSMagnifierShaderFilter operator=(const RSMagnifierShaderFilter&) = delete;
    ~RSMagnifierShaderFilter() override;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;
    float GetMagnifierOffsetX() const { return offsetX_; }
    float GetMagnifierOffsetY() const { return offsetY_; }

private:
    friend class RSMarshallingHelper;
    float offsetX_ = 0.f;
    float offsetY_ = 0.f;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MAGNIFIER_SHADER_FILTER_H