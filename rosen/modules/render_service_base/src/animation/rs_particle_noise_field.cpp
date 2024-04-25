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

#include "animation/rs_particle_noise_field.h"

namespace OHOS {
namespace Rosen {
constexpr float EPSILON = 1e-3;
constexpr float HALF = 0.5f;
constexpr float FEATHERMAX = 100.f;
bool ParticleNoiseField::isPointInField(
    const Vector2f& point, const ShapeType& fieldShape, const Vector2f& fieldCenter, float width, float height)
{
    if (fieldShape == ShapeType::RECT) {
        return ((point.x_ > fieldCenter_.x_ - width * HALF) && (point.x_ < fieldCenter_.x_ + width * HALF) &&
                (point.y_ >= fieldCenter_.y_ - height * HALF) && (point.y_ < fieldCenter_.y_ + height * HALF));
    } else {
        double normX = (point.x_ - fieldCenter_.x_) * (point.x_ - fieldCenter_.x_);
        double normY = (point.y_ - fieldCenter_.y_) * (point.y_ - fieldCenter_.y_);
        return ROSEN_EQ(width, 0.f) || ROSEN_EQ(height, 0.f) ? false :
               (normX / (width * HALF * width * HALF) + normY / (height * HALF * height * HALF) <= 1.0);
    }
    return false;
}

float ParticleNoiseField::calculateEllipseEdgeDistance(const Vector2f& direction)
{
    // direction is the direction vector from the center to the particle location.
    // Check whether the direction is zero vector.
    if ((ROSEN_EQ(direction.x_, 0.f) && ROSEN_EQ(direction.y_, 0.f)) ||
        ROSEN_EQ(fieldSize_.x_, 0.f) || ROSEN_EQ(fieldSize_.y_, 0.f)) {
        return 0;
    }
    // Implicit equation of the ellipse edge: (x/a)^2 + (y/b)^2 = 1
    // Solve the equation and find the edge point that is the same as the direction of the direction vector.
    // Because the direction vector may not be aligned with the elliptic axis, the parameter equation and elliptic
    // equation are used together to solve the problem.
    // The parameter equation is x = cx + t * dx, y = cy + t * dy.
    // Substituting the parameter equation into the elliptic equation to solve t
    float t = std::sqrt(1 / ((direction.x_ * direction.x_) / (fieldSize_.x_ * fieldSize_.x_ / 4) +
                                (direction.y_ * direction.y_) / (fieldSize_.y_ * fieldSize_.y_ / 4)));
    return t; // t is the distance from the center point to the edge.
}

float ParticleNoiseField::calculateDistanceToRectangleEdge(
    const Vector2f& position, const Vector2f& direction, const Vector2f& center, const Vector2f& size)
{
    if (ROSEN_EQ(direction.x_, 0.f) && ROSEN_EQ(direction.y_, 0.f)) {
        return 0.0f;
    }
    // Calculates the four boundaries of a rectangle.
    float left = center.x_ - size.x_ * HALF;
    float right = center.x_ + size.x_ * HALF;
    float top = center.y_ - size.y_ * HALF;
    float bottom = center.y_ + size.y_ * HALF;
    // Calculate the time required for reaching each boundary.
    float tLeft = (left - position.x_) / direction.x_;
    float tRight = (right - position.x_) / direction.x_;
    float tTop = (top - position.y_) / direction.y_;
    float tBottom = (bottom - position.y_) / direction.y_;

    // Particles advance to the boundary only if t is a positive number.
    std::vector<float> times;
    if (tLeft > 0.f) {
        times.push_back(tLeft);
    }
    if (tRight > 0.f) {
        times.push_back(tRight);
    }
    if (tTop > 0.f) {
        times.push_back(tTop);
    }
    if (tBottom > 0.f) {
        times.push_back(tBottom);
    }

    if (times.empty()) {
        return 0.f;
    }
    // The smallest value of t, which is the first time the particle will reach the boundary.
    float tEdge = *std::min_element(times.begin(), times.end());
    // Calculates the distance to the border.
    float distance = std::sqrt(std::pow(tEdge * direction.x_, 2) + std::pow(tEdge * direction.y_, 2));
    return distance;
}

Vector2f ParticleNoiseField::ApplyField(const Vector2f& position)
{
    // If the position is in the field, calculate the field force.
    Vector2f direction = position - fieldCenter_;
    float distance = direction.GetLength();
    float forceMagnitude = fieldStrength_;
    if (isPointInField(position, fieldShape_, fieldCenter_, fieldSize_.x_, fieldSize_.y_)) {
        if (fieldFeather_ > 0) {
            float edgeDistance = fieldSize_.x_ * HALF;
            if (fieldShape_ == ShapeType::RECT) {
                edgeDistance = calculateDistanceToRectangleEdge(position, direction, fieldCenter_, fieldSize_);
            } else if (fieldShape_ == ShapeType::ELLIPSE) {
                edgeDistance = calculateEllipseEdgeDistance(direction);
            }
            if (edgeDistance != 0 && !ROSEN_EQ(fieldSize_.GetLength(), 0.f)) {
                forceMagnitude *= (1.0f - ((float)fieldFeather_ / FEATHERMAX) * (distance / fieldSize_.GetLength()));
            }
        }
        Vector2f force = direction.Normalized() * forceMagnitude;
        return force;
    }
    return Vector2f { 0.f, 0.f };
}

Vector2f ParticleNoiseField::ApplyCurlNoise(const Vector2f& position)
{
    if (isPointInField(position, fieldShape_, fieldCenter_, fieldSize_.x_, fieldSize_.y_)) {
        PerlinNoise2D noise(noiseScale_, noiseFrequency_, noiseAmplitude_);
        return noise.curl(position.x_, position.y_) * noiseScale_;
    }
    return Vector2f { 0.f, 0.f };
}

float PerlinNoise2D::fade(float t)
{
    // Fade function as defined by Ken Perlin: 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise2D::lerp(float t, float a, float b)
{
    // Linear interpolate between a and b
    return a + t * (b - a);
}

float PerlinNoise2D::grad(int hash, float x, float y)
{
    // Convert low 4 bits of hash code into 12 gradient directions
    int h = hash & 15; // Use a bitwise AND operation (&) to get the lowest 4 bits of the hash value
    // The value of h determines whether the first component of the gradient vector is x or y.
    // If the value of h is less than 8, u is assigned the value x, otherwise y
    double u = h < 8 ? x : y;
    // Selects the second component of the gradient vector.
    // If h is less than 4, v is assigned the value y; If h is equal to 12 or 14, v is assigned to x; Otherwise, v is 0.
    // This allocation ensures diversity in the gradient vectors and helps to produce a more natural noise texture.
    double v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v); // h & 2 check the h's second least bit
}

PerlinNoise2D::PerlinNoise2D(float noiseScale, float noiseFrequency, float noiseAmplitude)
{
    noiseScale_ = noiseScale;
    noiseFrequency_ = noiseFrequency;
    noiseAmplitude_ = noiseAmplitude;
    // Initialize permutation vector with values from 0 to 255
    p.resize(256); // 256 is the vector size
    std::iota(p.begin(), p.end(), 0);

    // Shuffle using a default random engine
    std::default_random_engine engine;
    std::shuffle(p.begin(), p.end(), engine);

    // Duplicate the permutation vector
    p.insert(p.end(), p.begin(), p.end());
}

float PerlinNoise2D::noise(float x, float y)
{
    x *= noiseFrequency_;
    y *= noiseFrequency_;
    // Find the unit square that contains the point
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    // Find relative x, y of point in square
    x -= floor(x);
    y -= floor(y);

    // Compute fade curves for each of x, y
    float u = fade(x);
    float v = fade(y);

    // Hash coordinates of the 4 square corners
    int A = p[X] + Y;
    int AA = p[A];
    int AB = p[A + 1];
    int B = p[X + 1] + Y;
    int BA = p[B];
    int BB = p[B + 1];

    // And add blended results from the 4 corners of the square
    float res = lerp(v, lerp(u, grad(p[AA], x, y), grad(p[BA], x - 1, y)),
        lerp(u, grad(p[AB], x, y - 1), grad(p[BB], x - 1, y - 1)));

    return noiseAmplitude_ * (res + 1.f) / 2.f; // Normalize the result
}

// In the two-dimensional mode, curl actually returns a vector instead of a scalar.
Vector2f PerlinNoise2D::curl(float x, float y)
{
    // Calculate the partial derivative of the noise field.
    float noise_dx = noise(x + EPSILON, y) - noise(x - EPSILON, y);
    float noise_dy = noise(x, y + EPSILON) - noise(x, y - EPSILON);

    // The result of the two-dimensional curl is the vector obtained by rotating the gradient by 90 degrees.
    // Assume that Curl has a value only in the Z component (rotation in the two-dimensional plane).
    float curlx = -noise_dy / (2 * EPSILON);
    float curly = noise_dx / (2 * EPSILON);

    return Vector2f(curlx, curly);
}
} // namespace Rosen
} // namespace OHOS