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
#ifndef GRAPHICS_EFFECT_GE_GREY_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_GREY_SHADER_FILTER_H

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT GEGreyShaderFilter : public GEShaderFilter {
public:
    GEGreyShaderFilter(const Drawing::GEGreyShaderFilterParams& params);
    ~GEGreyShaderFilter() override;

    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    friend class RSMarshallingHelper;

    bool InitGreyAdjustmentEffect();

    float greyCoef1_ = 0.f;
    float greyCoef2_ = 0.f;

    static std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect_;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_GREY_SHADER_FILTER_H
