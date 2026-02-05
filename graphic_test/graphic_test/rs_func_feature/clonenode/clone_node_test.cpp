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
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class CloneNodeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
    Drawing::SamplingOptions sampling;
    Drawing::Rect frameRect;
    std::shared_ptr<Media::PixelMap> smpixelmap =
        DecodePixelMap("/data/local/tmp/fg_test.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
    RSSurfaceNode::SharedPtr CreateClonedNodeWithImageCanvas(const Vector4f& bounds, uint32_t colorValue)
    {
        RSSurfaceNodeConfig clonedSurfaceNodeConfig;
        clonedSurfaceNodeConfig.isSync = true;
        clonedSurfaceNodeConfig.SurfaceNodeName = "clonedSurfaceNode";
        auto clonedSurfaceNode = RSSurfaceNode::Create(clonedSurfaceNodeConfig, RSSurfaceNodeType::APP_WINDOW_NODE);
        if (!clonedSurfaceNode) {
            LOGE("CreateClonedNodeWithImageCanvas failed");
            return nullptr;
        }
        clonedSurfaceNode->SetBounds(bounds);
        clonedSurfaceNode->SetFrame(bounds);
        clonedSurfaceNode->SetBackgroundColor(colorValue);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({0, 0, bounds[2], bounds[3]});
        canvasNode->SetFrame({0, 0, bounds[2], bounds[3]});
        RegisterNode(canvasNode);
        clonedSurfaceNode->AddChild(canvasNode, 0);
        auto drawing = canvasNode->BeginRecording(bounds[2], bounds[3]);
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::SCALE_DOWN));
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);
        drawing->DrawPixelMapWithParm(smpixelmap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        return clonedSurfaceNode;
    }

    RSSurfaceNode::SharedPtr CreateSurfaceNode(const Vector4f& bounds, std::string& name)
    {
        RSSurfaceNodeConfig cloneSurfaceNodeConfig;
        cloneSurfaceNodeConfig.isSync = true;
        cloneSurfaceNodeConfig.SurfaceNodeName = name;
        auto cloneSurfaceNode = RSSurfaceNode::Create(cloneSurfaceNodeConfig, RSSurfaceNodeType::APP_WINDOW_NODE);
        if (!cloneSurfaceNode) {
            LOGE("CreateSurfaceNode failed");
            return nullptr;
        }
        cloneSurfaceNode->SetBounds(bounds);
        cloneSurfaceNode->SetFrame(bounds);
        return cloneSurfaceNode;
    }
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

GRAPHIC_TEST(CloneNodeTest, CONTENT_DISPLAY_TEST, CLONE_NODE_ONE2ONE_TEST)
{
    auto clonedSurfaceNode = CreateClonedNodeWithImageCanvas({0, 0, 500, 500}, 0xFFFF0000);
    if (!clonedSurfaceNode) {
        LOGE("Create CreateClonedNodeWithImageCanvas failed");
        return;
    }
    RegisterNode(clonedSurfaceNode);
    GetRootNode()->AddChild(clonedSurfaceNode);
    std::string containerSurfaceNodeName = "containerSurfaceNode";
    auto containerSurfaceNode = CreateSurfaceNode({500, 0, 500, 500}, containerSurfaceNodeName);
    if (!containerSurfaceNode) {
        LOGE("Create containerSurfaceNode failed");
        return;
    }
    RegisterNode(containerSurfaceNode);
    GetRootNode()->AddChild(containerSurfaceNode);

    std::string cloneSurfaceNodeName = "cloneSurfaceNode";
    auto cloneSurfaceNode = CreateSurfaceNode({0, 0, 500, 500}, cloneSurfaceNodeName);
    if (!cloneSurfaceNode) {
        LOGE("Create cloneSurfaceNode failed");
        return;
    }
    cloneSurfaceNode->SetClonedNodeInfo(clonedSurfaceNode->GetId(), true, true);
    RegisterNode(cloneSurfaceNode);
    containerSurfaceNode->AddChild(cloneSurfaceNode, 0);
}

GRAPHIC_TEST(CloneNodeTest, CONTENT_DISPLAY_TEST, CLONE_NODE_ONE2ONE_SCALE_TEST)
{
    auto clonedSurfaceNode = CreateClonedNodeWithImageCanvas({0, 0, 500, 500}, 0xFFFF0000);
    if (!clonedSurfaceNode) {
        LOGE("Create CreateClonedNodeWithImageCanvas failed");
        return;
    }
    RegisterNode(clonedSurfaceNode);
    GetRootNode()->AddChild(clonedSurfaceNode);
    std::string containerSurfaceNodeName = "containerSurfaceNode";
    auto containerSurfaceNode = CreateSurfaceNode({500, 0, 500, 500}, containerSurfaceNodeName);
    if (!containerSurfaceNode) {
        LOGE("Create containerSurfaceNode failed");
        return;
    }
    RegisterNode(containerSurfaceNode);
    GetRootNode()->AddChild(containerSurfaceNode);

    std::string cloneSurfaceNodeName = "cloneSurfaceNode";
    auto cloneSurfaceNode = CreateSurfaceNode({0, 0, 500, 500}, cloneSurfaceNodeName);
    if (!cloneSurfaceNode) {
        LOGE("Create cloneSurfaceNode failed");
        return;
    }
    cloneSurfaceNode->SetClonedNodeInfo(clonedSurfaceNode->GetId(), true, true);
    cloneSurfaceNode->SetScale(.5);
    RegisterNode(cloneSurfaceNode);
    containerSurfaceNode->AddChild(cloneSurfaceNode, 0);
}

