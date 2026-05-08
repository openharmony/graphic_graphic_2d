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

#include <array>
#include <cstring>
#include <string>
#include <vector>

#include "ng_sdf_test_utils.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"

#include "draw/path.h"
#include "effect/rs_render_shape_base.h"
#include "render/rs_path.h"
#include "text/font.h"
#include "text/text_blob.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int COLUMN_COUNT = 2;
const std::string BACKGROUND_IMAGE_PATH = "/data/local/tmp/fg_test.jpg";

struct PathShapeParam {
    Drawing::Path path;
    Vector2f offset;
    Vector2f Scale;
};

const std::vector<Vector2f> scaleParams = {
    { 1.0f, 1.0f }, { 0.5f, 0.5f },
    { 0.25f, 0.25f }, { -0.5f, -0.5f },
    { 1.5f, 1.5f }, { 0.6f, 0.6f }
};

const std::vector<Vector2f> offsetParams = {
    { 100.0f, 300.0f }, { 200.5f, 300.0f },
    { 200.25f, 400.0f }, { -0.5f, 400.0f },
    { 55.5f, 100.5f }, { 200.0f, 200.0f }
};

const std::vector<std::string> strParams = { "0", "7", "23", "456", "478", "91" };

} // namespace

static void GetPathData(const char* str, Drawing::Path& path)
{
    Drawing::Font font;
    font.SetSize(300.0f);
    auto textblob = Drawing::TextBlob::MakeFromText(str, strlen(str), font, Drawing::TextEncoding::UTF8);
    std::vector<Drawing::Point> points;
    Drawing::TextBlob::GetDrawingPointsForTextBlob(textblob.get(), points);
    std::vector<uint16_t> glyphIds;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(textblob.get(), glyphIds);
    for (size_t i = 0; i < glyphIds.size() && i < points.size(); i++) {
        auto tempPath = Drawing::TextBlob::GetDrawingPathforTextBlob(glyphIds[i], textblob.get());
        if (tempPath.IsValid()) {
            tempPath.Offset(points[i].GetX(), points[i].GetY());
            path.AddPath(tempPath);
        }
    }
}

static void InitFrostedGlassFilter(std::shared_ptr<RSNGFrostedGlassFilter>& frostedGlassFilter)
{
    if (frostedGlassFilter == nullptr) {
        frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    }

    frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(defaultBlurParams);
    frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(defaultWeightsEmboss);
    frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(defaultWeightsEdl);
    frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(defaultBgRates);
    frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(defaultBgKBS);
    frostedGlassFilter->Setter<FrostedGlassBgPosTag>(defaultBgPos);
    frostedGlassFilter->Setter<FrostedGlassBgNegTag>(defaultBgNeg);
    frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(defaultRefractParams);
    frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(defaultSdParams);
    frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(defaultSdRates);
    frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(defaultSdKBS);
    frostedGlassFilter->Setter<FrostedGlassSdPosTag>(defaultSdPos);
    frostedGlassFilter->Setter<FrostedGlassSdNegTag>(defaultSdNeg);
    frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(defaultEnvLightParams);
    frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(defaultEnvLightRates);
    frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(defaultEnvLightKBS);
    frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(defaultEnvLightPos);
    frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(defaultEnvLightNeg);
    frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(defaultEdLightParams);
    frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(defaultEdLightAngles);
    frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(defaultEdLightDir);
    frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(defaultEdLightRates);
    frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(defaultEdLightKBS);
    frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(defaultEdLightPos);
    frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(defaultEdLightNeg);
}

static void InitThickFrostedGlassFilter(std::shared_ptr<RSNGFrostedGlassFilter>& frostedGlassFilter)
{
    if (frostedGlassFilter == nullptr) {
        frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
    }

    frostedGlassFilter->Setter<FrostedGlassBlurParamsTag>(thickBlurParams);
    frostedGlassFilter->Setter<FrostedGlassWeightsEmbossTag>(thickWeightsEmboss);
    frostedGlassFilter->Setter<FrostedGlassWeightsEdlTag>(thickWeightsEdl);
    frostedGlassFilter->Setter<FrostedGlassBgRatesTag>(thickBgRates);
    frostedGlassFilter->Setter<FrostedGlassBgKBSTag>(thickBgKBS);
    frostedGlassFilter->Setter<FrostedGlassBgPosTag>(thickBgPos);
    frostedGlassFilter->Setter<FrostedGlassBgNegTag>(thickBgNeg);
    frostedGlassFilter->Setter<FrostedGlassRefractParamsTag>(thickRefractParams);
    frostedGlassFilter->Setter<FrostedGlassSdParamsTag>(thickSdParams);
    frostedGlassFilter->Setter<FrostedGlassSdRatesTag>(thickSdRates);
    frostedGlassFilter->Setter<FrostedGlassSdKBSTag>(thickSdKBS);
    frostedGlassFilter->Setter<FrostedGlassSdPosTag>(thickSdPos);
    frostedGlassFilter->Setter<FrostedGlassSdNegTag>(thickSdNeg);
    frostedGlassFilter->Setter<FrostedGlassEnvLightParamsTag>(thickEnvLightParams);
    frostedGlassFilter->Setter<FrostedGlassEnvLightRatesTag>(thickEnvLightRates);
    frostedGlassFilter->Setter<FrostedGlassEnvLightKBSTag>(thickEnvLightKBS);
    frostedGlassFilter->Setter<FrostedGlassEnvLightPosTag>(thickEnvLightPos);
    frostedGlassFilter->Setter<FrostedGlassEnvLightNegTag>(thickEnvLightNeg);
    frostedGlassFilter->Setter<FrostedGlassEdLightParamsTag>(thickEdLightParams);
    frostedGlassFilter->Setter<FrostedGlassEdLightAnglesTag>(thickEdLightAngles);
    frostedGlassFilter->Setter<FrostedGlassEdLightDirTag>(thickEdLightDir);
    frostedGlassFilter->Setter<FrostedGlassEdLightRatesTag>(thickEdLightRates);
    frostedGlassFilter->Setter<FrostedGlassEdLightKBSTag>(thickEdLightKBS);
    frostedGlassFilter->Setter<FrostedGlassEdLightPosTag>(thickEdLightPos);
    frostedGlassFilter->Setter<FrostedGlassEdLightNegTag>(thickEdLightNeg);
}

static void SetSDFPathShapeNode(
    const std::shared_ptr<RSCanvasNode>& node, const PathShapeParam& param, int sizeX, int sizeY)
{
    Rosen::Vector4f bounds { 0, 0, sizeX, sizeY };
    node->SetBounds(bounds);
    node->SetFrame(bounds);

    auto shape = CreateShape(RSNGEffectType::SDF_PATH_SHAPE);
    auto pathShape = std::static_pointer_cast<RSNGSDFPathShape>(shape);
    pathShape->Setter<SDFPathShapePathTag>(RSPath::CreateRSPath(param.path));
    pathShape->Setter<SDFPathShapeOffsetTag>(param.offset);
    pathShape->Setter<SDFPathShapeScaleTag>(param.Scale);
    node->SetSDFShape(shape);
}

class NGSDFPathShapeTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth_, screenHeight_);
    }

private:
    const int screenWidth_ = SCREEN_WIDTH;
    const int screenHeight_ = SCREEN_HEIGHT;
};

