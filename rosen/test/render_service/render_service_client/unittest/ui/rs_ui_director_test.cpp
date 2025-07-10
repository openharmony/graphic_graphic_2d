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
#include "surface.h"

#include "animation/rs_render_animation.h"
#include "modifier/rs_modifier_manager.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"
#include "ui/rs_ui_director.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

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
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIDirectorTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
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
    ASSERT_NE(director, nullptr);
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
    ASSERT_NE(director, nullptr);
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
    ASSERT_NE(director, nullptr);
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
    ASSERT_NE(director, nullptr);
    RSSurfaceNodeConfig c;
    auto surfaceNode = RSSurfaceNode::Create(c);
    director->SetRSSurfaceNode(surfaceNode);
    auto ret = director->GetRSSurfaceNode();
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: SetRSSurfaceNode002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetRSSurfaceNode002 , TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_NE(director, nullptr);
    director->SetRSSurfaceNode(nullptr);
}

/**
 * @tc.name: GetRSSurfaceNode001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, GetRSSurfaceNode001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_NE(director, nullptr);
    RSSurfaceNodeConfig c;
    auto surfaceNode = RSSurfaceNode::Create(c);
    director->SetRSSurfaceNode(surfaceNode);
    ASSERT_EQ(surfaceNode, director->GetRSSurfaceNode());
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

#ifdef RS_ENABLE_VK
/**
 * @tc.name: Init001
 * @tc.desc: Test Init
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSUIDirectorTest, Init001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    EXPECT_EQ(director->cacheDir_.empty(), true);
    director->Init(false);
    std::string cacheDir = "test";
    director->SetCacheDir(cacheDir);
    EXPECT_EQ(director->cacheDir_.empty(), false);
    director->Init(false);
}
#endif

/**
 * @tc.name: SetUITaskRunner001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetUITaskRunner001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_NE(director, nullptr);
    director->SetUITaskRunner([&](const std::function<void()>& task, uint32_t delay) {});
}

/**
 * @tc.name: SetUITaskRunner002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetUITaskRunner002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    director->isHgmConfigChangeCallbackReg_ = true;
    director->SetUITaskRunner([&](const std::function<void()>& task, uint32_t delay) {});
    ASSERT_TRUE(director != nullptr);

    director->isHgmConfigChangeCallbackReg_ = false;
    director->SetUITaskRunner([&](const std::function<void()>& task, uint32_t delay) {});
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
    ASSERT_NE(director, nullptr);
    director->SendMessages();
}

/**
 * @tc.name: DirectorSendMessages002
 * @tc.desc: test results of SendMessages
 * @tc.type: FUNC
 * @tc.require: issueICGEDM
 */
HWTEST_F(RSUIDirectorTest, DirectorSendMessages002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_NE(director, nullptr);
    bool result = false;
    FlushEmptyCallback callback = [&result](const uint64_t timestamp) -> bool {
        result = true;
        return true;
    };
    director->SetFlushEmptyCallback(callback);
    director->SendMessages();
    EXPECT_TRUE(result);
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
    ASSERT_NE(director, nullptr);
    RSNode::SharedPtr testNode = RSCanvasNode::Create();
    director->SetRoot(testNode->GetId());
    director->SetRoot(testNode->GetId());
}

