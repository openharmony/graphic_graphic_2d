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
#ifndef RENDER_SERVICE_BASE_CORE_RENDER_RS_DOT_MATRIX_SHADER_H
#define RENDER_SERVICE_BASE_CORE_RENDER_RS_DOT_MATRIX_SHADER_H

#include "render/rs_shader.h"
#include "ext/gex_dot_matrix_shader.h"
#include "ext/gex_dot_matrix_shader_params.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSDotMatrixShader : public RSShader {
public:
    RSDotMatrixShader();
    RSDotMatrixShader(Drawing::Color dotColor, float dotSpacing, float dotRadius,
        Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT);
    RSDotMatrixShader(const RSDotMatrixShader&) = delete;
    RSDotMatrixShader operator=(const RSDotMatrixShader&) = delete;
    ~RSDotMatrixShader() override = default;

    void SetNormalParams(Drawing::Color dotColor, float dotSpacing, float dotRadius,
        Drawing::Color bgColor = Drawing::Color::COLOR_TRANSPARENT);
    void SetNoneEffect();
    void SetRotateEffect(const RotateEffectParams& rotateParams);
    void SetRippleEffect(const RippleEffectParams& rippleParams);

    void MakeDrawingShader(const RectF& rect, float progress) override;
    const std::shared_ptr<Drawing::ShaderEffect>& GetDrawingShader() const override;

    bool Marshalling(Parcel& parcel) override;
    bool Unmarshalling(Parcel& parcel, bool& needReset) override;

private:
    std::shared_ptr<DotMatrixShaderParams> params_ = std::make_shared<DotMatrixShaderParams>();
    std::shared_ptr<GEXDotMatrixShader> geShader_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif