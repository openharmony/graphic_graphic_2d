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
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetSkipDraw(skipDrawList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50},
            400, 400);
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetForeground: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetForeground_Matrix_1x2)
{
    std::vector<bool> foregroundList = {false, true};

    for (size_t i = 0; i < foregroundList.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
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
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetHDRPresent(hdrPresentList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50},
            400, 400);
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetForceUIFirst: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetForceUIFirst_Matrix_1x2)
{
    std::vector<bool> forceUIFirstList = {false, true};

    for (size_t i = 0; i < forceUIFirstList.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetForceUIFirst(forceUIFirstList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50},
            400, 400);
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetSurfaceBufferOpaque: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetSurfaceBufferOpaque_Matrix_1x2)
{
    std::vector<bool> opaqueList = {false, true};

    for (size_t i = 0; i < opaqueList.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetSurfaceBufferOpaque(opaqueList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50},
            400, 400);
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetGlobalPositionEnabled: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetGlobalPositionEnabled_Matrix_1x2)
{
    std::vector<bool> enabledList = {false, true};

    for (size_t i = 0; i < enabledList.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetGlobalPositionEnabled(enabledList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50},
            400, 400);
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetFrameGravityNewVersionEnabled: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetFrameGravityNewVersionEnabled_Matrix_1x2)
{
    std::vector<bool> enabledList = {false, true};

    for (size_t i = 0; i < enabledList.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetFrameGravityNewVersionEnabled(enabledList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50},
            400, 400);
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
            auto surfaceNode = RSSurfaceNode::Create();
            float x = static_cast<float>(col * 300 + 50);
            float y = static_cast<float>(row * 300 + 50);
            surfaceNode->SetBounds({x, y, 200, 200});
            surfaceNode->SetAbilityState(states[row]);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);

            auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {x, y}, 200, 200);
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
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetHidePrivacyContent(hideList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50},
            400, 400);
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
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>((i % 2) * 500 + 50),
                                static_cast<float>((i / 2) * 300 + 50),
                                300, 300});
        surfaceNode->SetApiCompatibleVersion(versions[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 500 + 50),
             static_cast<float>((i / 2) * 300 + 50)},
            300, 300);
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
            auto surfaceNode = RSSurfaceNode::Create();
            float x = static_cast<float>(col * 300 + 50);
            float y = static_cast<float>(row * 300 + 50);
            surfaceNode->SetBounds({x, y, 200, 200});
            surfaceNode->SetHardwareEnableHint(hardwareEnabled[row], check[col]);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);

            auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {x, y}, 200, 200);
            GetRootNode()->AddChild(canvasNode);
            RegisterNode(canvasNode);
        }
    }
}

/* SetFingerprint: normal values */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetFingerprint)
{
    std::vector<std::string> fingerprints = {
        "",
        "default",
        "test_fingerprint",
        "long_fingerprint_string_for_testing"
    };

    for (size_t i = 0; i < fingerprints.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>((i % 2) * 500 + 50),
                                static_cast<float>((i / 2) * 300 + 50),
                                300, 300});
        surfaceNode->SetFingerprint(fingerprints[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 500 + 50),
             static_cast<float>((i / 2) * 300 + 50)},
            300, 300);
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetIsNotifyUIBufferAvailable: normal values - 1x2 matrix */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetIsNotifyUIBufferAvailable_Matrix_1x2)
{
    std::vector<bool> notifyList = {false, true};

    for (size_t i = 0; i < notifyList.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>(i * 500 + 50), 50,
                                400, 400});
        surfaceNode->SetIsNotifyUIBufferAvailable(notifyList[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50},
            400, 400);
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
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>((i % 2) * 500 + 50),
                                static_cast<float>((i / 2) * 300 + 50),
                                300, 300});
        surfaceNode->SetAncoFlags(flags[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 500 + 50),
             static_cast<float>((i / 2) * 300 + 50)},
            300, 300);
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

