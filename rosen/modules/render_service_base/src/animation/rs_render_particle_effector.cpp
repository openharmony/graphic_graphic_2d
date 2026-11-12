/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "animation/rs_render_particle_effector.h"

#include "animation/rs_render_particle_pool.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#ifdef ENABLE_RUST
#include "cxx.h"
#include "lib.rs.h"
#endif

#include "animation/rs_value_estimator.h"
namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t COLOR_CHANNEL_COUNT = 4;
constexpr float HALF = 0.5f;
} // namespace
constexpr float DEGREE_TO_RADIAN = M_PI / 180;
namespace {
Color ReadColor(const RSRenderParticle& data, size_t slot)
{
    return Color(static_cast<int16_t>(data.colorR[slot]),
        static_cast<int16_t>(data.colorG[slot]),
        static_cast<int16_t>(data.colorB[slot]),
        static_cast<int16_t>(data.colorA[slot]));
}

void WriteColor(RSRenderParticle& data, size_t slot, const Color& color)
{
    data.colorR[slot] = static_cast<float>(color.GetRed());
    data.colorG[slot] = static_cast<float>(color.GetGreen());
    data.colorB[slot] = static_cast<float>(color.GetBlue());
    data.colorA[slot] = static_cast<float>(color.GetAlpha());
}

int64_t ActiveTimeMillis(const RSRenderParticle& data, size_t slot)
{
    return static_cast<int64_t>(data.activeTime[slot] * NS_TO_S) / NS_PER_MS;
}

bool IsAccelerationCurrent(const RSRenderParticle& data, size_t slot, float value, float angle)
{
    return data.accelCacheValid[slot] != 0u &&
           ROSEN_EQ(data.accelCacheValue[slot], value) &&
           ROSEN_EQ(data.accelCacheAngle[slot], angle);
}

void SetAccelerationCache(RSRenderParticle& data, size_t slot, float value, float angle)
{
    data.accelCacheValue[slot] = value;
    data.accelCacheAngle[slot] = angle;
    data.accelCacheValid[slot] = 1u;
}

Color LerpColor(const Color& start, const Color& end, float t)
{
    Color result = start;
    result.SetRed(static_cast<int16_t>(std::round(start.GetRed() + (end.GetRed() - start.GetRed()) * t)));
    result.SetGreen(static_cast<int16_t>(std::round(start.GetGreen() + (end.GetGreen() - start.GetGreen()) * t)));
    result.SetBlue(static_cast<int16_t>(std::round(start.GetBlue() + (end.GetBlue() - start.GetBlue()) * t)));
    result.SetAlpha(static_cast<int16_t>(std::round(start.GetAlpha() + (end.GetAlpha() - start.GetAlpha()) * t)));
    return result;
}
} // namespace

