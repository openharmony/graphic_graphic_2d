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
#ifndef NG_SDF_TEST_UTILS_H
#define NG_SDF_TEST_UTILS_H

#include "effect/rs_render_shape_base.h"
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/property/include/rs_ui_shape_base.h"

namespace OHOS::Rosen {

std::shared_ptr<RSNGShapeBase> CreateShape(RSNGEffectType type);
void InitSmoothUnionShapes(
    std::shared_ptr<RSNGShapeBase>& rootShape, RRect rRectX, RRect rRectY, float spacing);

// Default values
const Vector2f defaultBlurParams = Vector2f(48.0f, 4.0f);
const Vector2f defaultWeightsEmboss = Vector2f(1.0f, 1.0f); // (envLight, sd)
const Vector2f defaultWeightsEdl = Vector2f(1.0f, 1.0f); // (envLight, sd)
const Vector3f defaultRefractParams = Vector3f(0.010834f, 0.007349f, 1.2f);
const Vector2f defaultBgRates = Vector2f(-0.00003f, 1.2f);
const Vector3f defaultBgKBS = Vector3f(0.010834f, 0.007349f, 1.2f);
const Vector3f defaultBgPos = Vector3f(0.3f, 0.5f, 1.0f);
const Vector3f defaultBgNeg = Vector3f(0.5f, 0.5f, 1.0f);
const Vector3f defaultSdParams = Vector3f(0.0f, 2.0f, 0.0f);
const Vector2f defaultSdRates = Vector2f(0.0f, 0.0f);
const Vector3f defaultSdKBS = Vector3f(-0.02f, 2.0f, 4.62f);
const Vector3f defaultSdPos = Vector3f(1.0f, 1.5f, 2.0f);
const Vector3f defaultSdNeg = Vector3f(1.7f, 3.0f, 1.0f);
const Vector3f defaultEnvLightParams = Vector3f(0.8f, 0.2745f, 2.0f);
const Vector2f defaultEnvLightRates = Vector2f(0.0f, 0.0f);
const Vector3f defaultEnvLightKBS = Vector3f(0.8f, 0.2745f, 2.0f);
const Vector3f defaultEnvLightPos = Vector3f(1.0f, 1.5f, 2.0f);
const Vector3f defaultEnvLightNeg = Vector3f(1.7f, 3.0f, 1.0f);
const Vector2f defaultEdLightParams = Vector2f(2.0f, -1.0f);
const Vector2f defaultEdLightAngles = Vector2f(30.0f, 30.0f);
const Vector2f defaultEdLightDir = Vector2f(-1.0f, 1.0f);
const Vector2f defaultEdLightRates = Vector2f(0.0f, 0.0f);
const Vector3f defaultEdLightKBS = Vector3f(0.6027f, 0.64f, 2.0f);
const Vector3f defaultEdLightPos = Vector3f(1.0f, 1.5f, 2.0f);
const Vector3f defaultEdLightNeg = Vector3f(1.7f, 3.0f, 1.0f);
const Vector2f defaultBorderSize = Vector2f(200.0f, 100.0f);
const float DEFAULT_CORNER_RADIUS = 100.0f;

// Param arrays
const std::vector<RRect> rectXParams = {
    RRect{RectT<float>{50, 150, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{100, 200, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{150, 250, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{150, 250, 150, 150}, 2.0f, 2.0f},
    RRect{RectT<float>{100, 200, 0, 10}, 1.0f, 1.0f}, // invalid
    RRect{RectT<float>{150, 250, 0, 10}, 1.0f, 1.0f}, // invalid
};

const std::vector<RRect> rectYParams = {
    RRect{RectT<float>{200, 300, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{200, 300, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{200, 300, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{180, 280, 150, 150}, 1.0f, 1.0f},
    RRect{RectT<float>{200, 300, 150, 150}, 2.0f, 2.0f},
    RRect{RectT<float>{200, 300, 0, 10}, 1.0f, 1.0f}, // invalid
};

const std::vector<RRect> rrectParams = {
    RRect{RectT<float>{50, 50, 300, 300}, 0.0f, 0.0f},
    RRect{RectT<float>{100, 100, 300, 300}, 10.0f, 10.0f},
    RRect{RectT<float>{100, 100, 300, 300}, 50.0f, 50.0f},
    RRect{RectT<float>{100, 100, 200, 200}, 50.0f, 50.0f},
    RRect{RectT<float>{100, 100, 150, 150}, 100.0f, 100.0f},
    RRect{RectT<float>{100, 100, 0, 10}, 1.0f, 1.0f}, // invalid
};

const std::vector<Matrix3f> matrix3fParams1 = {
    // scale_x, skew_x, trans_x, skew_y, scale_y, trans_y, persp_0, persp_1, persp_2
    Matrix3f{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // unit
    Matrix3f{1.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // translateX
    Matrix3f{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -50.0f, 0.0f, 0.0f, 1.0f}, // translateY
    Matrix3f{1.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // scaleX
    Matrix3f{1.0f, 0.0f, 0.0f, 0.0f, 1.5f, 0.0f, 0.0f, 0.0f, 1.0f}, // scaleY
    Matrix3f{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // invaid
};

const std::vector<Matrix3f> matrix3fParams2 = {
    // scale_x, skew_x, trans_x, skew_y, scale_y, trans_y, persp_0, persp_1, persp_2
    Matrix3f{0.866f, -0.5f, 0.0f, 0.5f, 0.866f, 0.0f, 0.0f, 0.0f, 1.0f}, // rotation30
    Matrix3f{0.5f, -0.866f, 0.0f, 0.866f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f}, // rotation60
    Matrix3f{0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // rotation90
    Matrix3f{1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // skewX
    Matrix3f{1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // skewY
    Matrix3f{1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // skewXY
};

const std::vector<Matrix3f> matrix3fParams3 = {
    // scale_x, skew_x, trans_x, skew_y, scale_y, trans_y, persp_0, persp_1, persp_2
    Matrix3f{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0003f, 0.0f, 1.0f}, // perpX
    Matrix3f{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0003f, 1.0f}, // perpY
    Matrix3f{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0003f, 0.0003f, 1.0f}, // perspXY
    Matrix3f{1.5f, 0.577f, -20.0f, 0.577f, 1.5f, -20.0f, 0.0f, 0.0f, 1.0f}, // combination1
    Matrix3f{0.5f, 1.17f, -100.0f, 1.17f, 0.5f, -30.0f, 0.0f, 0.0f, 1.0f}, // combination2
    Matrix3f{0.866f, -0.5f, 0.0f, 0.5f, 0.866f, 0.0f, 0.0f, 0.0006f, 1.0f}, // combination3
};
const std::vector<float> sdfShapeSpacingParams = {0.0f, 30.0f, 50.0f, 100.0f, 500000.0f, -5.0f};
}  // namespace OHOS::Rosen
#endif // NG_SDF_TEST_UTILS_H
