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

#ifndef RENDER_SERVICE_BASE_ANIMATION_RS_RENDER_PARTICLE_POOL_H
#define RENDER_SERVICE_BASE_ANIMATION_RS_RENDER_PARTICLE_POOL_H

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "animation/rs_render_particle.h"
#include "common/rs_common_def.h"

#if defined(__aarch64__) && !defined(__arm__) && defined(__ARM_NEON)
#include <arm_neon.h>
#endif

namespace OHOS {
namespace Rosen {

// Owns the particle data. There is no per-particle object: every operation is (pool, slot).
class RSB_EXPORT RSRenderParticlePool {
public:
    static constexpr size_t INITIAL_CAPACITY = 1024;
    static constexpr size_t SIMD_LANES = 4;
    static constexpr size_t INVALID_SLOT = static_cast<size_t>(-1);
    static constexpr size_t CAPACITY_GROWTH_FACTOR = 2;
    static constexpr float MAX_COLOR_COMPONENT = 255.0f;
    static constexpr float DEGREE_TO_RADIAN = static_cast<float>(M_PI / 180.0);
    static constexpr uint8_t FLAG_ALIVE = 0u;
    static constexpr uint8_t FLAG_DEAD = 1u;

    void Resize(size_t capacity)
    {
        data_.Resize(capacity);
        capacity_ = capacity;
        count_ = std::min(count_, capacity_);
    }

    size_t Capacity() const
    {
        return capacity_;
    }

    size_t Count() const
    {
        return count_;
    }

    size_t ActiveCount() const
    {
        return count_;
    }

    // Reserves a slot and clears its rows; the caller writes the initial state through the arrays.
    size_t Acquire();

    void Clear()
    {
        count_ = 0;
        hasDead_ = false;
        slotMap_.clear();
        generation_++;
        for (size_t i = 0; i < capacity_; i++) {
            data_.dead[i] = FLAG_ALIVE;
        }
    }

    void MarkDead(size_t slot)
    {
        data_.dead[slot] = FLAG_DEAD;
        hasDead_ = true;
    }

    bool IsAlive(size_t slot) const
    {
        if (data_.dead[slot] == FLAG_DEAD) {
            return false;
        }
        const float life = data_.lifeTime[slot];
        if (life < 0.0f) {
            return true;
        }
        return data_.activeTime[slot] < life;
    }

    bool HasDead() const
    {
        return hasDead_;
    }

    void Update(float deltaTime);

    // Swap-removes every dead slot. No slot is referenced from outside, so there is nothing to renumber.
    void Compact();

    RSRenderParticle& Data()
    {
        return data_;
    }

    const RSRenderParticle& Data() const
    {
        return data_;
    }

    // Incremented when Compact moves survivors; drawable uses SlotMap() to remap its ordered slot list.
    uint32_t Generation() const
    {
        return generation_;
    }
    const std::vector<size_t>& SlotMap() const
    {
        return slotMap_;
    }

