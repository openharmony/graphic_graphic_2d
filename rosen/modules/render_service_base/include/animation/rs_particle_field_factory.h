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

#ifndef ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_FIELD_FACTORY_H
#define ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_FIELD_FACTORY_H

#include <memory>

#include "animation/rs_particle_field_base.h"
#include "animation/rs_particle_field_collection.h"
#include "common/rs_macros.h"

namespace OHOS {
class Parcel;
}

namespace OHOS {
namespace Rosen {

class RSB_EXPORT ParticleFieldFactory {
public:
    static bool Marshalling(Parcel& parcel, const std::shared_ptr<ParticleFieldCollection>& val);
    static bool Unmarshalling(Parcel& parcel, std::shared_ptr<ParticleFieldCollection>& val);

private:
    static bool MarshalField(Parcel& parcel, const std::shared_ptr<ParticleFieldBase>& field);
    static bool UnmarshalField(Parcel& parcel, std::shared_ptr<ParticleFieldBase>& field);
    static std::shared_ptr<ParticleFieldBase> CreateField(ParticleFieldType type);
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_FIELD_FACTORY_H