namespace {
// Advances one RANDOM row over a slot group. The scalar pass decides kill/skip per lane, the arithmetic is 4-lane.
enum class RandomAction { UPDATE, SKIP, KILL };

// Configures how a random-driven value advances over its lifetime, instead of
// coupling two boolean "semantic switches" (clamp / kill) through every call site.
enum class RandomUpdatePolicy {
    ACCUMULATE,             // value += speed * deltaTime, unbounded
    CLAMP_UNIT,             // value += speed * deltaTime, clamped to [0,1], skip once saturated
    KILL_AT_REST,           // value += speed * deltaTime, kill particle when value and speed rest at <= 0
    CLAMP_AND_KILL_AT_REST, // clamp to [0,1] and kill at rest
};

// A random-driven attribute: the per-particle current value and its rate of change.
// Bundled to keep the update entry point within the parameter-count limit.
struct RandomUpdateValue {
    std::vector<float>& values;
    std::vector<float>& speeds;
};

bool ClampToUnit(RandomUpdatePolicy policy)
{
    return policy == RandomUpdatePolicy::CLAMP_UNIT ||
           policy == RandomUpdatePolicy::CLAMP_AND_KILL_AT_REST;
}

bool KillAtRest(RandomUpdatePolicy policy)
{
    return policy == RandomUpdatePolicy::KILL_AT_REST ||
           policy == RandomUpdatePolicy::CLAMP_AND_KILL_AT_REST;
}

RandomAction ClassifyRandom(float value, float speed, RandomUpdatePolicy policy)
{
    if (KillAtRest(policy) && value <= 0.0f && speed <= 0.0f) {
        return RandomAction::KILL;
    }
    if (ClampToUnit(policy) && value >= 1.0f && speed >= 0.0f) {
        return RandomAction::SKIP;
    }
    return RandomAction::UPDATE;
}

void ApplyRandomScalar(RSRenderParticlePool& pool, RandomUpdateValue& value, size_t slot,
    float deltaTime, RandomUpdatePolicy policy)
{
    const float cur = value.values[slot];
    const float speed = value.speeds[slot];
    const RandomAction action = ClassifyRandom(cur, speed, policy);
    if (action == RandomAction::KILL) {
        pool.MarkDead(slot);
        return;
    }
    if (action == RandomAction::SKIP) {
        return;
    }
    float next = cur + speed * deltaTime;
    if (ClampToUnit(policy)) {
        next = std::clamp(next, 0.0f, 1.0f);
    }
    value.values[slot] = next;
}

#if defined(__aarch64__) && !defined(__arm__) && defined(__ARM_NEON)
void ApplyRandomChunk(RSRenderParticlePool& pool, RandomUpdateValue& value, const size_t* slots,
    float deltaTime, RandomUpdatePolicy policy)
{
    float in[RSRenderParticlePool::SIMD_LANES];
    float sp[RSRenderParticlePool::SIMD_LANES];
    float out[RSRenderParticlePool::SIMD_LANES];
    size_t laneSlots[RSRenderParticlePool::SIMD_LANES];
    for (size_t lane = 0; lane < RSRenderParticlePool::SIMD_LANES; lane++) {
        laneSlots[lane] = slots[lane];
        in[lane] = value.values[laneSlots[lane]];
        sp[lane] = value.speeds[laneSlots[lane]];
    }
    float32x4_t next = vmlaq_f32(vld1q_f32(in), vld1q_f32(sp), vdupq_n_f32(deltaTime));
    if (ClampToUnit(policy)) {
        next = vmaxq_f32(vminq_f32(next, vdupq_n_f32(1.0f)), vdupq_n_f32(0.0f));
    }
    vst1q_f32(out, next);
    for (size_t lane = 0; lane < RSRenderParticlePool::SIMD_LANES; lane++) {
        const RandomAction action = ClassifyRandom(in[lane], sp[lane], policy);
        if (action == RandomAction::KILL) {
            pool.MarkDead(laneSlots[lane]);
        } else if (action == RandomAction::UPDATE) {
            value.values[laneSlots[lane]] = out[lane];
        }
    }
}
#endif

void UpdateRandomValues(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
    RandomUpdateValue& value, float deltaTime, RandomUpdatePolicy policy)
{
#if defined(__aarch64__) && !defined(__arm__) && defined(__ARM_NEON)
    size_t i = 0;
    for (; i + RSRenderParticlePool::SIMD_LANES <= slots.size(); i += RSRenderParticlePool::SIMD_LANES) {
        ApplyRandomChunk(pool, value, slots.data() + i, deltaTime, policy);
    }
    for (; i < slots.size(); i++) {
        ApplyRandomScalar(pool, value, slots[i], deltaTime, policy);
    }
#else
    for (const size_t slot : slots) {
        ApplyRandomScalar(pool, value, slot, deltaTime, policy);
    }
#endif
}
} // namespace

RSRenderParticleEffector::RSRenderParticleEffector() {}

Vector4<int16_t> RSRenderParticleEffector::CalculateColorInt(RSRenderParticlePool& pool, size_t slot,
    Vector4<int16_t> colorInt, Vector4<float> colorF, Vector4<float> colorSpeed, float deltaTime)
{
    auto& data = pool.Data();
    float* colorFloats[COLOR_CHANNEL_COUNT] = { data.redF.data(), data.greenF.data(), data.blueF.data(),
        data.alphaF.data() };
    for (size_t i = 0; i < COLOR_CHANNEL_COUNT; i++) {
        if ((colorInt.data_[i] <= 0 && colorSpeed.data_[i] <= 0.f) ||
            (colorInt.data_[i] >= UINT8_MAX && colorSpeed.data_[i] >= 0.f)) {
            continue;
        }
        colorF.data_[i] += colorSpeed.data_[i] * deltaTime;
        if (std::abs(colorF.data_[i]) >= 1.f) {
            colorInt.data_[i] += static_cast<int16_t>(colorF.data_[i]);
            colorF.data_[i] -= std::floor(colorF.data_[i]);
        }
        colorFloats[i][slot] = colorF.data_[i];
        colorInt.data_[i] = std::clamp<int16_t>(colorInt.data_[i], 0, UINT8_MAX);
    }
    return colorInt;
}

