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

#include "effect/gradient_shader_obj_base.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
#ifdef ROSEN_OHOS
constexpr uint32_t MAX_COLOR_COUNT = 256;
constexpr uint32_t MAX_POS_COUNT = 256;
constexpr uint32_t MAX_COLORSPACE_SIZE = 1024 * 1024; // 1MB
#endif

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
bool GradientShaderObjBase::MarshalColors(Parcel& parcel) const
{
    // Write color count
    uint32_t colorCount = colors_.size();
    if (!parcel.WriteUint32(colorCount)) {
        LOGE("GradientShaderObjBase::MarshalColors, failed to write color count");
        return false;
    }

    // Write colors
    for (const auto& color : colors_) {
        if (!parcel.WriteFloat(color.GetRed()) || !parcel.WriteFloat(color.GetGreen()) ||
            !parcel.WriteFloat(color.GetBlue()) || !parcel.WriteFloat(color.GetAlpha()) ||
            !parcel.WriteFloat(color.GetHeadroom())) {
            LOGE("GradientShaderObjBase::MarshalColors, failed to write color");
            return false;
        }
    }

    return true;
}

bool GradientShaderObjBase::MarshalPositions(Parcel& parcel) const
{
    // Write position count
    uint32_t posCount = pos_.size();
    if (!parcel.WriteUint32(posCount)) {
        LOGE("GradientShaderObjBase::MarshalPositions, failed to write position count");
        return false;
    }

    // Write positions
    for (const auto& p : pos_) {
        if (!parcel.WriteFloat(p)) {
            LOGE("GradientShaderObjBase::MarshalPositions, failed to write position");
            return false;
        }
    }

    return true;
}

bool GradientShaderObjBase::MarshalMatrix(Parcel& parcel) const
{
    // Write hasMatrix flag
    bool hasMatrix = (matrix_ != nullptr);
    if (!parcel.WriteBool(hasMatrix)) {
        LOGE("GradientShaderObjBase::MarshalMatrix, failed to write hasMatrix flag");
        return false;
    }

    // Write matrix if present
    if (hasMatrix) {
        Matrix::Buffer buffer;
        matrix_->GetAll(buffer);
        for (size_t i = 0; i < Matrix::MATRIX_SIZE; i++) {
            if (!parcel.WriteFloat(buffer[i])) {
                LOGE("GradientShaderObjBase::MarshalMatrix, failed to write matrix data");
                return false;
            }
        }
    }

    return true;
}

bool GradientShaderObjBase::MarshalColorSpace(Parcel& parcel) const
{
    // Write colorSpace
    auto colorSpaceData = colorSpace_ ? colorSpace_->Serialize() : nullptr;
    if (colorSpaceData && colorSpaceData->GetSize() > 0) {
        if (!parcel.WriteBool(true)) {
            LOGE("GradientShaderObjBase::MarshalColorSpace, failed to write colorSpace flag");
            return false;
        }
        if (!parcel.WriteUint32(colorSpaceData->GetSize())) {
            LOGE("GradientShaderObjBase::MarshalColorSpace, failed to write colorSpace size");
            return false;
        }
        if (!parcel.WriteBuffer(colorSpaceData->GetData(), colorSpaceData->GetSize())) {
            LOGE("GradientShaderObjBase::MarshalColorSpace, failed to write colorSpace data");
            return false;
        }
    } else {
        if (!parcel.WriteBool(false)) {
            LOGE("GradientShaderObjBase::MarshalColorSpace, failed to write colorSpace flag");
            return false;
        }
    }

    return true;
}

bool GradientShaderObjBase::MarshalCommonData(Parcel& parcel) const
{
    // Write colors, positions, tile mode, matrix, and colorSpace in original order
    if (!MarshalColors(parcel) || !MarshalPositions(parcel)) {
        return false;
    }

    // Write tile mode
    if (!parcel.WriteInt32(static_cast<int32_t>(mode_))) {
        LOGE("GradientShaderObjBase::MarshalCommonData, failed to write tile mode");
        return false;
    }

    // Write matrix and colorSpace
    if (!MarshalMatrix(parcel) || !MarshalColorSpace(parcel)) {
        return false;
    }

    return true;
}

bool GradientShaderObjBase::UnmarshalColors(Parcel& parcel)
{
    // Read color count
    uint32_t colorCount;
    if (!parcel.ReadUint32(colorCount)) {
        LOGE("GradientShaderObjBase::UnmarshalColors, failed to read color count");
        return false;
    }
    // Validate color count to prevent memory exhaustion
    if (colorCount > MAX_COLOR_COUNT) {
        LOGE("GradientShaderObjBase::UnmarshalColors, colorCount exceeds maximum value");
        return false;
    }
    // Read colors
    colors_.clear();
    colors_.reserve(colorCount);
    for (size_t i = 0; i < colorCount; i++) {
        float r;
        float g;
        float b;
        float a;
        float h;
        if (!parcel.ReadFloat(r) || !parcel.ReadFloat(g) || !parcel.ReadFloat(b) ||
            !parcel.ReadFloat(a) || !parcel.ReadFloat(h)) {
            LOGE("GradientShaderObjBase::UnmarshalColors, failed to read color");
            return false;
        }
        colors_.emplace_back(r, g, b, a, h);
    }

    return true;
}

