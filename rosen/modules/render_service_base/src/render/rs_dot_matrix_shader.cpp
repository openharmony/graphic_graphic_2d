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
#include "render/rs_dot_matrix_shader.h"
#include "platform/common/rs_log.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {

RSDotMatrixShader::RSDotMatrixShader()
{
    type_ = ShaderType::DOT_MATRIX;
}

RSDotMatrixShader::RSDotMatrixShader(Drawing::Color dotColor, float dotSpacing, float dotRadius, Drawing::Color bgColor)
{
    DotMatrixNormalParams normalParams {dotColor, dotSpacing, dotRadius, bgColor};
    if (params_) {
        params_->normalParams_ = normalParams;
    }
    type_ = ShaderType::DOT_MATRIX;
}

void RSDotMatrixShader::MakeDrawingShader(const RectF& rect, float progress)
{
    Drawing::Rect drawingRect = Drawing::Rect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    if (geShader_) {
        geShader_->MakeDrawingShader(drawingRect, progress);
    }
}

const std::shared_ptr<Drawing::ShaderEffect>& RSDotMatrixShader::GetDrawingShader() const
{
    if (geShader_) {
        return geShader_->GetDrawingShader();
    }
    return RSShader::GetDrawingShader();
}

void RSDotMatrixShader::SetNormalParams(Drawing::Color dotColor, float dotSpacing, float dotRadius,
    Drawing::Color bgColor)
{
    DotMatrixNormalParams normalParams {dotColor, dotSpacing, dotRadius, bgColor};
    if (params_) {
        params_->normalParams_ = normalParams;
    }
}

void RSDotMatrixShader::SetNoneEffect()
{
    if (params_) {
        params_->effectType_ = DotMatrixEffectType::NONE;
    }
}
void RSDotMatrixShader::SetRotateEffect(const RotateEffectParams& rotateParams)
{
    if (params_) {
        params_->rotateEffectParams_ = rotateParams;
        params_->effectType_ = DotMatrixEffectType::ROTATE;
    }
}

void RSDotMatrixShader::SetRippleEffect(const RippleEffectParams& rippleParams)
{
    if (params_) {
        params_->rippleEffectParams_ = rippleParams;
        params_->effectType_ = DotMatrixEffectType::RIPPLE;
    }
}

bool RSDotMatrixShader::Marshalling(Parcel& parcel)
{
    if (!params_) {
        return false;
    }

    return params_->Marshalling(parcel);
}

bool RSDotMatrixShader::Unmarshalling(Parcel& parcel, bool& needReset)
{
    needReset = false;
    params_ = std::make_shared<DotMatrixShaderParams>();
    if (!params_) {
        return false;
    }
    
    if (!params_->Unmarshalling(parcel)) {
        ROSEN_LOGE("unirender: RSDotMatrixShader::Unmarshalling failed");
        return false;
    }

    geShader_ = GEXDotMatrixShader::CreateDynamicImpl(params_->normalParams_);
    if (!geShader_) {
        ROSEN_LOGE("unirender: RSDotMatrixShader::Unmarshalling CreateDynamicImpl failed");
        return false;
    }
    if (params_->effectType_ == DotMatrixEffectType::ROTATE) {
        geShader_->SetRotateEffect(params_->rotateEffectParams_);
    } else if (params_->effectType_ == DotMatrixEffectType::RIPPLE) {
        geShader_->SetRippleEffect(params_->rippleEffectParams_);
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS