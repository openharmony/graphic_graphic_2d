/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ui_effect/mask/include/binocular_mask_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

bool BinocularMaskPara::Marshalling(Parcel& parcel) const
{
    bool isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteFloat(radiusX_) && parcel.WriteFloat(radiusY_) &&
        parcel.WriteFloat(gap_) && parcel.WriteFloat(softness_);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] BinocularMaskPara Marshalling write failed");
        return false;
    }
    return true;
}

void BinocularMaskPara::RegisterUnmarshallingCallback()
{
    MaskPara::RegisterUnmarshallingCallback(MaskPara::Type::BINOCULAR_MASK, OnUnmarshalling);
}

bool BinocularMaskPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val)
{
    uint16_t type = MaskPara::Type::NONE;
    if (!parcel.ReadUint16(type) || type != MaskPara::Type::BINOCULAR_MASK) {
        RS_LOGE("[ui_effect] BinocularMaskPara OnUnmarshalling read type failed, type=%{public}d", type);
        return false;
    }

    float radiusX = 0.28f;
    float radiusY = 0.48f;
    float gap = 0.52f;
    float softness = 0.20f;
    if (!parcel.ReadFloat(radiusX) || !parcel.ReadFloat(radiusY) ||
        !parcel.ReadFloat(gap) || !parcel.ReadFloat(softness)) {
        RS_LOGE("[ui_effect] BinocularMaskPara OnUnmarshalling read params failed");
        return false;
    }

    auto para = std::make_shared<BinocularMaskPara>();
    para->SetRadiusX(radiusX);
    para->SetRadiusY(radiusY);
    para->SetGap(gap);
    para->SetSoftness(softness);
    val = std::move(para);
    return true;
}

std::shared_ptr<MaskPara> BinocularMaskPara::Clone() const
{
    return std::make_shared<BinocularMaskPara>(*this);
}

} // namespace Rosen
} // namespace OHOS