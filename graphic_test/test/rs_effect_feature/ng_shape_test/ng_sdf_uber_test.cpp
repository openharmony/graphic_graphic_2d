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

#include <algorithm>
#include <string>
#include <vector>

#include "ng_sdf_test_utils.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"

#include "effect/rs_render_shape_base.h"
#include "ui/rs_union_node.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int SCENE_MARGIN = 40;
constexpr int SCENE_MARGIN_SIDE_COUNT = 2;
constexpr int SCENE_WIDTH = SCREEN_WIDTH - SCENE_MARGIN * SCENE_MARGIN_SIDE_COUNT;
constexpr int SCENE_HEIGHT = SCREEN_HEIGHT - SCENE_MARGIN * SCENE_MARGIN_SIDE_COUNT;
constexpr int BASIC_LEAF_MIN_COUNT = 7;
constexpr int BASIC_LEAF_MAX_COUNT = 30;
constexpr int BASIC_OVER_SPEC_COUNT = 31;
constexpr int PIXELMAP_LEAF_MIN_COUNT = 2;
constexpr int PIXELMAP_LEAF_MAX_COUNT = 6;
constexpr int PIXELMAP_OVER_SPEC_COUNT = 7;
constexpr int BASIC_COLUMN_COUNT = 6;
constexpr int BASIC_SHAPE_SIZE = 120;
constexpr int BASIC_GRID_STEP = 150;
constexpr int PIXELMAP_COLUMN_COUNT = 3;
constexpr int PIXELMAP_BOX_SIZE = 300;
constexpr int PIXELMAP_GAP = 15;
constexpr int PIXELMAP_OVER_COLUMN_COUNT = 4;
constexpr int PIXELMAP_OVER_BOX_SIZE = 250;
constexpr int PIXELMAP_OVER_GAP = 15;
constexpr int BASIC_LEAF_TYPE_COUNT = 3;
constexpr int RECT_CORNER_TYPE_COUNT = 4;
constexpr int RECT_KIND_G2 = 0;
constexpr int RECT_KIND_G1_UNIFORM = 1;
constexpr int RECT_KIND_CAPSULE = 2;
constexpr float BORDER_WIDTH = 5.0f;
constexpr float SMOOTH_UNION_SPACING = 50.0f;
constexpr float G2_CORNER_RADIUS = 10.0f;
constexpr float G1_UNIFORM_CORNER_RADIUS = 45.0f;
constexpr float CAPSULE_CORNER_RADIUS = 60.0f;
constexpr float TRIANGLE_INSET = 8.0f;
constexpr float TRIANGLE_CORNER_RADIUS = 12.0f;
constexpr float ELLIPSE_HEIGHT_RATIO = 0.8f;
constexpr float HALF_RATIO = 0.5f;
const Vector4f NON_UNIFORM_CORNER_RADIUS { 10.0f, 20.0f, 30.0f, 40.0f };
const std::string BACKGROUND_IMAGE_PATH = "/data/local/tmp/fg_test.jpg";
const std::vector<std::string> UBER_PIXELMAP_PATHS = {
    "/data/local/tmp/sdfImage0.png",
    "/data/local/tmp/sdfImage3.png",
    "/data/local/tmp/sdfImage5.png",
    "/data/local/tmp/sdfImage6.png",
    "/data/local/tmp/sdfImage8.png",
    "/data/local/tmp/sdfImage9.png",
};

enum class BasicLeafType {
    RRECT,
    TRIANGLE,
    ELLIPSE,
};

const Vector4f SOLID_BORDER_COLOR { 0.0f, 1.0f, 0.0f, 1.0f };
constexpr int32_t BORDER_STYLE_SOLID = 0;

void InitSolidBorderSDFShader(std::shared_ptr<RSNGBorderSDFShader>& shader)
{
    shader->Setter<BorderSDFShaderColorTag>(SOLID_BORDER_COLOR);
    shader->Setter<BorderSDFShaderWidthTag>(BORDER_WIDTH);
    shader->Setter<BorderSDFShaderIsOutlineTag>(false);
    shader->Setter<BorderSDFShaderStyleTag>(BORDER_STYLE_SOLID);
}
} // namespace

class NGSDFUberTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    std::shared_ptr<RSCanvasNode> CreateEffectNode(
        const std::shared_ptr<RSCanvasNode>& backgroundNode, int sizeX, int sizeY, float spacing, int unionMode)
    {
        auto unionNode = RSUnionNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        Rosen::Vector4f bounds { SCENE_MARGIN, SCENE_MARGIN, sizeX, sizeY };
        unionNode->SetBounds(bounds);
        unionNode->SetFrame(bounds);
        auto frostedGlassFilter = std::make_shared<RSNGFrostedGlassFilter>();
        InitFrostedGlassFilter(frostedGlassFilter);
        unionNode->SetMaterialNGFilter(frostedGlassFilter);
        unionNode->SetUnionSpacing(spacing);
        unionNode->SetUnionMode(unionMode);
        auto borderShader = std::make_shared<RSNGBorderSDFShader>();
        InitSolidBorderSDFShader(borderShader);
        unionNode->SetBorderSDFShader(borderShader);
        backgroundNode->AddChild(unionNode);
        RegisterNode(unionNode);
        return unionNode;
    }

    void AddRRectLeaf(const std::shared_ptr<RSCanvasNode>& unionNode, float x, float y, int size, int index)
    {
        auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        childNode->SetBounds({ x, y, static_cast<float>(size), static_cast<float>(size) });
        childNode->SetUseUnion(true);
        switch (index % RECT_CORNER_TYPE_COUNT) {
            case RECT_KIND_G2:
                childNode->SetCornerRadius(G2_CORNER_RADIUS);
                break;
            case RECT_KIND_G1_UNIFORM:
                childNode->SetCornerRadius(G1_UNIFORM_CORNER_RADIUS);
                break;
            case RECT_KIND_CAPSULE:
                childNode->SetCornerRadius(CAPSULE_CORNER_RADIUS);
                break;
            default:
                childNode->SetCornerRadius(NON_UNIFORM_CORNER_RADIUS);
                break;
        }
        unionNode->AddChild(childNode);
        RegisterNode(childNode);
    }

    void AddTriangleLeaf(const std::shared_ptr<RSCanvasNode>& unionNode, float x, float y, int size)
    {
        auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        childNode->SetBounds({ x, y, static_cast<float>(size), static_cast<float>(size) });
        childNode->SetUseUnion(true);
        auto shape = CreateShape(RSNGEffectType::SDF_TRIANGLE_SHAPE);
        auto triangleShape = std::static_pointer_cast<RSNGSDFTriangleShape>(shape);
        triangleShape->Setter<SDFTriangleShapeVertex0Tag>(Vector2f(size * HALF_RATIO, TRIANGLE_INSET));
        triangleShape->Setter<SDFTriangleShapeVertex1Tag>(Vector2f(TRIANGLE_INSET, size - TRIANGLE_INSET));
        triangleShape->Setter<SDFTriangleShapeVertex2Tag>(Vector2f(size - TRIANGLE_INSET, size - TRIANGLE_INSET));
        triangleShape->Setter<SDFTriangleShapeRadiusTag>(TRIANGLE_CORNER_RADIUS);
        childNode->SetSDFShape(shape);
        unionNode->AddChild(childNode);
        RegisterNode(childNode);
    }

    void AddEllipseLeaf(const std::shared_ptr<RSCanvasNode>& unionNode, float x, float y, int size)
    {
        auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        childNode->SetBounds({ x, y, static_cast<float>(size), static_cast<float>(size) });
        childNode->SetUseUnion(true);
        auto shape = CreateShape(RSNGEffectType::SDF_ELLIPSE_SHAPE);
        auto ellipseShape = std::static_pointer_cast<RSNGSDFEllipseShape>(shape);
        ellipseShape->Setter<SDFEllipseShapeCenterTag>(Vector2f(size * HALF_RATIO, size * HALF_RATIO));
        ellipseShape->Setter<SDFEllipseShapeWidthTag>(static_cast<float>(size));
        ellipseShape->Setter<SDFEllipseShapeHeightTag>(size * ELLIPSE_HEIGHT_RATIO);
        childNode->SetSDFShape(shape);
        unionNode->AddChild(childNode);
        RegisterNode(childNode);
    }

    void AddBasicLeaf(
        const std::shared_ptr<RSCanvasNode>& unionNode, float x, float y, int size, int index, BasicLeafType type)
    {
        switch (type) {
            case BasicLeafType::TRIANGLE:
                AddTriangleLeaf(unionNode, x, y, size);
                break;
            case BasicLeafType::ELLIPSE:
                AddEllipseLeaf(unionNode, x, y, size);
                break;
            default:
                AddRRectLeaf(unionNode, x, y, size, index);
                break;
        }
    }

    void AddPixelmapLeaf(
        const std::shared_ptr<RSCanvasNode>& unionNode, float x, float y, int boxSize, const std::string& path)
    {
        auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        childNode->SetBounds({ x, y, static_cast<float>(boxSize), static_cast<float>(boxSize) });
        childNode->SetUseUnion(true);
        std::shared_ptr<Media::PixelMap> pixelmap = DecodePixelMap(path, Media::AllocatorType::SHARE_MEM_ALLOC);
        auto shape = CreateShape(RSNGEffectType::SDF_PIXELMAP_SHAPE);
        auto pixelmapShape = std::static_pointer_cast<RSNGSDFPixelmapShape>(shape);
        pixelmapShape->Setter<SDFPixelmapShapeImageTag>(pixelmap);
        if (pixelmap != nullptr && pixelmap->GetWidth() > 0 && pixelmap->GetHeight() > 0) {
            float imageWidth = static_cast<float>(pixelmap->GetWidth());
            float imageHeight = static_cast<float>(pixelmap->GetHeight());
            float scale = static_cast<float>(boxSize) / std::max(imageWidth, imageHeight);
            auto transformShape = CreateShape(RSNGEffectType::SDF_TRANSFORM_SHAPE);
            auto transform = std::static_pointer_cast<RSNGSDFTransformShape>(transformShape);
            transform->Setter<SDFTransformShapeMatrixTag>(
                Matrix3f { scale, 0.0f, 0.0f, 0.0f, scale, 0.0f, 0.0f, 0.0f, 1.0f });
            transform->Setter<SDFTransformShapeShapeTag>(shape);
            childNode->SetSDFShape(transformShape);
        } else {
            childNode->SetSDFShape(shape);
        }
        unionNode->AddChild(childNode);
        RegisterNode(childNode);
    }

