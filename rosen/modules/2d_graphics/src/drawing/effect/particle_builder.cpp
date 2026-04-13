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
#include "effect/particle_builder.h"
#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
ParticleBuilder::ParticleBuilder() noexcept
    : impl_(ImplFactory::CreateParticleBuilderImpl()) {}

void ParticleBuilder::AddConfigData(const std::string& name, const std::string& configStr, uint32_t typeSize)
{
    if (impl_) {
        impl_->AddConfigData(name, configStr, typeSize);
    }
}

void ParticleBuilder::AddConfigImage(const std::string& name, const Image& image, const SamplingOptions& option)
{
    if (impl_) {
        impl_->AddConfigImage(name, image, option);
    }
}

void ParticleBuilder::SetUpdateCode(const std::string& code)
{
    if (impl_) {
        impl_->SetUpdateCode(code);
    }
}

std::shared_ptr<ParticleEffect> ParticleBuilder::MakeParticleEffect(uint32_t maxParticleSize)
{
    if (impl_) {
        return impl_->MakeParticleEffect(maxParticleSize);
    }
    return nullptr;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS