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

#include "effect/gradient_shader_obj_base.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

GradientShaderObjBase::GradientShaderObjBase(int32_t subType)
    : ShaderEffectObj(subType), colorSpace_(nullptr), mode_(TileMode::CLAMP), matrix_(nullptr)
{
}

GradientShaderObjBase::GradientShaderObjBase(int32_t subType, const std::vector<UIColor>& colors,
    std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos,
    TileMode mode, const Matrix* matrix)
    : ShaderEffectObj(subType), colors_(colors), colorSpace_(colorSpace), pos_(pos), mode_(mode),
      matrix_(matrix ? std::make_shared<Matrix>(*matrix) : nullptr)
{
}

#ifdef ROSEN_OHOS
bool GradientShaderObjBase::MarshalCommonData(Parcel& parcel) const
{
    // Write color count
    uint32_t colorCount = colors_.size();
    if (!parcel.WriteUint32(colorCount)) {
        LOGE("GradientShaderObjBase::MarshalCommonData, failed to write color count");
        return false;
    }

    // Write colors
    for (const auto& color : colors_) {
        if (!parcel.WriteFloat(color.GetRed()) || !parcel.WriteFloat(color.GetGreen()) ||
            !parcel.WriteFloat(color.GetBlue()) || !parcel.WriteFloat(color.GetAlpha()) ||
            !parcel.WriteFloat(color.GetHeadroom())) {
            LOGE("GradientShaderObjBase::MarshalCommonData, failed to write color");
            return false;
        }
    }

    // Write position count
    uint32_t posCount = pos_.size();
    if (!parcel.WriteUint32(posCount)) {
        LOGE("GradientShaderObjBase::MarshalCommonData, failed to write position count");
        return false;
    }

    // Write positions
    for (const auto& p : pos_) {
        if (!parcel.WriteFloat(p)) {
            LOGE("GradientShaderObjBase::MarshalCommonData, failed to write position");
            return false;
        }
    }

    // Write tile mode
    if (!parcel.WriteInt32(static_cast<int32_t>(mode_))) {
        LOGE("GradientShaderObjBase::MarshalCommonData, failed to write tile mode");
        return false;
    }

    // Write hasMatrix flag
    bool hasMatrix = (matrix_ != nullptr);
    if (!parcel.WriteBool(hasMatrix)) {
        LOGE("GradientShaderObjBase::MarshalCommonData, failed to write hasMatrix flag");
        return false;
    }

    // Write matrix if present
    if (hasMatrix) {
        const scalar* matrixData = matrix_->GetData();
        for (int i = 0; i < 9; i++) {
            if (!parcel.WriteFloat(matrixData[i])) {
                LOGE("GradientShaderObjBase::MarshalCommonData, failed to write matrix data");
                return false;
            }
        }
    }

    return true;
}

bool GradientShaderObjBase::UnmarshalCommonData(Parcel& parcel)
{
    // Read color count
    uint32_t colorCount;
    if (!parcel.ReadUint32(colorCount)) {
        LOGE("GradientShaderObjBase::UnmarshalCommonData, failed to read color count");
        return false;
    }

    // Read colors
    colors_.clear();
    colors_.reserve(colorCount);
    for (uint32_t i = 0; i < colorCount; i++) {
        float r, g, b, a, h;
        if (!parcel.ReadFloat(r) || !parcel.ReadFloat(g) || !parcel.ReadFloat(b) ||
            !parcel.ReadFloat(a) || !parcel.ReadFloat(h)) {
            LOGE("GradientShaderObjBase::UnmarshalCommonData, failed to read color");
            return false;
        }
        colors_.emplace_back(r, g, b, a, h);
    }

    // Read position count
    uint32_t posCount;
    if (!parcel.ReadUint32(posCount)) {
        LOGE("GradientShaderObjBase::UnmarshalCommonData, failed to read position count");
        return false;
    }

    // Read positions
    pos_.clear();
    pos_.reserve(posCount);
    for (uint32_t i = 0; i < posCount; i++) {
        scalar p;
        if (!parcel.ReadFloat(p)) {
            LOGE("GradientShaderObjBase::UnmarshalCommonData, failed to read position");
            return false;
        }
        pos_.push_back(p);
    }

    // Read tile mode
    int32_t modeValue;
    if (!parcel.ReadInt32(modeValue)) {
        LOGE("GradientShaderObjBase::UnmarshalCommonData, failed to read tile mode");
        return false;
    }
    mode_ = static_cast<TileMode>(modeValue);

    // Read hasMatrix flag
    bool hasMatrix;
    if (!parcel.ReadBool(hasMatrix)) {
        LOGE("GradientShaderObjBase::UnmarshalCommonData, failed to read hasMatrix flag");
        return false;
    }

    // Read matrix if present
    if (hasMatrix) {
        Matrix::Buffer buffer;
        for (int i = 0; i < 9; i++) {
            if (!parcel.ReadFloat(buffer[i])) {
                LOGE("GradientShaderObjBase::UnmarshalCommonData, failed to read matrix data");
                return false;
            }
        }
        matrix_ = std::make_shared<Matrix>();
        matrix_->SetAll(buffer);
    } else {
        matrix_ = nullptr;
    }

    // Note: colorSpace_ is not marshalled for now (requires complex handling)
    colorSpace_ = nullptr;

    return true;
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
