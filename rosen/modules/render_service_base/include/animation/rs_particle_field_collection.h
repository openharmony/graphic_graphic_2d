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

#ifndef ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_FIELD_COLLECTION_H
#define ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_FIELD_COLLECTION_H

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "animation/rs_particle_field_base.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT ParticleFieldCollection {
public:
    std::vector<std::shared_ptr<ParticleFieldBase>> fields_;

    ParticleFieldCollection() = default;

    void Add(const std::shared_ptr<ParticleFieldBase>& field);
    void Remove(size_t index);
    std::shared_ptr<ParticleFieldBase> Get(size_t index) const;
    size_t Count() const;
    void Clear();
    void RemoveByType(ParticleFieldType type);

    Vector2f ApplyAll(const Vector2f& position, float deltaTime);
    void UpdateAll(float deltaTime);

    bool operator==(const ParticleFieldCollection& rhs) const;
    void Dump(std::string& out) const;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_FIELD_COLLECTION_H
