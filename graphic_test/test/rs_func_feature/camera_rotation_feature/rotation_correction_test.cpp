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

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_utils.h"

#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_service_client.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "surface_type.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
// 测试常量定义
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr ScreenId TEST_SCREEN_ID = 0; // 使用屏幕ID 0
constexpr int PEN_WIDTH = 8;
} // namespace

class RotationCorrectionTest : public RSGraphicTest {
private:
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    RSSurfaceNode::SharedPtr CreateTestSurfaceNode()
    {
        struct RSSurfaceNodeConfig rsSurfaceNodeConfig1;
        rsSurfaceNodeConfig1.SurfaceNodeName = "AppWindow";
        rsSurfaceNodeConfig1.isSync = false;
        RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
        auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig1, rsSurfaceNodeType);
        surfaceNode->SetBounds({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        surfaceNode->SetFrame({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        surfaceNode->SetBackgroundColor(SK_ColorBLACK);
        return surfaceNode;
    }

    sptr<SurfaceBuffer> GetSurfaceBuffer(sptr<Surface> ohosSurface, int32_t width, int32_t height)
    {
        sptr<SurfaceBuffer> buffer;
        int32_t releaseFence = 0;
        BufferRequestConfig config = {
            .width = width,
            .height = height,
            .strideAlignment = 0x8,
            .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
            .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        };

        SurfaceError ret = ohosSurface->RequestBuffer(buffer, releaseFence, config);
        if (ret != SURFACE_ERROR_OK) {
            return nullptr;
        }
        return buffer;
    }

    void DoDraw(uint8_t* addr, uint32_t width, uint32_t height)
    {
        Drawing::Bitmap bitmap;
        Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
        bitmap.Build(width, height, format);

        Drawing::Canvas canvas;
        canvas.Bind(bitmap);

        // 背景色：白色
        canvas.Clear(Drawing::Color::COLOR_WHITE);

        // 左上角 - 红色块
        Drawing::Brush redBrush;
        redBrush.SetColor(Drawing::Color::COLOR_RED);
        canvas.AttachBrush(redBrush);
        Drawing::RectI redRect = { 0, 0, static_cast<int>(width) / 2, static_cast<int>(height) / 2 };
        canvas.DrawRect(redRect);

        // 右上角 - 绿色块
        Drawing::Brush greenBrush;
        greenBrush.SetColor(Drawing::Color::COLOR_GREEN);
        canvas.AttachBrush(greenBrush);
        Drawing::RectI greenRect = { static_cast<int>(width) / 2, 0, static_cast<int>(width),
            static_cast<int>(height) / 2 };
        canvas.DrawRect(greenRect);

        // 左下角 - 蓝色块
        Drawing::Brush blueBrush;
        blueBrush.SetColor(Drawing::Color::COLOR_BLUE);
        canvas.AttachBrush(blueBrush);
        Drawing::RectI blueRect = { 0, static_cast<int>(height) / 2, static_cast<int>(width) / 2,
            static_cast<int>(height) };
        canvas.DrawRect(blueRect);

        // 右下角 - 黄色块
        Drawing::Brush yellowBrush;
        yellowBrush.SetColor(Drawing::Color::COLOR_YELLOW);
        canvas.AttachBrush(yellowBrush);
        Drawing::RectI yellowRect = { static_cast<int>(width) / 2, static_cast<int>(height) / 2,
            static_cast<int>(width), static_cast<int>(height) };
        canvas.DrawRect(yellowRect);

        // 中心绘制方向指示
        Drawing::Pen pen;
        pen.SetColor(Drawing::Color::COLOR_BLACK);
        pen.SetWidth(PEN_WIDTH);
        pen.SetAntiAlias(true);
        canvas.AttachPen(pen);

        // 绘制向上的箭头
        int centerX = width / 2;
        int centerY = height / 2;
        canvas.DrawLine({centerX, centerY + 80}, {centerX, centerY - 80}); // 箭头杆
        canvas.DrawLine({centerX, centerY - 80}, {centerX - 30, centerY - 30}); // 左箭头
        canvas.DrawLine({centerX, centerY - 80}, {centerX + 30, centerY - 30}); // 右箭头

        static constexpr uint32_t stride = 4;
        uint32_t addrSize = width * height * stride;
        memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    }

    /**
     * @brief 将buffer赋给surfaceNode（必须调用，用于初始化SELF_DRAWING_NODE的内容）
     */
    void InitSurfaceStyle(std::shared_ptr<RSSurfaceNode> surfaceNode)
    {
        auto ohosSurface = surfaceNode->GetSurface();
        if (ohosSurface == nullptr) {
            return;
        }
        sptr<SurfaceBuffer> buffer = GetSurfaceBuffer(ohosSurface, surfaceNode->GetStagingProperties().GetBounds().z_,
            surfaceNode->GetStagingProperties().GetBounds().w_);
        if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
            return;
        }
        auto addr = static_cast<uint8_t*>(buffer->GetVirAddr());
        DoDraw(addr, buffer->GetWidth(), buffer->GetHeight());
        BufferFlushConfig flushConfig = {
            .damage = {
                .w = buffer->GetWidth(),
                .h = buffer->GetHeight(),
            }
        };
        ohosSurface->FlushBuffer(buffer, -1, flushConfig);
    }

    std::shared_ptr<RSSurfaceNode> CreateSelfDrawingNode()
    {
        struct RSSurfaceNodeConfig rsSurfaceNodeConfig1;
        rsSurfaceNodeConfig1.SurfaceNodeName = "SelfDrawingNode";
        rsSurfaceNodeConfig1.isSync = false;
        RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
        auto selfDrawingNode = RSSurfaceNode::Create(rsSurfaceNodeConfig1, rsSurfaceNodeType);
        selfDrawingNode->SetBounds({ 0, 0, 960, 960 });
        selfDrawingNode->SetFrame({ 0, 0, 960, 960 });
        selfDrawingNode->SetHardwareEnabled(true);
        selfDrawingNode->SetBackgroundColor(SK_ColorWHITE);

        InitSurfaceStyle(selfDrawingNode);

        selfDrawingNode->SetForceHardwareAndFixRotation(true);

        return selfDrawingNode;
    }
};

/*
 * @tc.name: ROTATION_CORRECTION_SCREEN_NORMAL_001
 * @tc.desc: 测试SetScreenCorrection接口 - 正常值ROTATION_0
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_SCREEN_NORMAL_001)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_0);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_SCREEN_NORMAL_002
 * @tc.desc: 测试SetScreenCorrection接口 - 正常值ROTATION_90
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_SCREEN_NORMAL_002)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_SCREEN_NORMAL_003
 * @tc.desc: 测试SetScreenCorrection接口 - 正常值ROTATION_180
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_SCREEN_NORMAL_003)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_180);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_SCREEN_NORMAL_004
 * @tc.desc: 测试SetScreenCorrection接口 - 正常值ROTATION_270
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_SCREEN_NORMAL_004)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_270);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_SCREEN_BOUNDARY_001
 * @tc.desc: 测试SetScreenCorrection接口 - 边界值ROTATION_0（枚举最小值）
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_SCREEN_BOUNDARY_001)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, static_cast<ScreenRotation>(0));
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_SCREEN_BOUNDARY_002
 * @tc.desc: 测试SetScreenCorrection接口 - 边界值ROTATION_270（枚举最大有效值）
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_SCREEN_BOUNDARY_002)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, static_cast<ScreenRotation>(3));
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_SCREEN_ABNORMAL_001
 * @tc.desc: 测试SetScreenCorrection接口 - 异常值INVALID_SCREEN_ROTATION
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_SCREEN_ABNORMAL_001)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::INVALID_SCREEN_ROTATION);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_LOGICAL_NORMAL_001
 * @tc.desc: 测试SetLogicalCameraRotationCorrection接口 - 正常值ROTATION_0
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_LOGICAL_NORMAL_001)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_0);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_LOGICAL_NORMAL_002
 * @tc.desc: 测试SetLogicalCameraRotationCorrection接口 - 正常值ROTATION_90
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_LOGICAL_NORMAL_002)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_LOGICAL_NORMAL_003
 * @tc.desc: 测试SetLogicalCameraRotationCorrection接口 - 正常值ROTATION_180
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_LOGICAL_NORMAL_003)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_180);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_LOGICAL_NORMAL_004
 * @tc.desc: 测试SetLogicalCameraRotationCorrection接口 - 正常值ROTATION_270
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_LOGICAL_NORMAL_004)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_270);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_LOGICAL_BOUNDARY_001
 * @tc.desc: 测试SetLogicalCameraRotationCorrection接口 - 边界值ROTATION_0
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_LOGICAL_BOUNDARY_001)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, static_cast<ScreenRotation>(0));
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_LOGICAL_BOUNDARY_002
 * @tc.desc: 测试SetLogicalCameraRotationCorrection接口 - 边界值ROTATION_270
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_LOGICAL_BOUNDARY_002)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, static_cast<ScreenRotation>(3));
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_LOGICAL_ABNORMAL_001
 * @tc.desc: 测试SetLogicalCameraRotationCorrection接口 - 异常值INVALID
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_LOGICAL_ABNORMAL_001)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::INVALID_SCREEN_ROTATION);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_APP_NORMAL_001
 * @tc.desc: 测试SetAppRotationCorrection接口 - 正常值ROTATION_0
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_APP_NORMAL_001)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();

    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::ROTATION_0);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_APP_NORMAL_002
 * @tc.desc: 测试SetAppRotationCorrection接口 - 正常值ROTATION_90
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_APP_NORMAL_002)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();

    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::ROTATION_90);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_APP_NORMAL_003
 * @tc.desc: 测试SetAppRotationCorrection接口 - 正常值ROTATION_180
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_APP_NORMAL_003)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();

    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::ROTATION_180);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_APP_NORMAL_004
 * @tc.desc: 测试SetAppRotationCorrection接口 - 正常值ROTATION_270
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_APP_NORMAL_004)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();

    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::ROTATION_270);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_APP_BOUNDARY_001
 * @tc.desc: 测试SetAppRotationCorrection接口 - 边界值ROTATION_0
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_APP_BOUNDARY_001)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();

    selfDrawingNode->SetAppRotationCorrection(static_cast<ScreenRotation>(0));

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_APP_BOUNDARY_002
 * @tc.desc: 测试SetAppRotationCorrection接口 - 边界值ROTATION_270
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_APP_BOUNDARY_002)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();

    selfDrawingNode->SetAppRotationCorrection(static_cast<ScreenRotation>(3));

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_APP_ABNORMAL_001
 * @tc.desc: 测试SetAppRotationCorrection接口 - 异常值INVALID（在RSSurfaceNode中被拦截）
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_APP_ABNORMAL_001)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();

    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::INVALID_SCREEN_ROTATION);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_COMBINE_001
 * @tc.desc: 组合测试 - Screen(90) + LogicalCamera(90)
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_COMBINE_001)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_COMBINE_002
 * @tc.desc: 组合测试 - Screen(180) + LogicalCamera(180)
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_COMBINE_002)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_180);
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_180);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_COMBINE_003
 * @tc.desc: 组合测试 - Screen(90) + App(90)
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_COMBINE_003)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::ROTATION_90);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_COMBINE_004
 * @tc.desc: 组合测试 - LogicalCamera(90) + App(90)
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_COMBINE_004)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::ROTATION_90);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_COMBINE_005
 * @tc.desc: 组合测试 - Screen(90) + LogicalCamera(90) + App(90) 三级组合
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_COMBINE_005)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::ROTATION_90);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_COMBINE_006
 * @tc.desc: 组合测试 - Screen(90) + LogicalCamera(270) 旋转抵消
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_COMBINE_006)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_270);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_COMBINE_007
 * @tc.desc: 组合测试 - 全部为0，无旋转效果
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_COMBINE_007)
{
    std::shared_ptr<RSUIDirector> rsUiDirector = RSGraphicTestDirector::Instance().rsUiDirector_;
    auto rsUIContext = rsUiDirector->GetRSUIContext();
    auto surfaceNode = CreateTestSurfaceNode();
    GetRootNode()->SetTestSurface(surfaceNode);
    auto rootNode = RSRootNode::Create(false, false, rsUIContext)->ReinterpretCastTo<RSRootNode>();
    rootNode->SetBounds(0, 0, 1000, 1000);
    rootNode->SetFrame(0, 0, 1000, 1000);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_BLACK);
    GetRootNode()->AddChild(rootNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_0);
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_0);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto selfDrawingNode = CreateSelfDrawingNode();
    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::ROTATION_0);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}
} // namespace OHOS::Rosen