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

#include "animation/rs_particle_field_collection.h"

namespace OHOS {
namespace Rosen {

void ParticleFieldCollection::Add(const std::shared_ptr<ParticleFieldBase>& field)
{
    if (field != nullptr) {
        fields_.push_back(field);
    }
}

void ParticleFieldCollection::Remove(size_t index)
{
    if (index < fields_.size()) {
        fields_.erase(fields_.begin() + static_cast<std::ptrdiff_t>(index));
    }
}

std::shared_ptr<ParticleFieldBase> ParticleFieldCollection::Get(size_t index) const
{
    if (index < fields_.size()) {
        return fields_[index];
    }
    return nullptr;
}

size_t ParticleFieldCollection::Count() const
{
    return fields_.size();
}

void ParticleFieldCollection::Clear()
{
    fields_.clear();
}

void ParticleFieldCollection::RemoveByType(ParticleFieldType type)
{
    auto it = std::remove_if(fields_.begin(), fields_.end(), [type](const std::shared_ptr<ParticleFieldBase>& field) {
        return field != nullptr && field->GetType() == type;
    });
    fields_.erase(it, fields_.end());
}

Vector2f ParticleFieldCollection::ApplyAll(const Vector2f& position, float deltaTime)
{
    Vector2f total(0.0f, 0.0f);
    for (const auto& field : fields_) {
        if (field != nullptr) {
            Vector2f result = field->Apply(position, deltaTime);
            total.x_ += result.x_;
            total.y_ += result.y_;
        }
    }
    return total;
}

void ParticleFieldCollection::UpdateAll(float deltaTime)
{
    for (const auto& field : fields_) {
        if (field != nullptr) {
            field->Update(deltaTime);
        }
    }
}

bool ParticleFieldCollection::operator==(const ParticleFieldCollection& rhs) const
{
    if (fields_.size() != rhs.fields_.size()) {
        return false;
    }
    for (size_t i = 0; i < fields_.size(); ++i) {
        if (fields_[i] == nullptr && rhs.fields_[i] == nullptr) {
            continue;
        }
        if (fields_[i] == nullptr || rhs.fields_[i] == nullptr) {
            return false;
        }
        if (!(*fields_[i] == *rhs.fields_[i])) {
            return false;
        }
    }
    return true;
}

void ParticleFieldCollection::Dump(std::string& out) const
{
    out += "[ParticleFieldCollection count=" + std::to_string(fields_.size()) + "]";
    for (const auto& field : fields_) {
        if (field != nullptr) {
            field->Dump(out);
        }
    }
}

} // namespace Rosen
} // namespace OHOS
