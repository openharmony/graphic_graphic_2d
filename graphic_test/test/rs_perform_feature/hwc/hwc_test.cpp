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
#include "ui/rs_surface_node.h"
#include "ui/rs_canvas_node.h"
#include "transaction/rs_interfaces.h"

#ifdef ROSEN_OHOS
#include <iconsumer_surface.h>
#include <surface.h>
#include <surface_buffer.h>
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000;
constexpr int RGBA_ALPHA_SHIFT = 24;
constexpr int RGBA_RED_SHIFT = 0;
constexpr int RGBA_GREEN_SHIFT = 8;
constexpr int RGBA_BLUE_SHIFT = 16;
}

class HwcTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void AfterEach() override {}

#ifdef ROSEN_OHOS
    std::vector<uint32_t> CreateGradientBuffer(int32_t width, int32_t height)
    {
        std::vector<uint32_t> buffer(width * height);
        for (int32_t y = 0; y < height; y++) {
            for (int32_t x = 0; x < width; x++) {
                uint8_t r = static_cast<uint8_t>(x * 255 / width);
                uint8_t g = static_cast<uint8_t>(y * 255 / height);
                uint8_t b = 128;
                uint8_t a = 255;
                buffer[y * width + x] = (a << RGBA_ALPHA_SHIFT) | (r << RGBA_RED_SHIFT) |
                                        (g << RGBA_GREEN_SHIFT) | (b << RGBA_BLUE_SHIFT);
            }
        }
        return buffer;
    }

    RSSurfaceNode::SharedPtr CreateSurfaceNodeWithBuffer(
        const std::string& name, int x, int y, int width, int height, uint32_t color)
    {
        RSSurfaceNodeConfig config;
        config.SurfaceNodeName = name;
        auto surfaceNode = RSSurfaceNode::Create(config, RSSurfaceNodeType::SELF_DRAWING_NODE);
        if (surfaceNode == nullptr) {
            return nullptr;
        }

        surfaceNode->SetBounds({x, y, width, height});
        surfaceNode->SetFrame({0, 0, width, height});
        surfaceNode->SetBackgroundColor(SK_ColorBLACK);
        surfaceNode->SetHardwareEnabled(true);
        surfaceNode->SetBootAnimation(true);
        surfaceNode->SetForceHardwareAndFixRotation(true);

        sptr<OHOS::Surface> surface = surfaceNode->GetSurface();
        if (surface == nullptr) {
            return nullptr;
        }
        surface->SetQueueSize(1);

        sptr<SurfaceBuffer> buffer = nullptr;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        BufferRequestConfig requestConfig = {
            .width = width,
            .height = height,
            .strideAlignment = 0x8,
            .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
            .usage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE |
                     BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA |
                     BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE,
        };

        GSError ret = surface->RequestBuffer(buffer, requestFence, requestConfig);
        if (ret != GSERROR_OK || buffer == nullptr) {
            return nullptr;
        }
        auto bufferAddr = static_cast<uint32_t*>(buffer->GetVirAddr());
        if (bufferAddr != nullptr) {
            int32_t stride = buffer->GetStride();
            int32_t strideInPixels = stride / sizeof(uint32_t);
            for (int32_t y = 0; y < height; y++) {
                uint32_t* rowStart = bufferAddr + y * strideInPixels;
                for (int32_t x = 0; x < strideInPixels; x++) {
                    rowStart[x] = color;
                }
            }
        }

        sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
        BufferFlushConfig flushConfig = {
            .damage = {0, 0, width, height},
        };
        if (surface->FlushBuffer(buffer, flushFence, flushConfig) != GSERROR_OK) {
            return nullptr;
        }

        return surfaceNode;
    }

    RSSurfaceNode::SharedPtr CreateGradientSurfaceNode(
        const std::string& name, int x, int y, int width, int height)
    {
        RSSurfaceNodeConfig config;
        config.SurfaceNodeName = name;
        auto surfaceNode = RSSurfaceNode::Create(config, RSSurfaceNodeType::SELF_DRAWING_NODE);
        if (surfaceNode == nullptr) {
            return nullptr;
        }

        surfaceNode->SetBounds({x, y, width, height});
        surfaceNode->SetFrame({0, 0, width, height});
        surfaceNode->SetHardwareEnabled(true);
        surfaceNode->SetBootAnimation(true);
        surfaceNode->SetForceHardwareAndFixRotation(true);
        surfaceNode->SetBackgroundColor(SK_ColorBLACK);

        sptr<OHOS::Surface> surface = surfaceNode->GetSurface();
        if (surface == nullptr) {
            return nullptr;
        }
        surface->SetQueueSize(1);

        sptr<SurfaceBuffer> buffer = nullptr;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        BufferRequestConfig requestConfig = {
            .width = width,
            .height = height,
            .strideAlignment = 0x8,
            .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
            .usage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE |
                     BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA |
                     BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE,
        };

        GSError ret = surface->RequestBuffer(buffer, requestFence, requestConfig);
        if (ret != GSERROR_OK || buffer == nullptr) {
            return nullptr;
        }

        auto bufferAddr = static_cast<uint32_t*>(buffer->GetVirAddr());
        if (bufferAddr != nullptr) {
            [[maybe_unused]] int32_t bufferWidth = buffer->GetWidth();
            [[maybe_unused]] int32_t bufferHeight = buffer->GetHeight();
            [[maybe_unused]] int32_t stride = buffer->GetStride();
            int32_t strideInPixels = stride / sizeof(uint32_t);
            std::vector<uint32_t> gradientRow(bufferWidth);
            for (int32_t y = 0; y < bufferHeight; y++) {
                uint32_t* rowStart = bufferAddr + y * strideInPixels;
                for (int32_t x = 0; x < bufferWidth; x++) {
                    uint8_t r = static_cast<uint8_t>(x * 255 / bufferWidth);
                    uint8_t g = static_cast<uint8_t>(y * 255 / bufferHeight);
                    uint8_t b = 128;
                    uint8_t a = 255;
                    gradientRow[x] = (a << RGBA_ALPHA_SHIFT) | (r << RGBA_RED_SHIFT) |
                                    (g << RGBA_GREEN_SHIFT) | (b << RGBA_BLUE_SHIFT);
                }
                memcpy(rowStart, gradientRow.data(), bufferWidth * sizeof(uint32_t));
                uint32_t lastPixelColor = gradientRow[bufferWidth - 1];
                for (int32_t x = bufferWidth; x < strideInPixels; x++) {
                    rowStart[x] = lastPixelColor;
                }
            }
        }

        sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
        BufferFlushConfig flushConfig = {
            .damage = {0, 0, width, height},
        };
        if (surface->FlushBuffer(buffer, flushFence, flushConfig) != GSERROR_OK) {
            return nullptr;
        }

        return surfaceNode;
    }
#endif
};

