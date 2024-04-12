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
#ifndef GRAPHICS_EFFECT_ENGINE_VISUAL_EFFECT_IMPL_H
#define GRAPHICS_EFFECT_ENGINE_VISUAL_EFFECT_IMPL_H

#include <memory>

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEVisualEffectImpl {
public:
    enum class FilterType { NONE, KAWASE_BLUR, GREY, AIBAR, CHAINED_FILTER, MAX };

public:
    GEVisualEffectImpl(const std::string &name);

    ~GEVisualEffectImpl();

    void SetParam(const std::string &tag, int32_t param);
    void SetParam(const std::string &tag, int64_t param);
    void SetParam(const std::string &tag, float param);
    void SetParam(const std::string &tag, double param);
    void SetParam(const std::string &tag, const char *const param);
    void SetParam(const std::string &tag, const std::shared_ptr<Drawing::Image> param);
    void SetParam(const std::string &tag, const std::shared_ptr<Drawing::ColorFilter> param);

    const std::vector<std::shared_ptr<GEVisualEffectImpl>> GetFilters() const
    {
        return filterVec_;
    }

    void SetFilterType(FilterType type)
    {
        filterType_ = type;
    }

    const FilterType& GetFilterType() const
    {
        return filterType_;
    }

    const std::shared_ptr<GEKawaseBlurShaderFilterParams> &GetKawaseParams() const
    {
        return kawaseParams_;
    }

    const std::shared_ptr<GEAIBarShaderFilterParams> &GetAIBarParams() const
    {
        return aiBarParams_;
    }

    const std::shared_ptr<GEGreyShaderFilterParams> &GetGreyParams() const
    {
        return greyParams_;
    }

private:
    static std::map<const std::string, std::function<void(GEVisualEffectImpl*)>> g_initialMap;

    FilterType filterType_ = GEVisualEffectImpl::FilterType::NONE;
    std::vector<std::shared_ptr<GEVisualEffectImpl>> filterVec_;

    std::shared_ptr<GEKawaseBlurShaderFilterParams> kawaseParams_ = nullptr;
    std::shared_ptr<GEAIBarShaderFilterParams> aiBarParams_ = nullptr;
    std::shared_ptr<GEGreyShaderFilterParams> greyParams_ = nullptr;
};

}  // namespace Drawing
}  // namespace Rosen
}  // namespace OHOS

#endif  // GRAPHICS_EFFECT_ENGINE_VISUAL_EFFECT_IMPL_H