void RSRenderParticleEffector::UpdateCurveValue(
    float& value, const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& valChangeOverLife, int64_t activeTime)
{
    bool hasPassed = false;
    float passedEndValue = value;
    for (size_t i = 0; i < valChangeOverLife.size(); i++) {
        if (valChangeOverLife[i] == nullptr) {
            continue;
        }
        int startTime = valChangeOverLife[i]->startMillis_;
        int endTime = valChangeOverLife[i]->endMillis_;
        if (activeTime > endTime) {
            hasPassed = true;
            passedEndValue = valChangeOverLife[i]->toValue_;
            continue;
        }
        if (activeTime < startTime) {
            continue;
        }
        float startValue = valChangeOverLife[i]->fromValue_;
        float endValue = valChangeOverLife[i]->toValue_;
#ifdef ENABLE_RUST
        value = generate_value(startValue, endValue, startTime, endTime, activeTime);
#else
        if (endTime - startTime > 0) {
            float t = static_cast<float>(activeTime - startTime) / static_cast<float>(endTime - startTime);
            auto& interpolator = valChangeOverLife[i]->interpolator_;
            t = (interpolator != nullptr) ? interpolator->Interpolate(t) : t;
            value = startValue * (1.0f - t) + endValue * t;
        }
#endif
        return;
    }
    if (hasPassed) {
        value = passedEndValue;
    }
}

void RSRenderParticleEffector::UpdateColorCurveValue(
    Color& color, const std::vector<std::shared_ptr<ChangeInOverLife<Color>>>& valChangeOverLife, int64_t activeTime)
{
    bool hasPassed = false;
    Color passedEndValue;
    for (size_t i = 0; i < valChangeOverLife.size(); i++) {
        if (valChangeOverLife[i] == nullptr) {
            continue;
        }
        int startTime = valChangeOverLife[i]->startMillis_;
        int endTime = valChangeOverLife[i]->endMillis_;
        if (activeTime > endTime) {
            hasPassed = true;
            passedEndValue = valChangeOverLife[i]->toValue_;
            continue;
        }
        if (activeTime < startTime) {
            continue;
        }
        if (endTime - startTime > 0) {
            Color startValue = valChangeOverLife[i]->fromValue_;
            Color endValue = valChangeOverLife[i]->toValue_;
            float t = static_cast<float>(activeTime - startTime) / static_cast<float>(endTime - startTime);
            auto& interpolator = valChangeOverLife[i]->interpolator_;
            t = (interpolator != nullptr) ? interpolator->Interpolate(t) : t;
            color = LerpColor(startValue, endValue, t);
        }
        color = Color(color.AsRgbaInt());
        return;
    }
    if (hasPassed) {
        color = Color(passedEndValue.AsRgbaInt());
    }
}