/* Single red SurfaceNode */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Single_Surface_Red_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Red_Surface",
        (screenWidth - 720) / 2, (screenHeight - 1280) / 2, 720, 1280, 0xFF0000FF);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Single green SurfaceNode */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Single_Surface_Green_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Green_Surface", 100, 100, 400, 300, 0xFF00FF00);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Single blue SurfaceNode */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Single_Surface_Blue_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Blue_Surface", 100, 100, 400, 300, 0xFFFF0000);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Gradient SurfaceNode */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Gradient_Surface_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateGradientSurfaceNode("Hwc_Gradient_Surface", 100, 100, 500, 400);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Multiple SurfaceNodes in grid layout */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Multi_Surface_Grid_Test_1)
{
#ifdef ROSEN_OHOS
    constexpr int startX = 50;
    constexpr int startY = 50;
    constexpr int surfaceWidth = 200;
    constexpr int surfaceHeight = 150;
    constexpr int gap = 20;
    constexpr int rows = 2;
    constexpr int cols = 3;
    uint32_t colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFF00, 0xFFFF00FF, 0xFF00FFFF};
    std::vector<RSSurfaceNode::SharedPtr> nodes;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int x = startX + col * (surfaceWidth + gap);
            int y = startY + row * (surfaceHeight + gap);
            auto surfaceNode = CreateSurfaceNodeWithBuffer(
                "Surface_" + std::to_string(row * cols + col), x, y, surfaceWidth,
                surfaceHeight, colors[row * cols + col]);
            GetRootNode()->SetTestSurface(surfaceNode);
            nodes.push_back(surfaceNode);
        }
    }
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* SurfaceNode with CanvasNode siblings */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Surface_With_Canvas_Child_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Surface_With_Child", 100, 100, 500, 400, 0xFF808080);
    GetRootNode()->SetTestSurface(surfaceNode);

    auto child1 = RSCanvasNode::Create();
    child1->SetBounds({120, 120, 150, 150});
    child1->SetBackgroundColor(SK_ColorRED);
    GetRootNode()->AddChild(child1, -1);

    auto child2 = RSCanvasNode::Create();
    child2->SetBounds({300, 120, 150, 150});
    child2->SetBackgroundColor(SK_ColorGREEN);
    GetRootNode()->AddChild(child2, -1);

    auto child3 = RSCanvasNode::Create();
    child3->SetBounds({210, 300, 150, 150});
    child3->SetBackgroundColor(SK_ColorBLUE);
    GetRootNode()->AddChild(child3, -1);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
    RegisterNode(child1);
    RegisterNode(child2);
    RegisterNode(child3);
#endif
}

/* Layer stack: multiple non-overlapping SurfaceNodes */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Layer_Stack_Test_1)
{
#ifdef ROSEN_OHOS
    uint32_t colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFF00};
    constexpr int nodeWidth = 200;
    constexpr int nodeHeight = 150;
    constexpr int gap = 20;
    std::vector<RSSurfaceNode::SharedPtr> nodes;

    for (int i = 0; i < 4; i++) {
        auto surfaceNode = CreateSurfaceNodeWithBuffer(
            "Hwc_Layer_" + std::to_string(i), 50 + i * (nodeWidth + gap), 100, nodeWidth, nodeHeight, colors[i]);
        GetRootNode()->SetTestSurface(surfaceNode);
        nodes.push_back(surfaceNode);
    }
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Large SurfaceNode */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Large_Surface_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Large_Surface", 50, 50, 800, 600, 0xFF800080);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with translation */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Translation_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Translation_Surface", 100, 100, 400, 300, 0xFFA5FFFF);
    surfaceNode->SetTranslate({200, 200});
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with scale: 1.5f */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Scale_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Scale_Surface", 100, 100, 400, 300, 0xFFFFFF00);
    surfaceNode->SetScale(1.5f, 1.5f);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with scale: 0.5f */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Scale_Test_2)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Scale_Surface", 200, 200, 400, 300, 0xFFFFFF00);
    surfaceNode->SetScale(0.5f, 0.5f);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with scale: 2.0f */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Scale_Test_3)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Scale_Surface", 300, 300, 300, 200, 0xFFFFFF00);
    surfaceNode->SetScale(2.0f, 2.0f);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with rotation: 45 degrees (non-90 multiple) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Rotation_Surface", 200, 200, 400, 300, 0xFFFF00FF);
    surfaceNode->SetRotation(45.0f);  // Non-90 degree rotation
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with rotation: 90 degrees */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_Test_2)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Rotation_Surface", 250, 250, 400, 300, 0xFF40E0D0);
    surfaceNode->SetRotation(90.0f);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with rotation: 180 degrees */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_Test_3)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Rotation_Surface", 250, 250, 400, 300, 0xFFFA8072);
    surfaceNode->SetRotation(180.0f);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with alpha: 0.5f */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Alpha_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Alpha_Surface", 150, 150, 400, 300, 0xFF0000FF);
    surfaceNode->SetAlpha(0.5f);  // Alpha not equal to 1.0
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with alpha: 0.1f */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Alpha_Test_2)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Alpha_Surface", 200, 200, 400, 300, 0xFF4B0082);
    surfaceNode->SetAlpha(0.1f);  // Alpha not equal to 1.0
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with alpha: 0.9f */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Alpha_Test_3)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Alpha_Surface", 250, 250, 400, 300, 0xFF4B0082);
    surfaceNode->SetAlpha(0.9f);  // Alpha not equal to 1.0
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with scale and rotation combined */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Scale_Rotation_Combined_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Scale_Rotation_Surface", 200, 200, 350, 250, 0xFF32CD32);
    surfaceNode->SetScale(0.8f, 0.8f);
    surfaceNode->SetRotation(30.0f);  // Non-90 degree rotation
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Overlapping SurfaceNodes with alpha */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Overlapping_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode1 = CreateSurfaceNodeWithBuffer("Hwc_Overlap_Surface_1", 100, 100, 400, 300, 0xFF0000FF);
    surfaceNode1->SetAlpha(0.7f);  // Alpha not equal to 1.0
    GetRootNode()->SetTestSurface(surfaceNode1);

    auto surfaceNode2 = CreateSurfaceNodeWithBuffer("Hwc_Overlap_Surface_2", 200, 200, 400, 300, 0xFF00FF00);
    GetRootNode()->SetTestSurface(surfaceNode2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode1);
    RegisterNode(surfaceNode2);
#endif
}

/* Small SurfaceNode: 50x50 */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Small_Surface_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Small_Surface", 50, 50, 50, 50, 0xFF008080);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Wide SurfaceNode: 1000x100 */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Wide_Surface_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Wide_Surface", 100, 100, 1000, 100, 0xFF800000);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Tall SurfaceNode: 100x1500 */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Tall_Surface_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Tall_Surface", 100, 100, 100, 1500, 0xFF000080);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Translucent SurfaceNode with background CanvasNode */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Translucent_With_Bg_Test_1)
{
#ifdef ROSEN_OHOS
    auto bgNode = RSCanvasNode::Create();
    bgNode->SetBounds({100, 100, 500, 400});
    bgNode->SetBackgroundColor(SK_ColorBLUE);
    GetRootNode()->AddChild(bgNode, -1);

    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Translucent_Surface", 150, 150, 400, 300, 0xFF008080);
    surfaceNode->SetAlpha(0.3f);  // Alpha not equal to 1.0
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(bgNode);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with pivot and rotation */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Pivot_Rotation_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Pivot_Surface", 200, 200, 400, 300, 0xFFC0C0C0);
    surfaceNode->SetPivot(0.5f, 0.5f);
    surfaceNode->SetRotation(60.0f);  // Non-90 degree rotation
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with offset pivot and rotation */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Pivot_Rotation_Test_2)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Pivot_Surface", 200, 200, 400, 300, 0xFFF0E68C);
    surfaceNode->SetPivot(0.2f, 0.8f);
    surfaceNode->SetRotation(45.0f);  // Non-90 degree rotation
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with translation and scale combined */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Translation_Scale_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Trans_Scale_Surface", 100, 100, 400, 300, 0xFFFF7F50);
    surfaceNode->SetTranslate({150, 150});
    surfaceNode->SetScale(1.2f, 1.2f);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Three stacked SurfaceNodes with overlap */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Three_Layer_Test_1)
{
#ifdef ROSEN_OHOS
    uint32_t colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000};
    constexpr int nodeWidth = 300;
    constexpr int nodeHeight = 200;
    std::vector<RSSurfaceNode::SharedPtr> nodes;

    for (int i = 0; i < 3; i++) {
        auto surfaceNode = CreateSurfaceNodeWithBuffer(
            "Hwc_Three_Layer_" + std::to_string(i), 100 + i * 50, 100 + i * 50, nodeWidth, nodeHeight, colors[i]);
        surfaceNode->SetAlpha(0.8f);  // Alpha not equal to 1.0 with overlap
        GetRootNode()->SetTestSurface(surfaceNode);
        nodes.push_back(surfaceNode);
    }
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* SurfaceNodes at corner positions */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Corner_Position_Test_1)
{
#ifdef ROSEN_OHOS
    uint32_t colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFF00};
    constexpr int nodeWidth = 200;
    constexpr int nodeHeight = 150;
    constexpr int margin = 20;

    auto surfaceNode1 = CreateSurfaceNodeWithBuffer("Hwc_Corner_0", margin, margin, nodeWidth, nodeHeight, colors[0]);
    GetRootNode()->SetTestSurface(surfaceNode1);

    auto surfaceNode2 = CreateSurfaceNodeWithBuffer("Hwc_Corner_1",
        screenWidth - nodeWidth - margin, margin, nodeWidth, nodeHeight, colors[1]);
    GetRootNode()->SetTestSurface(surfaceNode2);

    auto surfaceNode3 = CreateSurfaceNodeWithBuffer("Hwc_Corner_2",
        margin, screenHeight - nodeHeight - margin, nodeWidth, nodeHeight, colors[2]);
    GetRootNode()->SetTestSurface(surfaceNode3);

    auto surfaceNode4 = CreateSurfaceNodeWithBuffer("Hwc_Corner_3",
        screenWidth - nodeWidth - margin, screenHeight - nodeHeight - margin, nodeWidth, nodeHeight, colors[3]);
    GetRootNode()->SetTestSurface(surfaceNode4);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode1);
    RegisterNode(surfaceNode2);
    RegisterNode(surfaceNode3);
    RegisterNode(surfaceNode4);
