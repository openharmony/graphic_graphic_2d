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
void InitSmoothUnionShapesByPixelmap(std::shared_ptr<RSNGShapeBase>& rootShape,
    std::shared_ptr<Media::PixelMap> pixelmapX, std::shared_ptr<Media::PixelMap> pixelmapY, float spacing);

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

//Thick Values
const Vector2f thickBlurParams = Vector2f(80.0f, 1.0f);
const Vector2f thickWeightsEmboss = Vector2f(0.0f, 0.0f); // (envLight, sd)
const Vector2f thickWeightsEdl = Vector2f(0.0f, 0.0f); // (envLight, sd)
const Vector3f thickRefractParams = Vector3f(0.0f, 0.0f, 0.0f);
const Vector2f thickBgRates = Vector2f(-0.0f, 0.0f);
const Vector3f thickBgKBS = Vector3f(0.6975f, 0.2706f, 1.2f);
const Vector3f thickBgPos = Vector3f(1.0f, 1.5f, 1.0f);
const Vector3f thickBgNeg = Vector3f(1.2f, 1.0f, 1.0f);
const Vector3f thickSdParams = Vector3f(0.0f, 0.0f, 0.0f);
const Vector2f thickSdRates = Vector2f(0.0f, 0.0f);
const Vector3f thickSdKBS = Vector3f(0.0f, 0.0f, 0.0f);
const Vector3f thickSdPos = Vector3f(0.0f, 0.0f, 0.0f);
const Vector3f thickSdNeg = Vector3f(0.0f, 0.0f, 0.0f);
const Vector3f thickEnvLightParams = Vector3f(0.8f, 0.157f, 2.0f);
const Vector2f thickEnvLightRates = Vector2f(0.0f, 0.0f);
const Vector3f thickEnvLightKBS = Vector3f(1.0f, 0.2268f, 1.5f);
const Vector3f thickEnvLightPos = Vector3f(1.0f, 1.5f, 2.0f);
const Vector3f thickEnvLightNeg = Vector3f(1.7f, 3.0f, 1.0f);
const Vector2f thickEdLightParams = Vector2f(2.7f, 3.0f);
const Vector2f thickEdLightAngles = Vector2f(75.0f, 85.0f);
const Vector2f thickEdLightDir = Vector2f(0.0f, -1.0f);
const Vector2f thickEdLightRates = Vector2f(0.0f, 0.0f);
const Vector3f thickEdLightKBS = Vector3f(0.8f, 0.3922f, 1.5f);
const Vector3f thickEdLightPos = Vector3f(1.0f, 1.5f, 2.0f);
const Vector3f thickEdLightNeg = Vector3f(1.7f, 3.0f, 1.0f);

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
    Matrix3f{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, // invalid
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
    Matrix3f{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0003f, 0.0f, 1.0f}, // perspX
    Matrix3f{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0003f, 1.0f}, // perspY
    Matrix3f{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0003f, 0.0003f, 1.0f}, // perspXY
    Matrix3f{1.5f, 0.577f, -20.0f, 0.577f, 1.5f, -20.0f, 0.0f, 0.0f, 1.0f}, // combination1
    Matrix3f{0.5f, 1.17f, -100.0f, 1.17f, 0.5f, -30.0f, 0.0f, 0.0f, 1.0f}, // combination2
    Matrix3f{0.866f, -0.5f, 0.0f, 0.5f, 0.866f, 0.0f, 0.0f, 0.0006f, 1.0f}, // combination3
};
const std::vector<float> sdfShapeSpacingParams = {0.0f, 30.0f, 50.0f, 100.0f, 500000.0f, -5.0f};

const std::vector<std::string> sdfPixelmapShapePath = {
    "/data/local/tmp/sdfImage0.png",
    "/data/local/tmp/sdfImage3.png",
    "/data/local/tmp/sdfImage5.png",
    "/data/local/tmp/sdfImage6.png",
    "/data/local/tmp/sdfImage8.png",
    "/data/local/tmp/sdfImage9.png",
};

const std::vector<std::tuple<Vector2f, Vector2f, Vector2f>> triangleParams = {
    {Vector2f(100, 100), Vector2f(200, 100), Vector2f(150, 200)},
    {Vector2f(50, 50), Vector2f(300, 50), Vector2f(175, 300)},
    {Vector2f(100, 50), Vector2f(400, 50), Vector2f(250, 400)},
    {Vector2f(100, 100), Vector2f(150, 80), Vector2f(180, 150)},
};

const std::vector<float> triangleRadiusParams = {
    0.0f, 10.0f, 30.0f, 50.0f, 100.0f
};

const std::vector<std::tuple<Vector2f, Vector2f, Vector2f>> degenerateTriangleParams = {
    {Vector2f(100, 100), Vector2f(200, 100), Vector2f(300, 100)},
    {Vector2f(100, 100), Vector2f(100, 200), Vector2f(100, 300)},
    {Vector2f(100, 100), Vector2f(150, 150), Vector2f(200, 200)},
};

const std::vector<std::tuple<Vector2f, Vector2f, Vector2f>> negativeCoordTriangleParams = {
    {Vector2f(-50, 100), Vector2f(100, -50), Vector2f(150, 200)},
    {Vector2f(-100, -100), Vector2f(100, -100), Vector2f(0, 100)},
};

const std::vector<Vector4f> outlineWidthParams = {
    {-5, 5, 5, 5},
    {0, 0, 5, 5},
    {25, 0, 5, 5},
    {100, 0, 5, 5},
};

const std::vector<float> borderWidthParams = {
    -5.0f, 0.0f, 25.0f, 100.0f
};

const std::vector<float> shadowRadiusParams = {
    -5.0f, 5.0f, 25.0f, 100.0f
};

void InitSmoothUnionShapesByTriangle(
    std::shared_ptr<RSNGShapeBase>& rootShape,
    Vector2f v0, Vector2f v1, Vector2f v2, float radius1,
    Vector2f v3, Vector2f v4, Vector2f v5, float radius2,
    float spacing);
}  // namespace OHOS::Rosen
#endif // NG_SDF_TEST_UTILS_H