GRAPHIC_TEST(NGSDFPathShapeTest, EFFECT_TEST, Set_SDF_PathShape_Filter_Test_1)
{
    int rowCount = static_cast<int>(scaleParams.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;

        Drawing::Path path;
        GetPathData(strParams[i].c_str(), path);
        PathShapeParam param = { path, offsetParams[i], scaleParams[i] };
        auto testNode = RSCanvasNode::Create();
        SetSDFPathShapeNode(testNode, param, sizeX, sizeY);

        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(testNode);
        RegisterNode(testNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFPathShapeTest, EFFECT_TEST, Set_SDF_PathShape_ThickFilter_Test_1)
{
    int rowCount = static_cast<int>(scaleParams.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;

        Drawing::Path path;
        GetPathData(strParams[i].c_str(), path);
        PathShapeParam param = { path, offsetParams[i], scaleParams[i] };
        auto testNode = RSCanvasNode::Create();
        SetSDFPathShapeNode(testNode, param, sizeX, sizeY);

        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitThickFrostedGlassFilter(frostedGlassFilter);
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(testNode);
        RegisterNode(testNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFPathShapeTest, EFFECT_TEST, Set_SDF_PathShape_Shadow_Test_1)
{
    int rowCount = static_cast<int>(scaleParams.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    Color color(0, 0, 0);
    Vector4<Color> outlineColor = { color, color, color, color };
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;

        Drawing::Path path;
        GetPathData(strParams[i].c_str(), path);
        PathShapeParam param = { path, offsetParams[i], scaleParams[i] };
        auto testNode = RSCanvasNode::Create();
        SetSDFPathShapeNode(testNode, param, sizeX, sizeY);
        Rosen::Vector4f bounds { x, y, sizeX, sizeY };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);

        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetShadowRadius(25.0f);
        testNode->SetShadowColor(0xFF00FF00);
        testNode->SetBackgroundColor(0xFF888888);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(NGSDFPathShapeTest, EFFECT_TEST, Set_SDF_PathShape_Transform_Test_1)
{
    int rowCount = static_cast<int>(matrix3fParams1.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;

        Drawing::Path path;
        GetPathData(strParams[0].c_str(), path);
        PathShapeParam param = { path, offsetParams[0], scaleParams[0] };
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds { 0, 0, sizeX, sizeY };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);

        auto pathShape = CreateShape(RSNGEffectType::SDF_PATH_SHAPE);
        auto sdfPathShape = std::static_pointer_cast<RSNGSDFPathShape>(pathShape);
        sdfPathShape->Setter<SDFPathShapePathTag>(RSPath::CreateRSPath(param.path));
        sdfPathShape->Setter<SDFPathShapeOffsetTag>(param.offset);
        sdfPathShape->Setter<SDFPathShapeScaleTag>(param.Scale);

        auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto sdfTransformShape = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);
        sdfTransformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams1[i].Inverse());
        sdfTransformShape->Setter<SDFTransformShapeShapeTag>(pathShape);

        testNode->SetSDFShape(transformShape);

        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(testNode);
        RegisterNode(testNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFPathShapeTest, EFFECT_TEST, Set_SDF_PathShape_Transform_Test_2)
{
    int rowCount = static_cast<int>(matrix3fParams2.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;

        Drawing::Path path;
        GetPathData(strParams[1].c_str(), path);
        PathShapeParam param = { path, offsetParams[1], scaleParams[1] };
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds { 0, 0, sizeX, sizeY };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);

        auto pathShape = CreateShape(RSNGEffectType::SDF_PATH_SHAPE);
        auto sdfPathShape = std::static_pointer_cast<RSNGSDFPathShape>(pathShape);
        sdfPathShape->Setter<SDFPathShapePathTag>(RSPath::CreateRSPath(param.path));
        sdfPathShape->Setter<SDFPathShapeOffsetTag>(param.offset);
        sdfPathShape->Setter<SDFPathShapeScaleTag>(param.Scale);

        auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto sdfTransformShape = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);
        sdfTransformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams2[i].Inverse());
        sdfTransformShape->Setter<SDFTransformShapeShapeTag>(pathShape);

        testNode->SetSDFShape(transformShape);

        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(testNode);
        RegisterNode(testNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFPathShapeTest, EFFECT_TEST, Set_SDF_PathShape_UnionOp_Test_1)
{
    int rowCount = static_cast<int>(scaleParams.size()) / 2;
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;

        Drawing::Path pathX;
        GetPathData(strParams[i].c_str(), pathX);
        Drawing::Path pathY;
        GetPathData(strParams[i + 1].c_str(), pathY);

        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds { 0, 0, sizeX, sizeY };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);

        auto unionShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(unionShape);

        auto pathShapeX = CreateShape(RSNGEffectType::SDF_PATH_SHAPE);
        auto sdfPathShapeX = std::static_pointer_cast<RSNGSDFPathShape>(pathShapeX);
        sdfPathShapeX->Setter<SDFPathShapePathTag>(RSPath::CreateRSPath(pathX));
        sdfPathShapeX->Setter<SDFPathShapeOffsetTag>(offsetParams[i]);
        sdfPathShapeX->Setter<SDFPathShapeScaleTag>(scaleParams[i]);
        unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(pathShapeX);

        auto pathShapeY = CreateShape(RSNGEffectType::SDF_PATH_SHAPE);
        auto sdfPathShapeY = std::static_pointer_cast<RSNGSDFPathShape>(pathShapeY);
        sdfPathShapeY->Setter<SDFPathShapePathTag>(RSPath::CreateRSPath(pathY));
        sdfPathShapeY->Setter<SDFPathShapeOffsetTag>(offsetParams[i + 1]);
        sdfPathShapeY->Setter<SDFPathShapeScaleTag>(scaleParams[i + 1]);
        unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(pathShapeY);

        testNode->SetSDFShape(unionShape);

        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(testNode);
        RegisterNode(testNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFPathShapeTest, EFFECT_TEST, Set_SDF_PathShape_RRect_UnionOp_Test_1)
{
    int rowCount = static_cast<int>(rrectParams.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;

        Drawing::Path path;
        GetPathData(strParams[i % strParams.size()].c_str(), path);

        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds { 0, 0, sizeX, sizeY };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);

        auto unionShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(unionShape);

        auto pathShape = CreateShape(RSNGEffectType::SDF_PATH_SHAPE);
        auto sdfPathShape = std::static_pointer_cast<RSNGSDFPathShape>(pathShape);
        sdfPathShape->Setter<SDFPathShapePathTag>(RSPath::CreateRSPath(path));
        sdfPathShape->Setter<SDFPathShapeOffsetTag>(offsetParams[i % offsetParams.size()]);
        sdfPathShape->Setter<SDFPathShapeScaleTag>(scaleParams[i % scaleParams.size()]);
        unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(pathShape);

        auto rRectShape = CreateShape(RSNGEffectType::SDF_RRECT_SHAPE);
        auto sdfRRectShape = std::static_pointer_cast<RSNGSDFRRectShape>(rRectShape);
        sdfRRectShape->Setter<SDFRRectShapeRRectTag>(rrectParams[i]);
        unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(rRectShape);

        testNode->SetSDFShape(unionShape);

        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(testNode);
        RegisterNode(testNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFPathShapeTest, EFFECT_TEST, Set_SDF_PathShape_SmoothUnionOp_Test_1)
{
    int rowCount = static_cast<int>(scaleParams.size()) / 2;
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;

        Drawing::Path pathX;
        GetPathData(strParams[i].c_str(), pathX);
        Drawing::Path pathY;
        GetPathData(strParams[i + 1].c_str(), pathY);

        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds { 0, 0, sizeX, sizeY };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);

        auto smoothUnionShape = CreateShape(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE);
        auto sdfSmoothUnionShape = std::static_pointer_cast<RSNGSDFSmoothUnionOpShape>(smoothUnionShape);

        auto pathShapeX = CreateShape(RSNGEffectType::SDF_PATH_SHAPE);
        auto sdfPathShapeX = std::static_pointer_cast<RSNGSDFPathShape>(pathShapeX);
        sdfPathShapeX->Setter<SDFPathShapePathTag>(RSPath::CreateRSPath(pathX));
        sdfPathShapeX->Setter<SDFPathShapeOffsetTag>(offsetParams[i]);
        sdfPathShapeX->Setter<SDFPathShapeScaleTag>(scaleParams[i]);
        sdfSmoothUnionShape->Setter<SDFSmoothUnionOpShapeShapeXTag>(pathShapeX);

        auto pathShapeY = CreateShape(RSNGEffectType::SDF_PATH_SHAPE);
        auto sdfPathShapeY = std::static_pointer_cast<RSNGSDFPathShape>(pathShapeY);
        sdfPathShapeY->Setter<SDFPathShapePathTag>(RSPath::CreateRSPath(pathY));
        sdfPathShapeY->Setter<SDFPathShapeOffsetTag>(offsetParams[i + 1]);
        sdfPathShapeY->Setter<SDFPathShapeScaleTag>(scaleParams[i + 1]);
        sdfSmoothUnionShape->Setter<SDFSmoothUnionOpShapeShapeYTag>(pathShapeY);

        sdfSmoothUnionShape->Setter<SDFSmoothUnionOpShapeSpacingTag>(
            sdfShapeSpacingParams[i % sdfShapeSpacingParams.size()]);

        testNode->SetSDFShape(smoothUnionShape);

        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(testNode);
        RegisterNode(testNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

GRAPHIC_TEST(NGSDFPathShapeTest, EFFECT_TEST, Set_SDF_PathShape_Transform_UnionOp_Test_1)
{
    int rowCount = static_cast<int>(matrix3fParams1.size());
    auto sizeX = SCREEN_WIDTH / COLUMN_COUNT;
    auto sizeY = SCREEN_HEIGHT * COLUMN_COUNT / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % COLUMN_COUNT) * sizeX;
        int y = (i / COLUMN_COUNT) * sizeY;

        Drawing::Path pathX;
        GetPathData(strParams[0].c_str(), pathX);
        Drawing::Path pathY;
        GetPathData(strParams[1].c_str(), pathY);

        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds { 0, 0, sizeX, sizeY };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);

        auto unionShape = CreateShape(RSNGEffectType::SDF_UNION_OP_SHAPE);
        auto unionOpShape = std::static_pointer_cast<RSNGSDFUnionOpShape>(unionShape);

        auto pathShapeX = CreateShape(RSNGEffectType::SDF_PATH_SHAPE);
        auto sdfPathShapeX = std::static_pointer_cast<RSNGSDFPathShape>(pathShapeX);
        sdfPathShapeX->Setter<SDFPathShapePathTag>(RSPath::CreateRSPath(pathX));
        sdfPathShapeX->Setter<SDFPathShapeOffsetTag>(offsetParams[0]);
        sdfPathShapeX->Setter<SDFPathShapeScaleTag>(scaleParams[0]);
        unionOpShape->Setter<SDFUnionOpShapeShapeXTag>(pathShapeX);

        auto pathShapeY = CreateShape(RSNGEffectType::SDF_PATH_SHAPE);
        auto sdfPathShapeY = std::static_pointer_cast<RSNGSDFPathShape>(pathShapeY);
        sdfPathShapeY->Setter<SDFPathShapePathTag>(RSPath::CreateRSPath(pathY));
        sdfPathShapeY->Setter<SDFPathShapeOffsetTag>(offsetParams[1]);
        sdfPathShapeY->Setter<SDFPathShapeScaleTag>(scaleParams[1]);
        unionOpShape->Setter<SDFUnionOpShapeShapeYTag>(pathShapeY);

        auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
        auto sdfTransformShape = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);
        sdfTransformShape->Setter<SDFTransformShapeMatrixTag>(matrix3fParams1[i].Inverse());
        sdfTransformShape->Setter<SDFTransformShapeShapeTag>(unionShape);

        testNode->SetSDFShape(transformShape);

        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        testNode->SetMaterialNGFilter(frostedGlassFilter);

        auto childNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { x, y, sizeX, sizeY });
        childNode->AddChild(testNode);
        RegisterNode(testNode);
        GetRootNode()->AddChild(childNode);
        RegisterNode(childNode);
    }
}

} // namespace OHOS::Rosen
