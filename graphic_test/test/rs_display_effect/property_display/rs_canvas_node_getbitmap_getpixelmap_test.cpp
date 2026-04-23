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

#include "pixel_map.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSCanvasNodeGetBitmapGetPixelmapTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/*
 * @tc.name: RSCanvasNodeGetBitmapTest_001
 * @tc.desc: Test GetBitmap with default parameters
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeGetBitmapTest_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xff0000ff);

    Drawing::Bitmap bitmap;
    testNode->GetBitmap(bitmap);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSCanvasNodeGetBitmapTest_002
 * @tc.desc: Test GetBitmap with different bounds sizes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeGetBitmapTest_002)
{
    std::vector<Vector4f> boundsList = {
        { 50, 50, 100, 100 },
        { 200, 50, 200, 150 },
        { 500, 50, 300, 200 },
        { 50, 300, 150, 300 },
        { 250, 300, 250, 300 },
        { 550, 300, 350, 300 },
    };

    for (size_t i = 0; i < boundsList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds(
            { boundsList[i].x_, boundsList[i].y_, boundsList[i].z_, boundsList[i].w_ });
        testNode->SetBackgroundColor(0xff00ff00);

        Drawing::Bitmap bitmap;
        testNode->GetBitmap(bitmap);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeGetBitmapTest_003
 * @tc.desc: Test GetBitmap with alpha
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeGetBitmapTest_003)
{
    std::vector<float> alphaList = { 0.0f, 0.3f, 0.5f, 0.7f, 1.0f };

    for (size_t i = 0; i < alphaList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetAlpha(alphaList[i]);

        Drawing::Bitmap bitmap;
        testNode->GetBitmap(bitmap);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeGetPixelmapTest_001
 * @tc.desc: Test GetPixelmap with null pixelmap and null rect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeGetPixelmapTest_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    testNode->GetPixelmap(nullptr);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSCanvasNodeGetPixelmapTest_002
 * @tc.desc: Test GetPixelmap with valid pixelmap and rect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeGetPixelmapTest_002)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 500, 500 });
    testNode->SetBackgroundColor(0xff0000ff);

    Media::InitializationOptions opts;
    opts.size.width = 500;
    opts.size.height = 500;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.pixelFormat = Media::PixelFormat::ARGB_8888;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    Drawing::Rect rect(0, 0, 500, 500);
    testNode->GetPixelmap(pixelmap, nullptr, &rect);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSCanvasNodeGetPixelmapTest_003
 * @tc.desc: Test GetPixelmap with different bounds sizes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeGetPixelmapTest_003)
{
    std::vector<Vector4f> boundsList = {
        { 50, 50, 100, 100 },
        { 200, 50, 200, 150 },
        { 500, 50, 300, 200 },
        { 50, 300, 150, 300 },
        { 250, 300, 250, 300 },
        { 550, 300, 350, 300 },
    };

    for (size_t i = 0; i < boundsList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds(
            { boundsList[i].x_, boundsList[i].y_, boundsList[i].z_, boundsList[i].w_ });
        testNode->SetBackgroundColor(0xff00ff00);

        Media::InitializationOptions opts;
        opts.size.width = boundsList[i].z_;
        opts.size.height = boundsList[i].w_;
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
        opts.pixelFormat = Media::PixelFormat::ARGB_8888;
        std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
        Drawing::Rect rect(0, 0, boundsList[i].z_, boundsList[i].w_);
        testNode->GetPixelmap(pixelmap, nullptr, &rect);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeGetPixelmapTest_004
 * @tc.desc: Test GetPixelmap with alpha
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeGetPixelmapTest_004)
{
    std::vector<float> alphaList = { 0.0f, 0.3f, 0.5f, 0.7f, 1.0f };

    for (size_t i = 0; i < alphaList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetAlpha(alphaList[i]);

        Media::InitializationOptions opts;
        opts.size.width = 200;
        opts.size.height = 200;
        opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
        opts.pixelFormat = Media::PixelFormat::ARGB_8888;
        std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
        Drawing::Rect rect(0, 0, 200, 200);
        testNode->GetPixelmap(pixelmap, nullptr, &rect);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeGetPixelmapTest_005
 * @tc.desc: Test GetPixelmap with null pixelmap and valid rect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeGetPixelmapTest_005)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xff0000ff);

    Drawing::Rect rect(0, 0, 400, 400);
    testNode->GetPixelmap(nullptr, nullptr, &rect);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSCanvasNodeResetSurfaceTest_001
 * @tc.desc: Test ResetSurface with different width and height
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeResetSurfaceTest_001)
{
    int width = 400;
    int height = 400;
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, width, height });
    testNode->SetBackgroundColor(0xff0000ff);
    testNode->SetHybridRenderCanvas(true);
    testNode->ResetSurface(width, height);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSCanvasNodeResetSurfaceTest_002
 * @tc.desc: Test ResetSurface with multiple sizes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeResetSurfaceTest_002)
{
    std::vector<Vector4f> sizeList = {
        { 50, 50, 100, 100 },
        { 250, 50, 200, 150 },
        { 550, 50, 300, 200 },
        { 50, 350, 150, 250 },
        { 300, 350, 250, 300 },
    };

    for (size_t i = 0; i < sizeList.size(); i++) {
        int width = sizeList[i].z_;
        int height = sizeList[i].w_;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds(
            { sizeList[i].x_, sizeList[i].y_, sizeList[i].z_, sizeList[i].w_ });
        testNode->SetBackgroundColor(0xff00ff00);
        testNode->SetHybridRenderCanvas(true);
        testNode->ResetSurface(width, height);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeResetSurfaceTest_003
 * @tc.desc: Test ResetSurface without enabling hybrid render
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeGetBitmapGetPixelmapTest, CONTENT_DISPLAY_TEST,
    RSCanvasNodeResetSurfaceTest_003)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->ResetSurface(400, 400);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

} // namespace OHOS::Rosen