#endif
}

/* Gradient SurfaceNode with transform */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Gradient_Transform_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateGradientSurfaceNode("Hwc_Gradient_Transform_Surface", 150, 150, 500, 400);
    surfaceNode->SetRotation(15.0f);  // Non-90 degree rotation
    surfaceNode->SetScale(0.9f, 0.9f);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Full-screen SurfaceNode */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Full_Screen_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Full_Screen_Surface", 0, 0, screenWidth,
                                                   screenHeight, 0xFF2F4F4F);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* SurfaceNode with negative scale (flip) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Negative_Scale_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Negative_Scale_Surface", 200, 200, 400, 300, 0xFFDDA0DD);
    surfaceNode->SetScale(-1.0f, 1.0f);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Multiple scale factors */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Multi_Scale_Test_1)
{
#ifdef ROSEN_OHOS
    struct ScaleTest {
        float scaleX;
        float scaleY;
        uint32_t color;
    };
    ScaleTest scaleTests[] = {
        {0.5f, 0.5f, 0xFFFF6347},
        {1.0f, 1.0f, 0xFF4682B4},
        {1.5f, 1.5f, 0xFF32CD32},
        {2.0f, 1.0f, 0xFFDA70D6},
    };
    std::vector<RSSurfaceNode::SharedPtr> nodes;

    for (int i = 0; i < 4; i++) {
        auto surfaceNode = CreateSurfaceNodeWithBuffer(
            "Hwc_Multi_Scale_" + std::to_string(i), 100 + i * 250, 100, 200, 150, scaleTests[i].color);
        surfaceNode->SetScale(scaleTests[i].scaleX, scaleTests[i].scaleY);
        GetRootNode()->SetTestSurface(surfaceNode);
        nodes.push_back(surfaceNode);
    }
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Diagonal layout */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Diagonal_Test_1)
{
#ifdef ROSEN_OHOS
    uint32_t colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFF00, 0xFFFF00FF};
    std::vector<RSSurfaceNode::SharedPtr> nodes;

    for (int i = 0; i < 5; i++) {
        auto surfaceNode = CreateSurfaceNodeWithBuffer(
            "Hwc_Diagonal_" + std::to_string(i), 50 + i * 200, 100 + i * 200, 180, 130, colors[i]);
        GetRootNode()->SetTestSurface(surfaceNode);
        nodes.push_back(surfaceNode);
    }
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Occlusion: fully covered lower layer */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Occlusion_Full_Cover_Test_1)
{
#ifdef ROSEN_OHOS
    auto lowerNode = CreateSurfaceNodeWithBuffer("Hwc_Occlusion_Lower", 100, 100, 400, 300, 0xFF0000FF);
    GetRootNode()->SetTestSurface(lowerNode);

    auto upperNode = CreateSurfaceNodeWithBuffer("Hwc_Occlusion_Upper", 100, 100, 400, 300, 0xFF00FF00);
    GetRootNode()->SetTestSurface(upperNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(lowerNode);
    RegisterNode(upperNode);
#endif
}

/* Occlusion: partial coverage with transparency */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Occlusion_Partial_Test_1)
{
#ifdef ROSEN_OHOS
    auto lowerNode = CreateSurfaceNodeWithBuffer("Hwc_Occlusion_Lower", 100, 100, 500, 400, 0xFFFF0000);
    GetRootNode()->SetTestSurface(lowerNode);

    auto upperNode = CreateSurfaceNodeWithBuffer("Hwc_Occlusion_Upper", 200, 200, 300, 250, 0xFF00FF00);
    upperNode->SetAlpha(0.7f);  // Alpha not equal to 1.0 with overlap
    GetRootNode()->SetTestSurface(upperNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(lowerNode);
    RegisterNode(upperNode);
#endif
}

/* Occlusion: multiple layers stacking */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Occlusion_Multi_Layer_Test_1)
{
#ifdef ROSEN_OHOS
    std::vector<RSSurfaceNode::SharedPtr> nodes;
    uint32_t colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFF00};

    for (int i = 0; i < 4; i++) {
        auto node = CreateSurfaceNodeWithBuffer(
            "Hwc_Occlusion_Layer_" + std::to_string(i), 100 + i * 30, 100 + i * 30, 400, 300, colors[i]);
        node->SetAlpha(0.8f);  // Alpha not equal to 1.0 with in-app overlap
        GetRootNode()->SetTestSurface(node);
        nodes.push_back(node);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Occlusion: corner overlap */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Occlusion_Corner_Test_1)
{
#ifdef ROSEN_OHOS
    auto node1 = CreateSurfaceNodeWithBuffer("Hwc_Occlusion_Corner_1", 100, 100, 300, 300, 0xFF0000FF);
    GetRootNode()->SetTestSurface(node1);

    auto node2 = CreateSurfaceNodeWithBuffer("Hwc_Occlusion_Corner_2", 250, 250, 300, 300, 0xFF00FF00);
    GetRootNode()->SetTestSurface(node2);

    auto node3 = CreateSurfaceNodeWithBuffer("Hwc_Occlusion_Corner_3", 100, 400, 300, 300, 0xFFFF0000);
    GetRootNode()->SetTestSurface(node3);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(node1);
    RegisterNode(node2);
    RegisterNode(node3);
#endif
}

/* ClipRect: parent node with clip bounds */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Clip_Parent_Test_1)
{
#ifdef ROSEN_OHOS
    auto parentCanvas = RSCanvasNode::Create();
    parentCanvas->SetBounds({100, 100, 500, 400});
    parentCanvas->SetBackgroundColor(SK_ColorGRAY);
    parentCanvas->SetClipToBounds(true);
    GetRootNode()->AddChild(parentCanvas, -1);

    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Clip_Surface", 50, 50, 500, 400, 0xFF00FF00);
    surfaceNode->SetBounds({50, 50, 500, 400});
    parentCanvas->AddChild(surfaceNode, -1);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(parentCanvas);
    RegisterNode(surfaceNode);
#endif
}

