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
#ifndef GRAPHICS_EFFECT_GE_VISUAL_EFFECT_IMPL_H
#define GRAPHICS_EFFECT_GE_VISUAL_EFFECT_IMPL_H

#include <memory>

#include "ge_shader.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GE_EXPORT GEVisualEffectImpl {
public:
    enum class FilterType {
        NONE,
        KAWASE_BLUR,
        MESA_BLUR,
        GREY, AIBAR,
        LINEAR_GRADIENT_BLUR,
        MAGNIFIER,
        WATER_RIPPLE,
        DOT_MATRIX,
        FLOW_LIGHT_SWEEP,
        MAX
    };

    GEVisualEffectImpl(const std::string& name);

    ~GEVisualEffectImpl();

    void SetParam(const std::string& tag, int32_t param);
    void SetParam(const std::string& tag, int64_t param);
    void SetParam(const std::string& tag, float param);
    void SetParam(const std::string& tag, double param);
    void SetParam(const std::string& tag, const char* const param);

    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::Image> param);
    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::ColorFilter> param);
    void SetParam(const std::string& tag, const Drawing::Matrix param);
    void SetParam(const std::string& tag, const std::vector<std::pair<float, float>>);
    void SetParam(const std::string& tag, bool param);
    void SetParam(const std::string& tag, uint32_t param);

    void SetFilterType(FilterType type)
    {
        filterType_ = type;
    }

    const FilterType& GetFilterType() const
    {
        return filterType_;
    }

    void MakeMESAParams()
    {
        mesaParams_ = std::make_shared<GEMESABlurShaderFilterParams>();
    }

    const std::shared_ptr<GEMESABlurShaderFilterParams>& GetMESAParams() const
    {
        return mesaParams_;
    }

    void MakeKawaseParams()
    {
        kawaseParams_ = std::make_shared<GEKawaseBlurShaderFilterParams>();
    }

    const std::shared_ptr<GEKawaseBlurShaderFilterParams>& GetKawaseParams() const
    {
        return kawaseParams_;
    }

    void MakeWaterRippleParams()
    {
        waterRippleParams_ = std::make_shared<GEWaterRippleFilterParams>();
    }

    const std::shared_ptr<GEWaterRippleFilterParams>& GetWaterRippleParams() const
    {
        return waterRippleParams_;
    }

    void MakeAIBarParams()
    {
        aiBarParams_ = std::make_shared<GEAIBarShaderFilterParams>();
    }

    const std::shared_ptr<GEAIBarShaderFilterParams>& GetAIBarParams() const
    {
        return aiBarParams_;
    }

    void MakeGreyParams()
    {
        greyParams_ = std::make_shared<GEGreyShaderFilterParams>();
    }

    const std::shared_ptr<GEGreyShaderFilterParams>& GetGreyParams() const
    {
        return greyParams_;
    }

    void MakeLinearGradientBlurParams()
    {
        linearGradientBlurParams_ = std::make_shared<GELinearGradientBlurShaderFilterParams>();
    }

    const std::shared_ptr<GELinearGradientBlurShaderFilterParams>& GetLinearGradientBlurParams() const
    {
        return linearGradientBlurParams_;
    }

    void MakeMagnifierParams()
    {
        magnifierParams_ = std::make_shared<GEMagnifierShaderFilterParams>();
    }
 
    const std::shared_ptr<GEMagnifierShaderFilterParams>& GetMagnifierParams() const
    {
        return magnifierParams_;
    }

private:
    static std::map<const std::string, std::function<void(GEVisualEffectImpl*)>> g_initialMap;

    void SetMESABlurParams(const std::string& tag, float param);
    void SetAIBarParams(const std::string& tag, float param);
    void SetGreyParams(const std::string& tag, float param);
    void SetLinearGradientBlurParams(const std::string& tag, float param);

    void SetMagnifierParamsFloat(const std::string& tag, float param);
    void SetMagnifierParamsUint32(const std::string& tag, uint32_t param);

    void SetWaterRippleParams(const std::string& tag, float param);

    FilterType filterType_ = GEVisualEffectImpl::FilterType::NONE;

    std::shared_ptr<GEKawaseBlurShaderFilterParams> kawaseParams_ = nullptr;
    std::shared_ptr<GEMESABlurShaderFilterParams> mesaParams_ = nullptr;
    std::shared_ptr<GEAIBarShaderFilterParams> aiBarParams_ = nullptr;
    std::shared_ptr<GEGreyShaderFilterParams> greyParams_ = nullptr;
    std::shared_ptr<GELinearGradientBlurShaderFilterParams> linearGradientBlurParams_ = nullptr;

    std::shared_ptr<GEMagnifierShaderFilterParams> magnifierParams_ = nullptr;
    std::shared_ptr<GEWaterRippleFilterParams> waterRippleParams_ = nullptr;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VISUAL_EFFECT_IMPL_H