bool GradientShaderObjBase::UnmarshalPositions(Parcel& parcel)
{
    // Read position count
    uint32_t posCount;
    if (!parcel.ReadUint32(posCount)) {
        LOGE("GradientShaderObjBase::UnmarshalPositions, failed to read position count");
        return false;
    }
    // Validate position count to prevent memory exhaustion
    if (posCount > MAX_POS_COUNT) {
        LOGE("GradientShaderObjBase::UnmarshalPositions, posCount exceeds maximum value");
        return false;
    }
    // Read positions
    pos_.clear();
    pos_.reserve(posCount);
    for (size_t i = 0; i < posCount; i++) {
        scalar p;
        if (!parcel.ReadFloat(p)) {
            LOGE("GradientShaderObjBase::UnmarshalPositions, failed to read position");
            return false;
        }
        pos_.push_back(p);
    }

    return true;
}

bool GradientShaderObjBase::UnmarshalMatrix(Parcel& parcel)
{
    // Read hasMatrix flag
    bool hasMatrix;
    if (!parcel.ReadBool(hasMatrix)) {
        LOGE("GradientShaderObjBase::UnmarshalMatrix, failed to read hasMatrix flag");
        return false;
    }

    // Read matrix if present
    if (hasMatrix) {
        Matrix::Buffer buffer;
        for (size_t i = 0; i < Matrix::MATRIX_SIZE; i++) {
            if (!parcel.ReadFloat(buffer[i])) {
                LOGE("GradientShaderObjBase::UnmarshalMatrix, failed to read matrix data");
                return false;
            }
        }
        matrix_ = std::make_shared<Matrix>();
        matrix_->SetAll(buffer);
    } else {
        matrix_ = nullptr;
    }

    return true;
}

bool GradientShaderObjBase::UnmarshalColorSpace(Parcel& parcel)
{
    // Read colorSpace flag
    bool hasColorSpace;
    if (!parcel.ReadBool(hasColorSpace)) {
        LOGE("GradientShaderObjBase::UnmarshalColorSpace, failed to read colorSpace flag");
        return false;
    }

    if (hasColorSpace) {
        uint32_t colorSpaceSize;
        if (!parcel.ReadUint32(colorSpaceSize)) {
            LOGE("GradientShaderObjBase::UnmarshalColorSpace, failed to read colorSpace size");
            return false;
        }
        const uint8_t* colorSpaceBuffer = parcel.ReadBuffer(colorSpaceSize);
        if (!colorSpaceBuffer) {
            LOGE("GradientShaderObjBase::UnmarshalColorSpace, failed to read colorSpace data");
            return false;
        }
        // Validate colorSpace size to prevent memory exhaustion
        if (colorSpaceSize > MAX_COLORSPACE_SIZE) {
            LOGE("GradientShaderObjBase::UnmarshalColorSpace, colorSpaceSize exceeds maximum value");
            return false;
        }
        auto colorSpaceData = std::make_shared<Data>();
        if (!colorSpaceData->BuildWithCopy(colorSpaceBuffer, colorSpaceSize)) {
            LOGE("GradientShaderObjBase::UnmarshalColorSpace, failed to create colorSpace data");
            return false;
        }
        colorSpace_ = std::make_shared<ColorSpace>();
        if (!colorSpace_->Deserialize(colorSpaceData)) {
            LOGE("GradientShaderObjBase::UnmarshalColorSpace, failed to deserialize colorSpace");
            return false;
        }
    } else {
        colorSpace_ = nullptr;
    }

    return true;
}

bool GradientShaderObjBase::UnmarshalCommonData(Parcel& parcel)
{
    // Read colors, positions, tile mode, matrix, and colorSpace in original order
    if (!UnmarshalColors(parcel) || !UnmarshalPositions(parcel)) {
        return false;
    }

    // Read tile mode
    int32_t modeValue;
    if (!parcel.ReadInt32(modeValue)) {
        LOGE("GradientShaderObjBase::UnmarshalCommonData, failed to read tile mode");
        return false;
    }
    mode_ = static_cast<TileMode>(modeValue);

    // Read matrix and colorSpace
    if (!UnmarshalMatrix(parcel) || !UnmarshalColorSpace(parcel)) {
        return false;
    }

    return true;
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
