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

#include "animation/rs_particle_field_base.h"

#include <cmath>
#include <parcel.h>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {

ParticleFieldBase::~ParticleFieldBase() = default;

bool ParticleFieldBase::IsPointInRegion(const Vector2f& point) const
{
    return false;
}

bool ParticleFieldBase::MarshalBase(Parcel& parcel) const
{
    if (!parcel.WriteUint32(static_cast<uint32_t>(regionShape_))) {
        return false;
    }
    if (!parcel.WriteFloat(regionPosition_.x_) || !parcel.WriteFloat(regionPosition_.y_)) {
        return false;
    }
    if (!parcel.WriteFloat(regionSize_.x_) || !parcel.WriteFloat(regionSize_.y_)) {
        return false;
    }
    return true;
}

bool ParticleFieldBase::UnmarshalBase(Parcel& parcel)
{
    uint32_t shapeVal = 0;
    if (!parcel.ReadUint32(shapeVal)) {
        return false;
    }
    regionShape_ = static_cast<ShapeType>(shapeVal);

    float posX = 0.0f;
    float posY = 0.0f;
    if (!parcel.ReadFloat(posX) || !parcel.ReadFloat(posY)) {
        return false;
    }
    regionPosition_ = Vector2f(posX, posY);

    float sizeX = 0.0f;
    float sizeY = 0.0f;
    if (!parcel.ReadFloat(sizeX) || !parcel.ReadFloat(sizeY)) {
        return false;
    }
    regionSize_ = Vector2f(sizeX, sizeY);

    return true;
}

bool ParticleFieldBase::operator==(const ParticleFieldBase& rhs) const
{
    if (GetType() != rhs.GetType()) {
        return false;
    }
    if (regionShape_ != rhs.regionShape_) {
        return false;
    }
    if (regionPosition_ != rhs.regionPosition_) {
        return false;
    }
    if (regionSize_ != rhs.regionSize_) {
        return false;
    }
    return Equals(rhs);
}

} // namespace Rosen
} // namespace OHOS
