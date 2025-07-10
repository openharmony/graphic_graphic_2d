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
 
#ifndef RENDER_SERVICE_BASE_RENDER_RS_DISPERSION_SHADER_H
#define RENDER_SERVICE_BASE_RENDER_RS_DISPERSION_SHADER_H
  
#include <memory>
  
#include "common/rs_vector2.h"
#include "render/rs_shader_filter.h"
#include "render/rs_shader_mask.h"

namespace OHOS {
namespace Rosen {

struct DispersionShaderFilterParams {
    std::shared_ptr<RSShaderMask> mask { nullptr };
    float opacity;
    Vector2f redOffset;
    Vector2f greenOffset;
    Vector2f blueOffset;
};

class RSB_EXPORT RSDispersionShaderFilter : public Rosen::RSShaderFilter {
public:
    RSDispersionShaderFilter(const Rosen::DispersionShaderFilterParams& param);
    RSDispersionShaderFilter(const RSDispersionShaderFilter&) = delete;
    RSDispersionShaderFilter operator=(const RSDispersionShaderFilter&) = delete;
    ~RSDispersionShaderFilter() = default;

    void GenerateGEVisualEffect(
        std::shared_ptr<Rosen::Drawing::GEVisualEffectContainer> visualEffectContainer) override;

    const std::shared_ptr<Rosen::RSShaderMask>& GetMask() const;

private:
    std::shared_ptr<Rosen::RSShaderMask> mask_;
    float opacity_;
    Vector2f redOffset_;
    Vector2f greenOffset_;
    Vector2f blueOffset_;
};
}  // namespace Rosen
}  // namespace OHOS
  
#endif  // RENDER_SERVICE_BASE_RENDER_RS_DISPERSION_SHADER_H
