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

#ifndef RENDER_SERVICE_BASE_RENDER_RS_COLOR_GRADIENT_SHADER_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RS_COLOR_GRADIENT_SHADER_FILTER_H

#include <memory>
#include <vector>

#include "render/rs_shader_filter.h"
#include "render/rs_shader_mask.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSColorGradientShaderFilter : public RSShaderFilter {
public:
    RSColorGradientShaderFilter(std::vector<float> colors, std::vector<float> positions,
        std::vector<float> strengths, std::shared_ptr<RSShaderMask> mask);
    RSColorGradientShaderFilter(const RSColorGradientShaderFilter&) = delete;
    RSColorGradientShaderFilter operator=(const RSColorGradientShaderFilter&) = delete;
    ~RSColorGradientShaderFilter() override = default;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;

    const std::vector<float> GetColors() const;
    const std::vector<float> GetPositions() const;
    const std::vector<float> GetStrengths() const;
    const std::shared_ptr<RSShaderMask>& GetMask() const;

private:
    std::vector<float> colors_;
    std::vector<float> positions_;
    std::vector<float> strengths_;
    std::shared_ptr<RSShaderMask> mask_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RS_COLOR_GRADIENT_SHADER_FILTER_H
