/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <string>
#include "gtest/gtest.h"
#include "animation/rs_render_animation.h"
#include "pipeline/rs_render_result.h"
#include "modifier/rs_modifier_manager.h"
#include "surface.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIDirectorTest : public testing::Test {
public:
    static constexpr int g_normalInt_1 = 123;
    static constexpr int g_normalInt_2 = 34342;
    static constexpr int g_normalInt_3 = 3245;
    static constexpr int g_ExtremeInt_1 = 1;
    static constexpr int g_ExtremeInt_2 = -1;
    static constexpr int g_ExtremeInt_3 = 0;

    static constexpr uint64_t g_normalUInt64_1 = 123;
    static constexpr uint64_t g_normalUInt64_2 = 34342;
    static constexpr uint64_t g_normalUInt64_3 = 3245;
    static constexpr uint64_t g_vsyncPeriod = 11718750;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIDirectorTest::SetUpTestCase() {}
void RSUIDirectorTest::TearDownTestCase() {}
void RSUIDirectorTest::SetUp() {}
void RSUIDirectorTest::TearDown() {}

/**
 * @tc.name: SetTimeStamp001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetTimeStamp001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetTimeStamp(g_normalUInt64_1, "test");
}

/**
 * @tc.name: SetTimeStamp002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetTimeStamp002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetTimeStamp(-std::numeric_limits<uint64_t>::max(), "test");
}

/**
 * @tc.name: SetTimeStamp003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetTimeStamp003, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetTimeStamp(std::numeric_limits<int64_t>::min(), "test");
}

/**
 * @tc.name: SetRSSurfaceNode001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetRSSurfaceNode001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    RSSurfaceNodeConfig c;
    auto surfaceNode = RSSurfaceNode::Create(c);
    director->SetRSSurfaceNode(surfaceNode);
}

/**
 * @tc.name: SetRSSurfaceNode002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetRSSurfaceNode002 , TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetRSSurfaceNode(nullptr);
}

/**
 * @tc.name: PlatformInit001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, PlatformInit001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
}

/**
 * @tc.name: SetUITaskRunner001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetUITaskRunner001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SetUITaskRunner([&](const auto& uiTaskRunner) {});
}

/**
 * @tc.name: DirectorSendMessages001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, DirectorSendMessages001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->SendMessages();
}

/**
 * @tc.name: UIDirectorSetRoot001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, UIDirectorSetRoot001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    RSNode::SharedPtr testNode = RSCanvasNode::Create();
    director->SetRoot(testNode->GetId());
    director->SetRoot(testNode->GetId());
}

/**
 * @tc.name: UIDirectorTotal001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, UIDirectorTotal001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    RSNode::SharedPtr rootNode = RSRootNode::Create();
    RSNode::SharedPtr child1 = RSCanvasNode::Create();
    RSNode::SharedPtr child2 = RSCanvasNode::Create();
    RSNode::SharedPtr child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);

    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();

    director->SetRoot(rootNode->GetId());

    director->SetTimeStamp(345, "test");
    director->SetRSSurfaceNode(nullptr);
    RSSurfaceNodeConfig c;
    auto surfaceNode = RSSurfaceNode::Create(c);
    director->SetRSSurfaceNode(surfaceNode);

    director->SetUITaskRunner([&](const auto& uiTaskRunner) {});
    director->SendMessages();
}

/**
 * @tc.name: SetProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetProperty001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    std::string cacheDir = "/data/log";
    director->SetAbilityBGAlpha(0);
    director->SetContainerWindow(true, 1.f);
    director->SetAppFreeze(true);
    RSSurfaceNodeConfig c;
    auto surfaceNode = RSSurfaceNode::Create(c);
    director->SetRSSurfaceNode(surfaceNode);
    director->SetAbilityBGAlpha(0);
    director->SetContainerWindow(true, 1.f);
    director->SetAppFreeze(true);
    director->FlushAnimation(10);
    director->FlushModifier();
    director->SetCacheDir(cacheDir);
}

/**
 * @tc.name: DestroyTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, DestroyTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    NodeId nodeId = 10;
    director->SetRoot(nodeId);
    director->Destroy();
}

/**
 * @tc.name: SetRootTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetRootTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    NodeId nodeId = 10;
    director->SetRoot(nodeId);
    director->SetRoot(nodeId);
}

/**
 * @tc.name: setflushEmptyCallback
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, setflushEmptyCallbackTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    director->SetFlushEmptyCallback(nullptr);
}

/**
 * @tc.name: GetAnimateExpectedRate
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, GetAnimateExpectedRate, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    ASSERT_EQ(director->GetAnimateExpectedRate(), 0);
}


/**
 * @tc.name: FlushAnimation
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, FlushAnimation, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    bool hasRunningAnimation = director->FlushAnimation(g_normalUInt64_2, g_vsyncPeriod);
    director->PostFrameRateTask([](){return;});
    ASSERT_EQ(hasRunningAnimation, false);
}
} // namespace OHOS::Rosen
