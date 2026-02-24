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
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class SurfaceNodeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* SetSkipDraw: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetSkipDraw_Matrix_1x2)
{
    std::vector<bool> skipDrawList = {false, true};

    for (size_t i = 0; i < skipDrawList.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetSkipDraw(skipDrawList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50, 400, 400});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetForeground: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetForeground_Matrix_1x2)
{
    std::vector<bool> foregroundList = {false, true};

    for (size_t i = 0; i < foregroundList.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetForeground(foregroundList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                               400, 400});
        canvasNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetHDRPresent: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetHDRPresent_Matrix_1x2)
{
    std::vector<bool> hdrPresentList = {false, true};

    for (size_t i = 0; i < hdrPresentList.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetHDRPresent(hdrPresentList[i], 0);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50, 400, 400});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetForceUIFirst: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetForceUIFirst_Matrix_1x2)
{
    std::vector<bool> forceUIFirstList = {false, true};

    for (size_t i = 0; i < forceUIFirstList.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetForceUIFirst(forceUIFirstList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50, 400, 400});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetSurfaceBufferOpaque: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetSurfaceBufferOpaque_Matrix_1x2)
{
    std::vector<bool> opaqueList = {false, true};

    for (size_t i = 0; i < opaqueList.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetSurfaceBufferOpaque(opaqueList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50, 400, 400});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetGlobalPositionEnabled: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetGlobalPositionEnabled_Matrix_1x2)
{
    std::vector<bool> enabledList = {false, true};

    for (size_t i = 0; i < enabledList.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetGlobalPositionEnabled(enabledList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50, 400, 400});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetFrameGravityNewVersionEnabled: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetFrameGravityNewVersionEnabled_Matrix_1x2)
{
    std::vector<bool> enabledList = {false, true};

    for (size_t i = 0; i < enabledList.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetFrameGravityNewVersionEnabled(enabledList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50, 400, 400});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetAbilityState: normal values - matrix 2x2 */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetAbilityState_Matrix_2x2)
{
    std::vector<int32_t> states = {0, 1};
    std::vector<bool> actives = {false, true};

    for (size_t row = 0; row < states.size(); row++) {
        for (size_t col = 0; col < actives.size(); col++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
            auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
            float x = static_cast<float>(col * 300 + 50);
            float y = static_cast<float>(row * 300 + 50);
            surfaceNode->SetBounds({x, y, 200, 200});
            surfaceNode->SetAbilityState(static_cast<RSSurfaceNodeAbilityState>(states[row]));
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);

            auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {x, y, 200, 200});
            GetRootNode()->AddChild(canvasNode);
            RegisterNode(canvasNode);
        }
    }
}

/* SetHidePrivacyContent: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetHidePrivacyContent_Matrix_1x2)
{
    std::vector<bool> hideList = {false, true};

    for (size_t i = 0; i < hideList.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetHidePrivacyContent(hideList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50, 400, 400});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetApiCompatibleVersion: normal values */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetApiCompatibleVersion)
{
    std::vector<uint32_t> versions = {
        0,
        1,
        2,
        1000
    };

    for (size_t i = 0; i < versions.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>((i % 2) * 500 + 50),
                                static_cast<float>((i / 2) * 300 + 50),
                                300, 300});
        surfaceNode->SetApiCompatibleVersion(versions[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {static_cast<float>((i % 2) * 500 + 50),
             static_cast<float>((i / 2) * 300 + 50), 300, 300});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetHardwareEnableHint: normal values - matrix 2x2 */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetHardwareEnableHint_Matrix_2x2)
{
    std::vector<bool> hardwareEnabled = {false, true};
    std::vector<bool> check = {false, true};

    for (size_t row = 0; row < hardwareEnabled.size(); row++) {
        for (size_t col = 0; col < check.size(); col++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
            auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
            float x = static_cast<float>(col * 300 + 50);
            float y = static_cast<float>(row * 300 + 50);
            surfaceNode->SetBounds({x, y, 200, 200});
            surfaceNode->SetHardwareEnableHint(hardwareEnabled[row]);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);

            auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {x, y, 200, 200});
            GetRootNode()->AddChild(canvasNode);
            RegisterNode(canvasNode);
        }
    }
}

