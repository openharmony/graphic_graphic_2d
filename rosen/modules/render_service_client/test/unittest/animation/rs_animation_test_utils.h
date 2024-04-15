/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_RENDER_SERVICE_CLIENT_TEST_UNITTEST_ANIMATION_RS_ANIMATION_TEST_UTILS_H
#define ROSEN_MODULES_RENDER_SERVICE_CLIENT_TEST_UNITTEST_ANIMATION_RS_ANIMATION_TEST_UTILS_H

#include "animation/rs_animation_common.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "common/rs_matrix3.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"

namespace OHOS {
namespace Rosen {
namespace ANIMATIONTEST {
constexpr int64_t START_NUMBER = 181154000809;
constexpr int64_t INCREASE_NUMBER = 16666667;
constexpr  unsigned int FIRST_ANIMATION = 0;
constexpr  unsigned int SECOND_ANIMATION = 1;
constexpr  unsigned int CORRECT_SIZE = 1;
constexpr  unsigned int CORRECT_GROUP_SIZE = 2;
constexpr  unsigned int DELAY_TIME_ONE = 1;
constexpr  unsigned int DELAY_TIME_TWO = 2;
constexpr  unsigned int DELAY_TIME_THREE = 3;
constexpr  unsigned int DELAY_TIME_REFRESH = 16667;
constexpr float CANVAS_NODE_BOUNDS_WIDTH = 500.f;
constexpr float CANVAS_NODE_BOUNDS_HEIGHT = 500.f;
constexpr float ANIMATION_DEFAULT_VALUE = 0.f;
constexpr float ANIMATION_START_VALUE = 0.f;
constexpr float ANIMATION_END_VALUE = 500.f;
constexpr float ANIMATION_END_VALUE_ALPHA = 100.f;
constexpr uint32_t ANIMATION_DURATION = 1000;
constexpr uint32_t ANIMATION_DURATION_2 = 2000;
constexpr float KEYFRAME_ANIMATION_FRACTION = 1.f;
constexpr float KEYFRAME_ANIMATION_START_VALUE = 50.f;
constexpr float KEYFRAME_ANIMATION_END_VALUE = 150.f;
constexpr uint32_t ANIMATION_MOCK_ID = 1556;
constexpr float SPRING_ANIMATION_START_VALUE = 1.0f;
constexpr float SPRING_ANIMATION_END_VALUE = 100.0f;
constexpr uint32_t RGBA_DEFAULT_COLOR = 0x00;
constexpr uint32_t RGBA_START_COLOR = 0xFF000000;
constexpr uint32_t RGBA_END_COLOR = 0xFFFF0000;
const std::string  ANIMATION_PATH = "L350 0 L150 100";
const Vector2f PATH_ANIMATION_DEFAULT_VALUE = Vector2f(0.f, 0.f);
const Vector2f PATH_ANIMATION_START_VALUE = Vector2f(0.f, 0.f);
const Vector2f PATH_ANIMATION_END_VALUE = Vector2f(500.f, 500.f);
const Vector2f ANIMATION_NORMAL_SCALE = Vector2f(1.f, 1.f);
const Vector2f ANIMATION_TENTH_SCALE = Vector2f(0.1f, 0.1f);
const Vector2f ANIMATION_HALF_SCALE = Vector2f(0.5f, 0.5f);
const Vector2f ANIMATION_DOUBLE_SCALE = Vector2f(2.f, 2.f);
const Vector4f ANIMATION_START_BOUNDS = Vector4f(100.f, 100.f, 200.f, 300.f);
const Vector4f ANIMATION_MIDDLE_BOUNDS = Vector4f(100.f, 100.f, 250.f, 300.f);
const Vector4f ANIMATION_END_BOUNDS = Vector4f(100.f, 100.f, 300.f, 300.f);
const Vector4f ANIMATION_FIRST_BOUNDS = Vector4f(80.f, 100.f, 100.f, 250.f);
const Vector4f ANIMATION_SECOND_BOUNDS = Vector4f(50.f, 50.f, 150.f, 100.f);
const Vector4f ANIMATION_THIRD_BOUNDS = Vector4f(100.f, 80.f, 150.f, 500.f);
const Vector4f TRANSITION_EFFECT_ROTATE = Vector4f(50.f, 50.f, 50.f, 50.f);
const RotationMode ROTATION_MODE_DATA[] = {RotationMode::ROTATE_NONE,
    RotationMode::ROTATE_AUTO,
    RotationMode::ROTATE_AUTO_REVERSE};
const float BEGIN_FRACTION_DATA[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
const float END_FRACTION_DATA[] = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
constexpr uint32_t SUCCESS_INT = 1;
const std::string SUCCESS_STRING = "success";
constexpr bool SUCCESS_BOOL = true;

const Drawing::DrawingPiecewiseParameter BOUNCE_FIRST_PHASE_PARAS = {
    // the 16: duration, 0: delay, 1 and 1.01: scale
    OHOS::Rosen::Drawing::DrawingCurveType::LINEAR, {}, 16, 0, {{"sx", {1, 1.01}}, {"sy", {1, 1.01}}}
};
const Drawing::DrawingPiecewiseParameter BOUNCE_SECOND_PHASE_PARAS = {
    OHOS::Rosen::Drawing::DrawingCurveType::SPRING,
    // the -300: velocity, 1: mass,  328: stiffness, 16: damping is paramaters of animation
    {{"velocity", -300}, {"mass", 1}, {"stiffness", 328}, {"damping", 16}},
    0, 16, {{"sx", {1.01, 1}}, {"sy", {1.01, 1}}} // 16: duration, 0: delay, 1 and 1.01: scale
};
const Drawing::DrawingPiecewiseParameter APPEAR_FIRST_PHASE_PARAS = {
    OHOS::Rosen::Drawing::DrawingCurveType::SPRING,
    // the -300: velocity, 1: mass,  328: stiffness, 16: damping is paramaters of animation
    {{"velocity", 0}, {"mass", 1}, {"stiffness", 228}, {"damping", 22}},
    16, 0, {{"sx", {0.9, 1}}, {"sy", {0.9, 1}}} // 16: duration, 0: delay, 1.01: scale
};
const Drawing::DrawingPiecewiseParameter APPEAR_SECOND_PHASE_PARAS = {
    // the 100: duration, 0: delay, 0.0 and 1: alpha
    OHOS::Rosen::Drawing::DrawingCurveType::LINEAR, {}, 100, 0, {{"alpha", {0.0, 1}}}
};
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_MODULES_RENDER_SERVICE_CLIENT_TEST_UNITTEST_ANIMATION_RS_ANIMATION_TEST_UTILS_H
