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

#include "animation/rs_render_particle_pool.h"

namespace OHOS {
namespace Rosen {

size_t RSRenderParticlePool::Acquire()
{
    if (count_ >= capacity_) {
        const size_t grown = capacity_ == 0 ? INITIAL_CAPACITY : capacity_ * CAPACITY_GROWTH_FACTOR;
        Resize(grown);
    }
    const size_t slot = count_++;
    data_.ClearSlot(slot);
    data_.scale[slot] = 1.0f;
    data_.opacity[slot] = 1.0f;
    data_.spinCos[slot] = 1.0f;
    data_.colorR[slot] = MAX_COLOR_COMPONENT;
    data_.colorG[slot] = MAX_COLOR_COMPONENT;
    data_.colorB[slot] = MAX_COLOR_COMPONENT;
    data_.colorA[slot] = MAX_COLOR_COMPONENT;
    return slot;
}

void RSRenderParticlePool::Compact()
{
    if (count_ == 0) {
        hasDead_ = false;
        slotMap_.clear();
        return;
    }
    slotMap_.assign(count_, INVALID_SLOT);
    size_t write = 0;
    for (size_t i = 0; i < count_; i++) {
        if (data_.dead[i] == FLAG_DEAD) {
            continue;
        }
        if (write != i) {
            data_.CopySlot(write, i);
        }
        slotMap_[i] = write++;
    }
    count_ = write;
    hasDead_ = false;
    generation_++;
}
void RSRenderParticlePool::UpdateOne(size_t i, float deltaTime)
{
    if (!IsAlive(i)) {
        return;
    }
    const float accX = data_.accX[i];
    const float accY = data_.accY[i];
    const float velX = data_.velX[i] + accX * deltaTime;
    const float velY = data_.velY[i] + accY * deltaTime;
    data_.velX[i] = velX;
    data_.velY[i] = velY;
    data_.posX[i] += (velX + data_.forceX[i]) * deltaTime;
    data_.posY[i] += (velY + data_.forceY[i]) * deltaTime;
    const float active = data_.activeTime[i] + deltaTime;
    data_.activeTime[i] = active;
    const float life = data_.lifeTime[i];
    if (life > 0.0f && active >= life) {
        MarkDead(i);
    }
}

bool RSRenderParticlePool::HasDeadLane(size_t start) const
{
    for (size_t lane = start; lane < start + SIMD_LANES; lane++) {
        if (data_.dead[lane] == FLAG_DEAD) {
            return true;
        }
    }
    return false;
}

#if defined(__aarch64__) && !defined(__arm__) && defined(__ARM_NEON)
void RSRenderParticlePool::MarkExpiredLanes(size_t start, uint32x4_t expired)
{
    uint32_t lanes[SIMD_LANES] = { 0u, 0u, 0u, 0u };
    vst1q_u32(lanes, expired);
    for (size_t lane = 0; lane < SIMD_LANES; lane++) {
        if (lanes[lane] != 0u) {
            MarkDead(start + lane);
        }
    }
}

void RSRenderParticlePool::UpdateSimd(float deltaTime)
{
    const float32x4_t dt = vdupq_n_f32(deltaTime);
    const float32x4_t zero = vdupq_n_f32(0.0f);
    const size_t simdEnd = count_ & ~(SIMD_LANES - 1);
    size_t i = 0;
    for (; i < simdEnd; i += SIMD_LANES) {
        if (HasDeadLane(i)) {
            for (size_t lane = i; lane < i + SIMD_LANES; lane++) {
                UpdateOne(lane, deltaTime);
            }
            continue;
        }
        float* posX = data_.posX.data();
        float* posY = data_.posY.data();
        float* velX = data_.velX.data();
        float* velY = data_.velY.data();
        float* accX = data_.accX.data();
        float* accY = data_.accY.data();
        float* forceX = data_.forceX.data();
        float* forceY = data_.forceY.data();
        float* active = data_.activeTime.data();
        float* life = data_.lifeTime.data();
        float32x4_t vx = vmlaq_f32(vld1q_f32(velX + i), vld1q_f32(accX + i), dt);
        float32x4_t vy = vmlaq_f32(vld1q_f32(velY + i), vld1q_f32(accY + i), dt);
        vst1q_f32(velX + i, vx);
        vst1q_f32(velY + i, vy);
        float32x4_t px = vmlaq_f32(vld1q_f32(posX + i), vaddq_f32(vx, vld1q_f32(forceX + i)), dt);
        float32x4_t py = vmlaq_f32(vld1q_f32(posY + i), vaddq_f32(vy, vld1q_f32(forceY + i)), dt);
        vst1q_f32(posX + i, px);
        vst1q_f32(posY + i, py);
        float32x4_t age = vaddq_f32(vld1q_f32(active + i), dt);
        vst1q_f32(active + i, age);
        const float32x4_t lifeVec = vld1q_f32(life + i);
        const uint32x4_t expired = vandq_u32(vcgeq_f32(age, lifeVec), vcgtq_f32(lifeVec, zero));
        if (vmaxvq_u32(expired) != 0u) {
            MarkExpiredLanes(i, expired);
        }
    }
    for (; i < count_; i++) {
        UpdateOne(i, deltaTime);
    }
}
#endif

void RSRenderParticlePool::Update(float deltaTime)
{
    if (count_ == 0) {
        return;
    }
#if defined(__aarch64__) && !defined(__arm__) && defined(__ARM_NEON)
    UpdateSimd(deltaTime);
#else
    for (size_t i = 0; i < count_; i++) {
        UpdateOne(i, deltaTime);
    }
#endif
}

} // namespace Rosen
} // namespace OHOS