/**
 * @tc.name: UIDirectorSetRSRootNode001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, UIDirectorSetRSRootNode001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_NE(director, nullptr);
    director->Init(true, true);
    RSNode::SharedPtr rootNode = RSRootNode::Create(false, false, director->GetRSUIContext());
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    ASSERT_EQ(rootNode, director->rootNode_.lock());
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
    ASSERT_NE(director, nullptr);

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
    ASSERT_NE(director, nullptr);
    std::string cacheDir = "/data/log";
    director->SetAbilityBGAlpha(0);
    director->SetContainerWindow(true, rrect);
    director->SetAppFreeze(true);
    RSSurfaceNodeConfig c;
    auto surfaceNode = RSSurfaceNode::Create(c);
    director->SetRSSurfaceNode(surfaceNode);
    director->SetAbilityBGAlpha(0);
    director->SetContainerWindow(true, rrect);
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
    ASSERT_NE(director, nullptr);
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
    ASSERT_NE(director, nullptr);
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

/**
 * @tc.name: GoGround
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, GoGround, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    director->GoForeground();
    director->GoBackground();
    Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "WindowScene_";
    std::shared_ptr<RSSurfaceNode> surfaceNode = std::make_shared<RSSurfaceNode>(config, true);
    auto node = std::make_shared<RSRootNode>(false);
    node->AttachRSSurfaceNode(surfaceNode);
    director->SetRSSurfaceNode(surfaceNode);
    director->SetRoot(node->GetId());
    RSRootNode::SharedPtr nodePtr = std::make_shared<RSRootNode>(node->GetId());
    nodePtr->SetId(node->GetId());
    director->GoForeground();
    director->GoBackground();
    bool res = RSNodeMap::MutableInstance().RegisterNode(nodePtr);
    director->GoForeground();
    director->GoBackground();
    bool flag = false;
    if (director->isUniRenderEnabled_) {
        flag = true;
        director->isUniRenderEnabled_ = false;
    }
    director->GoForeground();
    director->GoForeground();
    director->GoBackground();
    director->GoBackground();
    director->isUniRenderEnabled_ = flag;
    ASSERT_TRUE(res);
}

/**
 * @tc.name: AttachSurface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, AttachSurface, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    director->AttachSurface();
}

/**
 * @tc.name: RecvMessages
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, RecvMessages, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    director->RecvMessages();
    RSUIDirector::RecvMessages(nullptr);
    std::shared_ptr<RSTransactionData> transactionData = std::make_shared<RSTransactionData>();
    RSUIDirector::RecvMessages(transactionData);
}

/**
 * @tc.name: ProcessMessagesTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, ProcessMessagesTest001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    std::shared_ptr<RSTransactionData> cmds = std::make_shared<RSTransactionData>();
    director->ProcessMessages(cmds);
}

/**
 * @tc.name: ProcessMessagesTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, ProcessMessagesTest002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_NE(director, nullptr);
    director->Init(true, true);
    director->requestVsyncCallback_ = nullptr;
    std::shared_ptr<RSTransactionData> cmds = std::make_shared<RSTransactionData>();
    auto uiContext = director->GetRSUIContext();
    ASSERT_NE(uiContext, nullptr);
    uiContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    uint64_t token = uiContext->GetToken();
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(token, token, token, FINISHED);
    cmds->AddCommand(command, token, FollowType::FOLLOW_TO_SELF);
    ASSERT_EQ(director->requestVsyncCallback_, nullptr);
    director->ProcessMessages(cmds, true);
    ASSERT_NE(RSUIContextManager::Instance().GetRSUIContext(token), nullptr);
    ASSERT_NE(RSUIContextManager::Instance().GetRSUIContext(token)->GetRSTransaction(), nullptr);
}

/**
 * @tc.name: ProcessMessagesTest003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, ProcessMessagesTest003, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_NE(director, nullptr);
    director->Init(true, true);
    director->requestVsyncCallback_ = nullptr;
    std::shared_ptr<RSTransactionData> cmds = std::make_shared<RSTransactionData>();
    auto uiContext = director->GetRSUIContext();
    ASSERT_NE(uiContext, nullptr);
    uiContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    uint64_t token = uiContext->GetToken();
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(token, token, token, FINISHED);
    cmds->AddCommand(command, token, FollowType::FOLLOW_TO_SELF);
    ASSERT_EQ(director->requestVsyncCallback_, nullptr);
    uiContext->rsTransactionHandler_ = nullptr;
    director->ProcessMessages(cmds, true);
    ASSERT_NE(RSUIContextManager::Instance().GetRSUIContext(token), nullptr);
    ASSERT_EQ(RSUIContextManager::Instance().GetRSUIContext(token)->GetRSTransaction(), nullptr);
}

/**
 * @tc.name: AnimationCallbackProcessor
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, AnimationCallbackProcessor, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    // for test
    NodeId nodeId = 0;
    AnimationId animId = 0;
    uint64_t token = 0;
    AnimationCallbackEvent event = REPEAT_FINISHED;
    director->AnimationCallbackProcessor(nodeId, animId, token, event);
}

/**
 * @tc.name: PostTask
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, PostTask, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    const std::function<void()>& task = []() {
        std::cout << "for test" << std::endl;
    };
    director->PostTask(task);
}

/**
 * @tc.name: PostDelayTask001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, PostDelayTask001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    const std::function<void()>& task = []() {
        std::cout << "for test" << std::endl;
    };
    director->PostDelayTask(task, 0, 0);
    director->PostDelayTask(task, 0, -1);
    director->PostDelayTask(task, 0, 1);
    ASSERT_TRUE(director != nullptr);
}

/**
 * @tc.name: SetRTRenderForcedTest002
 * @tc.desc: SetRTRenderForced Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSUIDirectorTest, SetRTRenderForcedTest002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    director->SetRTRenderForced(true);
}

/**
 * @tc.name: SetRequestVsyncCallbackTest003
 * @tc.desc: SetRequestVsyncCallback Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSUIDirectorTest, SetRequestVsyncCallbackTest003, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    std::function<void()> callback = nullptr;
    director->SetRequestVsyncCallback(callback);
    EXPECT_TRUE(nullptr == director->requestVsyncCallback_);
}

/**
 * @tc.name: DumpNodeTreeProcessor001
 * @tc.desc: DumpNodeTreeProcessor Test
 * @tc.type: FUNC
 * @tc.require: issueIAKME2
 */
