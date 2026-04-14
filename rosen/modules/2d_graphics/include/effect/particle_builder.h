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

#ifndef PARTICLE_BUILDER_H
#define PARTICLE_BUILDER_H

#include <string>
#include <vector>
#include "drawing/engine_adapter/impl_interface/particle_builder_impl.h"
#include "image/image.h"
#include "utils/data.h"
#include "utils/sampling_options.h"
#include "effect/particle_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API ParticleBuilder {
public:
    /**
     * @brief Construct a new Particle Builder object.
     */
    ParticleBuilder() noexcept;

    virtual ~ParticleBuilder() = default;

    /**
     * @brief Add configuration data to the particle builder.
     * @param name       The name of the configuration parameter.
     * @param configStr  The configuration shader string.
     * @param typeSize   The size of the configuration.
     */
    void AddConfigData(const std::string& name, const std::string& configStr, uint32_t typeSize);

    /**
     * @brief Add configuration image to the particle builder.
     * @param name   The name of the image.
     * @param image  The image to add.
     * @param option The sampling options for the image.
     */
    void AddConfigImage(const std::string& name, const Image& image, const SamplingOptions& option);

    /**
     * @brief Set the shader code for the particle effect.
     * @param code  The shader code string.
     */
    void SetUpdateCode(const std::string& code);

    /**
     * @brief Create a particle effect from the builder configuration.
     * @param maxParticleSize  The maximum particle size.
     * @return Shared pointer to the created ParticleEffect.
     */
    std::shared_ptr<ParticleEffect> MakeParticleEffect(uint32_t maxParticleSize);

private:
    std::shared_ptr<ParticleBuilderImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
