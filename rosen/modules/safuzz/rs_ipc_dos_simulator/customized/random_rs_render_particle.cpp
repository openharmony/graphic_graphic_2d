/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "customized/random_rs_render_particle.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_interpolator.h"
#include "animation/rs_render_particle.h"
#include "animation/rs_spring_interpolator.h"
#include "animation/rs_steps_interpolator.h"
#include "common/rs_color.h"
#include "customized/random_rs_image.h"
#include "random/random_data.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
namespace {
std::shared_ptr<RSInterpolator> RandomRSInterpolator()
{
    int index = RandomEngine::GetRandomIndex(4);
    switch (index) {
        case 0: /* RSCubicBezierInterpolator */
            return std::make_shared<RSCubicBezierInterpolator>(RandomData::GetRandomFloat(),
                RandomData::GetRandomFloat(), RandomData::GetRandomFloat(), RandomData::GetRandomFloat());
        case 1: /* LinearInterpolator */
            return std::make_shared<LinearInterpolator>(RandomData::GetRandomUint64());
        case 2: /* RSCustomInterpolator */
            return std::make_shared<RSCustomInterpolator>([](float a) -> float { return a; },
                RandomData::GetRandomInt());
        case 3: /* RSSpringInterpolator */
            return std::make_shared<RSSpringInterpolator>(RandomData::GetRandomFloat(),
                RandomData::GetRandomFloat(), RandomData::GetRandomFloat());
        case 4: /* RSStepsInterpolator */
            return std::make_shared<RSStepsInterpolator>(RandomData::GetRandomInt32(),
                static_cast<StepsCurvePosition>(RandomEngine::GetRandomIndex(1)));
        default:
            return nullptr;
    }
}

RenderParticleParaType<float> RandomFloatRenderParticleParaType()
{
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> changes;
    int vectorLength = RandomEngine::GetRandomSmallVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        changes.emplace_back(std::make_shared<ChangeInOverLife<float>>(RandomData::GetRandomFloat(),
            RandomData::GetRandomFloat(), RandomData::GetRandomInt(),
            RandomData::GetRandomInt(), RandomRSInterpolator()));
    }
    return RenderParticleParaType<float>(RandomData::GetRandomFloatRange(),
        static_cast<ParticleUpdator>(RandomEngine::GetRandomIndex(2)),
        RandomData::GetRandomFloatRange(), changes);
}

RenderParticleColorParaType RandomRenderParticleColorParaType()
{
    std::vector<std::shared_ptr<ChangeInOverLife<Color>>> changes;
    int vectorLength = RandomEngine::GetRandomSmallVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        changes.emplace_back(std::make_shared<ChangeInOverLife<Color>>(RandomData::GetRandomColor(),
            RandomData::GetRandomColor(), RandomData::GetRandomInt(),
            RandomData::GetRandomInt(), RandomRSInterpolator()));
    }
    return RenderParticleColorParaType(RandomData::GetRandomColorRange(),
        static_cast<DistributionType>(RandomEngine::GetRandomIndex(1)),
        static_cast<ParticleUpdator>(RandomEngine::GetRandomIndex(2)),
        RandomData::GetRandomFloatRange(), RandomData::GetRandomFloatRange(),
        RandomData::GetRandomFloatRange(), RandomData::GetRandomFloatRange(), changes);
}
} // namespace

std::shared_ptr<RSRenderParticle> RandomRSRenderParticle::GetRandomRSRenderParticle()
{
    auto params = GetRandomParticleRenderParams();
    return std::make_shared<RSRenderParticle>(params);
}

RSRenderParticleVector RandomRSRenderParticle::GetRandomRSRenderParticleVector()
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    int vectorLength = RandomEngine::GetRandomSmallVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        particles.emplace_back(GetRandomRSRenderParticle());
    }
    return RSRenderParticleVector(std::move(particles), RandomRSImage::GetRandomRSImageVector());
}

std::shared_ptr<ParticleRenderParams> RandomRSRenderParticle::GetRandomParticleRenderParams()
{
    EmitterConfig config(RandomData::GetRandomInt(), RandomData::GetRandomShapeType(), RandomData::GetRandomVector2f(),
        RandomData::GetRandomVector2f(), RandomData::GetRandomInt32(), RandomData::GetRandomInt64Range(),
        static_cast<ParticleType>(RandomEngine::GetRandomIndex(1)), RandomData::GetRandomFloat(),
        RandomRSImage::GetRandomRSImage(), RandomData::GetRandomVector2f());
    ParticleVelocity velocity(RandomData::GetRandomFloatRange(), RandomData::GetRandomFloatRange());
    RenderParticleAcceleration acceleration(RandomFloatRenderParticleParaType(), RandomFloatRenderParticleParaType());
    RenderParticleColorParaType color = RandomRenderParticleColorParaType();
    RenderParticleParaType<float> opacity = RandomFloatRenderParticleParaType();
    RenderParticleParaType<float> scale = RandomFloatRenderParticleParaType();
    RenderParticleParaType<float> spin = RandomFloatRenderParticleParaType();
    auto params = std::make_shared<ParticleRenderParams>(config, velocity, acceleration, color, opacity, scale, spin);
    return params;
}

std::vector<std::shared_ptr<ParticleRenderParams>> RandomRSRenderParticle::GetRandomParticleRenderParamsVector()
{
    std::vector<std::shared_ptr<ParticleRenderParams>> params;
    int vectorLength = RandomEngine::GetRandomVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        params.emplace_back(GetRandomParticleRenderParams());
    }
    return params;
}
} // namespace Rosen
} // namespace OHOS