GRAPHIC_TEST(CloneNodeTest, CONTENT_DISPLAY_TEST, CLONE_NODE_ONE2ONE_ALPHA_TEST)
{
    auto clonedSurfaceNode = CreateClonedNodeWithImageCanvas({0, 0, 500, 500}, 0xFFFF0000);
    if (!clonedSurfaceNode) {
        LOGE("Create CreateClonedNodeWithImageCanvas failed");
        return;
    }
    RegisterNode(clonedSurfaceNode);
    GetRootNode()->AddChild(clonedSurfaceNode);
    std::string containerSurfaceNodeName = "containerSurfaceNode";
    auto containerSurfaceNode = CreateSurfaceNode({500, 0, 500, 500}, containerSurfaceNodeName);
    if (!containerSurfaceNode) {
        LOGE("Create containerSurfaceNode failed");
        return;
    }
    RegisterNode(containerSurfaceNode);
    GetRootNode()->AddChild(containerSurfaceNode);

    std::string cloneSurfaceNodeName = "cloneSurfaceNode";
    auto cloneSurfaceNode = CreateSurfaceNode({0, 0, 500, 500}, cloneSurfaceNodeName);
    if (!cloneSurfaceNode) {
        LOGE("Create cloneSurfaceNode failed");
        return;
    }
    cloneSurfaceNode->SetClonedNodeInfo(clonedSurfaceNode->GetId(), true, true);
    cloneSurfaceNode->SetAlpha(.5);
    RegisterNode(cloneSurfaceNode);
    containerSurfaceNode->AddChild(cloneSurfaceNode, 0);
}

GRAPHIC_TEST(CloneNodeTest, CONTENT_DISPLAY_TEST, CLONE_NODE_MULTIPLE_ONE2ONE_TEST)
{
    int width = 500;
    int height = 500;
    int gap = 100;
    std::vector<uint32_t> backgroundColor =
        {0xFF000000,
        0xFFFF0000,
        0xFFFFFF00,
        0xFFFFFFFF};
    std::string cloneSurfaceNodeName = "cloneSurfaceNode";
    std::string containerSurfaceNodeName = "containerSurfaceNode";

    for (int i = 0; i < 3; i++) {
        auto clonedSurfaceNode = CreateClonedNodeWithImageCanvas({0, (width + gap) * i, width, height},
            backgroundColor[i]);
        if (!clonedSurfaceNode) {
            LOGE("Create CreateClonedNodeWithImageCanvas failed");
            return;
        }
        RegisterNode(clonedSurfaceNode);
        GetRootNode()->AddChild(clonedSurfaceNode);
        
        auto containerSurfaceNode = CreateSurfaceNode({width + gap, (height + gap) * i, width, height},
            containerSurfaceNodeName);
        if (!containerSurfaceNode) {
            LOGE("Create containerSurfaceNode failed");
            return;
        }
        RegisterNode(containerSurfaceNode);
        GetRootNode()->AddChild(containerSurfaceNode);

        auto cloneSurfaceNode = CreateSurfaceNode({0, 0, width, height + (height + gap) * i},
            cloneSurfaceNodeName);
        if (!cloneSurfaceNode) {
            LOGE("Create cloneSurfaceNode failed");
            return;
        }
        cloneSurfaceNode->SetClonedNodeInfo(clonedSurfaceNode->GetId(), true, true);
        RegisterNode(cloneSurfaceNode);
        containerSurfaceNode->AddChild(cloneSurfaceNode, 0);
    }
}

GRAPHIC_TEST(CloneNodeTest, CONTENT_DISPLAY_TEST, CLONE_NODE_ONE2N_TEST)
{
    int width = 500;
    int height = 500;
    int gap = 100;
    std::string cloneSurfaceNodeName = "cloneSurfaceNode";
    std::string containerSurfaceNodeName = "containerSurfaceNode";
    auto clonedSurfaceNode = CreateClonedNodeWithImageCanvas({0, 0, width, height}, 0xFF000000);
    if (!clonedSurfaceNode) {
        LOGE("Create CreateClonedNodeWithImageCanvas failed");
        return;
    }
    RegisterNode(clonedSurfaceNode);
    GetRootNode()->AddChild(clonedSurfaceNode);

    for (int i = 0; i < 3; i++) {
        auto containerSurfaceNode = CreateSurfaceNode({width + gap, (height + gap) * i, width, height},
            containerSurfaceNodeName);
        if (!containerSurfaceNode) {
            LOGE("Create containerSurfaceNode failed");
            return;
        }
        RegisterNode(containerSurfaceNode);
        GetRootNode()->AddChild(containerSurfaceNode);

        auto cloneSurfaceNode = CreateSurfaceNode({0, 0, width, height}, cloneSurfaceNodeName);
        if (!cloneSurfaceNode) {
            LOGE("Create cloneSurfaceNode failed");
            return;
        }
        cloneSurfaceNode->SetScale(0.5 * (i + 1));
        cloneSurfaceNode->SetClonedNodeInfo(clonedSurfaceNode->GetId(), true, true);
        RegisterNode(cloneSurfaceNode);
        containerSurfaceNode->AddChild(cloneSurfaceNode, 0);
    }
}

GRAPHIC_TEST(CloneNodeTest, CONTENT_DISPLAY_TEST, CLONE_NODE_MULTIPLE_ONE2N_TEST)
{
    int width = 200;
    int height = 200;
    int gap = 50;
    int lineGap = 800;
    int rowGap = 500;
    std::vector<uint32_t> backgroundColor =
        {0xFF000000,
        0xFFFF0000,
        0xFFFFFF00,
        0xFFFFFFFF};

    std::string cloneSurfaceNodeName = "cloneSurfaceNode";
    std::string containerSurfaceNodeName = "containerSurfaceNode";
    int backgroundCount = 0;
    for (int y = 0; y < 2; y++) {
        for (int i = 0; i < 2; i++) {
            auto clonedSurfaceNode = CreateClonedNodeWithImageCanvas({rowGap * y, lineGap * i, width, height},
                backgroundColor[backgroundCount++]);
            if (!clonedSurfaceNode) {
                LOGE("Create CreateClonedNodeWithImageCanvas failed");
                return;
            }
            RegisterNode(clonedSurfaceNode);
            GetRootNode()->AddChild(clonedSurfaceNode);
            for (int j = 0; j < 3; j++) {
                auto containerSurfaceNode = CreateSurfaceNode(
                    {width + gap + rowGap * y, (height + gap) * j + lineGap * i, width, height},
                    containerSurfaceNodeName);
                if (!containerSurfaceNode) {
                    LOGE("Create containerSurfaceNode failed");
                    return;
                }
                RegisterNode(containerSurfaceNode);
                GetRootNode()->AddChild(containerSurfaceNode);

                auto cloneSurfaceNode = CreateSurfaceNode({0, 0, width, height}, cloneSurfaceNodeName);
                if (!cloneSurfaceNode) {
                    LOGE("Create cloneSurfaceNode failed");
                    return;
                }
                cloneSurfaceNode->SetScale(0.5 * (j + 1));
                cloneSurfaceNode->SetClonedNodeInfo(clonedSurfaceNode->GetId(), false, true);
                RegisterNode(cloneSurfaceNode);
                containerSurfaceNode->AddChild(cloneSurfaceNode, 0);
            }
        }
    }
}

GRAPHIC_TEST(CloneNodeTest, CONTENT_DISPLAY_TEST, CLONE_NODE_MULTIPLE_ONE2N_TEST2)
{
    int width = 200;
    int height = 200;
    int gap = 50;
    int lineGap = 800;
    int rowGap = 500;
    std::vector<uint32_t> backgroundColor =
        {0xFF000000,
        0xFFFF0000,
        0xFFFFFF00,
        0xFFFFFFFF};

    std::string cloneSurfaceNodeName = "cloneSurfaceNode";
    std::string containerSurfaceNodeName = "containerSurfaceNode";
    int backgroundCount = 0;
    for (int y = 0; y < 2; y++) {
        for (int i = 0; i < 2; i++) {
            auto clonedSurfaceNode = CreateClonedNodeWithImageCanvas({rowGap * y, lineGap * i, width, height},
                backgroundColor[backgroundCount++]);
            if (!clonedSurfaceNode) {
                LOGE("Create CreateClonedNodeWithImageCanvas failed");
                return;
            }
            for (int j = 0; j < 3; j++) {
                auto containerSurfaceNode = CreateSurfaceNode(
                    {width + gap + rowGap * y, (height + gap) * j + lineGap * i, width, height},
                    containerSurfaceNodeName);
                if (!containerSurfaceNode) {
                    LOGE("Create containerSurfaceNode failed");
                    return;
                }
                RegisterNode(containerSurfaceNode);
                GetRootNode()->AddChild(containerSurfaceNode);

                
                auto cloneSurfaceNode = CreateSurfaceNode({0, 0, width, height}, cloneSurfaceNodeName);
                if (!cloneSurfaceNode) {
                    LOGE("Create cloneSurfaceNode failed");
                    return;
                }
                cloneSurfaceNode->SetAlpha(0.2 * (j + 1));
                cloneSurfaceNode->SetClonedNodeInfo(clonedSurfaceNode->GetId(), false, true);
                RegisterNode(cloneSurfaceNode);
                containerSurfaceNode->AddChild(cloneSurfaceNode, 0);
            }
            RegisterNode(clonedSurfaceNode);
            GetRootNode()->AddChild(clonedSurfaceNode);
        }
    }
}
}