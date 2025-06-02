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

#include "ge_shader_mask.h"
#include "render/rs_render_mask.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSShaderMask {
public:
    RSShaderMask(const std::shared_ptr<RSRenderMaskPara>& renderMask);
    RSShaderMask(const RSShaderMask&) = delete;
    RSShaderMask operator=(const RSShaderMask&) = delete;
    virtual ~RSShaderMask() = default;
    std::shared_ptr<Drawing::GEShaderMask> GenerateGEShaderMask() const;
    inline uint32_t Hash() const
    {
        return hash_;
    }

protected:
    std::shared_ptr<RSRenderMaskPara> renderMask_ = nullptr;
    uint32_t hash_ = 0;

private:
    void CalHash();
};
} // Rosen
} // OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RS_SHADER_MASK_H