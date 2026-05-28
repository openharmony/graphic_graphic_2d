/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_director.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui/rs_effect_node.h"
#include "../ng_shape_test/ng_sdf_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const std::string g_foregroundImagePath = "/data/local/tmp/fg_test.jpg";
const std::string g_backgroundImagePath = "/data/local/tmp/Images/backGroundImage.jpg";
// Frosted Glass Default values
const Vector2f DEFAULT_WEIGHTS_EMBOSS = Vector2f(1.0f, 0.5f);
const Vector2f DEFAULT_WEIGHTS_EDL = Vector2f(1.0f, 1.0f);
// Background darken parameters
const Vector2f DEFAULT_BG_RATES = Vector2f(0.0f, 0.0f);
const Vector3f DEFAULT_BG_KBS = Vector3f(0.641f, 0.4966f, 1.5f);
const Vector3f DEFAULT_BG_POS = Vector3f(1.0f, 1.0f, 1.0f);
const Vector3f DEFAULT_BG_NEG = Vector3f(2.f, 3.0f, 1.0f);
// Refraction parameters
const Vector3f DEFAULT_REFRACT_PARAMS = Vector3f(1.0f, 0.3f, 0.3f);
// Inner shadow parameters
const Vector3f DEFAULT_SD_PARAMS = Vector3f(-50.0f, 6.0f, 6.62f);
const Vector2f DEFAULT_SD_RATES = Vector2f(0.0f, 0.0f);
const Vector3f DEFAULT_SD_KBS = Vector3f(0.9f, 0.0f, 1.0f);
const Vector3f DEFAULT_SD_POS = Vector3f(1.0f, 1.7f, 1.5f);
const Vector3f DEFAULT_SD_NEG = Vector3f(3.0f, 2.0f, 1.0f);
// Env refraction parameters
const Vector2f DEFAULT_ENV_LIGHT_PARAMS = Vector2f(20.0f, 5.0f); // envB, envS
const Vector2f DEFAULT_ENV_LIGHT_RATES = Vector2f(0.0f, 0.0f);
const Vector3f DEFAULT_ENV_LIGHT_KBS = Vector3f(0.8f, 0.27451f, 2.0f);
const Vector3f DEFAULT_ENV_LIGHT_POS = Vector3f(1.0f, 1.7f, 1.5f);
const Vector3f DEFAULT_ENV_LIGHT_NEG = Vector3f(3.0f, 2.0f, 1.0f);
// Edge highlights parameters
const Vector2f DEFAULT_ED_LIGHT_PARAMS = Vector2f(2.0f, 2.0f);
const Vector2f DEFAULT_ED_LIGHT_ANGLES = Vector2f(40.0f, 20.0f);
const Vector2f DEFAULT_ED_LIGHT_DIR = Vector2f(2.5f, 2.5f);
const Vector2f DEFAULT_ED_LIGHT_RATES = Vector2f(0.0f, 0.0f);
const Vector3f DEFAULT_ED_LIGHT_KBS = Vector3f(0.6027f, 0.627451f, 2.0f);
const Vector3f DEFAULT_ED_LIGHT_POS = Vector3f(1.0f, 1.7f, 1.5f);
const Vector3f DEFAULT_ED_LIGHT_NEG = Vector3f(3.2f, 2.0f, 1.0f);
const Vector4f DEFAULT_MATERIAL_COLOR = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
// Blur parameters
const float DEFAULT_BLUR_RADIUS = 2.0f;
const float DEFAULT_BLUR_PARAMS = 0.5f; // K times downsample
const float DEFAULT_BLUR_REFRACT_OUT_PX = 0.010834f;
}

class SpatialGlassEffectTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    std::shared_ptr<Rosen::RSCanvasNode> SetCommonBackgroundNode()
    {
        // set background node
        auto backgroundTestNode = SetUpNodeBgImage(g_backgroundImagePath, {0, 0, screenWidth, screenHeight});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        return backgroundTestNode;
    }

    std::shared_ptr<RSEffectNode> SetDefaultFrostedGlassBlurEffectNode()
    {
        auto effectNode = RSEffectNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        effectNode->SetBounds({0, 0, screenWidth, screenHeight});
        effectNode->SetFrame({0, 0, screenWidth, screenHeight});
 
        auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusTag>(DEFAULT_BLUR_RADIUS);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRefractOutPxTag>(DEFAULT_BLUR_REFRACT_OUT_PX);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusScaleKTag>(DEFAULT_BLUR_PARAMS);
 
        effectNode->SetBackgroundNGFilter(frostedGlassBlurFilter);
        effectNode->SetClipToBounds(true);
        GetRootNode()->AddChild(effectNode);
        RegisterNode(effectNode);
 
        return effectNode;
    }

    void SetEffectChildNode(const int i, const int columnCount, const int rowCount,
        std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGSpatialGlassEffect>& spatialGlassEffect)
    {
        auto sizeX = (columnCount != 0) ? (screenWidth / columnCount) : screenWidth;
        auto sizeY = (rowCount != 0) ? (screenHeight * columnCount / rowCount) : screenHeight;
        int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;
        // set effect child node
        auto effectChildNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        effectChildNode->SetBounds(x, y, sizeX, sizeY);
        effectChildNode->SetFrame(x, y, sizeX, sizeY);
        //  apply sdf on effect effect child node
        const RRect defaultRectParam = {
            RectT<float>{sizeX / 4, sizeY / 4, sizeX / 2, sizeY / 2}, sizeX / 16, sizeX / 16
        };
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, defaultRectParam, defaultRectParam, 0.0);
        effectChildNode->SetSDFShape(sdfShape);
        // apply frostedGlassEffect on child node
        effectChildNode->SetBackgroundNGShader(spatialGlassEffect);
        effectNode->AddChild(effectChildNode);
        RegisterNode(effectChildNode);
    }
private:
    const int screenWidth = 960;
    const int screenHeight = 1600;
};

static void InitSpatialGlassEffect(std::shared_ptr<RSNGSpatialGlassEffect>& spatialGlassEffect)
{
    spatialglasseffect->Setter<SpatialGlassEffectLeftTopTag>(Vector3f(-0.8f, 1.5f, -1.2f));
    spatialglasseffect->Setter<SpatialGlassEffectRightTopTag>(Vector3f(1.4f, 1.5f, -1.0f));
    spatialglasseffect->Setter<SpatialGlassEffectLeftBotTag>(Vector3f(-0.8f, -1.5f, -1.2f));
    spatialglasseffect->Setter<SpatialGlassEffectRightBotTag>(Vector3f(1.4f, -1.5f, -1.0f));
    spatialglasseffect->Setter<SpatialGlassEffectCameraPositionTag>(Vector3f(0.0f, 0.0f, 0.0f));
    spatialglasseffect->Setter<SpatialGlassEffectCameraIntrinsicsTag>(Vector2<float>(0.0f, 0.0f));
    spatialglasseffect->Setter<SpatialGlassEffectLightDirTag>(Vector3f(1.0f, 1.0f, 0.0f));
    spatialglasseffect->Setter<SpatialGlassEffectLightColorTag>(Vector3f(1.0f, 1.0f, 1.0f));
    spatialglasseffect->Setter<SpatialGlassEffectLightIntensityTag>(1.0f);
    spatialglasseffect->Setter<SpatialGlassEffectThickParamsTag>(Vector2f(0.01f, 0.01f));
    spatialglasseffect->Setter<SpatialGlassEffectGlassBaseColorTag>(Vector4f(0.0f, 0.0f, 0.0f, 0.2f));
    spatialglasseffect->Setter<SpatialGlassEffectFresnelEnvColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    spatialglasseffect->Setter<SpatialGlassEffectFresnelEnvParamsTag>(Vector2f(0.05f, 0.5f));
    spatialglasseffect->Setter<SpatialGlassEffectBgRatesTag>(Vector2f(0.0f, 0.0f));
    spatialglasseffect->Setter<SpatialGlassEffectBgKBSTag>(Vector3f(0.97f, 0.23f, 1.2f));
    spatialglasseffect->Setter<SpatialGlassEffectBgPosTag>(Vector3f(0.5f, 0.5f, 1.0f));
    spatialglasseffect->Setter<SpatialGlassEffectBgNegTag>(Vector3f(2.0f, 5.0f, 4.0f));
    spatialglasseffect->Setter<SpatialGlassEffectRefractParamsTag>(Vector3f(0.2f, 0.1f, 0.0f));
    spatialglasseffect->Setter<SpatialGlassEffectWeightsEdlTag>(Vector2f(1.0f, 1.0f));
    spatialglasseffect->Setter<SpatialGlassEffectEnvLightParamsTag>(Vector3f(0.0f, 0.0f, 0.0f));
    spatialglasseffect->Setter<SpatialGlassEffectEnvLightRatesTag>(Vector2f(0.0f, 0.0f));
    spatialglasseffect->Setter<SpatialGlassEffectEnvLightKBSTag>(Vector3f(0.010834f, 0.007349f, 1.2f));
    spatialglasseffect->Setter<SpatialGlassEffectEnvLightPosTag>(Vector3f(0.3f, 0.5f, 1.0f));
    spatialglasseffect->Setter<SpatialGlassEffectEnvLightNegTag>(Vector3f(0.5f, 0.5f, 1.0f));
    spatialglasseffect->Setter<SpatialGlassEffectSdParamsTag>(Vector3f(0.0f, 0.0f, 0.0f));
    spatialglasseffect->Setter<SpatialGlassEffectSdRatesTag>(Vector2f(0.0f, 0.0f));
    spatialglasseffect->Setter<SpatialGlassEffectSdKBSTag>(Vector3f(0.010834f, 0.007349f, 1.2f));
    spatialglasseffect->Setter<SpatialGlassEffectSdPosTag>(Vector3f(0.3f, 0.5f, 1.0f));
    spatialglasseffect->Setter<SpatialGlassEffectSdNegTag>(Vector3f(0.5f, 0.5f, 1.0f));
}