    // Slot accessors. Callers keep the slot, never a particle object.
    Vector2f GetPosition(size_t i) const
    {
        return Vector2f(data_.posX[i], data_.posY[i]);
    }
    void SetPosition(size_t i, const Vector2f& position)
    {
        data_.posX[i] = position.x_;
        data_.posY[i] = position.y_;
    }
    Vector2f GetVelocity(size_t i) const
    {
        return Vector2f(data_.velX[i], data_.velY[i]);
    }
    void SetVelocity(size_t i, const Vector2f& velocity)
    {
        data_.velX[i] = velocity.x_;
        data_.velY[i] = velocity.y_;
    }
    Vector2f GetAcceleration(size_t i) const
    {
        return Vector2f(data_.accX[i], data_.accY[i]);
    }
    void SetAcceleration(size_t i, const Vector2f& acceleration)
    {
        data_.accX[i] = acceleration.x_;
        data_.accY[i] = acceleration.y_;
    }
    int64_t GetActiveTime(size_t i) const
    {
        return static_cast<int64_t>(data_.activeTime[i] * NS_TO_S);
    }
    void SetActiveTime(size_t i, int64_t activeTime)
    {
        data_.activeTime[i] = static_cast<float>(activeTime) / NS_TO_S;
    }
    int64_t GetLifeTime(size_t i) const
    {
        return static_cast<int64_t>(data_.lifeTime[i] * NS_TO_S);
    }
    void SetLifeTime(size_t i, int64_t lifeTime)
    {
        data_.lifeTime[i] = static_cast<float>(lifeTime) / NS_TO_S;
    }
    float GetScale(size_t i) const
    {
        return data_.scale[i];
    }
    void SetScale(size_t i, float scale)
    {
        data_.scale[i] = scale;
    }
    float GetOpacity(size_t i) const
    {
        return data_.opacity[i];
    }
    void SetOpacity(size_t i, float opacity)
    {
        data_.opacity[i] = opacity;
    }
    float GetSpin(size_t i) const
    {
        return data_.spin[i];
    }
    void SetSpin(size_t i, float spin)
    {
        data_.spin[i] = spin;
        data_.spinCos[i] = std::cos(spin * DEGREE_TO_RADIAN);
        data_.spinSin[i] = std::sin(spin * DEGREE_TO_RADIAN);
    }
    void GetSpinTrig(size_t i, float& cosValue, float& sinValue) const
    {
        cosValue = data_.spinCos[i];
        sinValue = data_.spinSin[i];
    }
    Color GetColor(size_t i) const
    {
        return Color(static_cast<int16_t>(data_.colorR[i]), static_cast<int16_t>(data_.colorG[i]),
            static_cast<int16_t>(data_.colorB[i]), static_cast<int16_t>(data_.colorA[i]));
    }
    void SetColor(size_t i, const Color& color)
    {
        data_.colorR[i] = static_cast<float>(color.GetRed());
        data_.colorG[i] = static_cast<float>(color.GetGreen());
        data_.colorB[i] = static_cast<float>(color.GetBlue());
        data_.colorA[i] = static_cast<float>(color.GetAlpha());
    }
    float GetRadius(size_t i) const
    {
        return data_.radius[i];
    }
    void SetRadius(size_t i, float radius)
    {
        data_.radius[i] = radius;
    }
    float GetDepth(size_t i) const
    {
        return data_.depth[i];
    }
    void SetDepth(size_t i, float depth)
    {
        data_.depth[i] = depth;
    }
    float GetImageDrawWidth(size_t i) const
    {
        return data_.imageDrawWidth[i];
    }
    void SetImageDrawWidth(size_t i, float width)
    {
        data_.imageDrawWidth[i] = width;
    }
    Vector2f GetImageSize(size_t i) const
    {
        return Vector2f(data_.imageSizeX[i], data_.imageSizeY[i]);
    }
    void SetImageSize(size_t i, const Vector2f& size)
    {
        data_.imageSizeX[i] = size.x_;
        data_.imageSizeY[i] = size.y_;
    }
    uint32_t GetImageIndex(size_t i) const
    {
        return data_.imageIndex[i];
    }
    void SetImageIndex(size_t i, uint32_t imageIndex)
    {
        data_.imageIndex[i] = imageIndex;
    }
    uint32_t GetDrawStamp(size_t i) const
    {
        return data_.drawStamp[i];
    }
    void SetDrawStamp(size_t i, uint32_t stamp)
    {
        data_.drawStamp[i] = stamp;
    }
    ParticleType GetParticleType(size_t i) const
    {
        return static_cast<ParticleType>(data_.particleType[i]);
    }
    void SetParticleType(size_t i, ParticleType type)
    {
        data_.particleType[i] = static_cast<uint32_t>(type);
    }
    uint32_t GetParamsIndex(size_t i) const
    {
        return data_.paramsIndex[i];
    }
    float GetAccelerationValue(size_t i) const
    {
        return data_.accelValue[i];
    }
    void SetAccelerationValue(size_t i, float value)
    {
        data_.accelValue[i] = value;
    }
    float GetAccelerationAngle(size_t i) const
    {
        return data_.accelAngle[i];
    }
    void SetAccelerationAngle(size_t i, float angle)
    {
        data_.accelAngle[i] = angle;
    }
    float GetOpacitySpeed(size_t i) const
    {
        return data_.opacitySpeed[i];
    }
    void SetOpacitySpeed(size_t i, float speed)
    {
        data_.opacitySpeed[i] = speed;
    }
    float GetScaleSpeed(size_t i) const
    {
        return data_.scaleSpeed[i];
    }
    void SetScaleSpeed(size_t i, float speed)
    {
        data_.scaleSpeed[i] = speed;
    }
    float GetSpinSpeed(size_t i) const
    {
        return data_.spinSpeed[i];
    }
    void SetSpinSpeed(size_t i, float speed)
    {
        data_.spinSpeed[i] = speed;
    }

private:
    void UpdateOne(size_t i, float deltaTime);
    bool HasDeadLane(size_t start) const;
#if defined(__aarch64__) && !defined(__arm__) && defined(__ARM_NEON)
    void UpdateSimd(float deltaTime);
    void MarkExpiredLanes(size_t start, uint32x4_t expired);
#endif

    RSRenderParticle data_;
    size_t capacity_ = 0;
    size_t count_ = 0;
    bool hasDead_ = false;
    uint32_t generation_ = 0;
    std::vector<size_t> slotMap_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_ANIMATION_RS_RENDER_PARTICLE_POOL_H