HWTEST_F(RSUIDirectorTest, DumpNodeTreeProcessor001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    RSNode::SharedPtr rsNode = RSCanvasNode::Create();
    director->DumpNodeTreeProcessor(rsNode->GetId(), 0, 0);
    const NodeId invalidId = 1;
    director->DumpNodeTreeProcessor(invalidId, 0, 0);
    SUCCEED();
}

/**
 * @tc.name: GetIndexTest001
 * @tc.desc: GetIndex Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSUIDirectorTest, GetIndexTest001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    director->index_ = g_ExtremeInt_1;
    ASSERT_EQ(director->GetIndex(), g_ExtremeInt_1);
}

/**
 * @tc.name: HasFirstFrameAnimationTest
 * @tc.desc: test HasFirstFrameAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, HasFirstFrameAnimationTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    bool res = director->HasFirstFrameAnimation();
    ASSERT_FALSE(res);
}

/**
 * @tc.name: ReportUiSkipEvent
 * @tc.desc: test ReportUiSkipEvent
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, ReportUiSkipEventTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    EXPECT_NE(RSTransactionProxy::GetInstance(), nullptr);
    director->lastUiSkipTimestamp_ = 0;
    director->ReportUiSkipEvent("test");
    director->lastUiSkipTimestamp_ = INT64_MAX;
    director->ReportUiSkipEvent("test");

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    EXPECT_TRUE(RSTransactionProxy::instance_ == nullptr);
    EXPECT_EQ(RSTransactionProxy::GetInstance(), nullptr);
    director->lastUiSkipTimestamp_ = 0;
    director->ReportUiSkipEvent("test");
    director->lastUiSkipTimestamp_ = INT64_MAX;
    director->ReportUiSkipEvent("test");
    EXPECT_TRUE(RSTransactionProxy::GetInstance() == nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: IsHybridRenderEnabled001
 * @tc.desc: IsHybridRenderEnabled Test with default param
 * @tc.type: FUNC
 * @tc.require: issueICJVZA
 */
HWTEST_F(RSUIDirectorTest, IsHybridRenderEnabled001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    bool systemPropertiesRet = RSSystemProperties::GetHybridRenderEnabled();
    bool directorRet = director->IsHybridRenderEnabled();
    EXPECT_EQ(systemPropertiesRet, directorRet);
}

/**
 * @tc.name: GetHybridRenderSwitch001
 * @tc.desc: GetHybridRenderSwitch Test with normal param ComponentEnableSwitch::TEXTBLOB
 * @tc.type: FUNC
 * @tc.require: issueICJVZA
 */
HWTEST_F(RSUIDirectorTest, GetHybridRenderSwitch001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    ComponentEnableSwitch bitSeq = ComponentEnableSwitch::TEXTBLOB;
    bool systemPropertiesRet = RSSystemProperties::GetHybridRenderSwitch(bitSeq);
    bool directorRet = director->GetHybridRenderSwitch(bitSeq);
    EXPECT_EQ(systemPropertiesRet, directorRet);
}

