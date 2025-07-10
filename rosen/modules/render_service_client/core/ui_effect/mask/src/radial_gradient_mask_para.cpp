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

#include "ui_effect/mask/include/radial_gradient_mask_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

bool RadialGradientMaskPara::Marshalling(Parcel& parcel) const
{
    if (!(parcel.WriteUint16(static_cast<uint16_t>(type_)) && parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteFloat(center_.x_) && parcel.WriteFloat(center_.y_) &&
        parcel.WriteFloat(radiusX_) && parcel.WriteFloat(radiusY_))) {
        RS_LOGE("[ui_effect] RadialGradientMaskPara Marshalling write type failed");
        return false;
    }

    if (colors_.size() > MaskPara::UNMARSHALLING_MAX_VECTOR_SIZE ||
        positions_.size() > MaskPara::UNMARSHALLING_MAX_VECTOR_SIZE) {
        RS_LOGE("[ui_effect] RadialGradientMaskPara Marshalling colors or positions size is invalid");
        return false;
    }
    if (!parcel.WriteUint32(colors_.size())) {
        RS_LOGE("[ui_effect] RadialGradientMaskPara Marshalling write colors size failed");
        return false;
    }
    for (const auto& color : colors_) {
        if (!parcel.WriteFloat(color)) {
            RS_LOGE("[ui_effect] RadialGradientMaskPara Marshalling write color failed");
            return false;
        }
    }
    if (!parcel.WriteUint32(positions_.size())) {
        RS_LOGE("[ui_effect] RadialGradientMaskPara Marshalling write positions size failed");
        return false;
    }
    for (const auto& position : positions_) {
        if (!parcel.WriteFloat(position)) {
            RS_LOGE("[ui_effect] RadialGradientMaskPara Marshalling write position failed");
            return false;
        }
    }
    return true;
}

void RadialGradientMaskPara::RegisterUnmarshallingCallback()
{
    MaskPara::RegisterUnmarshallingCallback(MaskPara::Type::RADIAL_GRADIENT_MASK, OnUnmarshalling);
}

bool RadialGradientMaskPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val)
{
    uint16_t type = MaskPara::Type::NONE;
    if (!parcel.ReadUint16(type) || type != MaskPara::Type::RADIAL_GRADIENT_MASK) {
        RS_LOGE("[ui_effect] RadialGradientMaskPara OnUnmarshalling read type failed");
        return false;
    }

    auto para = std::make_shared<RadialGradientMaskPara>();
    Vector2f center = { 0.0f, 0.0f };
    Vector2f radius = { 0.0f, 0.0f };
    if (!(parcel.ReadFloat(center.x_) && parcel.ReadFloat(center.y_) &&
        parcel.ReadFloat(radius.x_) && parcel.ReadFloat(radius.y_))) {
        RS_LOGE("[ui_effect] RadialGradientMaskPara OnUnmarshalling read center or radius failed");
        return false;
    }
    para->SetCenter(center);
    para->SetRadiusX(radius.x_);
    para->SetRadiusY(radius.y_);

    uint32_t colorsSize = 0;
    if (!parcel.ReadUint32(colorsSize) || colorsSize > MaskPara::UNMARSHALLING_MAX_VECTOR_SIZE) {
        RS_LOGE("[ui_effect] RadialGradientMaskPara OnUnmarshalling read colors size failed");
        return false;
    }
    std::vector<float> colors;
    for (uint32_t i = 0; i < colorsSize; ++i) {
        float color = 0.0f;
        if (!parcel.ReadFloat(color)) {
            RS_LOGE("[ui_effect] RadialGradientMaskPara OnUnmarshalling read color failed");
            return false;
        }
        colors.emplace_back(color);
    }
    para->SetColors(colors);

    uint32_t positionsSize = 0;
    if (!parcel.ReadUint32(positionsSize) || positionsSize > MaskPara::UNMARSHALLING_MAX_VECTOR_SIZE) {
        RS_LOGE("[ui_effect] RadialGradientMaskPara OnUnmarshalling read positions size failed");
        return false;
    }
    std::vector<float> positions;
    for (uint32_t i = 0; i < positionsSize; ++i) {
        float position = 0.0f;
        if (!parcel.ReadFloat(position)) {
            RS_LOGE("[ui_effect] RadialGradientMaskPara OnUnmarshalling read position failed");
            return false;
        }
        positions.emplace_back(position);
    }
    para->SetPositions(positions);

    val = std::move(para);
    return true;
}

std::shared_ptr<MaskPara> RadialGradientMaskPara::Clone() const
{
    return std::make_shared<RadialGradientMaskPara>(*this);
}

} // namespace Rosen
} // namespace OHOS