private:
    void RunBasicShapeLeafCountTest(int leafCount, float spacing)
    {
        auto backgroundNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);

        auto unionNode = CreateEffectNode(backgroundNode, SCENE_WIDTH, SCENE_HEIGHT, spacing, 0);
        int rowCount = (leafCount + BASIC_COLUMN_COUNT - 1) / BASIC_COLUMN_COUNT;
        int gridWidth = (BASIC_COLUMN_COUNT - 1) * BASIC_GRID_STEP + BASIC_SHAPE_SIZE;
        int gridHeight = (rowCount - 1) * BASIC_GRID_STEP + BASIC_SHAPE_SIZE;
        int startX = (SCENE_WIDTH - gridWidth) / 2;
        int startY = (SCENE_HEIGHT - gridHeight) / 2;

        for (int i = 0; i < leafCount; i++) {
            float x = static_cast<float>(startX + (i % BASIC_COLUMN_COUNT) * BASIC_GRID_STEP);
            float y = static_cast<float>(startY + (i / BASIC_COLUMN_COUNT) * BASIC_GRID_STEP);
            BasicLeafType type = static_cast<BasicLeafType>(i % BASIC_LEAF_TYPE_COUNT);
            AddBasicLeaf(unionNode, x, y, BASIC_SHAPE_SIZE, i, type);
        }
    }

    void RunPixelmapLeafCountTest(int leafCount, float spacing, int columnCount, int boxSize, int gap)
    {
        auto backgroundNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);

        auto unionNode = CreateEffectNode(backgroundNode, SCENE_WIDTH, SCENE_HEIGHT, spacing, 0);
        int rowCount = columnCount == 0 ? 0 : (leafCount + columnCount - 1) / columnCount;
        int gridWidth = (columnCount - 1) * (boxSize + gap) + boxSize;
        int gridHeight = (rowCount - 1) * (boxSize + gap) + boxSize;
        int startX = (SCENE_WIDTH - gridWidth) / 2;
        int startY = (SCENE_HEIGHT - gridHeight) / 2;

        for (int i = 0; i < leafCount; i++) {
            if (columnCount == 0) {
                break;
            }
            float x = static_cast<float>(startX + (i % columnCount) * (boxSize + gap));
            float y = static_cast<float>(startY + (i / columnCount) * (boxSize + gap));
            AddPixelmapLeaf(unionNode, x, y, boxSize, UBER_PIXELMAP_PATHS[i % PIXELMAP_LEAF_MAX_COUNT]);
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
};

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_BaseShape_7_Min_Test)
{
    RunBasicShapeLeafCountTest(BASIC_LEAF_MIN_COUNT, 0.0f);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_BaseShape_7_Min_SmoothUnion_Test)
{
    RunBasicShapeLeafCountTest(BASIC_LEAF_MIN_COUNT, SMOOTH_UNION_SPACING);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_BaseShape_30_AllTypes_Test)
{
    RunBasicShapeLeafCountTest(BASIC_LEAF_MAX_COUNT, 0.0f);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_BaseShape_30_AllTypes_SmoothUnion_Test)
{
    RunBasicShapeLeafCountTest(BASIC_LEAF_MAX_COUNT, SMOOTH_UNION_SPACING);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_BaseShape_31_OverSpec_Test)
{
    RunBasicShapeLeafCountTest(BASIC_OVER_SPEC_COUNT, 0.0f);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_BaseShape_31_OverSpec_SmoothUnion_Test)
{
    RunBasicShapeLeafCountTest(BASIC_OVER_SPEC_COUNT, SMOOTH_UNION_SPACING);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_Pixelmap_2_Min_Test)
{
    RunPixelmapLeafCountTest(PIXELMAP_LEAF_MIN_COUNT, 0.0f, PIXELMAP_LEAF_MIN_COUNT, PIXELMAP_BOX_SIZE, PIXELMAP_GAP);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_Pixelmap_2_Min_SmoothUnion_Test)
{
    RunPixelmapLeafCountTest(
        PIXELMAP_LEAF_MIN_COUNT, SMOOTH_UNION_SPACING, PIXELMAP_LEAF_MIN_COUNT, PIXELMAP_BOX_SIZE, PIXELMAP_GAP);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_Pixelmap_6_Test)
{
    RunPixelmapLeafCountTest(PIXELMAP_LEAF_MAX_COUNT, 0.0f, PIXELMAP_COLUMN_COUNT, PIXELMAP_BOX_SIZE, PIXELMAP_GAP);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_Pixelmap_6_SmoothUnion_Test)
{
    RunPixelmapLeafCountTest(
        PIXELMAP_LEAF_MAX_COUNT, SMOOTH_UNION_SPACING, PIXELMAP_COLUMN_COUNT, PIXELMAP_BOX_SIZE, PIXELMAP_GAP);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_Pixelmap_7_OverSpec_Test)
{
    RunPixelmapLeafCountTest(
        PIXELMAP_OVER_SPEC_COUNT, 0.0f, PIXELMAP_OVER_COLUMN_COUNT, PIXELMAP_OVER_BOX_SIZE, PIXELMAP_OVER_GAP);
}

GRAPHIC_TEST(NGSDFUberTest, EFFECT_TEST, Set_SDF_Uber_Pixelmap_7_OverSpec_SmoothUnion_Test)
{
    RunPixelmapLeafCountTest(PIXELMAP_OVER_SPEC_COUNT, SMOOTH_UNION_SPACING, PIXELMAP_OVER_COLUMN_COUNT,
        PIXELMAP_OVER_BOX_SIZE, PIXELMAP_OVER_GAP);
}
} // namespace OHOS::Rosen