/* ClipRect: multiple nested clip bounds */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Clip_Nested_Test_1)
{
#ifdef ROSEN_OHOS
    auto outerClip = RSCanvasNode::Create();
    outerClip->SetBounds({50, 50, 600, 500});
    outerClip->SetClipToBounds(true);
    outerClip->SetBackgroundColor(SK_ColorLTGRAY);
    GetRootNode()->AddChild(outerClip, -1);

    auto innerClip = RSCanvasNode::Create();
    innerClip->SetBounds({100, 100, 400, 300});
    innerClip->SetClipToBounds(true);
    innerClip->SetBackgroundColor(SK_ColorDKGRAY);
    outerClip->AddChild(innerClip, -1);

    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Clip_Nested_Surface", 50, 50, 400, 350, 0xFFFF0000);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(outerClip);
    RegisterNode(innerClip);
    RegisterNode(surfaceNode);
#endif
}

/* LayerCrop: crop region */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Layer_Crop_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Crop_Surface", 100, 100, 500, 400, 0xFF800080);
    surfaceNode->SetFrame({0, 0, 500, 400});
    surfaceNode->SetBounds({100, 100, 400, 300});
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Boundary: screen edge aligned */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Boundary_Screen_Edge_Test_1)
{
#ifdef ROSEN_OHOS
    auto nodeLeft = CreateSurfaceNodeWithBuffer("Hwc_Boundary_Left", 0, 100, 200, 400, 0xFF0000FF);
    GetRootNode()->SetTestSurface(nodeLeft);

    auto nodeRight = CreateSurfaceNodeWithBuffer("Hwc_Boundary_Right", screenWidth - 200, 100, 200, 400, 0xFF00FF00);
    GetRootNode()->SetTestSurface(nodeRight);

    auto nodeTop = CreateSurfaceNodeWithBuffer("Hwc_Boundary_Top", 300, 0, 300, 150, 0xFFFF0000);
    GetRootNode()->SetTestSurface(nodeTop);

    auto nodeBottom = CreateSurfaceNodeWithBuffer("Boundary_Bottom", 300,
                                                   screenHeight - 150, 300, 150, 0xFFFFFF00);
    GetRootNode()->SetTestSurface(nodeBottom);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(nodeLeft);
    RegisterNode(nodeRight);
    RegisterNode(nodeTop);
    RegisterNode(nodeBottom);
#endif
}

/* Boundary: overflow screen */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Boundary_Overflow_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Overflow_Surface",
        screenWidth - 300, screenHeight - 300, 500, 400, 0xFF008080);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* CornerRadius: rounded corners on SurfaceNode */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Corner_Radius_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Corner_Radius_Surface", 200, 200, 400, 400, 0xFF4169E1);
    surfaceNode->SetCornerRadius(40.0f);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* CornerRadius: varying radius values */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Corner_Radius_Multi_Test_1)
{
#ifdef ROSEN_OHOS
    struct CornerRadiusTest {
        float radius;
        uint32_t color;
        int x;
    };
    CornerRadiusTest tests[] = {
        {10.0f, 0xFFFF6347, 100},
        {30.0f, 0xFF4682B4, 350},
        {50.0f, 0xFF32CD32, 600},
        {80.0f, 0xFFDA70D6, 850},
    };
    std::vector<RSSurfaceNode::SharedPtr> nodes;

    for (int i = 0; i < 4; i++) {
        auto node = CreateSurfaceNodeWithBuffer(
            "Hwc_Corner_Radius_" + std::to_string(i), tests[i].x, 100, 200, 200, tests[i].color);
        node->SetCornerRadius(tests[i].radius);
        GetRootNode()->SetTestSurface(node);
        nodes.push_back(node);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* ClipToBounds: with rounded corners */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Clip_Corner_Radius_Test_1)
{
#ifdef ROSEN_OHOS
    auto clipNode = RSCanvasNode::Create();
    clipNode->SetBounds({150, 150, 500, 400});
    clipNode->SetClipToBounds(true);
    clipNode->SetCornerRadius(50.0f);
    clipNode->SetBackgroundColor(SK_ColorYELLOW);
    GetRootNode()->AddChild(clipNode, -1);

    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Clip_Corner_Surface", 100, 100, 500, 400, 0xFF008000);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(clipNode);
    RegisterNode(surfaceNode);
#endif
}

/* Filter: blur effect */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Filter_Blur_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Filter_Blur_Surface", 200, 200, 400, 300, 0xFF87CEEB);
    surfaceNode->SetFilter(RSFilter::CreateBlurFilter(20.0f, 20.0f));  // Filter effect applied
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Filter: varying blur radius */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Filter_Blur_Multi_Test_1)
{
#ifdef ROSEN_OHOS
    struct BlurTest {
        float blurRadius;
        uint32_t color;
        int x;
    };
    BlurTest tests[] = {
        {5.0f, 0xFFFF6347, 100},
        {15.0f, 0xFF4682B4, 350},
        {30.0f, 0xFF32CD32, 600},
        {50.0f, 0xFFDA70D6, 850},
    };
    std::vector<RSSurfaceNode::SharedPtr> nodes;

    for (int i = 0; i < 4; i++) {
        auto node = CreateSurfaceNodeWithBuffer(
            "Hwc_Blur_" + std::to_string(i), tests[i].x, 200, 200, 200, tests[i].color);
        node->SetFilter(RSFilter::CreateBlurFilter(tests[i].blurRadius, tests[i].blurRadius));  // Blur filter applied
        GetRootNode()->SetTestSurface(node);
        nodes.push_back(node);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Filter: background color with alpha */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Filter_Bg_Alpha_Test_1)
{
#ifdef ROSEN_OHOS
    auto bgCanvas = RSCanvasNode::Create();
    bgCanvas->SetBounds({100, 100, 600, 500});
    bgCanvas->SetBackgroundColor(0x80000000); // Semi-transparent black
    GetRootNode()->AddChild(bgCanvas, -1);

    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Filter_Bg_Surface", 150, 150, 500, 400, 0xFF00CED1);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(bgCanvas);
    RegisterNode(surfaceNode);
#endif
}

/* Filter: combined with scale */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Filter_Scale_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Filter_Scale_Surface", 200, 200, 400, 300, 0xFF9370DB);
    surfaceNode->SetScale(1.3f, 1.3f);
    surfaceNode->SetFilter(RSFilter::CreateBlurFilter(15.0f, 15.0f));  // Blur filter applied
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Buffer rotation: 90 degree rotation */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Buffer_Rotation_90_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Buf_Rot_90", 200, 200, 400, 300, 0xFF20B2AA);
    surfaceNode->SetRotation(90.0f);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Buffer rotation: 180 degree rotation */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Buffer_Rotation_180_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Buf_Rot_180", 250, 250, 400, 300, 0xFFCD5C5C);
    surfaceNode->SetRotation(180.0f);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Buffer rotation: 270 degree rotation */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Buffer_Rotation_270_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Buf_Rot_270", 250, 250, 400, 300, 0xFFDB7093);
    surfaceNode->SetRotation(270.0f);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Gravity: center alignment */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Gravity_Center_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Gravity_Center", 200, 200, 400, 300, 0xFFBA55D3);
    surfaceNode->SetFrameGravity(Gravity::CENTER);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Gravity: top alignment */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Gravity_Top_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Gravity_Top", 200, 200, 400, 300, 0xFF00FA9A);
    surfaceNode->SetFrameGravity(Gravity::TOP);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Gravity: bottom alignment */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Gravity_Bottom_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Gravity_Bottom", 200, 200, 400, 300, 0xFFFF69B4);
    surfaceNode->SetFrameGravity(Gravity::BOTTOM);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Gravity: resize aspect */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Gravity_Resize_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Gravity_Resize", 200, 200, 400, 300, 0xFF7CFC00);
    surfaceNode->SetFrameGravity(Gravity::RESIZE);
    surfaceNode->SetScale(1.2f, 1.2f);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* ScalingMode: crop center */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Scaling_Crop_Center_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Scale_Crop", 200, 200, 400, 300, 0xFFADD8E6);
    surfaceNode->SetBounds({200, 200, 320, 240});
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Transform: 3D rotation X */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Transform_Rotation_X_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Transform_RotX", 250, 250, 400, 300, 0xFFF08080);
    surfaceNode->SetRotationX(45.0f);  // Non-Z axis rotation
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Transform: 3D rotation Y */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Transform_Rotation_Y_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Transform_RotY", 250, 250, 400, 300, 0xFF87CEFA);
    surfaceNode->SetRotationY(45.0f);  // Non-Z axis rotation
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Transform: quaternion rotation */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Transform_Quaternion_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Transform_Quat", 250, 250, 400, 300, 0xFFDDA0DD);
    Quaternion q(0.383f, 0.0f, 0.0f, 0.924f); // 45 degrees around X axis
    surfaceNode->SetRotation(q);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* BlendMode: alpha blending */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Blend_Alpha_Test_1)
{
#ifdef ROSEN_OHOS
    auto bottomNode = CreateSurfaceNodeWithBuffer("Hwc_Blend_Bottom", 200, 200, 400, 300, 0xFFFF0000);
    GetRootNode()->SetTestSurface(bottomNode);

    auto topNode = CreateSurfaceNodeWithBuffer("Hwc_Blend_Top", 250, 250, 400, 300, 0xFF0000FF);
    topNode->SetAlpha(0.7f);  // Alpha not equal to 1.0 with overlap
    GetRootNode()->SetTestSurface(topNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(bottomNode);
    RegisterNode(topNode);
#endif
}

/* Special position: center screen */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Pos_Center_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Pos_Center",
        (screenWidth - 400) / 2, (screenHeight - 300) / 2, 400, 300, 0xFF6495ED);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Special position: exact center alignment */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Pos_Exact_Center_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Pos_Exact_Center",
        screenWidth / 2 - 200, screenHeight / 2 - 150, 400, 300, 0xFF40E0D0);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Special position: negative coordinates (clipped) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Pos_Negative_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Pos_Negative", -100, -100, 300, 300, 0xFF7B68EE);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Offscreen: node outside visible area */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Offscreen_Test_1)
{
#ifdef ROSEN_OHOS
    auto visibleNode = CreateSurfaceNodeWithBuffer("Hwc_Offscreen_Visible", 100, 100, 300, 200, 0xFF0000FF);
    GetRootNode()->SetTestSurface(visibleNode);

    auto offscreenNode = CreateSurfaceNodeWithBuffer("Offscreen_Hidden", screenWidth + 100, 100,
                                                     300, 200, 0xFF00FF00);
    GetRootNode()->SetTestSurface(offscreenNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(visibleNode);
    RegisterNode(offscreenNode);
#endif
}

/* UpdatePrepareClip: complex clip scenario */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Prepare_Clip_Test_1)
{
#ifdef ROSEN_OHOS
    auto clipContainer = RSCanvasNode::Create();
    clipContainer->SetBounds({150, 150, 500, 400});
    clipContainer->SetClipToBounds(true);
    GetRootNode()->AddChild(clipContainer, -1);

    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Prepare_Clip_Surface", 100, 100, 600, 500, 0xFFFFD700);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(clipContainer);
    RegisterNode(surfaceNode);
#endif
}

