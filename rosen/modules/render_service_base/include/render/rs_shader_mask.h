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
#ifndef RENDER_SERVICE_BASE_RENDER_RS_SHADER_MASK_H
#define RENDER_SERVICE_BASE_RENDER_RS_SHADER_MASK_H
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSShaderMask {
public:
    RSShaderMask() = default;
    RSShaderMask(const RSShaderMask&) = delete;
    RSShaderMask operator=(const RSShaderMask&) = delete;
    virtual ~RSShaderMask() = default;

    enum class Type {
        NONE = 0,
        RADIAL_GRADIENT,
    };

    Type GetShaderMaskType() const
    {
        return type_;
    }

    virtual void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer);

    uint32_t Hash() const
    {
        return hash_;
    }

protected:
    Type type_;
    uint32_t hash_ = 0;
};
} // Rosen
} // OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RS_SHADER_MASK_H