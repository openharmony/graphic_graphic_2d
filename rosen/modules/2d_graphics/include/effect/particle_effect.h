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

#ifndef PARTICLE_EFFECT_H
#define PARTICLE_EFFECT_H

#include "utils/drawing_macros.h"

#include <memory>

#include "drawing/engine_adapter/impl_interface/particle_effect_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class DRAWING_API ParticleEffect {
public:
    /**
     * @brief Construct a new Particle Effect object.
     */
    ParticleEffect();
    virtual ~ParticleEffect() = default;
    ParticleEffect(std::shared_ptr<ParticleEffectImpl> impl) : impl_(impl) {}

    /**
     * @brief Update the configuration data for the particle effect.
     * @param name  The name of the configuration parameter.
     * @param data  Pointer to the data to update.
     * @param offset Offset in the data buffer.
     * @param size  Size of the data to update.
     */
    void UpdateConfigData(const std::string& name, const void* data, uint32_t offset, uint32_t size);

    /**
     * @brief Update the configuration image for the particle effect.
     * @param name  The name of the image.
     * @param image Shared pointer to the image to update.
     */
    void UpdateConfigImage(const std::string& name, std::shared_ptr<Image> image);

    /**
     * @brief Get the implementation object of the particle effect.
     * @tparam T   The type of the implementation object.
     * @return     Pointer to the implementation object.
     */
    template<typename T>
    T* GetImpl()
    {
        return static_cast<T*>(impl_.get());
    }

    /**
     * @brief Serialize the particle effect to data.
     * @return Shared pointer to the serialized data.
     */
    std::shared_ptr<Data> Serialize() const;

    /**
     * @brief Deserialize the particle effect from data.
     * @param data  Shared pointer to the data to deserialize.
     * @return      True if deserialization succeeded, false otherwise.
     */
    bool Deserialize(std::shared_ptr<Data> data);

private:
    std::shared_ptr<ParticleEffectImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // PARTICLE_EFFECT_H
