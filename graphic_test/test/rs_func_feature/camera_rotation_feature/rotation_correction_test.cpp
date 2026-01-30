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

/**
 * @file rotation_correction_test.cpp
 * @brief 旋转校正接口像素级比对测试用例
 *
 * 测试三个旋转校正接口：
 * 1. SetScreenCorrection - 屏幕级旋转校正
 * 2. SetLogicalCameraRotationCorrection - 逻辑显示级旋转校正
 * 3. SetAppRotationCorrection - 应用节点级旋转校正
 *
 * 重要：所有测试用例都必须先创建SELF_DRAWING_NODE节点，并调用
 * SetForceHardwareAndFixRotation(true)使能固定旋转功能，才能观察旋转效果
 *
 * 覆盖场景：
 * - 正常值：ROTATION_0, ROTATION_90, ROTATION_180, ROTATION_270
 * - 边界值：枚举最小值和最大值
 * - 异常值：INVALID_SCREEN_ROTATION 及超出范围的值
 * - 组合场景：三个接口交叉使用
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
constexpr int NODE_SIZE = 200;
constexpr int GAP = 30;
} // namespace

/**
 * @class RotationCorrectionTest
 * @brief 旋转校正接口测试类
 */
class RotationCorrectionTest : public RSGraphicTest {
private:
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

public:
    /**
     * @brief 每个测试用例执行前调用
     */
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    /**
     * @brief 创建APP_WINDOW类型的SurfaceNode（作为根Surface）
     */
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

    /**
     * @brief 获取SurfaceBuffer
     */
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

    /**
     * @brief 在buffer上绘制方向指示内容
     * 使用四角不同颜色来显示旋转效果：
     * 左上：红色，右上：绿色，左下：蓝色，右下：黄色
     * 中心：黑色向上箭头和"TOP"文字标记
     */
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
        Drawing::RectI greenRect = { static_cast<int>(width) / 2, 0, static_cast<int>(width), static_cast<int>(height) / 2 };
        canvas.DrawRect(greenRect);

        // 左下角 - 蓝色块
        Drawing::Brush blueBrush;
        blueBrush.SetColor(Drawing::Color::COLOR_BLUE);
        canvas.AttachBrush(blueBrush);
        Drawing::RectI blueRect = { 0, static_cast<int>(height) / 2, static_cast<int>(width) / 2, static_cast<int>(height) };
        canvas.DrawRect(blueRect);

        // 右下角 - 黄色块
        Drawing::Brush yellowBrush;
        yellowBrush.SetColor(Drawing::Color::COLOR_YELLOW);
        canvas.AttachBrush(yellowBrush);
        Drawing::RectI yellowRect = { static_cast<int>(width) / 2, static_cast<int>(height) / 2, static_cast<int>(width), static_cast<int>(height) };
        canvas.DrawRect(yellowRect);

        // 中心绘制方向指示
        Drawing::Pen pen;
        pen.SetColor(Drawing::Color::COLOR_BLACK);
        pen.SetWidth(8);
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

    /**
     * @brief 创建SELF_DRAWING_NODE节点并完成初始化
     * 注意：必须调用SetForceHardwareAndFixRotation(true)使能固定旋转功能
     */
    std::shared_ptr<RSSurfaceNode> CreateSelfDrawingNode()
    {
        struct RSSurfaceNodeConfig rsSurfaceNodeConfig1;
        rsSurfaceNodeConfig1.SurfaceNodeName = "SelfDrawingNode";
        rsSurfaceNodeConfig1.isSync = false;
        RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
        auto selfDrawingNode = RSSurfaceNode::Create(rsSurfaceNodeConfig1, rsSurfaceNodeType);
        selfDrawingNode->SetBounds({ 0, 0, 1200, 1200 });
        selfDrawingNode->SetFrame({ 0, 0, 1200, 1200 });
        selfDrawingNode->SetHardwareEnabled(true);
        selfDrawingNode->SetBackgroundColor(SK_ColorWHITE);

        // 必须调用InitSurfaceStyle来初始化buffer和绘制内容
        InitSurfaceStyle(selfDrawingNode);

        // 必须调用！使能固定旋转功能，这是SetAppRotationCorrection生效的前提
        selfDrawingNode->SetForceHardwareAndFixRotation(true);

        return selfDrawingNode;
    }

    /**
     * @brief 创建测试矩阵节点（辅助节点，用于对比观察）
     */
    void CreateTestMatrix(std::shared_ptr<RSRootNode>& rootNode, int cols, int rows, int startX, int startY)
    {
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                int x = startX + col * (NODE_SIZE + GAP);
                int y = startY + row * (NODE_SIZE + GAP);

                auto canvasNode = RSCanvasNode::Create();
                canvasNode->SetBounds({ x, y, NODE_SIZE, NODE_SIZE });
                canvasNode->SetFrame({ x, y, NODE_SIZE, NODE_SIZE });
                canvasNode->SetBackgroundColor(GetTestColor(row * cols + col));

                rootNode->AddChild(canvasNode);
                RegisterNode(canvasNode);
            }
        }
    }

    /**
     * @brief 获取测试颜色
     */
    SkColor GetTestColor(int index)
    {
        SkColor colors[] = {
            SK_ColorRED, SK_ColorGREEN, SK_ColorBLUE, SK_ColorYELLOW,
            SK_ColorCYAN, 0xFFFF00FF, 0xFFFFA500, 0xFF808080,
            0xFF800000, 0xFF808000, 0xFF008000, 0xFF800080
        };
        return colors[index % 12];
    }
};

