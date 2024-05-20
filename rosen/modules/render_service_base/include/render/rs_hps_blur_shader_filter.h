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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_HPS_BLUR_SHADER_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_HPS_BLUR_SHADER_FILTER_H

#include "render/rs_drawing_filter.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSHpsBlurShaderFilter : public RSShaderFilter {
public:
    RSHpsBlurShaderFilter(float radius, float saturation, float brightness);
    ~RSHpsBlurShaderFilter() override;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;

private:
    float radius_ = 0.f;
    float saturation_ = 1.f;
    float brightness_ = 1.f;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_HPS_BLUR_SHADER_FILTER_H