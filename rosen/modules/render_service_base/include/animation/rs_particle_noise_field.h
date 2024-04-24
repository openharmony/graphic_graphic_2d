/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ROSEN_ENGINE_CORE_ANIMATION_RS_NOISE_FIELD_H
#define ROSEN_ENGINE_CORE_ANIMATION_RS_NOISE_FIELD_H
#include <algorithm>
#include <cmath>
#include <random>

#include "rs_render_particle.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT ParticleNoiseField {
public:
    int fieldStrength_;
    ShapeType fieldShape_;
    Vector2f fieldSize_;
    Vector2f fieldCenter_;
    uint16_t fieldFeather_;
    float noiseScale_;
    float noiseFrequency_;
    float noiseAmplitude_;

    explicit ParticleNoiseField(const int fieldStrength, const ShapeType& fieldShape, const Vector2f& fieldSize,
        const Vector2f& fieldCenter, uint16_t fieldFeather, float noiseScale, float noiseFrequency,
        float noiseAmplitude)
        : fieldStrength_(fieldStrength), fieldShape_(fieldShape), fieldSize_(fieldSize), fieldCenter_(fieldCenter),
          fieldFeather_(fieldFeather), noiseScale_(noiseScale), noiseFrequency_(noiseFrequency),
          noiseAmplitude_(noiseAmplitude)
    {}
    ParticleNoiseField(const ParticleNoiseField& config) = default;
    ParticleNoiseField& operator=(const ParticleNoiseField& config) = default;
    ~ParticleNoiseField() = default;

    Vector2f ApplyField(const Vector2f& position);
    Vector2f ApplyCurlNoise(const Vector2f& position);

    bool operator==(const ParticleNoiseField& rhs) const
    {
        bool equal = (this->fieldStrength_ == rhs.fieldStrength_) && (this->fieldShape_ == rhs.fieldShape_) &&
                     (this->fieldSize_ == rhs.fieldSize_) && (this->fieldCenter_ == rhs.fieldCenter_) &&
                     (this->fieldFeather_ == rhs.fieldFeather_) && (this->noiseScale_ == rhs.noiseScale_) &&
                     (this->noiseFrequency_ == rhs.noiseFrequency_) && (this->noiseAmplitude_ == rhs.noiseAmplitude_);
        return equal;
    }

private:
    bool isPointInField(
        const Vector2f& point, const ShapeType& fieldShape, const Vector2f& fieldCenter, float width, float height);
    float calculateEllipseEdgeDistance(const Vector2f& direction);
    float calculateDistanceToRectangleEdge(
        const Vector2f& position, const Vector2f& direction, const Vector2f& center, const Vector2f& size);
};

class RSB_EXPORT PerlinNoise2D {
private:
    std::vector<int> p; // Permutation vector
    float fade(float t);
    float lerp(float t, float a, float b);
    float grad(int hash, float x, float y);
    float noiseScale_;
    float noiseFrequency_;
    float noiseAmplitude_;

public:
    PerlinNoise2D(float noiseScale, float noiseFrequency, float noiseAmplitude);
    float noise(float x, float y);
    Vector2f curl(float x, float y);
};

class ParticleNoiseFields {
public:
    std::vector<std::shared_ptr<ParticleNoiseField>> fields_;

    ParticleNoiseFields() = default;

    void AddField(const std::shared_ptr<ParticleNoiseField>& field)
    {
        fields_.push_back(field);
    }

    void RemoveField(size_t index)
    {
        if (index < fields_.size()) {
            fields_.erase(fields_.begin() + index);
        }
    }

    std::shared_ptr<ParticleNoiseField> GetField(size_t index)
    {
        if (index < fields_.size()) {
            return fields_[index];
        }
        return nullptr;
    }

    size_t GetFieldCount() const
    {
        return fields_.size();
    }

    Vector2f ApplyAllFields(const Vector2f& position)
    {
        Vector2f totalEffect = {0.0f, 0.0f};
        for (auto& field : fields_) {
            totalEffect = totalEffect + field->ApplyField(position) + field->ApplyCurlNoise(position);
        }
        return totalEffect;
    }

    void ClearFields()
    {
        fields_.clear();
    }

    bool operator==(const ParticleNoiseFields& rhs) const
    {
        return fields_ == rhs.fields_;
    }
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_NOISE_FIELD_H
