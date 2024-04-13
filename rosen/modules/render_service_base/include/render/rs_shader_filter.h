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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SHADER_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SHADER_FILTER_H

#include "draw/canvas.h"
#include "effect/runtime_shader_builder.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSB_EXPORT RSShaderFilter {
public:
    RSShaderFilter() = default;
    RSShaderFilter(const RSShaderFilter&) = delete;
    RSShaderFilter operator=(const RSShaderFilter&) = delete;
    virtual ~RSShaderFilter() = default;

    enum ShaderFilterType {
        NONE = 0,
        KAWASE,
        GREY,
        AIBAR,
        LINEAR_GRADIENT_BLUR,
    };

    ShaderFilterType GetShaderFilterType() const
    {
        return type_;
    }

    virtual void PreProcess(std::shared_ptr<Drawing::Image>& image) {};
    virtual std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
    {
        return image;
    }
    virtual void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) {};
    virtual void PostProcess(RSPaintFilterCanvas& canvas) {};

    uint32_t Hash() const
    {
        return hash_;
    }

protected:
    ShaderFilterType type_;
    uint32_t hash_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SHADER_FILTER_H