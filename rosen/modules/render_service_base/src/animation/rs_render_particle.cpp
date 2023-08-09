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
#include "animation/rs_render_particle.h"

#include <cstdint>
#include <random>

#include "animation/rs_interpolator.h"
#include "animation/rs_render_particle_system.h"
#include "common/rs_color.h"
namespace OHOS {
namespace Rosen {

int ParticleRenderParams::GetEmitRate() const
{
    return emitterConfig_.emitRate_;
}
ShapeType ParticleRenderParams::GetEmitShape() const
{
    return emitterConfig_.emitShape_;
}
Vector2f ParticleRenderParams::GetEmitPosition() const
{
    return emitterConfig_.position_;
}
Vector2f ParticleRenderParams::GetEmitSize() const
{
    return emitterConfig_.emitSize_;
}
uint32_t ParticleRenderParams::GetParticleCount() const
{
    return emitterConfig_.particleCount_;
}
int64_t ParticleRenderParams::GetParticleLifeTime() const
{
    return emitterConfig_.lifeTime_;
}
ParticleType ParticleRenderParams::GetParticleType() const
{
    return emitterConfig_.type_;
}
float ParticleRenderParams::GetParticleRadius() const
{
    return emitterConfig_.radius_;
}
std::shared_ptr<RSImage> ParticleRenderParams::GetParticleImage()
{
    return emitterConfig_.image_;
}

float ParticleRenderParams::GetVelocityStartValue() const
{
    return velocity_.velocityValue_.start_;
}
float ParticleRenderParams::GetVelocityEndValue() const
{
    return velocity_.velocityValue_.end_;
}
float ParticleRenderParams::GetVelocityStartAngle() const
{
    return velocity_.velocityAngle_.start_;
}
float ParticleRenderParams::GetVelocityEndAngle() const
{
    return velocity_.velocityAngle_.end_;
}

float ParticleRenderParams::GetAccelerationStartValue() const
{
    return acceleration_.accelerationValue_.val_.start_;
}
float ParticleRenderParams::GetAccelerationEndValue() const
{
    return acceleration_.accelerationValue_.val_.end_;
}
float ParticleRenderParams::GetAccelerationStartAngle() const
{
    return acceleration_.accelerationAngle_.val_.start_;
}
float ParticleRenderParams::GetAccelerationEndAngle() const
{
    return acceleration_.accelerationAngle_.val_.end_;
}
ParticleUpdator ParticleRenderParams::GetAccelerationValueUpdator()
{
    return acceleration_.accelerationValue_.updator_;
}
ParticleUpdator ParticleRenderParams::GetAccelerationAngleUpdator()
{
    return acceleration_.accelerationAngle_.updator_;
}
float ParticleRenderParams::GetAccelRandomValueStart() const
{
    return acceleration_.accelerationValue_.random_.start_;
}
float ParticleRenderParams::GetAccelRandomValueEnd() const
{
    return acceleration_.accelerationValue_.random_.end_;
}
float ParticleRenderParams::GetAccelRandomAngleStart() const
{
    return acceleration_.accelerationAngle_.random_.start_;
}
float ParticleRenderParams::GetAccelRandomAngleEnd() const
{
    return acceleration_.accelerationAngle_.random_.end_;
}

Color ParticleRenderParams::GetColorStartValue()
{
    return color_.colorVal_.start_;
}
Color ParticleRenderParams::GetColorEndValue()
{
    return color_.colorVal_.end_;
}
ParticleUpdator ParticleRenderParams::GetColorUpdator()
{
    return color_.updator_;
}
float ParticleRenderParams::GetRedRandomStart() const
{
    return color_.redRandom_.start_;
}
float ParticleRenderParams::GetRedRandomEnd() const
{
    return color_.redRandom_.end_;
}
float ParticleRenderParams::GetGreenRandomStart() const
{
    return color_.greenRandom_.start_;
}
float ParticleRenderParams::GetGreenRandomEnd() const
{
    return color_.greenRandom_.end_;
}
float ParticleRenderParams::GetBlueRandomStart() const
{
    return color_.blueRandom_.start_;
}
float ParticleRenderParams::GetBlueRandomEnd() const
{
    return color_.blueRandom_.end_;
}
float ParticleRenderParams::GetAlphaRandomStart() const
{
    return color_.alphaRandom_.start_;
}
float ParticleRenderParams::GetAlphaRandomEnd() const
{
    return color_.alphaRandom_.end_;
}

float ParticleRenderParams::GetOpacityStartValue()
{
    return opacity_.val_.start_;
}
float ParticleRenderParams::GetOpacityEndValue()
{
    return opacity_.val_.end_;
}
ParticleUpdator ParticleRenderParams::GetOpacityUpdator()
{
    return opacity_.updator_;
}
float ParticleRenderParams::GetOpacityRandomStart() const
{
    return opacity_.random_.start_;
}
float ParticleRenderParams::GetOpacityRandomEnd() const
{
    return opacity_.random_.end_;
}

float ParticleRenderParams::GetScaleStartValue()
{
    return scale_.val_.start_;
}
float ParticleRenderParams::GetScaleEndValue()
{
    return scale_.val_.end_;
}
ParticleUpdator ParticleRenderParams::GetScaleUpdator()
{
    return scale_.updator_;
}
float ParticleRenderParams::GetScaleRandomStart() const
{
    return scale_.random_.start_;
}
float ParticleRenderParams::GetScaleRandomEnd() const
{
    return scale_.random_.end_;
}

float ParticleRenderParams::GetSpinStartValue()
{
    return spin_.val_.start_;
}
float ParticleRenderParams::GetSpinEndValue()
{
    return spin_.val_.end_;
}
ParticleUpdator ParticleRenderParams::GetSpinUpdator()
{
    return spin_.updator_;
}
float ParticleRenderParams::GetSpinRandomStart() const
{
    return spin_.random_.start_;
}
float ParticleRenderParams::GetSpinRandomEnd() const
{
    return spin_.random_.end_;
}

void ParticleRenderParams::SetEmitConfig(const EmitterConfig& emiterConfig)
{
    emitterConfig_ = emiterConfig;
}
void ParticleRenderParams::SetParticleVelocity(const ParticleVelocity& velocity)
{
    velocity_ = velocity;
}
void ParticleRenderParams::SetParticleAcceleration(const RenderParticleAcceleration& acceleration)
{
    acceleration_ = acceleration;
}
void ParticleRenderParams::SetParticleColor(const RenderParticleColorParaType& color)
{
    color_ = color;
}
void ParticleRenderParams::SetParticleOpacity(const RenderParticleParaType<float>& opacity)
{
    opacity_ = opacity;
}
void ParticleRenderParams::SetParticleScale(const RenderParticleParaType<float>& scale)
{
    scale_ = scale;
}
void ParticleRenderParams::SetParticleSpin(const RenderParticleParaType<float>& spin)
{
    spin_ = spin;
}

RSRenderParticle::RSRenderParticle(std::shared_ptr<ParticleRenderParams> particleParams)
{
    particleRenderParams_ = particleParams;
    InitProperty(particleParams);
}

// Set methods
void RSRenderParticle::SetPosition(const Vector2f& position)
{
    position_ = position;
}

void RSRenderParticle::SetVelocity(const Vector2f& velocity)
{
    velocity_ = velocity;
}

void RSRenderParticle::SetAcceleration(const Vector2f& acceleration)
{
    acceleration_ = acceleration;
}

void RSRenderParticle::SetSpin(const float& spin)
{
    spin_ = spin;
}

void RSRenderParticle::SetOpacity(const float& opacity)
{
    opacity_ = opacity;
}

void RSRenderParticle::SetColor(const Color& color)
{
    color_ = color;
}

void RSRenderParticle::SetScale(const float& scale)
{
    scale_ = scale;
}

void RSRenderParticle::SetRadius(const float& radius)
{
    radius_ = radius;
}

void RSRenderParticle::SetImage(const std::shared_ptr<RSImage>& image)
{
    image_ = image;
}

void RSRenderParticle::SetParticleType(const ParticleType& particleType)
{
    particleType_ = particleType;
}

void RSRenderParticle::SetActiveTime(const int64_t& activeTime)
{
    activeTime_ = activeTime;
}

// Get methods
Vector2f RSRenderParticle::GetPosition()
{
    return position_;
}

Vector2f RSRenderParticle::GetVelocity()
{
    return velocity_;
}

Vector2f RSRenderParticle::GetAcceleration()
{
    return acceleration_;
}

float RSRenderParticle::GetSpin()
{
    return spin_;
}

float RSRenderParticle::GetOpacity()
{
    return opacity_;
}

Color RSRenderParticle::GetColor()
{
    return color_;
}

float RSRenderParticle::GetScale()
{
    return scale_;
}

float RSRenderParticle::GetRadius()
{
    return radius_;
}

std::shared_ptr<RSImage> RSRenderParticle::GetImage()
{
    return image_;
}

ParticleType RSRenderParticle::GetParticleType()
{
    return particleType_;
}

int64_t RSRenderParticle::GetActiveTime()
{
    return activeTime_;
}

std::shared_ptr<ParticleRenderParams> RSRenderParticle::GetParticleRenderParams()
{
    return particleRenderParams_;
}

// Other methods
void RSRenderParticle::InitProperty(std::shared_ptr<ParticleRenderParams> particleParams)
{
    // Initialize particle properties
    auto emitShape = particleParams->GetEmitShape();
    auto position = particleParams->GetEmitPosition();
    auto emitSize = particleParams->GetEmitSize();
    position_ = CalculateParticlePosition(emitShape, position, emitSize);

    float velocityValue =
        GetRandomValue(particleParams->GetVelocityStartValue(), particleParams->GetVelocityEndValue());
    float velocityAngle =
        GetRandomValue(particleParams->GetVelocityStartAngle(), particleParams->GetVelocityEndAngle());
    velocity_ = Vector2f { velocityValue * cos(velocityAngle), velocityValue * sin(velocityAngle) };

    float accelerationValue =
        GetRandomValue(particleParams->GetAccelerationStartValue(), particleParams->GetAccelerationEndValue());
    float accelerationAngle =
        GetRandomValue(particleParams->GetAccelerationStartAngle(), particleParams->GetAccelerationEndAngle());
    acceleration_ = Vector2f { accelerationValue * cos(accelerationAngle), accelerationValue * sin(accelerationAngle) };

    spin_ = GetRandomValue(particleParams->GetSpinStartValue(), particleParams->GetSpinEndValue());
    opacity_ = GetRandomValue(particleParams->GetOpacityStartValue(), particleParams->GetOpacityEndValue());
    scale_ = GetRandomValue(particleParams->GetScaleStartValue(), particleParams->GetScaleEndValue());

    particleType_ = particleParams->GetParticleType();
    if (particleType_ == ParticleType::POINTS) {
        float colorRandomValue = GetRandomValue(0.0f, 1.0f);
        color_ = Lerp(particleParams->GetColorStartValue(), particleParams->GetColorEndValue(), colorRandomValue);
        radius_ = particleParams->GetParticleRadius() * scale_;
    } else if (particleType_ == ParticleType::IMAGES) {
        image_ = particleParams->GetParticleImage();
    }
    activeTime_ = 0;
    lifeTime_ = particleParams->GetParticleLifeTime();
}

bool RSRenderParticle::IsAlive() const
{
    return activeTime_ < lifeTime_;
}

float RSRenderParticle::GetRandomValue(float min, float max)
{
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

Vector2f RSRenderParticle::CalculateParticlePosition(
    const ShapeType& emitShape, const Vector2f& position, const Vector2f& emitSize)
{
    float positionX = 0.f;
    float positionY = 0.f;
    if (emitShape == ShapeType::RECT) {
        float minX = position.x_;
        float maxX = position.x_ + emitSize.x_;
        positionX = GetRandomValue(minX, maxX);
        float minY = position.y_;
        float maxY = position.y_ + emitSize.y_;
        positionY = GetRandomValue(minY, maxY);
    }
    if (emitShape == ShapeType::CIRCLE || emitShape == ShapeType::ELLIPSE) {
        float rx = GetRandomValue(0.f, emitSize.x_);
        float ry = GetRandomValue(0.f, emitSize.y_);
        float theta = GetRandomValue(0.f, 2 * PI);
        positionX = position.x_ + rx * cos(theta);
        positionY = position.y_ + ry * sin(theta);
    }
    return Vector2f { positionX, positionY };
}

Color RSRenderParticle::Lerp(const Color& start, const Color& end, float t)
{
    Color result;
    result.SetRed(start.GetRed() + static_cast<int>(std::round((end.GetRed() - start.GetRed()) * t)));
    result.SetGreen(start.GetGreen() + static_cast<int>(std::round((end.GetGreen() - start.GetGreen()) * t)));
    result.SetBlue(start.GetBlue() + static_cast<int>(std::round((end.GetBlue() - start.GetBlue()) * t)));
    result.SetAlpha(start.GetAlpha() + static_cast<int>(std::round((end.GetAlpha() - start.GetAlpha()) * t)));
    return result;
}

} // namespace Rosen
} // namespace OHOS