void RSRenderParticleEffector::UpdateColor(
    RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, float deltaTime)
{
    if (params.GetParticleType() == ParticleType::IMAGES) {
        return;
    }
    auto& data = pool.Data();
    Color color = ReadColor(data, slot);
    const auto colorUpdator = params.GetColorUpdator();
    if (colorUpdator == ParticleUpdator::RANDOM) {
        auto colorInt = Vector4<int16_t>(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
        auto colorSpeed = Vector4f(data.redSpeed[slot],
            data.greenSpeed[slot], data.blueSpeed[slot],
            data.alphaSpeed[slot]);
        auto colorF = Vector4f(data.redF[slot], data.greenF[slot],
            data.blueF[slot], data.alphaF[slot]);
        colorInt = CalculateColorInt(pool, slot, colorInt, colorF, colorSpeed, deltaTime);
        color.SetRed(colorInt.x_);
        color.SetGreen(colorInt.y_);
        color.SetBlue(colorInt.z_);
        color.SetAlpha(colorInt.w_);
    } else if (colorUpdator == ParticleUpdator::CURVE) {
        auto& valChangeOverLife = params.GetColorChangeOverLife();
        UpdateColorCurveValue(color, valChangeOverLife, ActiveTimeMillis(data, slot));
    }
    WriteColor(data, slot, color);
}

void RSRenderParticleEffector::UpdateOpacity(
    RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, float deltaTime)
{
    auto& data = pool.Data();
    float opacity = data.opacity[slot];
    const auto opacityUpdator = params.GetOpacityUpdator();
    if (opacityUpdator == ParticleUpdator::RANDOM) {
        const float opacitySpeed = data.opacitySpeed[slot];
        if (opacity <= 0 && opacitySpeed <= 0) {
            pool.MarkDead(slot);
            return;
        }
        if (opacity >= 1.0 && opacitySpeed >= 0.f) {
            return;
        }
        opacity = std::clamp<float>(opacity + opacitySpeed * deltaTime, 0.f, 1.f);
    } else if (opacityUpdator == ParticleUpdator::CURVE) {
        auto& valChangeOverLife = params.GetOpacityChangeOverLife();
        UpdateCurveValue(opacity, valChangeOverLife, ActiveTimeMillis(data, slot));
    }
    data.opacity[slot] = opacity;
}

void RSRenderParticleEffector::UpdateScale(
    RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, float deltaTime)
{
    auto& data = pool.Data();
    float scale = data.scale[slot];
    const auto scaleUpdator = params.GetScaleUpdator();
    if (scaleUpdator == ParticleUpdator::RANDOM) {
        const float scaleSpeed = data.scaleSpeed[slot];
        if (scale <= 0 && scaleSpeed <= 0) {
            pool.MarkDead(slot);
            return;
        }
        scale += scaleSpeed * deltaTime;
    } else if (scaleUpdator == ParticleUpdator::CURVE) {
        auto& valChangeOverLife = params.GetScaleChangeOverLife();
        UpdateCurveValue(scale, valChangeOverLife, ActiveTimeMillis(data, slot));
    }
    data.scale[slot] = scale;
}

void RSRenderParticleEffector::UpdateSpin(
    RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, float deltaTime)
{
    auto& data = pool.Data();
    const float oldSpin = data.spin[slot];
    float spin = oldSpin;
    const auto spinUpdator = params.GetSpinUpdator();
    if (spinUpdator == ParticleUpdator::RANDOM) {
        spin += data.spinSpeed[slot] * deltaTime;
    } else if (spinUpdator == ParticleUpdator::CURVE) {
        auto& valChangeOverLife = params.GetSpinChangeOverLife();
        UpdateCurveValue(spin, valChangeOverLife, ActiveTimeMillis(data, slot));
    }
    if (spin != oldSpin) {
        data.spin[slot] = spin;
        data.spinCos[slot] = std::cos(spin * DEGREE_TO_RADIAN);
        data.spinSin[slot] = std::sin(spin * DEGREE_TO_RADIAN);
    }
}

void RSRenderParticleEffector::UpdateAccelerationAngle(
    RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, float deltaTime)
{
    auto& data = pool.Data();
    float accelerationAngle = data.accelAngle[slot];
    const auto acceAngleUpdator = params.GetAccelerationAngleUpdator();
    if (acceAngleUpdator == ParticleUpdator::RANDOM) {
        accelerationAngle += data.accelAngleSpeed[slot] * deltaTime;
    } else if (acceAngleUpdator == ParticleUpdator::CURVE) {
        auto& valChangeOverLife = params.GetAcceAngChangeOverLife();
        UpdateCurveValue(accelerationAngle, valChangeOverLife, ActiveTimeMillis(data, slot));
    }
    data.accelAngle[slot] = accelerationAngle;
}

void RSRenderParticleEffector::UpdateAccelerationValue(
    RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, float deltaTime)
{
    auto& data = pool.Data();
    float accelerationValue = data.accelValue[slot];
    const auto acceValueUpdator = params.GetAccelerationValueUpdator();
    if (acceValueUpdator == ParticleUpdator::RANDOM) {
        accelerationValue += data.accelValueSpeed[slot] * deltaTime;
    } else if (acceValueUpdator == ParticleUpdator::CURVE) {
        auto& valChangeOverLife = params.GetAcceValChangeOverLife();
        UpdateCurveValue(accelerationValue, valChangeOverLife, ActiveTimeMillis(data, slot));
    }
    data.accelValue[slot] = accelerationValue;
}

void RSRenderParticleEffector::UpdatePosition(RSRenderParticlePool& pool, size_t slot,
    const std::shared_ptr<ParticleFieldCollection>& fields, float deltaTime)
{
    auto& data = pool.Data();
    const Vector2f position(data.posX[slot], data.posY[slot]);
    Vector2f fieldForce(0.f, 0.f);
    if (fields) {
        fieldForce = fields->ApplyAll(position, deltaTime);
    }
    const float accelerationValue = data.accelValue[slot];
    const float accelerationAngle = data.accelAngle[slot];
    if (!IsAccelerationCurrent(data, slot, accelerationValue, accelerationAngle)) {
        const float radian = accelerationAngle * DEGREE_TO_RADIAN;
        data.accX[slot] = accelerationValue * std::cos(radian);
        data.accY[slot] = accelerationValue * std::sin(radian);
        SetAccelerationCache(data, slot, accelerationValue, accelerationAngle);
    }
    // The pool integration advances velocity and position; here only the field force is published.
    data.forceX[slot] = fieldForce.x_;
    data.forceY[slot] = fieldForce.y_;
}

void RSRenderParticleEffector::Update(RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params,
    const std::shared_ptr<ParticleFieldCollection>& fields, int64_t deltaTime)
{
    if (slot >= pool.Count()) {
        return;
    }
    const float dt = static_cast<float>(deltaTime) / NS_TO_S;
    if (params.GetAccelerationValueUpdator() != ParticleUpdator::NONE) {
        UpdateAccelerationValue(pool, slot, params, dt);
    }
    if (params.GetAccelerationAngleUpdator() != ParticleUpdator::NONE) {
        UpdateAccelerationAngle(pool, slot, params, dt);
    }
    if (params.GetParticleType() != ParticleType::IMAGES && params.GetColorUpdator() != ParticleUpdator::NONE) {
        UpdateColor(pool, slot, params, dt);
    }
    if (params.GetOpacityUpdator() != ParticleUpdator::NONE) {
        UpdateOpacity(pool, slot, params, dt);
    }
    if (params.GetScaleUpdator() != ParticleUpdator::NONE) {
        UpdateScale(pool, slot, params, dt);
    }
    if (params.GetSpinUpdator() != ParticleUpdator::NONE) {
        UpdateSpin(pool, slot, params, dt);
    }
    UpdatePosition(pool, slot, fields, dt);
}

void RSRenderParticleEffector::UpdateAccelerationGroup(RSRenderParticlePool& pool,
    const std::vector<size_t>& slots, ParticleRenderParams& params, float deltaTime)
{
    auto& data = pool.Data();
    const auto valueUpdator = params.GetAccelerationValueUpdator();
    if (valueUpdator == ParticleUpdator::RANDOM) {
        RandomUpdateValue accelValue{data.accelValue, data.accelValueSpeed};
        UpdateRandomValues(pool, slots, accelValue, deltaTime, RandomUpdatePolicy::ACCUMULATE);
    } else if (valueUpdator == ParticleUpdator::CURVE) {
        for (const size_t slot : slots) {
            UpdateAccelerationValue(pool, slot, params, deltaTime);
        }
    }
    const auto angleUpdator = params.GetAccelerationAngleUpdator();
    if (angleUpdator == ParticleUpdator::RANDOM) {
        RandomUpdateValue accelAngle{data.accelAngle, data.accelAngleSpeed};
        UpdateRandomValues(pool, slots, accelAngle, deltaTime, RandomUpdatePolicy::ACCUMULATE);
    } else if (angleUpdator == ParticleUpdator::CURVE) {
        for (const size_t slot : slots) {
            UpdateAccelerationAngle(pool, slot, params, deltaTime);
        }
    }
}

void RSRenderParticleEffector::UpdateColorGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
    ParticleRenderParams& params, float deltaTime)
{
    if (params.GetParticleType() == ParticleType::IMAGES ||
        params.GetColorUpdator() == ParticleUpdator::NONE) {
        return;
    }
    for (const size_t slot : slots) {
        UpdateColor(pool, slot, params, deltaTime);
    }
}

