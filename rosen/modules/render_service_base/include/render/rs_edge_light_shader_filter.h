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
#ifndef RENDER_SERVICE_BASE_RENDER_RS_EDGE_LIGHT_SHADER_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RS_EDGE_LIGHT_SHADER_FILTER_H

#include <memory>
#include <vector>
#include <optional>

#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "render/rs_shader_filter.h"
#include "render/rs_shader_mask.h"

namespace OHOS {
namespace Rosen {
struct EdgeLightShaderFilterParams {
    float alpha{ 0.f };
    bool bloom{ true };
    Vector4f color;
    bool useRawColor{ false };
    std::shared_ptr<RSShaderMask> mask{ nullptr };
};

class RSB_EXPORT RSEdgeLightShaderFilter : public RSShaderFilter {
public:
    RSEdgeLightShaderFilter(const EdgeLightShaderFilterParams& para);
    RSEdgeLightShaderFilter(const RSEdgeLightShaderFilter&) = delete;
    RSEdgeLightShaderFilter operator=(const RSEdgeLightShaderFilter&) = delete;
    ~RSEdgeLightShaderFilter() override;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;
private:
    float alpha_{ 0.f };
    bool bloom_{ true };
    Vector4f color_;
    bool useRawColor_ { false };
    std::shared_ptr<RSShaderMask> mask_{ nullptr };
};
} // Rosen
} // OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RS_EDGE_LIGHT_SHADER_FILTER_H
