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
#include "render/rs_border_light_shader.h"
#include "platform/common/rs_log.h"
#include "ge_border_light_shader.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {

RSBorderLightShader::RSBorderLightShader()
{
    type_ = ShaderType::BORDER_LIGHT;
}

RSBorderLightShader::RSBorderLightShader(const RSBorderLightParams& borderLightParam)
{
    borderLightParam_ = borderLightParam;
    type_ = ShaderType::BORDER_LIGHT;
}

void RSBorderLightShader::MakeDrawingShader(const RectF& rect, float progress)
{
    Drawing::Rect drawingRect = Drawing::Rect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
    if (geShader_) {
        geShader_->MakeDrawingShader(drawingRect, progress);
    }
}

const std::shared_ptr<Drawing::ShaderEffect>& RSBorderLightShader::GetDrawingShader() const
{
    if (geShader_) {
        return geShader_->GetDrawingShader();
    }
    return RSShader::GetDrawingShader();
}

void RSBorderLightShader::SetRSBorderLightParams(const RSBorderLightParams& borderLightParam)
{
    borderLightParam_ = borderLightParam;
    if (geShader_ == nullptr) {
        return;
    }
    BorderLightParams borderLightParams = {
        borderLightParam_.lightPosition_,
        borderLightParam_.lightColor_,
        borderLightParam_.lightIntensity_,
        borderLightParam_.lightWidth_,
        borderLightParam_.rotationAngle_,
        borderLightParam_.cornerRadius_
    };
    geShader_->SetBorderLightParams(borderLightParams);
}

void RSBorderLightShader::SetRotationAngle(const Vector3f& rotationAngle)
{
    borderLightParam_.rotationAngle_ = rotationAngle;
    if (geShader_) {
        geShader_->SetRotationAngle(rotationAngle);
    }
}

void RSBorderLightShader::SetCornerRadius(float cornerRadius)
{
    borderLightParam_.cornerRadius_ = cornerRadius;
    if (geShader_) {
        geShader_->SetCornerRadius(cornerRadius);
    }
}

bool RSBorderLightShader::Marshalling(Parcel& parcel)
{
    if (!RSMarshallingHelper::Marshalling(parcel, borderLightParam_.lightPosition_)
        || !RSMarshallingHelper::Marshalling(parcel, borderLightParam_.lightColor_)
        || !RSMarshallingHelper::Marshalling(parcel, borderLightParam_.lightIntensity_)
        || !RSMarshallingHelper::Marshalling(parcel, borderLightParam_.lightWidth_)) {
        ROSEN_LOGE("unirender: RSBorderLightShader::Marshalling failed");
        return false;
    }
    return true;
}

bool RSBorderLightShader::Unmarshalling(Parcel& parcel, bool& needReset)
{
    needReset = false;
    if (!RSMarshallingHelper::Unmarshalling(parcel, borderLightParam_.lightPosition_)
        || !RSMarshallingHelper::Unmarshalling(parcel, borderLightParam_.lightColor_)
        || !RSMarshallingHelper::Unmarshalling(parcel, borderLightParam_.lightIntensity_)
        || !RSMarshallingHelper::Unmarshalling(parcel, borderLightParam_.lightWidth_)) {
        ROSEN_LOGE("unirender: RSBorderLightShader::Unmarshalling failed");
        return false;
    }
    BorderLightParams borderLightParams = {
        borderLightParam_.lightPosition_,
        borderLightParam_.lightColor_,
        borderLightParam_.lightIntensity_,
        borderLightParam_.lightWidth_,
        borderLightParam_.rotationAngle_,
        borderLightParam_.cornerRadius_
    };
    geShader_ = GEBorderLightShader::CreateBorderLightShader(borderLightParams);
    if (!geShader_) {
        ROSEN_LOGE("unirender: RSBorderLightShader::GEBorderLightShader::Createl failed");
        return false;
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS