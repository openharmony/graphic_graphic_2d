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
#include "ui/rs_display_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class DisplayNodeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* SetScreenId: normal values */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_SetScreenId)
{
    std::vector<uint64_t> screenIds = {
        0,
        1,
        100,
        1000,
        0xFFFFFFFFFFFFFFFF
    };

    for (size_t i = 0; i < screenIds.size(); i++) {
        RSDisplayNodeConfig config;
        auto displayNode = RSDisplayNode::Create(config);
        displayNode->SetScreenId(screenIds[i]);

        // Create a child node to visualize the display
        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 500 + 50),
             static_cast<float>((i / 2) * 300 + 50),
             300.0f, 300.0f});
        displayNode->AddChild(canvasNode);
        RegisterNode(displayNode);
        RegisterNode(canvasNode);
    }
}

/* SetScreenId: boundary values */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_SetScreenId_Boundary)
{
    std::vector<uint64_t> boundaryIds = {
        0,
        1,
        std::numeric_limits<uint64_t>::max(),
        std::numeric_limits<uint64_t>::max() - 1
    };

    for (size_t i = 0; i < boundaryIds.size(); i++) {
        RSDisplayNodeConfig config;
        auto displayNode = RSDisplayNode::Create(config);
        displayNode->SetScreenId(boundaryIds[i]);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 400 + 100),
             static_cast<float>((i / 2) * 300 + 100),
             200.0f, 200.0f});
        displayNode->AddChild(canvasNode);
        RegisterNode(displayNode);
        RegisterNode(canvasNode);
    }
}

/* SetForceCloseHdr: normal values - 1x2 matrix */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_SetForceCloseHdr_Matrix_1x2)
{
    std::vector<bool> forceCloseHdrList = {false, true};

    for (size_t i = 0; i < forceCloseHdrList.size(); i++) {
        RSDisplayNodeConfig config;
        auto displayNode = RSDisplayNode::Create(config);
        displayNode->SetForceCloseHdr(forceCloseHdrList[i]);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50.0f, 400.0f, 400.0f});
        displayNode->AddChild(canvasNode);
        RegisterNode(displayNode);
        RegisterNode(canvasNode);
    }
}

/* SetForceCloseHdr: combined with rotation - matrix 2x2 */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_ForceCloseHdr_Rotation_Matrix_2x2)
{
    std::vector<bool> forceCloseHdrList = {false, true};
    std::vector<uint32_t> rotations = {0, 90};

    for (size_t row = 0; row < forceCloseHdrList.size(); row++) {
        for (size_t col = 0; col < rotations.size(); col++) {
            RSDisplayNodeConfig config;
            auto displayNode = RSDisplayNode::Create(config);
            displayNode->SetForceCloseHdr(forceCloseHdrList[row]);
            displayNode->SetScreenRotation(rotations[col]);

            auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 100),
                 static_cast<float>(row * 300 + 100),
                 200.0f, 200.0f});
            displayNode->AddChild(canvasNode);
            RegisterNode(displayNode);
            RegisterNode(canvasNode);
        }
    }
}

/* SetVirtualScreenMuteStatus: normal values - 1x2 matrix */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_SetVirtualScreenMuteStatus_Matrix_1x2)
{
    std::vector<bool> muteStatusList = {false, true};

    for (size_t i = 0; i < muteStatusList.size(); i++) {
        RSDisplayNodeConfig config;
        auto displayNode = RSDisplayNode::Create(config);
        displayNode->SetVirtualScreenMuteStatus(muteStatusList[i]);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 500 + 50), 50.0f, 400.0f, 400.0f});
        displayNode->AddChild(canvasNode);
        RegisterNode(displayNode);
        RegisterNode(canvasNode);
    }
}

/* Combined: multiple display properties - matrix 2x2x2 */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_Combined_Properties_Matrix_2x2x2)
{
    std::vector<bool> boolValues = {false, true};
    std::vector<uint64_t> screenIds = {0, 100};

    for (size_t i = 0; i < boolValues.size(); i++) {
        for (size_t j = 0; j < boolValues.size(); j++) {
            for (size_t k = 0; k < screenIds.size(); k++) {
                RSDisplayNodeConfig config;
                auto displayNode = RSDisplayNode::Create(config);
                displayNode->SetScreenId(screenIds[k]);
                displayNode->SetForceCloseHdr(boolValues[i]);
                displayNode->SetVirtualScreenMuteStatus(boolValues[j]);

                auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                    {static_cast<float>(((j + k) % 2) * 300 + 100),
                     static_cast<float>((i + (k / 2)) * 200 + 100),
                     150.0f, 150.0f});
                displayNode->AddChild(canvasNode);
                RegisterNode(displayNode);
                RegisterNode(canvasNode);
            }
        }
    }
}