// ============================================================================
// SetScreenCorrection 接口测试用例
// ============================================================================

/*
 * @tc.name: ROTATION_CORRECTION_SCREEN_NORMAL_001
 * @tc.desc: 测试SetScreenCorrection接口 - 正常值ROTATION_0
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_SCREEN_NORMAL_001)
{
    // 1. 创建APP_WINDOW_NODE作为根Surface
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

    // 2. 创建SELF_DRAWING_NODE（必须，用于观察旋转效果）
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 3. 设置屏幕旋转校正为ROTATION_0
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_0);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 4. 注册节点
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置屏幕旋转校正为ROTATION_90
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置屏幕旋转校正为ROTATION_180
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置屏幕旋转校正为ROTATION_270
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置为枚举最小值
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置为枚举最大有效值
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置为INVALID值（测试健壮性）
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::INVALID_SCREEN_ROTATION);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

// ============================================================================
// SetLogicalCameraRotationCorrection 接口测试用例
// ============================================================================

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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置逻辑相机旋转校正为ROTATION_0
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置逻辑相机旋转校正为ROTATION_90
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置逻辑相机旋转校正为ROTATION_180
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置逻辑相机旋转校正为ROTATION_270
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置为枚举最小值
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置为枚举最大有效值
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 设置为INVALID值
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::INVALID_SCREEN_ROTATION);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

// ============================================================================
// SetAppRotationCorrection 接口测试用例
// ============================================================================

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

    // 创建SELF_DRAWING_NODE（已包含SetForceHardwareAndFixRotation(true)调用）
    auto selfDrawingNode = CreateSelfDrawingNode();

    // 设置应用旋转校正为ROTATION_0
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();

    // 设置应用旋转校正为ROTATION_90
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();

    // 设置应用旋转校正为ROTATION_180
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();

    // 设置应用旋转校正为ROTATION_270
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();

    // 设置为枚举最小值
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();

    // 设置为枚举最大有效值
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();

    // 设置为INVALID值（测试健壮性，该调用在RSSurfaceNode中被拦截）
    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::INVALID_SCREEN_ROTATION);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

// ============================================================================
// 组合测试用例 - 三个接口交叉使用
// ============================================================================

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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    // 组合设置: Screen 90度 + LogicalCamera 90度
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    // 组合设置: Screen 180度 + LogicalCamera 180度
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

    // Screen设置90度
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 创建SELF_DRAWING_NODE并设置App 90度
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

    // LogicalCamera设置90度
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 创建SELF_DRAWING_NODE并设置App 90度
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

    // 三级组合: Screen 90 + LogicalCamera 90 + App 90
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 创建SELF_DRAWING_NODE并设置App 90度
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    // 组合设置: Screen 90度 + LogicalCamera 270度 (部分抵消)
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

    // 全部设为0
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_0);
    rsInterfaces.SetLogicalCameraRotationCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_0);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 创建SELF_DRAWING_NODE并设置App 0度
    auto selfDrawingNode = CreateSelfDrawingNode();
    selfDrawingNode->SetAppRotationCorrection(ScreenRotation::ROTATION_0);

    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_DYNAMIC_001
 * @tc.desc: 动态切换测试 - 从ROTATION_0切换到ROTATION_90
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_DYNAMIC_001)
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    // 初始设置为0度
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_0);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 动态切换到90度
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

/*
 * @tc.name: ROTATION_CORRECTION_DYNAMIC_002
 * @tc.desc: 动态切换测试 - 连续切换旋转值
 * @tc.type: FUNC
 * @tc.require: I8XXXX
 */
GRAPHIC_TEST(RotationCorrectionTest, CONTENT_DISPLAY_TEST, ROTATION_CORRECTION_DYNAMIC_002)
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

    // 创建SELF_DRAWING_NODE
    auto selfDrawingNode = CreateSelfDrawingNode();
    rootNode->AddChild(selfDrawingNode);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto& rsInterfaces = RSInterfaces::GetInstance();

    // 初始0度
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_0);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 切换到90度
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_90);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 切换到180度
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_180);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    // 切换到270度
    rsInterfaces.SetScreenCorrection(TEST_SCREEN_ID, ScreenRotation::ROTATION_270);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    RegisterNode(rootNode);
    RegisterNode(selfDrawingNode);
}

} // namespace OHOS::Rosen