/**
 * @tc.name: GetHybridRenderSwitch002
 * @tc.desc: GetHybridRenderSwitch Test with abnormal param ComponentEnableSwitch::MAX_VALUE
 * @tc.type: FUNC
 * @tc.require: issueICJVZA
 */
HWTEST_F(RSUIDirectorTest, GetHybridRenderSwitch002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    ComponentEnableSwitch bitSeq = ComponentEnableSwitch::MAX_VALUE;
    bool systemPropertiesRet = RSSystemProperties::GetHybridRenderSwitch(bitSeq);
    bool directorRet = director->GetHybridRenderSwitch(bitSeq);
    EXPECT_EQ(systemPropertiesRet, directorRet);
}

/**
 * @tc.name: GetHybridRenderSwitch003
 * @tc.desc: GetHybridRenderSwitch Test with abnormal param -1
 * @tc.type: FUNC
 * @tc.require: issueICJVZA
 */
HWTEST_F(RSUIDirectorTest, GetHybridRenderSwitch003, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    ComponentEnableSwitch bitSeq = static_cast<ComponentEnableSwitch>(-1);
    bool systemPropertiesRet = RSSystemProperties::GetHybridRenderSwitch(bitSeq);
    bool directorRet = director->GetHybridRenderSwitch(bitSeq);
    EXPECT_EQ(systemPropertiesRet, directorRet);
}

/**
 * @tc.name: GetHybridRenderSwitch004
 * @tc.desc: GetHybridRenderSwitch Test with abnormal param ComponentEnableSwitch::MAX_VALUE + 1
 * @tc.type: FUNC
 * @tc.require: issueICJVZA
 */
HWTEST_F(RSUIDirectorTest, GetHybridRenderSwitch004, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    ComponentEnableSwitch bitSeq = static_cast<ComponentEnableSwitch>(
        static_cast<uint8_t>(ComponentEnableSwitch::MAX_VALUE) + 1);
    bool systemPropertiesRet = RSSystemProperties::GetHybridRenderSwitch(bitSeq);
    bool directorRet = director->GetHybridRenderSwitch(bitSeq);
    EXPECT_EQ(systemPropertiesRet, directorRet);
}

/**
 * @tc.name: GetHybridRenderTextBlobLenCount001
 * @tc.desc: GetHybridRenderTextBlobLenCount Test with default param
 * @tc.type: FUNC
 * @tc.require: issueICJVZA
 */
HWTEST_F(RSUIDirectorTest, GetHybridRenderTextBlobLenCount001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    uint32_t systemPropertiesRet = RSSystemProperties::GetHybridRenderTextBlobLenCount();
    uint32_t directorRet = director->GetHybridRenderTextBlobLenCount();
    EXPECT_EQ(systemPropertiesRet, directorRet);
}

/**
 * @tc.name: StartTextureExportTest001
 * @tc.desc: StartTextureExport Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSUIDirectorTest, StartTextureExportTest001, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN) {
        std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
        ASSERT_TRUE(director != nullptr);
        director->isUniRenderEnabled_ = true;
        RSRenderThread::Instance().thread_ = std::make_unique<std::thread>([]{});
        director->StartTextureExport();
        EXPECT_NE(RSTransactionProxy::GetInstance(), nullptr);
    }
}

/**
 * @tc.name: SetTypicalResidentProcessTest001
 * @tc.desc: SetTypicalResidentProcess Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSUIDirectorTest, SetTypicalResidentProcessTest001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create();
    ASSERT_TRUE(director != nullptr);
    bool enabled = RSSystemProperties::GetTypicalResidentProcess();
    director->SetTypicalResidentProcess(!enabled);
    EXPECT_EQ(RSSystemProperties::GetTypicalResidentProcess(), !enabled);
    director->SetTypicalResidentProcess(enabled);
    // isTypicalResidentProcess_ will only be set once
    EXPECT_EQ(RSSystemProperties::GetTypicalResidentProcess(), !enabled);
    // recover isTypicalResidentProcess_
    RSSystemProperties::SetTypicalResidentProcess(enabled);
    EXPECT_EQ(RSSystemProperties::GetTypicalResidentProcess(), enabled);
}
} // namespace OHOS::Rosen