/* SetScreenId: multiple displays */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_Multiple_Screens)
{
    // Create multiple display nodes with different screen IDs
    for (size_t i = 0; i < 3; i++) {
        RSDisplayNodeConfig config;
        auto displayNode = RSDisplayNode::Create(config);
        displayNode->SetScreenId(i);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 350 + 50), 50.0f, 300.0f, 300.0f});
        displayNode->AddChild(canvasNode);
        RegisterNode(displayNode);
        RegisterNode(canvasNode);
    }
}

/* ClearModifierByPid: normal values */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_ClearModifierByPid)
{
    std::vector<pid_t> pids = {0, 1, 1000};

    for (size_t i = 0; i < pids.size(); i++) {
        RSDisplayNodeConfig config;
        auto displayNode = RSDisplayNode::Create(config);
        displayNode->ClearModifierByPid(pids[i]);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>(i * 400 + 50), 50.0f, 300.0f, 300.0f});
        displayNode->AddChild(canvasNode);
        RegisterNode(displayNode);
        RegisterNode(canvasNode);
    }
}

/* Rapid state changes */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_Rapid_State_Changes)
{
    RSDisplayNodeConfig config;
    auto displayNode = RSDisplayNode::Create(config);

    auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100.0f, 100.0f, 400.0f, 400.0f});
    displayNode->AddChild(canvasNode);
    RegisterNode(displayNode);
    RegisterNode(canvasNode);

    // Rapid state changes
    displayNode->SetForceCloseHdr(false);
    displayNode->SetVirtualScreenMuteStatus(false);

    displayNode->SetForceCloseHdr(true);
    displayNode->SetVirtualScreenMuteStatus(true);

    displayNode->SetForceCloseHdr(false);
    displayNode->SetVirtualScreenMuteStatus(false);
}

/* SetScreenId with rotation combinations */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_ScreenId_Rotation_Combinations)
{
    struct TestCase {
        uint64_t screenId;
        uint32_t rotation;
        const char* description;
    };

    std::vector<TestCase> testCases = {
        {0, 0, "Screen 0, no rotation"},
        {0, 90, "Screen 0, 90 degree rotation"},
        {1, 0, "Screen 1, no rotation"},
        {1, 180, "Screen 1, 180 degree rotation"},
        {2, 270, "Screen 2, 270 degree rotation"}
    };

    for (size_t i = 0; i < testCases.size(); i++) {
        RSDisplayNodeConfig config;
        auto displayNode = RSDisplayNode::Create(config);
        displayNode->SetScreenId(testCases[i].screenId);
        displayNode->SetScreenRotation(testCases[i].rotation);

        auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {static_cast<float>((i % 2) * 500 + 50),
             static_cast<float>((i / 2) * 300 + 700),
             200, 200});
        displayNode->AddChild(canvasNode);
        RegisterNode(displayNode);
        RegisterNode(canvasNode);
    }
}

/* SetVirtualScreenMuteStatus: boundary and normal combined */
GRAPHIC_TEST(DisplayNodeTest, CONTENT_DISPLAY_TEST, DisplayNode_VirtualScreenMute_Combined)
{
    std::vector<bool> muteStatus = {false, true};
    std::vector<bool> forceCloseHdr = {false, true};
    std::vector<uint64_t> screenIds = {0, 100};

    for (size_t i = 0; i < muteStatus.size(); i++) {
        for (size_t j = 0; j < forceCloseHdr.size(); j++) {
            RSDisplayNodeConfig config;
            auto displayNode = RSDisplayNode::Create(config);
            displayNode->SetScreenId(screenIds[i]);
            displayNode->SetVirtualScreenMuteStatus(muteStatus[i]);
            displayNode->SetForceCloseHdr(forceCloseHdr[j]);

            auto canvasNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(j * 300 + 100),
                 static_cast<float>(i * 300 + 100),
                 200, 200});
            displayNode->AddChild(canvasNode);
            RegisterNode(displayNode);
            RegisterNode(canvasNode);
        }
    }
}

} // namespace OHOS::Rosen