/* UpdateHwcNodeByTransform: combined transforms */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Combine_Transform_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Combine_Transform", 200, 200, 400, 300, 0xFF00CED1);
    surfaceNode->SetTranslate({100, 50});
    surfaceNode->SetScale(1.2f, 0.8f);
    surfaceNode->SetRotation(30.0f);  // Non-90 degree rotation
    surfaceNode->SetPivot(0.3f, 0.7f);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(surfaceNode);
#endif
}

/* Multiple small surfaces: stress test */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Multi_Small_Test_1)
{
#ifdef ROSEN_OHOS
    std::vector<RSSurfaceNode::SharedPtr> nodes;
    constexpr int cols = 6;
    constexpr int rows = 5;
    constexpr int smallWidth = 80;
    constexpr int smallHeight = 60;
    constexpr int gap = 10;
    constexpr int startX = 50;
    constexpr int startY = 50;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            uint32_t color = 0xFF000000 | ((row * 50) << 16) | ((col * 40) << 8) | 0x80;
            auto node = CreateSurfaceNodeWithBuffer(
                "Hwc_Small_" + std::to_string(row * cols + col),
                startX + col * (smallWidth + gap), startY + row * (smallHeight + gap),
                smallWidth, smallHeight, color);
            GetRootNode()->SetTestSurface(node);
            nodes.push_back(node);
        }
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Global position test */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Global_Pos_Test_1)
{
#ifdef ROSEN_OHOS
    auto container = RSCanvasNode::Create();
    container->SetBounds({100, 100, 600, 500});
    container->SetTranslate({50, 50});
    GetRootNode()->AddChild(container, -1);

    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Global_Pos_Surface", 50, 50, 400, 300, 0xFFFF6347);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(container);
    RegisterNode(surfaceNode);
#endif
}

