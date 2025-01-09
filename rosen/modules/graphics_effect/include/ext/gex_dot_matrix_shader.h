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
#ifndef GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_H
#define GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_H

#include "ge_shader.h"
#include "gex_dot_matrix_shader_params.h"

namespace OHOS {
namespace Rosen {

class GE_EXPORT GEXDotMatrixShader : public GEShader {
public:
    GEXDotMatrixShader() = default;
    ~GEXDotMatrixShader() override = default;

    static std::shared_ptr<GEXDotMatrixShader> CreateDynamicImpl(DotMatrixNormalParams& param);

    void MakeDrawingShader(const Drawing::Rect& rect, float progress) override { }

    virtual const std::string GetDescription() const;
    virtual void SetNormalParams(Drawing::Color dotColor, float dotRadius, float dotSpacing,
        Drawing::Color bgColor = {}) { }
    virtual void SetNoneEffect() { }
    virtual void SetRotateEffect(const RotateEffectParams& rotateParams) { }
    virtual void SetRippleEffect(const RippleEffectParams& rippleParams) { }
};

} // namespace Rosen
} // namespace OHOS
#endif // GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_H