/* SetWindowId: normal values */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetWindowId)
{
    std::variant<uint64_t, std::string> id1 = 1000;
    std::variant<uint64_t, std::string> id2 = "test_window_id";

    auto surfaceNode1 = RSSurfaceNode::Create();
    surfaceNode1->SetBounds({50, 50, 300, 300});
    std::visit([&surfaceNode1](auto&& arg) { surfaceNode1->SetWindowId(arg); }, id1);
    GetRootNode()->AddChild(surfaceNode1);
    RegisterNode(surfaceNode1);

    auto canvasNode1 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {50, 50}, 300, 300);
    GetRootNode()->AddChild(canvasNode1);
    RegisterNode(canvasNode1);

    auto surfaceNode2 = RSSurfaceNode::Create();
    surfaceNode2->SetBounds({400, 50, 300, 300});
    std::visit([&surfaceNode2](auto&& arg) { surfaceNode2->SetWindowId(arg); }, id2);
    GetRootNode()->AddChild(surfaceNode2);
    RegisterNode(surfaceNode2);

    auto canvasNode2 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {400, 50}, 300, 300);
    GetRootNode()->AddChild(canvasNode2);
    RegisterNode(canvasNode2);
}

/* Combined: multiple properties - matrix 2x2x2 */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_Combined_Properties_Matrix_2x2x2)
{
    std::vector<bool> boolValues = {false, true};

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            for (size_t k = 0; k < 2; k++) {
                auto surfaceNode = RSSurfaceNode::Create();
                float x = static_cast<float>(((i + j) % 2) * 300 + 50);
                float y = static_cast<float>((k + (i / 2)) * 300 + 50);
                surfaceNode->SetBounds({x, y, 200, 200});
                surfaceNode->SetSkipDraw(boolValues[i]);
                surfaceNode->SetHDRPresent(boolValues[j]);
                surfaceNode->SetForceUIFirst(boolValues[k]);
                GetRootNode()->AddChild(surfaceNode);
                RegisterNode(surfaceNode);

                auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                    {x, y}, 200, 200);
                GetRootNode()->AddChild(canvasNode);
                RegisterNode(canvasNode);
            }
        }
    }
}

/* Rapid state changes */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_Rapid_State_Changes)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({100, 100, 400, 400});
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);

    auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100}, 400, 400);
    GetRootNode()->AddChild(canvasNode);
    RegisterNode(canvasNode);

    // Rapid state changes
    surfaceNode->SetSkipDraw(false);
    surfaceNode->SetHDRPresent(false);
    surfaceNode->SetForceUIFirst(false);

    surfaceNode->SetSkipDraw(true);
    surfaceNode->SetHDRPresent(true);
    surfaceNode->SetForceUIFirst(true);

    surfaceNode->SetSkipDraw(false);
    surfaceNode->SetHDRPresent(false);
    surfaceNode->SetForceUIFirst(false);
}

/* SetContainerWindow: normal values */
GRAPHIC_TEST(SurfaceNodeTest, CONTENT_DISPLAY_TEST, SurfaceNode_SetContainerWindow)
{
    struct WindowInfo {
        bool hasContainerWindow;
        RRect rrect;
    };

    std::vector<WindowInfo> windowInfos = {
        {true, RRect(0, 0, 200, 200, 0)},
        {true, RRect(0, 0, 200, 200, 25)},
        {true, RRect(0, 0, 200, 200, 50)},
        {false, RRect(0, 0, 0, 0, 0)}
    };

    for (size_t i = 0; i < windowInfos.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({static_cast<float>((i % 2) * 400 + 100),
                                static_cast<float>((i / 2) * 400 + 100),
                                300, 300});
        surfaceNode->SetContainerWindow(windowInfos[i].hasContainerWindow, windowInfos[i].rrect);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 400 + 100),
             static_cast<float>((i / 2) * 400 + 100)},
            300, 300);
        GetRootNode()->AddChild(canvasNode);
        RegisterNode(canvasNode);
    }
}

} // namespace OHOS::Rosen
