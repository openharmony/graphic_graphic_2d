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

#include "draw/canvas.h"
#include "effect/particle_effect.h"
#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
ParticleEffect::ParticleEffect() : impl_(ImplFactory::CreateParticleEffectImpl()) {}

void ParticleEffect::UpdateConfigData(const std::string& name, const void* data, uint32_t offset, uint32_t size)
{
    if (impl_) {
        impl_->UpdateConfigData(name, data, offset, size);
    }
}

void ParticleEffect::UpdateConfigImage(const std::string& name, std::shared_ptr<Image> image)
{
    if (impl_) {
        impl_->UpdateConfigImage(name, image);
    }
}

std::shared_ptr<Data> ParticleEffect::Serialize() const
{
    if (!impl_) {
        return nullptr;
    }
    return impl_->Serialize();
}

bool ParticleEffect::Deserialize(std::shared_ptr<Data> data)
{
    if (!impl_) {
        return false;
    }
    return impl_->Deserialize(data);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS