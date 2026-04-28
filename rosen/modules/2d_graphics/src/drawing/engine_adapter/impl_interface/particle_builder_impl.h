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

#ifndef PARTICLE_BUILDER_IMPL_H
#define PARTICLE_BUILDER_IMPL_H

#include "base_impl.h"
#include "effect/particle_effect.h"
#include "image/image.h"
#include "utils/sampling_options.h"
#include <memory>
#include <string>

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ParticleBuilderImpl : public BaseImpl {
public:
    ParticleBuilderImpl() {}
    ~ParticleBuilderImpl() override {}

    virtual void AddConfigData(const std::string& name, const std::string& configStr, uint32_t typeSize) = 0;

    virtual void AddConfigImage(const std::string& name, const Image& image, const SamplingOptions& option) = 0;

    virtual void SetUpdateCode(const std::string& code) = 0;

    virtual std::shared_ptr<ParticleEffect> MakeParticleEffect(uint32_t maxParticleSize) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif