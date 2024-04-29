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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_PARTICLE_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_PARTICLE_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdint.h>
#include <sys/types.h>
#include <vector>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_render_particle.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_vector2.h"
#include "render/rs_image.h"

namespace OHOS {
namespace Rosen {

template<typename T>
class RSB_EXPORT Change {
public:
    T fromValue_;
    T toValue_;
    int startMillis_;
    int endMillis_;
    RSAnimationTimingCurve curve_;
    Change() : fromValue_(), toValue_(), startMillis_(), endMillis_(), curve_(RSAnimationTimingCurve::LINEAR) {}
    Change(T fromValue, T toValue, int startMillis, int endMillis, RSAnimationTimingCurve& curve)
    {
        fromValue_ = fromValue;
        toValue_ = toValue;
        startMillis_ = startMillis;
        endMillis_ = endMillis;
        curve_ = curve;
    }
    Change(const Change& change) = default;
    Change& operator=(const Change& change) = default;
    ~Change() = default;
};

template<typename T>
class RSB_EXPORT ParticleParaType {
public:
    Range<T> val_;
    ParticleUpdator updator_;
    Range<float> random_;
    std::vector<Change<T>> valChangeOverLife_;
    ParticleParaType(
        Range<T>& val, ParticleUpdator updator, Range<float> random, std::vector<Change<T>>& valChangeOverLife)
    {
        val_ = val;
        updator_ = updator;
        random_ = random;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            auto change = valChangeOverLife[i];
            valChangeOverLife_.push_back(change);
        }
    }
    ParticleParaType() : val_(), updator_(ParticleUpdator::NONE), random_() {}
    ParticleParaType(const ParticleParaType& paraType) = default;
    ParticleParaType& operator=(const ParticleParaType& paraType) = default;
    ~ParticleParaType() = default;
};

class RSB_EXPORT ParticleAcceleration {
public:
    ParticleParaType<float> accelerationValue_;
    ParticleParaType<float> accelerationAngle_;

    ParticleAcceleration() = default;
    ParticleAcceleration(ParticleParaType<float> accelerationValue, ParticleParaType<float> accelerationAngle)
    {
        accelerationValue_ = accelerationValue;
        accelerationAngle_ = accelerationAngle;
    }
    ParticleAcceleration(const ParticleAcceleration& acceleration) = default;
    ParticleAcceleration& operator=(const ParticleAcceleration& acceleration) = default;
    ~ParticleAcceleration() = default;
};

class RSB_EXPORT ParticleColorParaType {
public:
    Range<Color> colorVal_;
    DistributionType distribution_;
    ParticleUpdator updator_;
    Range<float> redRandom_;
    Range<float> greenRandom_;
    Range<float> blueRandom_;
    Range<float> alphaRandom_;

    std::vector<Change<Color>> valChangeOverLife_;
    ParticleColorParaType(const Range<Color>& colorVal, const DistributionType& distribution,
        const ParticleUpdator& updator, const Range<float>& redRandom, const Range<float>& greenRandom,
        const Range<float>& blueRandom, const Range<float>& alphaRandom, std::vector<Change<Color>>& valChangeOverLife)
    {
        colorVal_ = colorVal;
        distribution_ = distribution;
        updator_ = updator;
        redRandom_ = redRandom;
        greenRandom_ = greenRandom;
        blueRandom_ = blueRandom;
        alphaRandom_ = alphaRandom;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            auto change = valChangeOverLife[i];
            valChangeOverLife_.push_back(change);
        }
    }
    ParticleColorParaType()
        : colorVal_(), distribution_(DistributionType::UNIFORM), updator_(ParticleUpdator::NONE), redRandom_(),
          greenRandom_(), blueRandom_(), alphaRandom_(), valChangeOverLife_(0)
    {}
    ParticleColorParaType(const ParticleColorParaType& velocity) = default;
    ParticleColorParaType& operator=(const ParticleColorParaType& velocity) = default;
    ~ParticleColorParaType() = default;
};

class RSB_EXPORT ParticleParams {
public:
    EmitterConfig emitterConfig_;
    ParticleVelocity velocity_;
    ParticleAcceleration acceleration_;
    ParticleColorParaType color_;
    ParticleParaType<float> opacity_;
    ParticleParaType<float> scale_;
    ParticleParaType<float> spin_;
    explicit ParticleParams(const EmitterConfig& emitterConfig, const ParticleVelocity& velocity,
        const ParticleAcceleration& acceleration, const ParticleColorParaType& color,
        const ParticleParaType<float>& opacity, const ParticleParaType<float>& scale,
        const ParticleParaType<float>& spin)
    {
        emitterConfig_ = emitterConfig;
        velocity_ = velocity;
        acceleration_ = acceleration;
        color_ = color;
        opacity_ = opacity;
        scale_ = scale;
        spin_ = spin;
    }
    ParticleParams() : emitterConfig_(), velocity_(), acceleration_(), color_(), opacity_(), scale_(), spin_() {};
    ParticleParams(const ParticleParams& params) = default;
    ParticleParams& operator=(const ParticleParams& params) = default;
    ~ParticleParams() = default;

    RenderParticleParaType<float> translateValToRender(const ParticleParaType<float>& val) const
    {
        RenderParticleParaType<float> value;
        value.val_ = val.val_;
        value.updator_ = val.updator_;
        if (val.updator_ == ParticleUpdator::RANDOM) {
            value.random_ = val.random_;
        } else if (val.updator_ == ParticleUpdator::CURVE) {
            uint32_t size = val.valChangeOverLife_.size();
            std::vector<std::shared_ptr<ChangeInOverLife<float>>> valChangeOverLife;

            for (uint32_t i = 0; i < size; i++) {
                float fromValue = val.valChangeOverLife_[i].fromValue_;
                float toValue = val.valChangeOverLife_[i].toValue_;
                float startMillis = val.valChangeOverLife_[i].startMillis_;
                float endMillis = val.valChangeOverLife_[i].endMillis_;
                int duration = endMillis - startMillis;
                RSAnimationTimingCurve curve = val.valChangeOverLife_[i].curve_;
                auto interpolator = curve.GetInterpolator(duration);

                valChangeOverLife.push_back(std::make_shared<ChangeInOverLife<float>>(
                    fromValue, toValue, startMillis, endMillis, std::move(interpolator)));
            }
            value.valChangeOverLife_ = valChangeOverLife;
        }
        return value;
    }

    RenderParticleColorParaType translateColorToRender(const ParticleColorParaType& val) const
    {
        RenderParticleColorParaType color;
        color.colorVal_ = val.colorVal_;
        color.distribution_ = val.distribution_;
        color.updator_ = val.updator_;
        if (color.updator_ == ParticleUpdator::RANDOM) {
            color.redRandom_ = val.redRandom_;
            color.greenRandom_ = val.greenRandom_;
            color.blueRandom_ = val.blueRandom_;
            color.alphaRandom_ = val.alphaRandom_;
        } else if (color.updator_ == ParticleUpdator::CURVE) {
            uint32_t size = val.valChangeOverLife_.size();
            std::vector<std::shared_ptr<ChangeInOverLife<Color>>> colorChangeOverLife;
            for (uint32_t i = 0; i < size; i++) {
                auto fromValue = val.valChangeOverLife_[i].fromValue_;
                auto toValue = val.valChangeOverLife_[i].toValue_;
                float startMillis = val.valChangeOverLife_[i].startMillis_;
                float endMillis = val.valChangeOverLife_[i].endMillis_;
                int duration = endMillis - startMillis;
                RSAnimationTimingCurve curve = val.valChangeOverLife_[i].curve_;
                auto interpolator = curve.GetInterpolator(duration);

                colorChangeOverLife.push_back(std::make_shared<ChangeInOverLife<Color>>(
                    fromValue, toValue, startMillis, endMillis, std::move(interpolator)));
            }
            color.valChangeOverLife_ = std::move(colorChangeOverLife);
        }
        return color;
    }

    std::shared_ptr<ParticleRenderParams> SetParamsToRenderParticle() const
    {
        auto particleRenderParams = std::make_shared<ParticleRenderParams>();
        particleRenderParams->SetEmitConfig(emitterConfig_);
        particleRenderParams->SetParticleVelocity(velocity_);

        auto acceleration = RenderParticleAcceleration();
        acceleration.accelerationValue_ = translateValToRender(acceleration_.accelerationValue_);
        acceleration.accelerationAngle_ = translateValToRender(acceleration_.accelerationAngle_);

        particleRenderParams->SetParticleAcceleration(acceleration);
        particleRenderParams->SetParticleColor(translateColorToRender(color_));
        particleRenderParams->SetParticleOpacity(translateValToRender(opacity_));
        particleRenderParams->SetParticleScale(translateValToRender(scale_));
        particleRenderParams->SetParticleSpin(translateValToRender(spin_));
        return particleRenderParams;
    }
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_PARTICLES_H
