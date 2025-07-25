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
#ifndef RENDER_SERVICE_BASE_RENDER_RS_DISPLACEMENT_DISTORT_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RS_DISPLACEMENT_DISTORT_FILTER_H

#include <memory>

#include "common/rs_vector2.h"
#include "render/rs_shader_filter.h"
#include "render/rs_shader_mask.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSDisplacementDistortFilter : public RSShaderFilter {
public:
    RSDisplacementDistortFilter(std::shared_ptr<RSShaderMask> mask, Vector2f factor = {1.f, 1.f});
    RSDisplacementDistortFilter(const RSDisplacementDistortFilter&) = delete;
    RSDisplacementDistortFilter operator=(const RSDisplacementDistortFilter&) = delete;
    ~RSDisplacementDistortFilter() override = default;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;

    const Vector2f& GetFactor() const;
    const std::shared_ptr<RSShaderMask>& GetMask() const;

private:
    static constexpr char GE_FILTER_DISPLACEMENT_DISTORT_FACTOR[] = "DispDistort_Factor";
    static constexpr char GE_FILTER_DISPLACEMENT_DISTORT_MASK[] = "DispDistort_Mask";
    std::shared_ptr<RSShaderMask> mask_ = nullptr;
    Vector2f factor_ = {1.f, 1.f};
};
} // Rosen
} // OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RS_DISPLACEMENT_DISTORT_FILTER_H