void RSRenderParticleEffector::UpdateOpacityGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
    ParticleRenderParams& params, float deltaTime)
{
    const auto opacityUpdator = params.GetOpacityUpdator();
    if (opacityUpdator == ParticleUpdator::RANDOM) {
        auto& data = pool.Data();
        RandomUpdateValue opacity{data.opacity, data.opacitySpeed};
        UpdateRandomValues(pool, slots, opacity, deltaTime, RandomUpdatePolicy::CLAMP_AND_KILL_AT_REST);
    } else if (opacityUpdator == ParticleUpdator::CURVE) {
        for (const size_t slot : slots) {
            UpdateOpacity(pool, slot, params, deltaTime);
        }
    }
}

void RSRenderParticleEffector::UpdateScaleGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
    ParticleRenderParams& params, float deltaTime)
{
    const auto scaleUpdator = params.GetScaleUpdator();
    if (scaleUpdator == ParticleUpdator::RANDOM) {
        auto& data = pool.Data();
        RandomUpdateValue scale{data.scale, data.scaleSpeed};
        UpdateRandomValues(pool, slots, scale, deltaTime, RandomUpdatePolicy::KILL_AT_REST);
    } else if (scaleUpdator == ParticleUpdator::CURVE) {
        for (const size_t slot : slots) {
            UpdateScale(pool, slot, params, deltaTime);
        }
    }
}

