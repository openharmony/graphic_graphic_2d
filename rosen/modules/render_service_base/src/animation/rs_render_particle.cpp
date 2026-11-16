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

#include "animation/rs_render_particle_pool.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <random>

#include "animation/rs_interpolator.h"
#include "animation/rs_render_particle_system.h"
#include "common/rs_color.h"
namespace OHOS {
namespace Rosen {
namespace {
constexpr float DEGREE_TO_RADIAN = static_cast<float>(M_PI / 180);
constexpr int RAND_PRECISION = 10000;
constexpr float SIGMA = 3.0f;
constexpr float FULL_CIRCLE = 2.0f * static_cast<float>(M_PI);
constexpr float HALF = 0.5f;
constexpr double COLOR_MEAN_DIVISOR = 2.0;
constexpr int64_t INFINITE_LIFE_NS = -1 * NS_PER_MS;
constexpr float LIFE_TIME_INFINITE = -1.0f;
} // namespace

int ParticleRenderParams::GetEmitRate() const
{
    return emitterConfig_.emitRate_;
}

const ShapeType& ParticleRenderParams::GetEmitShape() const
{
    return emitterConfig_.emitShape_;
}

const Vector2f& ParticleRenderParams::GetEmitPosition() const
{
    return emitterConfig_.position_;
}

const Vector2f& ParticleRenderParams::GetEmitSize() const
{
    return emitterConfig_.emitSize_;
}

int32_t ParticleRenderParams::GetParticleCount() const
{
    return emitterConfig_.particleCount_;
}

int64_t ParticleRenderParams::GetLifeTimeStartValue() const
{
    if (emitterConfig_.lifeTime_.start_ > std::numeric_limits<int64_t>::max() / NS_PER_MS) {
        return std::numeric_limits<int64_t>::max();
    }
    return emitterConfig_.lifeTime_.start_ * NS_PER_MS;
}

int64_t ParticleRenderParams::GetLifeTimeEndValue() const
{
    if (emitterConfig_.lifeTime_.end_ > std::numeric_limits<int64_t>::max() / NS_PER_MS) {
        return std::numeric_limits<int64_t>::max();
    }
    return emitterConfig_.lifeTime_.end_ * NS_PER_MS;
}

const ParticleType& ParticleRenderParams::GetParticleType() const
{
    return emitterConfig_.type_;
}

float ParticleRenderParams::GetParticleRadius() const
{
    return emitterConfig_.radius_;
}

const std::shared_ptr<RSImage>& ParticleRenderParams::GetParticleImage()
{
    return emitterConfig_.image_;
}

const Vector2f& ParticleRenderParams::GetImageSize() const
{
    return emitterConfig_.imageSize_;
}

const std::shared_ptr<Shape>& ParticleRenderParams::GetShape() const
{
    return emitterConfig_.shape_;
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

const ParticleUpdator& ParticleRenderParams::GetAccelerationValueUpdator()
{
    return acceleration_.accelerationValue_.updator_;
}

const ParticleUpdator& ParticleRenderParams::GetAccelerationAngleUpdator()
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

const Color& ParticleRenderParams::GetColorStartValue()
{
    return color_.colorVal_.start_;
}

const Color& ParticleRenderParams::GetColorEndValue()
{
    return color_.colorVal_.end_;
}

const DistributionType& ParticleRenderParams::GetColorDistribution()
{
    return color_.distribution_;
}

const ParticleUpdator& ParticleRenderParams::GetColorUpdator()
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

const ParticleUpdator& ParticleRenderParams::GetOpacityUpdator()
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

const ParticleUpdator& ParticleRenderParams::GetScaleUpdator()
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

const ParticleUpdator& ParticleRenderParams::GetSpinUpdator()
{
    return spin_.updator_;
}

const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& ParticleRenderParams::GetAcceValChangeOverLife()
{
    return acceleration_.accelerationValue_.valChangeOverLife_;
}

const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& ParticleRenderParams::GetAcceAngChangeOverLife()
{
    return acceleration_.accelerationAngle_.valChangeOverLife_;
}

const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& ParticleRenderParams::GetOpacityChangeOverLife()
{
    return opacity_.valChangeOverLife_;
}

const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& ParticleRenderParams::GetScaleChangeOverLife()
{
    return scale_.valChangeOverLife_;
}

const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& ParticleRenderParams::GetSpinChangeOverLife()
{
    return spin_.valChangeOverLife_;
}

const std::vector<std::shared_ptr<ChangeInOverLife<Color>>>& ParticleRenderParams::GetColorChangeOverLife()
{
    return color_.valChangeOverLife_;
}

float ParticleRenderParams::GetSpinRandomStart() const
{
    return spin_.random_.start_;
}

float ParticleRenderParams::GetSpinRandomEnd() const
{
    return spin_.random_.end_;
}

size_t ParticleRenderParams::GetImageIndex() const
{
    return imageIndex_;
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

void ParticleRenderParams::SetImageIndex(size_t imageIndex)
{
    imageIndex_ = imageIndex;
}

float RSRenderParticle::GetRandomValue(float min, float max)
{
    if (ROSEN_EQ(min, max)) {
        return min;
    }
    if (min > max) {
        std::swap(min, max);
    }
    int rand_int = rand() % RAND_PRECISION;
    float rand_float = min + (static_cast<float>(rand_int) / RAND_PRECISION) * (max - min);
    return rand_float;
}

// Generate color components that follow normal distribution
int RSRenderParticle::GenerateColorComponent(double mean, double stddev)
{
    // Creates a random number engine and a normal distribution object.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(mean, stddev);

    double number = dis(gen);
    int component = static_cast<int>(std::round(std::abs(number)));
    component = std::max(0, std::min(component, 255)); // 255 is RGB Component max.
    return component;
}

void AnnulusRegion::CalculatePosition(float& positionX, float& positionY)
{
    float radius = RSRenderParticle::GetRandomValue(innerRadius_, outerRadius_);
    float theta = RSRenderParticle::GetRandomValue(startAngle_, endAngle_) * DEGREE_TO_RADIAN;
    positionX = center_.x_ + radius * cos(theta);
    positionY = center_.y_ + radius * sin(theta);
}

void EmitterUpdater::Dump(std::string& out) const
{
    out += "[emitterIndex:" + std::to_string(emitterIndex_);
    if (position_) {
        out += " position[" + std::to_string(position_->x_) + " ";
        out += std::to_string(position_->y_) + "]";
    }
    if (emitSize_) {
        out += " emitSize[" + std::to_string(emitSize_->x_) + " ";
        out += std::to_string(emitSize_->y_) + "]";
    }
    if (emitRate_) {
        out += " emitRate:" + std::to_string(emitRate_.value());
    }
    out += ']';
}

void EmitterConfig::SetConfigShape(const std::shared_ptr<Shape>& shape)
{
    shape_ = shape;
}

void EmitterUpdater::SetShape(const std::shared_ptr<Shape>& shape)
{
    shape_ = shape;
}

int RSRenderParticleVector::GetParticleSize() const
{
    return pool_ == nullptr ? 0 : static_cast<int>(pool_->Count());
}

bool RSRenderParticleVector::operator==(const RSRenderParticleVector& rhs) const
{
    const size_t count = pool_ == nullptr ? 0 : pool_->Count();
    const size_t rhsCount = rhs.pool_ == nullptr ? 0 : rhs.pool_->Count();
    if (count != rhsCount || imageVector_.size() != rhs.imageVector_.size()) {
        return false;
    }
    if (count == 0 && imageVector_.empty()) {
        return true;
    }
    // A live pool mutates in place, so two properties sharing it must not compare equal (that would freeze redraw).
    return false;
}

Vector2f RSRenderParticle::CalculateParticlePosition(const ShapeType& emitShape, const Vector2f& position,
    const Vector2f& emitSize, const std::shared_ptr<Shape>& shape)
{
    float x = 0.0f;
    float y = 0.0f;
    if (emitShape == ShapeType::RECT) {
        x = GetRandomValue(position.x_, position.x_ + emitSize.x_);
        y = GetRandomValue(position.y_, position.y_ + emitSize.y_);
    }
    if (emitShape == ShapeType::CIRCLE || emitShape == ShapeType::ELLIPSE) {
        const float centerX = position.x_ + emitSize.x_ * HALF;
        const float centerY = position.y_ + emitSize.y_ * HALF;
        const float theta = GetRandomValue(0.0f, FULL_CIRCLE);
        if (emitShape == ShapeType::CIRCLE) {
            const float radius = GetRandomValue(0.0f, std::min(emitSize.x_, emitSize.y_)) * HALF;
            x = centerX + radius * std::cos(theta);
            y = centerY + radius * std::sin(theta);
        } else {
            const float radiusX = GetRandomValue(0.0f, emitSize.x_) * HALF;
            const float radiusY = GetRandomValue(0.0f, emitSize.y_) * HALF;
            x = centerX + radiusX * std::cos(theta);
            y = centerY + radiusY * std::sin(theta);
        }
    }
    if (shape != nullptr && emitShape == ShapeType::ANNULUS) {
        shape->CalculatePosition(x, y);
    }
    return Vector2f { x, y };
}

Color RSRenderParticle::LerpColor(const Color& start, const Color& end, float t)
{
    Color result = start;
    result.SetRed(static_cast<int16_t>(std::round(start.GetRed() + (end.GetRed() - start.GetRed()) * t)));
    result.SetGreen(static_cast<int16_t>(std::round(start.GetGreen() + (end.GetGreen() - start.GetGreen()) * t)));
    result.SetBlue(static_cast<int16_t>(std::round(start.GetBlue() + (end.GetBlue() - start.GetBlue()) * t)));
    result.SetAlpha(static_cast<int16_t>(std::round(start.GetAlpha() + (end.GetAlpha() - start.GetAlpha()) * t)));
    return result;
}

void RSRenderParticle::InitPointColor(RSRenderParticle& data, size_t slot, ParticleRenderParams& params)
{
    Color color;
    if (params.GetColorDistribution() == DistributionType::GAUSSIAN) {
        const auto& start = params.GetColorStartValue();
        const auto& end = params.GetColorEndValue();
        const double redScale = std::abs(end.GetRed() - start.GetRed());
        const double greenScale = std::abs(end.GetGreen() - start.GetGreen());
        const double blueScale = std::abs(end.GetBlue() - start.GetBlue());
        const double alphaScale = std::abs(end.GetAlpha() - start.GetAlpha());
        color.SetRed(static_cast<int16_t>(GenerateColorComponent(
            (start.GetRed() + end.GetRed()) / COLOR_MEAN_DIVISOR, SIGMA * redScale)));
        color.SetGreen(static_cast<int16_t>(GenerateColorComponent(
            (start.GetGreen() + end.GetGreen()) / COLOR_MEAN_DIVISOR, SIGMA * greenScale)));
        color.SetBlue(static_cast<int16_t>(GenerateColorComponent(
            (start.GetBlue() + end.GetBlue()) / COLOR_MEAN_DIVISOR, SIGMA * blueScale)));
        color.SetAlpha(static_cast<int16_t>(GenerateColorComponent(
            (start.GetAlpha() + end.GetAlpha()) / COLOR_MEAN_DIVISOR, SIGMA * alphaScale)));
    } else {
        color = LerpColor(params.GetColorStartValue(), params.GetColorEndValue(), GetRandomValue(0.0f, 1.0f));
    }
    data.colorR[slot] = static_cast<float>(color.GetRed());
    data.colorG[slot] = static_cast<float>(color.GetGreen());
    data.colorB[slot] = static_cast<float>(color.GetBlue());
    data.colorA[slot] = static_cast<float>(color.GetAlpha());
    data.redSpeed[slot] = GetRandomValue(params.GetRedRandomStart(), params.GetRedRandomEnd());
    data.greenSpeed[slot] =
        GetRandomValue(params.GetGreenRandomStart(), params.GetGreenRandomEnd());
    data.blueSpeed[slot] =
        GetRandomValue(params.GetBlueRandomStart(), params.GetBlueRandomEnd());
    data.alphaSpeed[slot] =
        GetRandomValue(params.GetAlphaRandomStart(), params.GetAlphaRandomEnd());
}

void RSRenderParticle::InitImageState(RSRenderParticle& data, size_t slot, ParticleRenderParams& params,
    const Vector2f& position)
{
    const Vector2f imageSize = params.GetImageSize();
    data.imageSizeX[slot] = imageSize.x_;
    data.imageSizeY[slot] = imageSize.y_;
    data.imageIndex[slot] = static_cast<uint32_t>(params.GetImageIndex());
    auto image = params.GetParticleImage();
    if (image == nullptr) {
        return;
    }
    auto pixelMap = image->GetPixelMap();
    if (pixelMap != nullptr) {
        image->SetDstRect(RectF(position.x_, position.y_, pixelMap->GetWidth(), pixelMap->GetHeight()));
    }
}

void RSRenderParticle::InitMotion(RSRenderParticle& data, size_t slot, ParticleRenderParams& params)
{
    const float velocityValue = GetRandomValue(params.GetVelocityStartValue(), params.GetVelocityEndValue());
    const float velocityAngle =
        GetRandomValue(params.GetVelocityStartAngle(), params.GetVelocityEndAngle()) * DEGREE_TO_RADIAN;
    data.velX[slot] = velocityValue * std::cos(velocityAngle);
    data.velY[slot] = velocityValue * std::sin(velocityAngle);

    const float accelerationValue =
        GetRandomValue(params.GetAccelerationStartValue(), params.GetAccelerationEndValue());
    const float accelerationAngle =
        GetRandomValue(params.GetAccelerationStartAngle(), params.GetAccelerationEndAngle());
    data.accelValue[slot] = accelerationValue;
    data.accelAngle[slot] = accelerationAngle;
    data.accX[slot] = accelerationValue * std::cos(accelerationAngle * DEGREE_TO_RADIAN);
    data.accY[slot] = accelerationValue * std::sin(accelerationAngle * DEGREE_TO_RADIAN);

    const float spin = GetRandomValue(params.GetSpinStartValue(), params.GetSpinEndValue());
    data.spin[slot] = spin;
    data.spinCos[slot] = std::cos(spin * DEGREE_TO_RADIAN);
    data.spinSin[slot] = std::sin(spin * DEGREE_TO_RADIAN);
    data.opacity[slot] = GetRandomValue(params.GetOpacityStartValue(), params.GetOpacityEndValue());
    data.scale[slot] = GetRandomValue(params.GetScaleStartValue(), params.GetScaleEndValue());
    data.opacitySpeed[slot] = GetRandomValue(params.GetOpacityRandomStart(), params.GetOpacityRandomEnd());
    data.scaleSpeed[slot] = GetRandomValue(params.GetScaleRandomStart(), params.GetScaleRandomEnd());
    data.spinSpeed[slot] = GetRandomValue(params.GetSpinRandomStart(), params.GetSpinRandomEnd());
    data.accelValueSpeed[slot] = GetRandomValue(params.GetAccelRandomValueStart(), params.GetAccelRandomValueEnd());
    data.accelAngleSpeed[slot] = GetRandomValue(params.GetAccelRandomAngleStart(), params.GetAccelRandomAngleEnd());
}

void RSRenderParticle::InitAppearance(RSRenderParticle& data, size_t slot, ParticleRenderParams& params,
    const Vector2f& position)
{
    data.colorA[slot] = static_cast<float>(UINT8_MAX);
    const ParticleType type = params.GetParticleType();
    data.particleType[slot] = static_cast<uint32_t>(type);
    if (type == ParticleType::POINTS) {
        InitPointColor(data, slot, params);
        data.radius[slot] = params.GetParticleRadius();
    } else if (type == ParticleType::IMAGES) {
        InitImageState(data, slot, params, position);
    }
}

void InitParticle(RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, uint32_t paramsIndex)
{
    auto& data = pool.Data();
    const Vector2f position = RSRenderParticle::CalculateParticlePosition(
        params.GetEmitShape(), params.GetEmitPosition(), params.GetEmitSize(), params.GetShape());
    data.posX[slot] = position.x_;
    data.posY[slot] = position.y_;
    RSRenderParticle::InitMotion(data, slot, params);
    RSRenderParticle::InitAppearance(data, slot, params, position);
    data.paramsIndex[slot] = paramsIndex;
    data.activeTime[slot] = 0.0f;
    const int64_t lifeStartNs = params.GetLifeTimeStartValue();
    const int64_t lifeEndNs = params.GetLifeTimeEndValue();
    if (lifeStartNs == INFINITE_LIFE_NS && lifeEndNs == INFINITE_LIFE_NS) {
        data.lifeTime[slot] = LIFE_TIME_INFINITE;
    } else {
        const float lifeNs = RSRenderParticle::GetRandomValue(static_cast<float>(lifeStartNs),
            static_cast<float>(lifeEndNs));
        // A non-infinite negative life behaves like the baseline: dead from birth, not infinite.
        data.lifeTime[slot] = lifeNs > 0.0f ? lifeNs / static_cast<float>(NS_TO_S) : 0.0f;
    }
}
} // namespace Rosen
} // namespace OHOS
