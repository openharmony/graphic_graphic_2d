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

#include "animation/rs_particle_field_factory.h"

#include <parcel.h>

#include "animation/rs_particle_noise_field.h"
#include "animation/rs_particle_ripple_field.h"
#include "animation/rs_particle_velocity_field.h"
#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

bool ParticleFieldFactory::Marshalling(Parcel& parcel, const std::shared_ptr<ParticleFieldCollection>& val)
{
    if (val == nullptr) {
        if (!parcel.WriteInt32(-1)) {
            ROSEN_LOGE("ParticleFieldFactory::Marshalling write null marker failed");
            return false;
        }
        return true;
    }
    if (!parcel.WriteInt32(1)) {
        ROSEN_LOGE("ParticleFieldFactory::Marshalling write valid marker failed");
        return false;
    }
    uint32_t size = static_cast<uint32_t>(val->Count());
    if (!parcel.WriteUint32(size)) {
        ROSEN_LOGE("ParticleFieldFactory::Marshalling write size failed");
        return false;
    }
    for (uint32_t i = 0; i < size; ++i) {
        auto field = val->Get(i);
        if (!MarshalField(parcel, field)) {
            ROSEN_LOGE("ParticleFieldFactory::Marshalling MarshalField %{public}u failed", i);
            return false;
        }
    }
    return true;
}

bool ParticleFieldFactory::Unmarshalling(Parcel& parcel, std::shared_ptr<ParticleFieldCollection>& val)
{
    int32_t marker = 0;
    if (!parcel.ReadInt32(marker)) {
        ROSEN_LOGE("ParticleFieldFactory::Unmarshalling read marker failed");
        return false;
    }
    if (marker == -1) {
        val = nullptr;
        return true;
    }
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        ROSEN_LOGE("ParticleFieldFactory::Unmarshalling read size failed");
        return false;
    }
    if (size > PARTICLE_EMMITER_UPPER_LIMIT) {
        ROSEN_LOGE("ParticleFieldFactory::Unmarshalling size %{public}u exceeds upper limit", size);
        return false;
    }
    val = std::make_shared<ParticleFieldCollection>();
    for (uint32_t i = 0; i < size; ++i) {
        std::shared_ptr<ParticleFieldBase> field;
        if (!UnmarshalField(parcel, field)) {
            ROSEN_LOGE("ParticleFieldFactory::Unmarshalling UnmarshalField %{public}u failed", i);
            return false;
        }
        if (field != nullptr) {
            val->Add(field);
        }
    }
    return true;
}

bool ParticleFieldFactory::MarshalField(Parcel& parcel, const std::shared_ptr<ParticleFieldBase>& field)
{
    if (field == nullptr) {
        if (!parcel.WriteInt32(-1)) {
            ROSEN_LOGE("ParticleFieldFactory::MarshalField write null marker failed");
            return false;
        }
        return true;
    }
    if (!parcel.WriteInt32(1)) {
        ROSEN_LOGE("ParticleFieldFactory::MarshalField write valid marker failed");
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(field->GetType()))) {
        ROSEN_LOGE("ParticleFieldFactory::MarshalField write type failed");
        return false;
    }
    if (!field->MarshalBase(parcel)) {
        ROSEN_LOGE("ParticleFieldFactory::MarshalField MarshalBase failed");
        return false;
    }
    if (!field->MarshalSpecific(parcel)) {
        ROSEN_LOGE("ParticleFieldFactory::MarshalField MarshalSpecific failed");
        return false;
    }
    return true;
}

bool ParticleFieldFactory::UnmarshalField(Parcel& parcel, std::shared_ptr<ParticleFieldBase>& field)
{
    int32_t marker = 0;
    if (!parcel.ReadInt32(marker)) {
        ROSEN_LOGE("ParticleFieldFactory::UnmarshalField read marker failed");
        return false;
    }
    if (marker == -1) {
        field = nullptr;
        return true;
    }
    uint32_t typeVal = 0;
    if (!parcel.ReadUint32(typeVal)) {
        ROSEN_LOGE("ParticleFieldFactory::UnmarshalField read type failed");
        return false;
    }
    auto type = static_cast<ParticleFieldType>(typeVal);
    field = CreateField(type);
    if (field == nullptr) {
        ROSEN_LOGE("ParticleFieldFactory::UnmarshalField CreateField failed for type %{public}u", typeVal);
        return false;
    }
    if (!field->UnmarshalBase(parcel)) {
        ROSEN_LOGE("ParticleFieldFactory::UnmarshalField UnmarshalBase failed");
        return false;
    }
    if (!field->UnmarshalSpecific(parcel)) {
        ROSEN_LOGE("ParticleFieldFactory::UnmarshalField UnmarshalSpecific failed");
        return false;
    }
    return true;
}

std::shared_ptr<ParticleFieldBase> ParticleFieldFactory::CreateField(ParticleFieldType type)
{
    switch (type) {
        case ParticleFieldType::NOISE:
            return std::make_shared<ParticleNoiseField>();
        case ParticleFieldType::RIPPLE:
            return std::make_shared<ParticleRippleField>();
        case ParticleFieldType::VELOCITY:
            return std::make_shared<ParticleVelocityField>();
        default:
            ROSEN_LOGE("ParticleFieldFactory::CreateField unknown type %{public}u", static_cast<uint32_t>(type));
            return nullptr;
    }
}

} // namespace Rosen
} // namespace OHOS
