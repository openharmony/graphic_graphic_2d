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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ContentEffectNodeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void setNode(std::shared_ptr<RSCanvasNode>& node, float alpha)
    {
        int borderstyle = 0;
        Vector4f borderWidth = { 5, 5, 5, 5 };
        node->SetBorderStyle(borderstyle);
        node->SetBorderWidth(borderWidth);
        node->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        node->SetAlpha(alpha);
    };
};

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test1)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 100, 100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(0); // Set border style to initial
        testFaNode->SetBorderWidth({ 5, 5, 5, 5 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test2)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 50, 100, 150,100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(0); // Set border style to initial
        testFaNode->SetBorderWidth({ 5, 5, 5, 5 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test3)
{
    int columnCount = 1;
    int rowCount = 1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 100, 100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(0); // Set border style to initial
        testFaNode->SetBorderWidth({ 5, 5, 5, 5 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test4)
{
    int columnCount = 100;
    int rowCount = 100;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 100, 100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(0); // Set border style to initial
        testFaNode->SetBorderWidth({ 5, 5, 5, 5 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test5)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 100, 100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(0); // Set border style to initial
        testFaNode->SetBorderWidth({ 5, 5, 5, 5 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::White()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test6)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 100, 100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(0); // Set border style to initial
        testFaNode->SetBorderWidth({ 5, 5, 5, 5 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::Black()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test7)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 100, 100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(0); // Set border style to initial
        testFaNode->SetBorderWidth({ 0, 0, 0, 0 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test8)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 100, 100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(0); // Set border style to initial
        testFaNode->SetBorderWidth({ 100, 100, 100, 100 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test9)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 100, 100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(0); // Set border style to initial
        testFaNode->SetBorderWidth({ -1, -1, -1, -1 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test10)
{
    int columnCount = 0;
    int rowCount = 0;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 100, 100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(0); // Set border style to initial
        testFaNode->SetBorderWidth({ 5, 5, 5, 5 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

GRAPHIC_TEST(ContentEffectNodeTest, CONTENT_DISPLAY_TEST, EffectNode_Use_Effect_Test11)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> blurRadiusList = { 0, 100, 100 };
    std::vector<bool> useEffectList = { false, true };
    std::vector<UseEffectType> useEffectTypeList = {
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::EFFECT_COMPONENT,
        UseEffectType::BEHIND_WINDOW,
    };

    for (int i = 0; i < columnCount * rowCount; i++) {
        float x = (i % columnCount) * sizeX;
        float y = (i / columnCount) * sizeY;
        auto testFaNode = RSEffectNode::Create();
        std::shared_ptr<Media::PixelMap> pixelmap =
            DecodePixelMap("/data/local/tmp/Images/backGroundImage.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
        auto image = std::make_shared<Rosen::RSImage>();
        image->SetPixelMap(pixelmap);
        image->SetImageFit((int)ImageFit::FILL);
        testFaNode->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testFaNode->SetBgImageWidth(sizeX - 10);  // image width
        testFaNode->SetBgImageHeight(sizeY - 10); // image height
        testFaNode->SetBgImage(image);
        testFaNode->SetBorderStyle(10); // Set border style to initial
        testFaNode->SetBorderWidth({ 5, 5, 5, 5 }); // Set border width to 5
        testFaNode->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        // BlurMerge need FatherNode have a blur
        testFaNode->SetBackgroundBlurRadius(blurRadiusList[i / columnCount]);
        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode1, 1);
        childNode1->SetUseEffect(useEffectList[i % columnCount]);
        childNode1->SetUseEffectType(useEffectTypeList[i / columnCount]);

        auto childNode2 =
            SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        setNode(childNode2, 1);
        childNode2->SetUseEffect(useEffectList[i % columnCount]);
        childNode2->SetUseEffectType(useEffectTypeList[i / columnCount]);

        childNode1->AddChild(childNode2);
        testFaNode->AddChild(childNode1);
        GetRootNode()->AddChild(testFaNode);

        RegisterNode(testFaNode);
        RegisterNode(childNode1);
        RegisterNode(childNode2);
    }
}

} // namespace OHOS::Rosen