GRAPHIC_TEST(SpatialGlassEffectTest, EFFECT_TEST, Set_SpatialGlassEffect_ThickParams_Test)
{
    const int columnCount = 1;
    const int rowCount = 2;
    auto backgroundTestNode = SetCommonBackgroundNode();
    auto effectNode = SetDefaultFrostedGlassBlurEffectNode();
    const std::vector<Vector2f> ThickParams = {Vector2f(0.03f, 0.01f), Vector2f{0.5f, 1.0f}};
    for (int i = 0; i < rowCount; i++) {
        auto spatialGlassEffect = std::make_shared<RSNGSpatialGlassEffect>();
        InitSpatialGlassEffect(spatialGlassEffect);
        spatialGlassEffect->Setter<SpatialGlassEffectThickParamsTag>(ThickParams[i]);
        SetEffectChildNode(i, columnCount, rowCount, effectNode, spatialGlassEffect);
    }
}

GRAPHIC_TEST(SpatialGlassEffectTest, EFFECT_TEST, Set_SpatialGlassEffect_LightDirParams_Test)
{
    const int columnCount = 1;
    const int rowCount = 2;
    auto backgroundTestNode = SetCommonBackgroundNode();
    auto effectNode = SetDefaultFrostedGlassBlurEffectNode();
    const std::vector<Vector3f> LightDirParams = {Vector3f(1.0f, 1.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f)};
    for (int i = 0; i < rowCount; i++) {
        auto spatialGlassEffect = std::make_shared<RSNGSpatialGlassEffect>();
        InitSpatialGlassEffect(spatialGlassEffect);
        spatialGlassEffect->Setter<SpatialGlassEffectLightDirTag>(LightDirParams[i]);
        SetEffectChildNode(i, columnCount, rowCount, effectNode, spatialGlassEffect);
    }
}

GRAPHIC_TEST(SpatialGlassEffectTest, EFFECT_TEST, Set_SpatialGlassEffect_RefractParams_Test)
{
    const int columnCount = 1;
    const int rowCount = 2;
    auto backgroundTestNode = SetCommonBackgroundNode();
    auto effectNode = SetDefaultFrostedGlassBlurEffectNode();
    const std::vector<Vector3f> RefractParams = {
        Vector3f(30.0f, 1.0f, 0.0f),
        Vector3f(-30.0f, 1.0f, 0.0f)
    };
    for (int i = 0; i < rowCount; i++) {
        auto spatialGlassEffect = std::make_shared<RSNGSpatialGlassEffect>();
        InitSpatialGlassEffect(spatialGlassEffect);
        spatialglasseffect->Setter<SpatialGlassEffectRefractParamsTag>(RefractParams[i]);
        SetEffectChildNode(i, columnCount, rowCount, effectNode, spatialGlassEffect);}
}

GRAPHIC_TEST(SpatialGlassEffectTest, EFFECT_TEST, Set_SpatialGlassEffect_SdParams_Test)
{
    const int columnCount = 1;
    const int rowCount = 2;
    auto backgroundTestNode = SetCommonBackgroundNode();
    auto effectNode = SetDefaultFrostedGlassBlurEffectNode();
    const std::vector<Vector3f> SdParams = {
        Vector3f(0.1f, 0.0f, 0.0f),
        Vector3f(0.3f, 0.0f, 0.0f)
    };
    for (int i = 0; i < rowCount; i++) {
        auto spatialGlassEffect = std::make_shared<RSNGSpatialGlassEffect>();
        InitSpatialGlassEffect(spatialGlassEffect);
        spatialGlassEffect->Setter<SpatialGlassEffectSdParamsTag>(SdParams[i]);
        SetEffectChildNode(i, columnCount, rowCount, effectNode, spatialGlassEffect);}
}
}  // namespace OHOS::Rosen