/* Comprehensive test: grid + stacking + free layout with transforms */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Comprehensive_Test_1)
{
#ifdef ROSEN_OHOS
    // ==================== Region 1: Grid Layout (2x3) ====================
    constexpr int gridStartX = 50;
    constexpr int gridStartY = 50;
    constexpr int gridCellWidth = 120;
    constexpr int gridCellHeight = 90;
    constexpr int gridGap = 10;
    constexpr int gridRows = 2;
    constexpr int gridCols = 3;
    uint32_t gridColors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000,
                             0xFFFFFF00, 0xFFFF00FF, 0xFF00FFFF};

    for (int row = 0; row < gridRows; row++) {
        for (int col = 0; col < gridCols; col++) {
            int x = gridStartX + col * (gridCellWidth + gridGap);
            int y = gridStartY + row * (gridCellHeight + gridGap);
            auto node = CreateSurfaceNodeWithBuffer(
                "Hwc_Compre_Grid_" + std::to_string(row * gridCols + col),
                x, y, gridCellWidth, gridCellHeight, gridColors[row * gridCols + col]);
            GetRootNode()->SetTestSurface(node);
            RegisterNode(node);
        }
    }

    // ==================== Region 2: Stacking Layout (5 layers) ====================
    constexpr int stackCenterX = 600;
    constexpr int stackCenterY = 200;
    constexpr int stackWidth = 250;
    constexpr int stackHeight = 180;
    uint32_t stackColors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000,
                             0xFFFFFF00, 0xFF808080};
    float stackAlphas[] = {1.0f, 0.85f, 0.7f, 0.55f, 0.4f};

    for (int i = 0; i < 5; i++) {
        int offset = i * 15;
        auto node = CreateSurfaceNodeWithBuffer(
            "Hwc_Compre_Stack_" + std::to_string(i),
            stackCenterX - offset, stackCenterY - offset,
            stackWidth, stackHeight, stackColors[i]);
        node->SetAlpha(stackAlphas[i]);  // Various alpha values with overlap
        GetRootNode()->SetTestSurface(node);
        RegisterNode(node);
    }

    // ==================== Region 3: Free Layout with Transforms ====================
    // Node 1: Scaled and rotated
    auto node1 = CreateSurfaceNodeWithBuffer("Hwc_Compre_Transform_1",
        100, 400, 200, 150, 0xFF9370DB);
    node1->SetScale(1.3f, 1.3f);
    node1->SetRotation(15.0f);  // Non-90 degree rotation
    node1->SetAlpha(0.9f);
    GetRootNode()->SetTestSurface(node1);
    RegisterNode(node1);

    // Node 2: Translated with corner radius (ENABLE: no rotation, alpha=1)
    auto node2 = CreateSurfaceNodeWithBuffer("Hwc_Compre_Transform_2",
        400, 450, 220, 180, 0xFF20B2AA);
    node2->SetTranslate({50, 30});
    node2->SetCornerRadius(30.0f);
    GetRootNode()->SetTestSurface(node2);
    RegisterNode(node2);

    // Node 3: Scaled with negative value (flip)
    auto node3 = CreateSurfaceNodeWithBuffer("Hwc_Compre_Transform_3",
        700, 400, 200, 150, 0xFFFF6347);
    node3->SetScale(-1.2f, 1.2f);
    node3->SetAlpha(0.8f);  // Alpha not equal to 1.0
    GetRootNode()->SetTestSurface(node3);
    RegisterNode(node3);

    // Node 4: Gradient with rotation
    auto node4 = CreateGradientSurfaceNode("Hwc_Compre_Gradient",
        950, 380, 200, 200);
    node4->SetRotation(25.0f);  // Non-90 degree rotation
    node4->SetAlpha(0.85f);
    GetRootNode()->SetTestSurface(node4);
    RegisterNode(node4);

    // ==================== Region 4: Mixed CanvasNode + SurfaceNode ====================
    // Background CanvasNode
    auto bgCanvas = RSCanvasNode::Create();
    bgCanvas->SetBounds({100, 700, 400, 300});
    bgCanvas->SetBackgroundColor(SK_ColorLTGRAY);
    bgCanvas->SetCornerRadius(20.0f);
    GetRootNode()->AddChild(bgCanvas, -1);

    // Overlay SurfaceNode with alpha
    auto overlayNode = CreateSurfaceNodeWithBuffer("Hwc_Compre_Overlay",
        150, 750, 300, 200, 0xFF4682B4);
    overlayNode->SetAlpha(0.6f);  // Alpha not equal to 1.0
    GetRootNode()->SetTestSurface(overlayNode);

    // Small accent CanvasNodes
    auto accent1 = RSCanvasNode::Create();
    accent1->SetBounds({120, 720, 60, 60});
    accent1->SetBackgroundColor(SK_ColorRED);
    accent1->SetCornerRadius(10.0f);
    GetRootNode()->AddChild(accent1, -1);

    auto accent2 = RSCanvasNode::Create();
    accent2->SetBounds({420, 920, 60, 60});
    accent2->SetBackgroundColor(SK_ColorGREEN);
    accent2->SetCornerRadius(10.0f);
    GetRootNode()->AddChild(accent2, -1);

    RegisterNode(bgCanvas);
    RegisterNode(overlayNode);
    RegisterNode(accent1);
    RegisterNode(accent2);

    // ==================== Region 5: Pivot Rotation Test ====================
    auto pivotNode = CreateSurfaceNodeWithBuffer("Hwc_Compre_Pivot",
        600, 700, 250, 200, 0xFFDDA0DD);
    pivotNode->SetPivot(0.5f, 0.5f);
    pivotNode->SetRotation(45.0f);  // Non-90 degree rotation
    pivotNode->SetScale(1.1f, 1.1f);
    GetRootNode()->SetTestSurface(pivotNode);
    RegisterNode(pivotNode);

    // ==================== Region 6: Clip with overflow ====================
    auto clipContainer = RSCanvasNode::Create();
    clipContainer->SetBounds({900, 650, 250, 350});
    clipContainer->SetClipToBounds(true);
    clipContainer->SetBackgroundColor(SK_ColorYELLOW);
    clipContainer->SetCornerRadius(15.0f);
    GetRootNode()->AddChild(clipContainer, -1);

    auto overflowNode = CreateSurfaceNodeWithBuffer("Hwc_Compre_Overflow",
        950, 700, 250, 350, 0xFF32CD32);
    GetRootNode()->SetTestSurface(overflowNode);

    RegisterNode(clipContainer);
    RegisterNode(overflowNode);

    // ==================== Flush Transaction ====================
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
#endif
}

