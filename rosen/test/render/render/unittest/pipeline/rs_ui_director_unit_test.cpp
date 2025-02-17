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

#include <string>
#include "gtest/gtest.h"
#include "animation/rs_render_animation.h"
#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_node_map.h"
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
    static constexpr int gNormalInt1 = 123;
    static constexpr int gNormalInt2 = 34342;
    static constexpr int gNormalInt3 = 3245;
    static constexpr int gExtremeInt1 = 1;
    static constexpr int gExtremeInt2 = -1;
    static constexpr int gExtremeInt3 = 0;

    static constexpr uint64_t NOMAL_U_INT64_1 = 123;
    static constexpr uint64_t NOMAL_U_INT64_2 = 34342;
    static constexpr uint64_t NOMAL_U_INT64_3 = 3245;
    static constexpr uint64_t vsyncPeriod = 11718750;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIDirectorTest::SetUpTestCase() {}
void RSUIDirectorTest::TearDownTestCase()
{
    RSRenderThread::Instance().renderContext_ = nullptr;
}
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
    ASSERT_TRUE(director != nullptr);
    director->SetTimeStamp(NOMAL_U_INT64_1, "test");
}

/**
 * @tc.name: SetTimeStamp002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetTimeStamp002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
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
    ASSERT_TRUE(director != nullptr);
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
    ASSERT_TRUE(director != nullptr);
    RSSurfaceNodeConfig c;
    auto surfaceNode = RSSurfaceNode::Create(c);
    director->SetRSSurfaceNode(surfaceNode);
}

/**
 * @tc.name: SetRSSurfaceNode002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetRSSurfaceNode002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
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
    director->Init();
    ASSERT_TRUE(director->cacheDir_.empty());
    director->Init(true);
    std::string cacheDir = "test";
    director->SetCacheDir(cacheDir);
    ASSERT_TRUE(!director->cacheDir_.empty());
    director->Init(false);
}

/**
 * @tc.name: SetUITaskRunner001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetUITaskRunner001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    director->SetUITaskRunner([&](const std::function<void()>& task, uint32_t delay) {});
}

/**
 * @tc.name: StartTextureExport001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, StartTextureExport001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN) {
        director->StartTextureExport();
    }
    ASSERT_TRUE(director != nullptr);
}

/**
 * @tc.name: DirectorSendMessages001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, DirectorSendMessages001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
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
    ASSERT_TRUE(director != nullptr);
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
    ASSERT_TRUE(director != nullptr);

    director->SetRoot(rootNode->GetId());

    director->SetTimeStamp(345, "test");
    director->SetRSSurfaceNode(nullptr);
    RSSurfaceNodeConfig c;
    auto surfaceNode = RSSurfaceNode::Create(c);
    director->SetRSSurfaceNode(surfaceNode);

    director->SetUITaskRunner([&](const std::function<void()>& task, uint32_t delay) {});
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
    ASSERT_TRUE(director != nullptr);
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
    ASSERT_TRUE(director != nullptr);
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
    ASSERT_TRUE(director != nullptr);
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
    bool hasRunningAnimation = director->FlushAnimation(NOMAL_U_INT64_2, vsyncPeriod);
    director->PostFrameRateTask([](){return;});
    ASSERT_EQ(hasRunningAnimation, false);
}

/**
 * @tc.name: GetCurrentRefreshRateMode
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, GetCurrentRefreshRateMode, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    int32_t res = director->GetCurrentRefreshRateMode();
    ASSERT_TRUE(res == -1);
}

/**
 * @tc.name: PostFrameRateTask
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, PostFrameRateTask, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    const std::function<void()>& task = []() {
        std::cout << "for test" << std::endl;
    };
    director->PostFrameRateTask(task);
}

/**
 * @tc.name: SetRequestVsyncCallback
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetRequestVsyncCallback, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    const std::function<void()>& callback = []() {
        std::cout << "for test" << std::endl;
    };
    director->SetRequestVsyncCallback(callback);
}

/**
 * @tc.name: FlushAnimationStartTime
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, FlushAnimationStartTime, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    uint64_t timeStamp = 0;
    director->FlushAnimationStartTime(timeStamp);
}

/**
 * @tc.name: HasUIRunningAnimation
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, HasUIRunningAnimation, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    bool res = director->HasUIRunningAnimation();
    ASSERT_TRUE(res == false);
}

/**
 * @tc.name: SetCacheDir
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetCacheDir, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    // for test
    const std::string& cacheFilePath = "1";
    director->SetCacheDir(cacheFilePath);
}

/**
 * @tc.name: SetRTRenderForced
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetRTRenderForced, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    // for test
    bool isRenderForced = true;
    director->SetRTRenderForced(isRenderForced);
}
} // namespace OHOS::Rosen