/* SetFingerprint: normal values */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetFingerprint)
{
    std::vector<bool> fingerprints = {
        false,
        true,
        false,
        true
    };

    for (size_t i = 0; i < fingerprints.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>((i % 2) * 500 + 50),
                                static_cast<float>((i / 2) * 300 + 50),
                                300, 300});
        surfaceNode->SetFingerprint(fingerprints[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {static_cast<float>((i % 2) * 500 + 50),
             static_cast<float>((i / 2) * 300 + 50), 300, 300});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetIsNotifyUIBufferAvailable: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetIsNotifyUIBufferAvailable_Matrix_1x2)
{
    std::vector<bool> notifyList = {false, true};

    for (size_t i = 0; i < notifyList.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetIsNotifyUIBufferAvailable(notifyList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50, 400, 400});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetAncoFlags: normal values */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetAncoFlags)
{
    std::vector<uint32_t> flags = {
        0,
        1,
        0xFF,
        0xFFFF
    };

    for (size_t i = 0; i < flags.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>((i % 2) * 500 + 50),
                                static_cast<float>((i / 2) * 300 + 50),
                                300, 300});
        surfaceNode->SetAncoFlags(flags[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {static_cast<float>((i % 2) * 500 + 50),
             static_cast<float>((i / 2) * 300 + 50), 300, 300});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetWindowId: normal values */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetWindowId)
{
    std::vector<uint32_t> windowIds = {
        0,
        1,
        1000,
        0xFFFFFFFF
    };

    for (size_t i = 0; i < windowIds.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>((i % 2) * 500 + 50),
                                static_cast<float>((i / 2) * 300 + 50),
                                300, 300});
        surfaceNode->SetWindowId(windowIds[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {static_cast<float>((i % 2) * 500 + 50),
             static_cast<float>((i / 2) * 300 + 50), 300, 300});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* Combined: multiple properties - matrix 2x2x2 */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_Combined_Properties_Matrix_2x2x2)
{
    std::vector<bool> boolValues = {false, true};

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            for (size_t k = 0; k < 2; k++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
                auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
                float x = static_cast<float>(((i + j) % 2) * 300 + 50);
                float y = static_cast<float>((k + (i / 2)) * 300 + 50);
                surfaceNode->SetBounds({x, y, 200, 200});
                surfaceNode->SetSkipDraw(boolValues[i]);
                surfaceNode->SetHDRPresent(boolValues[j], 0);
                surfaceNode->SetForceUIFirst(boolValues[k]);
                GetRootNode()->AddChild(surfaceNode);
                RegisterNode(surfaceNode);

                auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {x, y, 200, 200});
                GetRootNode()->AddChild(canvasNode);
                RegisterNode(canvasNode);
            }
        }
    }
}

/* Rapid state changes */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_Rapid_State_Changes)
{
        RSSurfaceNodeConfig surfaceNodeConfig;
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    surfaceNode->SetBounds({100, 100, 400, 400});
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);

    auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {100, 100, 400, 400});
    GetRootNode()->AddChild(canvasNode);
    RegisterNode(canvasNode);

    // Rapid state changes
    surfaceNode->SetSkipDraw(false);
    surfaceNode->SetHDRPresent(false, 0);
    surfaceNode->SetForceUIFirst(false);

    surfaceNode->SetSkipDraw(true);
    surfaceNode->SetHDRPresent(true, 0);
    surfaceNode->SetForceUIFirst(true);

    surfaceNode->SetSkipDraw(false);
    surfaceNode->SetHDRPresent(false, 0);
    surfaceNode->SetForceUIFirst(false);
}

/* SetContainerWindow: normal values */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetContainerWindow)
{
    struct WindowInfo {
        bool hasContainerWindow;
        RectT<float> rect;
        float radiusX;
        float radiusY;
    };

    std::vector<WindowInfo> windowInfos = {
        {true, {0.f, 0.f, 200.f, 200.f}, 0.f, 0.f},
        {true, {0.f, 0.f, 200.f, 200.f}, 25.f, 25.f},
        {true, {0.f, 0.f, 200.f, 200.f}, 50.f, 50.f},
        {false, {0.f, 0.f, 0.f, 0.f}, 0.f, 0.f}
    };

    for (size_t i = 0; i < windowInfos.size(); i++) {
        RSSurfaceNodeConfig surfaceNodeConfig;
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
        surfaceNode->SetBounds({static_cast<float>((i % 2) * 400 + 100),
                                static_cast<float>((i / 2) * 400 + 100),
                                300, 300});

        RRect rrect(windowInfos[i].rect, windowInfos[i].radiusX, windowInfos[i].radiusY);
        surfaceNode->SetContainerWindow(windowInfos[i].hasContainerWindow, rrect);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", {static_cast<float>((i % 2) * 400 + 100),
             static_cast<float>((i / 2) * 400 + 100), 300, 300});
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

} // namespace OHOS::Rosen