/* Extreme stress test: many overlapping layers with varying properties */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Stress_Many_Layers_Test_1)
{
#ifdef ROSEN_OHOS
    const int centerX = screenWidth / 2;
    const int centerY = screenHeight / 2;
    const int numLayers = 15;
    const int baseWidth = 600;
    const int baseHeight = 450;

    std::vector<RSSurfaceNode::SharedPtr> nodes;

    // Create concentric layers with decreasing size and varying opacity
    for (int i = 0; i < numLayers; i++) {
        float scale = 1.0f - (i * 0.05f);
        int width = static_cast<int>(baseWidth * scale);
        int height = static_cast<int>(baseHeight * scale);
        int x = centerX - width / 2;
        int y = centerY - height / 2;

        // Generate color with gradient
        uint8_t r = static_cast<uint8_t>((i * 17) % 256);
        uint8_t g = static_cast<uint8_t>((i * 23) % 256);
        uint8_t b = static_cast<uint8_t>((i * 31) % 256);
        uint32_t color = 0xFF000000 | (b << 16) | (g << 8) | r;

        auto node = CreateSurfaceNodeWithBuffer(
            "Hwc_Stress_Layer_" + std::to_string(i),
            x, y, width, height, color);

        // Vary alpha, rotation, and scale
        // Most have alpha not equal to 1.0 and various rotation angles
        node->SetAlpha(0.9f - (i * 0.04f));  // 0.9, 0.86, ..., 0.26 (all not 1.0 except i=0)
        node->SetRotation(i * 5.0f);  // 0°, 5°, 10°, ..., 70° (only 0° enables HWC)
        node->SetCornerRadius(20.0f - i);

        GetRootNode()->SetTestSurface(node);
        nodes.push_back(node);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Small tiles pattern: dense grid of mini surfaces */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Tiles_Pattern_Test_1)
{
#ifdef ROSEN_OHOS
    constexpr int tileSize = 60;
    constexpr int gap = 5;
    constexpr int cols = 15;
    constexpr int rows = 20;
    constexpr int startX = 50;
    constexpr int startY = 50;

    std::vector<RSSurfaceNode::SharedPtr> nodes;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int x = startX + col * (tileSize + gap);
            int y = startY + row * (tileSize + gap);

            // Checkerboard pattern colors
            uint32_t color = ((row + col) % 2 == 0) ? 0xFF4682B4 : 0xFFFFA07A;

            auto node = CreateSurfaceNodeWithBuffer(
                "Hwc_Tile_" + std::to_string(row * cols + col),
                x, y, tileSize, tileSize, color);

            GetRootNode()->SetTestSurface(node);
            nodes.push_back(node);
        }
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* ==================== Hardware Acceleration Boundary Tests ==================== */
/* The following tests verify hardware acceleration behavior at specification boundaries */

/* Rotation: 45 degrees (non-90 multiple) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_45_Disable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Rot45_Disable", 200, 200, 400, 300, 0xFF20B2AA);
    surfaceNode->SetRotation(45.0f);  // Non-90 degree rotation
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Rotation: 30 degrees (non-90 multiple) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_30_Disable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Rot30_Disable", 250, 250, 350, 250, 0xFFCD5C5C);
    surfaceNode->SetRotation(30.0f);  // Non-90 degree rotation
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Rotation: 15 degrees with scale (non-90 multiple) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_15_Scale_Disable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Rot15_Scale_Disable", 200, 200, 400, 300, 0xFFDB7093);
    surfaceNode->SetRotation(15.0f);   // Non-90 degree rotation
    surfaceNode->SetScale(1.2f, 1.2f);
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Rotation: 90 degrees (90 degree multiple) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_90_Enable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Rot90_Enable", 200, 200, 400, 300, 0xFF20B2AA);
    surfaceNode->SetRotation(90.0f);  // 90 degree multiple
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Rotation: 180 degrees (90 degree multiple) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_180_Enable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Rot180_Enable", 250, 250, 400, 300, 0xFFCD5C5C);
    surfaceNode->SetRotation(180.0f);  // 90 degree multiple
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Rotation: 270 degrees (90 degree multiple) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_270_Enable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Rot270_Enable", 250, 250, 400, 300, 0xFFDB7093);
    surfaceNode->SetRotation(270.0f);  // 90 degree multiple
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Alpha: 0.7 (not equal to 1.0) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Alpha_0_7_Disable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Alpha_0_7_Disable", 200, 200, 400, 300, 0xFF87CEEB);
    surfaceNode->SetAlpha(0.7f);  // Alpha not equal to 1.0
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Alpha: 0.5 (not equal to 1.0) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Alpha_0_5_Disable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Alpha_0_5_Disable", 250, 250, 350, 250, 0xFF9370DB);
    surfaceNode->SetAlpha(0.5f);  // Alpha not equal to 1.0
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Alpha: 0.3 (not equal to 1.0) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Alpha_0_3_Disable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Alpha_0_3_Disable", 300, 300, 400, 300, 0xFFDDA0DD);
    surfaceNode->SetAlpha(0.3f);  // Alpha not equal to 1.0
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Alpha: 1.0 (exactly 1.0) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Alpha_1_0_Enable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Alpha_1_0_Enable", 200, 200, 400, 300, 0xFF20B2AA);
    surfaceNode->SetAlpha(1.0f);  // Alpha exactly 1.0
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Overlapping nodes in app */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Overlap_In_App_Test_1)
{
#ifdef ROSEN_OHOS
    // Two overlapping nodes
    auto bottomNode = CreateSurfaceNodeWithBuffer("Hwc_Overlap_Bottom", 200, 200, 400, 300, 0xFF0000FF);
    GetRootNode()->SetTestSurface(bottomNode);

    auto topNode = CreateSurfaceNodeWithBuffer("Hwc_Overlap_Top", 300, 250, 400, 300, 0xFF00FF00);
    GetRootNode()->SetTestSurface(topNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(bottomNode);
    RegisterNode(topNode);
#endif
}

/* Multiple overlapping chain in app */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Overlap_Chain_In_App_Test_1)
{
#ifdef ROSEN_OHOS
    std::vector<RSSurfaceNode::SharedPtr> nodes;
    uint32_t colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFF00};

    // Create overlapping chain: each overlaps with previous
    for (int i = 0; i < 4; i++) {
        auto node = CreateSurfaceNodeWithBuffer(
            "Hwc_Overlap_Chain_" + std::to_string(i),
            150 + i * 50, 150 + i * 50, 350, 250, colors[i]);
        GetRootNode()->SetTestSurface(node);
        nodes.push_back(node);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Corner radius with node below (may DISABLE HWC) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Corner_With_Below_Test_1)
{
#ifdef ROSEN_OHOS
    // Bottom node without corner
    auto bottomNode = CreateSurfaceNodeWithBuffer("Hwc_Corner_Bottom", 200, 200, 400, 300, 0xFF0000FF);
    GetRootNode()->SetTestSurface(bottomNode);

    // Top node with corner radius overlapping bottom
    auto topNode = CreateSurfaceNodeWithBuffer("Hwc_Corner_Top", 250, 250, 400, 300, 0xFF00FF00);
    topNode->SetCornerRadius(50.0f);  // Corner + overlap: may DISABLE HWC
    GetRootNode()->SetTestSurface(topNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(bottomNode);
    RegisterNode(topNode);
#endif
}

/* Multiple corner radius nodes stacked */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Multi_Corner_Stack_Test_1)
{
#ifdef ROSEN_OHOS
    std::vector<RSSurfaceNode::SharedPtr> nodes;
    uint32_t colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000};

    for (int i = 0; i < 3; i++) {
        auto node = CreateSurfaceNodeWithBuffer(
            "Hwc_Multi_Corner_" + std::to_string(i),
            200 + i * 30, 200 + i * 30, 380, 280, colors[i]);
        node->SetCornerRadius(40.0f - i * 10.0f);
        GetRootNode()->SetTestSurface(node);
        nodes.push_back(node);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Complex scenario: rotation + alpha */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_Alpha_Disable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Rot_Alpha_Disable", 200, 200, 400, 300, 0xFF9370DB);
    surfaceNode->SetRotation(45.0f);   // Non-90 multiple
    surfaceNode->SetAlpha(0.8f);       // Not 1.0f
    surfaceNode->SetScale(1.2f, 1.2f);
    // Multiple violations: non-90 rotation and alpha not equal to 1.0
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Complex scenario: corner + overlap + rotation */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Corner_Overlap_Rotation_Test_1)
{
#ifdef ROSEN_OHOS
    auto bottomNode = CreateSurfaceNodeWithBuffer("Hwc_Complex_Bottom", 150, 150, 400, 300, 0xFF4682B4);
    GetRootNode()->SetTestSurface(bottomNode);

    auto topNode = CreateSurfaceNodeWithBuffer("Hwc_Complex_Top", 200, 200, 400, 300, 0xFFFF6347);
    topNode->SetCornerRadius(45.0f);
    topNode->SetRotation(30.0f);  // Non-90 degree rotation
    GetRootNode()->SetTestSurface(topNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(bottomNode);
    RegisterNode(topNode);
#endif
}

/* DstRect different from Bounds (Frame test) */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Frame_Bounds_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Frame_Bounds", 200, 200, 500, 400, 0xFF800080);
    // Set Frame different from Bounds
    surfaceNode->SetFrame({0, 0, 500, 400});
    surfaceNode->SetBounds({200, 200, 400, 300});
    // DstRect different from Bounds
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Filter with blur effect */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Filter_Blur_Disable_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Filter_Blur_Disable", 200, 200, 400, 300, 0xFF87CEEB);
    surfaceNode->SetFilter(RSFilter::CreateBlurFilter(25.0f, 25.0f));
    // Blur filter applied
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Multiple alpha variations test */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Alpha_Gradient_Test_1)
{
#ifdef ROSEN_OHOS
    std::vector<RSSurfaceNode::SharedPtr> nodes;
    float alphas[] = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    uint32_t colors[] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFFFFFF00, 0xFFFF00FF};

    for (int i = 0; i < 5; i++) {
        auto node = CreateSurfaceNodeWithBuffer(
            "Hwc_Alpha_Grad_" + std::to_string(i),
            100 + i * 200, 100, 180, 130, colors[i]);
        node->SetAlpha(alphas[i]);
        // Various alpha values including 1.0
        GetRootNode()->SetTestSurface(node);
        nodes.push_back(node);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Rotation degree boundary tests */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Rotation_Boundary_Test_1)
{
#ifdef ROSEN_OHOS
    struct RotationTest {
        float degree;
        bool shouldEnable;  // Expected: true if 90 multiple
        uint32_t color;
        int x;
    };
    RotationTest tests[] = {
        {0.0f, true, 0xFFFF0000, 100},      // 0°: ENABLE
        {45.0f, false, 0xFF00FF00, 350},    // 45°: DISABLE
        {89.9f, false, 0xFF0000FF, 600},    // 89.9°: DISABLE
        {90.0f, true, 0xFFFFFF00, 100},     // 90°: ENABLE
        {90.1f, false, 0xFFFF00FF, 350},    // 90.1°: DISABLE
        {180.0f, true, 0xFF00FFFF, 600},    // 180°: ENABLE
        {270.0f, true, 0xFF808080, 100},    // 270°: ENABLE
        {360.0f, true, 0xFF800000, 350},    // 360°: ENABLE
    };
    std::vector<RSSurfaceNode::SharedPtr> nodes;

    for (int i = 0; i < 8; i++) {
        auto node = CreateSurfaceNodeWithBuffer(
            "Hwc_Rot_Boundary_" + std::to_string(i),
            tests[i].x, 100 + (i / 4) * 200, 180, 130, tests[i].color);
        node->SetRotation(tests[i].degree);
        GetRootNode()->SetTestSurface(node);
        nodes.push_back(node);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Edge case: very small alpha */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Alpha_Very_Small_Test_1)
{
#ifdef ROSEN_OHOS
    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Alpha_Tiny", 200, 200, 400, 300, 0xFF4169E1);
    surfaceNode->SetAlpha(0.01f);  // Very small but not zero: alpha not equal to 1.0
    GetRootNode()->SetTestSurface(surfaceNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    RegisterNode(surfaceNode);
#endif
}

/* Mixed: some nodes enable hardware acceleration, some disable */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Mixed_Enable_Disable_Test_1)
{
#ifdef ROSEN_OHOS
    std::vector<RSSurfaceNode::SharedPtr> nodes;

    // Node 1: Alpha 1.0 + Rotation 90: should ENABLE
    auto node1 = CreateSurfaceNodeWithBuffer("Hwc_Mixed_Enable1", 100, 100, 200, 150, 0xFF00FF00);
    node1->SetAlpha(1.0f);
    node1->SetRotation(90.0f);
    GetRootNode()->SetTestSurface(node1);
    nodes.push_back(node1);

    // Node 2: Alpha 0.7: should DISABLE
    auto node2 = CreateSurfaceNodeWithBuffer("Hwc_Mixed_Disable1", 350, 100, 200, 150, 0xFFFF0000);
    node2->SetAlpha(0.7f);
    GetRootNode()->SetTestSurface(node2);
    nodes.push_back(node2);

    // Node 3: Rotation 45: should DISABLE
    auto node3 = CreateSurfaceNodeWithBuffer("Hwc_Mixed_Disable2", 600, 100, 200, 150, 0xFF0000FF);
    node3->SetRotation(45.0f);
    GetRootNode()->SetTestSurface(node3);
    nodes.push_back(node3);

    // Node 4: Normal: should ENABLE
    auto node4 = CreateSurfaceNodeWithBuffer("Hwc_Mixed_Enable2", 100, 300, 200, 150, 0xFFFFFF00);
    GetRootNode()->SetTestSurface(node4);
    nodes.push_back(node4);

    // Node 5: With filter: should DISABLE
    auto node5 = CreateSurfaceNodeWithBuffer("Hwc_Mixed_Disable3", 350, 300, 200, 150, 0xFFFF00FF);
    node5->SetFilter(RSFilter::CreateBlurFilter(15.0f, 15.0f));
    GetRootNode()->SetTestSurface(node5);
    nodes.push_back(node5);

    // Node 6: Rotation 180: should ENABLE
    auto node6 = CreateSurfaceNodeWithBuffer("Hwc_Mixed_Enable3", 600, 300, 200, 150, 0xFF00FFFF);
    node6->SetRotation(180.0f);
    GetRootNode()->SetTestSurface(node6);
    nodes.push_back(node6);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* ClipToBounds affecting overlapping nodes */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Clip_Overlap_Test_1)
{
#ifdef ROSEN_OHOS
    auto clipContainer = RSCanvasNode::Create();
    clipContainer->SetBounds({150, 150, 500, 400});
    clipContainer->SetClipToBounds(true);
    clipContainer->SetBackgroundColor(SK_ColorGRAY);
    GetRootNode()->AddChild(clipContainer, -1);

    auto surfaceNode = CreateSurfaceNodeWithBuffer("Hwc_Clip_Overlap_Surface", 100, 100, 500, 400, 0xFF00FF00);
    GetRootNode()->SetTestSurface(surfaceNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(clipContainer);
    RegisterNode(surfaceNode);
#endif
}

/* Scale and rotation combination test */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Scale_Rotation_Test_1)
{
#ifdef ROSEN_OHOS
    std::vector<RSSurfaceNode::SharedPtr> nodes;
    struct ScaleRotTest {
        float scale;
        float rotation;
        bool shouldEnable;
        uint32_t color;
    };
    ScaleRotTest tests[] = {
        {1.0f, 0.0f, true, 0xFFFF0000},     // No transform: ENABLE
        {1.0f, 90.0f, true, 0xFF00FF00},    // 90°: ENABLE
        {1.0f, 45.0f, false, 0xFF0000FF},   // 45°: DISABLE
        {1.5f, 0.0f, true, 0xFFFFFF00},     // Scale only: ENABLE
        {1.5f, 90.0f, true, 0xFFFF00FF},    // Scale + 90°: ENABLE
        {1.5f, 45.0f, false, 0xFF00FFFF},   // Scale + 45°: DISABLE
    };

    for (int i = 0; i < 6; i++) {
        auto node = CreateSurfaceNodeWithBuffer(
            "Hwc_Scale_Rot_" + std::to_string(i),
            100 + (i % 3) * 300, 100 + (i / 3) * 250, 200, 150, tests[i].color);
        node->SetScale(tests[i].scale, tests[i].scale);
        if (tests[i].rotation != 0.0f) {
            node->SetRotation(tests[i].rotation);
        }
        GetRootNode()->SetTestSurface(node);
        nodes.push_back(node);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

/* Translate with overlapping */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Translate_Overlap_Test_1)
{
#ifdef ROSEN_OHOS
    auto node1 = CreateSurfaceNodeWithBuffer("Hwc_Trans_1", 100, 100, 300, 200, 0xFF0000FF);
    GetRootNode()->SetTestSurface(node1);

    auto node2 = CreateSurfaceNodeWithBuffer("Hwc_Trans_2", 200, 150, 300, 200, 0xFF00FF00);
    node2->SetTranslate({50, 50});  // Translation creates overlap
    GetRootNode()->SetTestSurface(node2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(node1);
    RegisterNode(node2);
#endif
}

/* Pivot rotation test with overlap */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Pivot_Overlap_Test_1)
{
#ifdef ROSEN_OHOS
    auto node1 = CreateSurfaceNodeWithBuffer("Hwc_Pivot_1", 200, 200, 350, 250, 0xFF4682B4);
    GetRootNode()->SetTestSurface(node1);

    auto node2 = CreateSurfaceNodeWithBuffer("Hwc_Pivot_2", 250, 250, 350, 250, 0xFF9370DB);
    node2->SetPivot(0.5f, 0.5f);
    node2->SetRotation(30.0f);  // Non-90: DISABLE
    GetRootNode()->SetTestSurface(node2);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(node1);
    RegisterNode(node2);
#endif
}

/* Multiple small nodes with various alphas */
GRAPHIC_TEST(HwcTest, CONTENT_DISPLAY_TEST, Hwc_Multi_Alpha_Variation_Test_1)
{
#ifdef ROSEN_OHOS
    std::vector<RSSurfaceNode::SharedPtr> nodes;
    constexpr int cols = 5;
    constexpr int rows = 4;
    constexpr int cellWidth = 100;
    constexpr int cellHeight = 80;
    constexpr int gap = 10;
    constexpr int startX = 100;
    constexpr int startY = 100;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int x = startX + col * (cellWidth + gap);
            int y = startY + row * (cellHeight + gap);
            float alpha = 1.0f - (row * 0.2f + col * 0.05f);
            alpha = (alpha < 0.1f) ? 0.1f : alpha;

            uint32_t color = 0xFF000000 | ((row * 50) << 16) | ((col * 40) << 8) | 0x80;

            auto node = CreateSurfaceNodeWithBuffer(
                "Hwc_Multi_Alpha_" + std::to_string(row * cols + col),
                x, y, cellWidth, cellHeight, color);
            node->SetAlpha(alpha);
            GetRootNode()->SetTestSurface(node);
            nodes.push_back(node);
        }
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    for (auto& node : nodes) {
        RegisterNode(node);
    }
#endif
}

} // namespace OHOS::Rosen