void RSRenderParticleEffector::UpdateSpinGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
    ParticleRenderParams& params, float deltaTime)
{
    auto& data = pool.Data();
    const auto spinUpdator = params.GetSpinUpdator();
    if (spinUpdator == ParticleUpdator::RANDOM) {
        RandomUpdateValue spin{data.spin, data.spinSpeed};
        UpdateRandomValues(pool, slots, spin, deltaTime, RandomUpdatePolicy::ACCUMULATE);
        for (const size_t slot : slots) {
            if (data.spinSpeed[slot] != 0.0f) {
                const float spin = data.spin[slot];
                data.spinCos[slot] = std::cos(spin * DEGREE_TO_RADIAN);
                data.spinSin[slot] = std::sin(spin * DEGREE_TO_RADIAN);
            }
        }
    } else if (spinUpdator == ParticleUpdator::CURVE) {
        for (const size_t slot : slots) {
            UpdateSpin(pool, slot, params, deltaTime);
        }
    }
}

void RSRenderParticleEffector::UpdatePositionGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
    const std::shared_ptr<ParticleFieldCollection>& fields, float deltaTime)
{
    for (const size_t slot : slots) {
        if (pool.IsAlive(slot)) {
            UpdatePosition(pool, slot, fields, deltaTime);
        }
    }
}

void RSRenderParticleEffector::UpdateGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
    ParticleRenderParams& params, const std::shared_ptr<ParticleFieldCollection>& fields, int64_t deltaTime)
{
    if (slots.empty()) {
        return;
    }
    const float dt = static_cast<float>(deltaTime) / NS_TO_S;
    UpdateAccelerationGroup(pool, slots, params, dt);
    UpdateColorGroup(pool, slots, params, dt);
    UpdateOpacityGroup(pool, slots, params, dt);
    UpdateScaleGroup(pool, slots, params, dt);
    UpdateSpinGroup(pool, slots, params, dt);
    UpdatePositionGroup(pool, slots, fields, dt);
}
void RSRenderParticleEffector::UpdateToActiveTime(RSRenderParticlePool& pool, size_t slot,
    ParticleRenderParams& params, const std::shared_ptr<ParticleFieldCollection>& fields, int64_t activeTimeNs)
{
    if (slot >= pool.Count()) {
        return;
    }
    auto& data = pool.Data();
    const float dt = static_cast<float>(activeTimeNs) / NS_TO_S;
    data.activeTime[slot] = dt;
    UpdateAccelerationValue(pool, slot, params, dt);
    UpdateAccelerationAngle(pool, slot, params, dt);
    UpdateColor(pool, slot, params, dt);
    UpdateOpacity(pool, slot, params, dt);
    UpdateScale(pool, slot, params, dt);
    UpdateSpin(pool, slot, params, dt);

    Vector2f position(data.posX[slot], data.posY[slot]);
    Vector2f fieldForce(0.f, 0.f);
    if (fields) {
        fieldForce = fields->ApplyAll(position, dt);
    }
    const float accelerationValue = data.accelValue[slot];
    const float accelerationAngle = data.accelAngle[slot];
    if (!IsAccelerationCurrent(data, slot, accelerationValue, accelerationAngle)) {
        const float radian = accelerationAngle * DEGREE_TO_RADIAN;
        data.accX[slot] = accelerationValue * std::cos(radian);
        data.accY[slot] = accelerationValue * std::sin(radian);
        SetAccelerationCache(data, slot, accelerationValue, accelerationAngle);
    }
    const Vector2f acceleration(data.accX[slot], data.accY[slot]);
    Vector2f velocity(data.velX[slot], data.velY[slot]);
    position.x_ += velocity.x_ * dt + HALF * acceleration.x_ * dt * dt + fieldForce.x_ * dt;
    position.y_ += velocity.y_ * dt + HALF * acceleration.y_ * dt * dt + fieldForce.y_ * dt;
    velocity.x_ += acceleration.x_ * dt;
    velocity.y_ += acceleration.y_ * dt;
    data.posX[slot] = position.x_;
    data.posY[slot] = position.y_;
    data.velX[slot] = velocity.x_;
    data.velY[slot] = velocity.y_;
}

} // namespace Rosen
} // namespace OHOS
