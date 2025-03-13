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

#ifndef GRAPHICS_EFFECT_GE_SHADER_H
#define GRAPHICS_EFFECT_GE_SHADER_H

#include "draw/canvas.h"
#include "utils/rect.h"
#include "effect/shader_effect.h"
#include "ge_common.h"

namespace OHOS {
namespace Rosen {
class GE_EXPORT GEShader {
public:
    GEShader() = default;
    GEShader(const GEShader&) = delete;
    virtual ~GEShader() = default;

    virtual void MakeDrawingShader(const Drawing::Rect& rect, float progress) = 0;

    virtual const std::shared_ptr<Drawing::ShaderEffect>& GetDrawingShader() { return drShader_; }

    uint32_t Hash() const { return hash_; }

protected:
    uint32_t hash_ = 0;
    std::shared_ptr<Drawing::ShaderEffect> drShader_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_H
