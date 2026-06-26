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
#include "transaction/rs_interfaces.h"
#include "pipeline/rs_node_map.h"
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

    std::shared_ptr<RSUIDirector> CreateRSUIDirector();
    static constexpr int g_normalInt_1 = 123;
    static constexpr int g_normalInt_2 = 34342;
    static constexpr int g_normalInt_3 = 3245;
    static constexpr int g_ExtremeInt_1 = 1;
    static constexpr int g_ExtremeInt_2 = -1;
    static constexpr int g_ExtremeInt_3 = 0;

    static constexpr uint64_t NORMAL_U_INT64_1 = 123;
    static constexpr uint64_t NORMAL_U_INT64_2 = 34342;
    static constexpr uint64_t NORMAL_U_INT64_3 = 3245;
    static constexpr uint64_t VSYNC_PERIOD = 11718750;
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({ 0, 0, 0, 0 }, outerRadius, outerRadius);
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

std::shared_ptr<RSUIDirector> RSUIDirectorTest::CreateRSUIDirector()
{
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    sptr<IRemoteObject> connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    auto rsUIContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRender);
    std::shared_ptr<RSUIDirector> rsUiDirector = OHOS::Rosen::RSUIDirector::Create(connectToRender, rsUIContext);
    return rsUiDirector;
}

/**
 * @tc.name: SetTimeStamp001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetTimeStamp001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->SetTimeStamp(NORMAL_U_INT64_1, "test");
}

/**
 * @tc.name: SetTimeStamp002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetTimeStamp002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director->cacheDir_.empty());
    std::string cacheDir = "test";
    director->SetCacheDir(cacheDir);
    ASSERT_TRUE(!director->cacheDir_.empty());
}

/**
 * @tc.name: SetCacheDir001
 * @tc.desc: Test SetCacheDir001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUIDirectorTest, SetCacheDir001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = RSUIDirector::Create(nullptr, nullptr);
    std::string cacheDir;
    director->SetCacheDir(cacheDir);
    EXPECT_EQ(director->cacheDir_.empty(), true);
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    EXPECT_EQ(director->cacheDir_.empty(), true);
    std::string cacheDir = "test";
    director->SetCacheDir(cacheDir);
    EXPECT_EQ(director->cacheDir_.empty(), false);
}
#endif

/**
 * @tc.name: SetUITaskRunner001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, SetUITaskRunner001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->SetUITaskRunner([&](const std::function<void()>& task, uint32_t delay) {});
}

/**
 * @tc.name: DirectorSendMessages001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, DirectorSendMessages001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->SendMessages();
}

/**
 * @tc.name: SendMessagesTest001
 * @tc.desc: test results of SendMessages
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, SendMessagesTest001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    std::function<void()> callback = []() { std::cout << "for test" << std::endl; };
    auto transaction = std::make_shared<RSTransactionHandler>();
    auto node = RSCanvasNode::Create();
    NodeId nodeId = node->GetId();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    transaction->AddCommonCommand(command);

    director->rsUIContext_->rsTransactionHandler_ = transaction;
    director->SendMessages(callback);

    callback = nullptr;
    director->SendMessages(callback);
    EXPECT_NE(director->rsUIContext_->GetRSTransaction(), nullptr);

    director->rsUIContext_->rsTransactionHandler_ = nullptr;
    director->SendMessages(callback);
    EXPECT_EQ(director->rsUIContext_->GetRSTransaction(), nullptr);
}

/**
 * @tc.name: SendMessagesTest002
 * @tc.desc: test results of SendMessages
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, SendMessagesTest002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);

    auto rsTransactionProxy = RSTransactionProxy::GetInstance();
    auto node = RSCanvasNode::Create();
    NodeId nodeId = node->GetId();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(nodeId, 1, 1, AnimationCallbackEvent::FINISHED);
    RSTransactionProxy::GetInstance()->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 1);
    director->rsUIContext_ = nullptr;
    std::function<void()> callback = []() { std::cout << "for test" << std::endl; };
    director->SendMessages(callback);

    callback = nullptr;
    director->SendMessages(callback);
    EXPECT_NE(rsTransactionProxy, nullptr);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    director->SendMessages(callback);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    RSNode::SharedPtr rootNode = RSRootNode::Create(false, false, director->GetRSUIContext());
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    ASSERT_EQ(rootNode, director->rootNode_.lock());
}

/**
 * @tc.name: GetUIDescendantCount001
 * @tc.desc: Test GetUIDescendantCount returns 0 when root node is not set
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GetUIDescendantCount001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    EXPECT_EQ(director->GetUIDescendantCount(), 0U);
}

/**
 * @tc.name: GetUIDescendantCount002
 * @tc.desc: Test GetUIDescendantCount returns correct count with children
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GetUIDescendantCount002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    RSNode::SharedPtr rootNode = RSRootNode::Create(false, false, director->GetRSUIContext());
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, -1);

    EXPECT_EQ(director->GetUIDescendantCount(), 2U);
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

    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    bool hasRunningAnimation = director->FlushAnimation(NORMAL_U_INT64_2, VSYNC_PERIOD);
    director->PostFrameRateTask([]() { return; });
    ASSERT_EQ(hasRunningAnimation, false);
}

/**
 * @tc.name: GetCurrentRefreshRateMode
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, GetCurrentRefreshRateMode, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    const std::function<void()>& task = []() { std::cout << "for test" << std::endl; };
    director->PostFrameRateTask(task);
}

/**
 * @tc.name: SetRequestVsyncCallbackTest001
 * @tc.desc: SetRequestVsyncCallback Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSUIDirectorTest, SetRequestVsyncCallback001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    const std::function<void()>& callback = []() { std::cout << "for test" << std::endl; };
    director->SetRequestVsyncCallback(callback);
}

/**
 * @tc.name: SetRequestVsyncCallbackTest002
 * @tc.desc: SetRequestVsyncCallback Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSUIDirectorTest, SetRequestVsyncCallbackTest002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    director->SetRequestVsyncCallback(nullptr);
}

/**
 * @tc.name: ColorPickerCallbackProcessorTest001
 * @tc.desc: Test ColorPickerCallbackProcessor with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, ColorPickerCallbackProcessorTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    bool callbackInvoked = false;
    uint32_t receivedColor = 0;
    auto callback = [&callbackInvoked, &receivedColor](uint32_t color) {
        callbackInvoked = true;
        receivedColor = color;
    };

    node->RegisterColorPickerCallback(100, callback, 50);

    uint32_t testColor = 0xFFAABBCC;
    RSUIDirector::ColorPickerCallbackProcessor(node->GetId(), 0, testColor);

    EXPECT_TRUE(callbackInvoked);
    EXPECT_EQ(receivedColor, testColor);
}

/**
 * @tc.name: ColorPickerCallbackProcessorTest002
 * @tc.desc: Test ColorPickerCallbackProcessor with invalid node ID
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, ColorPickerCallbackProcessorTest002, TestSize.Level1)
{
    NodeId invalidNodeId = 999999;
    uint32_t testColor = 0xFF112233;

    // Should not crash, just log error
    RSUIDirector::ColorPickerCallbackProcessor(invalidNodeId, 0, testColor);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ColorPickerCallbackProcessorTest003
 * @tc.desc: Test ColorPickerCallbackProcessor with node without callback
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, ColorPickerCallbackProcessorTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_NE(node, nullptr);

    // Node exists but has no callback registered
    uint32_t testColor = 0xFF445566;
    RSUIDirector::ColorPickerCallbackProcessor(node->GetId(), 0, testColor);

    // Should not crash
    EXPECT_TRUE(true);
}

/**
 * @tc.name: FlushAnimationStartTime
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, FlushAnimationStartTime, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    std::shared_ptr<RSTransactionData> cmds = std::make_shared<RSTransactionData>();
    auto uiContext = director->GetRSUIContext();
    ASSERT_NE(uiContext, nullptr);
    uiContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    uint64_t token = uiContext->GetToken();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(token, token, token, AnimationCallbackEvent::FINISHED);
    ASSERT_NE(command->GetToken(), 0);
    cmds->AddCommand(command, token, FollowType::FOLLOW_TO_SELF);
    director->ProcessMessages(cmds);
    ASSERT_FALSE(cmds->IsEmpty());
    ASSERT_EQ(std::get<2>(cmds->payload_.front()), nullptr);
}

/**
 * @tc.name: ProcessMessagesTest003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, ProcessMessagesTest003, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    std::shared_ptr<RSTransactionData> cmds = std::make_shared<RSTransactionData>();
    auto uiContext = director->GetRSUIContext();
    ASSERT_NE(uiContext, nullptr);
    int32_t instanceId = 10;
    director->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); }, instanceId, true);
    auto node1 = RSCanvasNode::Create();
    auto node2 = RSCanvasNode::Create();
    node2->SetInstanceId(instanceId);
    std::unique_ptr<RSCommand> command1 =
        std::make_unique<RSAnimationCallback>(node1->GetId(), 0, 0, AnimationCallbackEvent::FINISHED);
    std::unique_ptr<RSCommand> command2 =
        std::make_unique<RSAnimationCallback>(node2->GetId(), 0, 0, AnimationCallbackEvent::FINISHED);
    ASSERT_EQ(command1->GetToken(), 0);
    ASSERT_EQ(command2->GetToken(), 0);
    cmds->AddCommand(command1, node1->GetId(), FollowType::FOLLOW_TO_SELF);
    cmds->AddCommand(command2, node2->GetId(), FollowType::FOLLOW_TO_SELF);
    director->ProcessMessages(cmds);
    ASSERT_FALSE(cmds->IsEmpty());
    ASSERT_EQ(std::get<2>(cmds->payload_.front()), nullptr);
}

/**
 * @tc.name: ProcessUIContextMessagesTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, ProcessUIContextMessagesTest001, TestSize.Level1)
{
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> cmdMap;
    uint64_t token = 12345;
    std::vector<std::unique_ptr<RSCommand>> commands;
    auto command = std::make_unique<RSAnimationCallback>(token, token, token, AnimationCallbackEvent::FINISHED);
    commands.push_back(std::move(command));
    cmdMap[token] = std::move(commands);
    RSUIDirector::ProcessUIContextMessages(cmdMap, 1);
    ASSERT_EQ(RSUIContextManager::Instance().GetRSUIContext(token), nullptr);
}

/**
 * @tc.name: ProcessUIContextMessagesTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, ProcessUIContextMessagesTest002, TestSize.Level1)
{
    auto director = CreateRSUIDirector();
    auto uiContext = director->GetRSUIContext();
    ASSERT_NE(uiContext, nullptr);
    uiContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });

    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> cmdMap;
    uint64_t token = uiContext->GetToken();
    std::vector<std::unique_ptr<RSCommand>> commands;
    auto command = std::make_unique<RSAnimationCallback>(token, token, token, AnimationCallbackEvent::FINISHED);
    commands.push_back(std::move(command));
    cmdMap[token] = std::move(commands);
    director->ProcessUIContextMessages(cmdMap, 1);
    ASSERT_NE(RSUIContextManager::Instance().GetRSUIContext(token), nullptr);
}

/**
 * @tc.name: ProcessUIContextMessagesTest003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, ProcessUIContextMessagesTest003, TestSize.Level1)
{
    auto director = CreateRSUIDirector();
    auto uiContext = director->GetRSUIContext();
    uiContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto token = 12345;
    auto command = std::make_unique<RSAnimationCallback>(0, 0, token, AnimationCallbackEvent::FINISHED);
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> cmdMap;
    std::vector<std::unique_ptr<RSCommand>> commands;
    commands.push_back(std::move(command));
    cmdMap[token] = std::move(commands);
    RSUIDirector::ProcessUIContextMessages(cmdMap, 1);
    ASSERT_EQ(RSUIContextManager::Instance().GetRSUIContext(token), nullptr);
    ASSERT_NE(RSUIContextManager::Instance().GetRandomUITaskRunnerCtx(), nullptr);
}

/**
 * @tc.name: PostTask
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIDirectorTest, PostTask, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    const std::function<void()>& task = []() { std::cout << "for test" << std::endl; };
    director->PostTask(task);
}

/**
 * @tc.name: SetRTRenderForcedTest002
 * @tc.desc: SetRTRenderForced Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSUIDirectorTest, SetRTRenderForcedTest002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    director->SetRTRenderForced(true);
}

/**
 * @tc.name: DumpNodeTreeProcessor001
 * @tc.desc: DumpNodeTreeProcessor Test
 * @tc.type: FUNC
 * @tc.require: issueIAKME2
 */
HWTEST_F(RSUIDirectorTest, DumpNodeTreeProcessor001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    int32_t instanceId = 10;
    director->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) {}, 0);
    director->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) {}, instanceId);
    RSNode::SharedPtr rsNode = RSCanvasNode::Create();
    rsNode->SetInstanceId(instanceId);
    director->DumpNodeTreeProcessor(rsNode->GetId(), 0, 0, 0);
    const NodeId invalidId = 1;
    director->DumpNodeTreeProcessor(invalidId, 0, 0, 0);
    SUCCEED();
}

/**
 * @tc.name: DumpNodeTreeProcessor002
 * @tc.desc: DumpNodeTreeProcessor Test
 * @tc.type: FUNC
 * @tc.require: issueICPQSU
 */
HWTEST_F(RSUIDirectorTest, DumpNodeTreeProcessor002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    auto uiContext = director->GetRSUIContext();
    ASSERT_TRUE(uiContext != nullptr);
    RSNode::SharedPtr rsNode = RSCanvasNode::Create();
    uint64_t token = uiContext->GetToken();
    director->DumpNodeTreeProcessor(rsNode->GetId(), 0, token, 0);
    ASSERT_NE(RSUIContextManager::Instance().GetRSUIContext(token), nullptr);
}

/**
 * @tc.name: GetIndexTest001
 * @tc.desc: GetIndex Test
 * @tc.type: FUNC
 * @tc.require: issueI9N1QF
 */
HWTEST_F(RSUIDirectorTest, GetIndexTest001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
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
 * @tc.name: TestTransactionHandler001
 * @tc.desc: Test the input param transactionHandler of callback is nullptr under single instance.
 * @tc.type: FUNC
 * @tc.require: issueICQP7Q
 */
HWTEST_F(RSUIDirectorTest, TestTransactionHandler001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    // isMultiInstance is false

    // test adding multiple cmds
    std::unique_ptr<RSTransactionData> cmds = std::make_unique<RSTransactionData>();
    uint64_t id = 1;
    auto node1 = RSCanvasNode::Create();
    auto node2 = RSCanvasNode::Create();
    std::unique_ptr<RSCommand> cmd1 =
        std::make_unique<RSAnimationCallback>(node1->GetId(), id, id, AnimationCallbackEvent::FINISHED);
    std::unique_ptr<RSCommand> cmd2 =
        std::make_unique<RSAnimationCallback>(node2->GetId(), id, id, AnimationCallbackEvent::FINISHED);
    cmds->AddCommand(cmd1, node1->GetId(), FollowType::FOLLOW_TO_SELF);
    cmds->AddCommand(cmd2, node2->GetId(), FollowType::FOLLOW_TO_SELF);
    // test cmd is nullptr
    std::get<2>(cmds->GetPayload()[0]) = nullptr;
    std::swap(cmds, RSTransactionProxy::GetInstance()->implicitRemoteTransactionData_);
    ASSERT_NE(RSTransactionProxy::GetInstance()->implicitRemoteTransactionData_, nullptr);

    director->SendMessages();
}

/**
 * @tc.name: TestTransactionHandler002
 * @tc.desc: Test the input param transactionHandler of callback is nullptr under single instance.
 * @tc.type: FUNC
 * @tc.require: issueICQP7Q
 */
HWTEST_F(RSUIDirectorTest, TestTransactionHandler002, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    // isMultiInstance is false

    // test adding multiple cmds
    std::unique_ptr<RSTransactionData> cmds = std::make_unique<RSTransactionData>();
    uint64_t id = 1;
    auto node1 = RSCanvasNode::Create();
    auto node2 = RSCanvasNode::Create();
    std::unique_ptr<RSCommand> cmd1 =
        std::make_unique<RSAnimationCallback>(node1->GetId(), id, id, AnimationCallbackEvent::FINISHED);
    std::unique_ptr<RSCommand> cmd2 =
        std::make_unique<RSAnimationCallback>(node2->GetId(), id, id, AnimationCallbackEvent::FINISHED);
    RSNodeMap::MutableInstance().RegisterAnimationInstanceId(0, node1->GetId(), id);
    RSNodeMap::MutableInstance().RegisterAnimationInstanceId(0, node2->GetId(), ++id);
    cmds->AddCommand(cmd1, node1->GetId(), FollowType::FOLLOW_TO_SELF);
    cmds->AddCommand(cmd2, node2->GetId(), FollowType::FOLLOW_TO_SELF);
    std::swap(cmds, RSTransactionProxy::GetInstance()->implicitRemoteTransactionData_);
    ASSERT_NE(RSTransactionProxy::GetInstance()->implicitRemoteTransactionData_, nullptr);

    director->SendMessages();
}

#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: TestTransactionHandler003
 * @tc.desc: Test the input param transactionHandler of callback is nullptr under multiple instances.
 * @tc.type: FUNC
 * @tc.require: issueICQP7Q
 */
HWTEST_F(RSUIDirectorTest, TestTransactionHandler003, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    // isMultiInstance is true

    // test adding multiple cmds
    std::unique_ptr<RSTransactionData> cmds = std::make_unique<RSTransactionData>();
    uint64_t id = 1;
    auto node1 = RSCanvasNode::Create();
    auto node2 = RSCanvasNode::Create();
    std::unique_ptr<RSCommand> cmd1 =
        std::make_unique<RSAnimationCallback>(node1->GetId(), id, id, AnimationCallbackEvent::FINISHED);
    std::unique_ptr<RSCommand> cmd2 =
        std::make_unique<RSAnimationCallback>(node2->GetId(), id, id, AnimationCallbackEvent::FINISHED);
    cmds->AddCommand(cmd1, node1->GetId(), FollowType::FOLLOW_TO_SELF);
    cmds->AddCommand(cmd2, node2->GetId(), FollowType::FOLLOW_TO_SELF);
    ASSERT_NE(director->rsUIContext_, nullptr);
    std::swap(cmds, director->rsUIContext_->GetRSTransaction()->implicitRemoteTransactionData_);
    ASSERT_NE(director->rsUIContext_->GetRSTransaction()->implicitRemoteTransactionData_, nullptr);
    director->SendMessages();
    director->Destroy();
}

/**
 * @tc.name: SetDVSyncUpdate001
 * @tc.desc: SetDVSyncUpdate Test
 * @tc.type: FUNC
 * @tc.require: issueICNDJ4
 */
HWTEST_F(RSUIDirectorTest, SetDVSyncUpdate001, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_TRUE(director != nullptr);
    const uint64_t time = 1000;
    director->SetDVSyncUpdate(time);
    EXPECT_EQ(time, director->dvsyncTime_);
    EXPECT_EQ(true, director->dvsyncUpdate_);
}
/**
 * @tc.name: GoCreateTest
 * @tc.desc: Test GoCreate sets state to CREATE and adds command
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoCreateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->GoCreate();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);
}

/**
 * @tc.name: GoCreateWithNullUIContextTest
 * @tc.desc: Test GoCreate handles null RSUIContext in AddUIDirectorCommand
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoCreateWithNullUIContextTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->rsUIContext_ = nullptr;
    director->GoCreate();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);
}

/**
 * @tc.name: AddUIDirectorCommandNullTransactionTest
 * @tc.desc: Test AddUIDirectorCommand returns when transaction is null
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, AddUIDirectorCommandNullTransactionTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    ASSERT_NE(director->rsUIContext_, nullptr);
    director->rsUIContext_->rsTransactionHandler_ = nullptr;
    director->GoCreate();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);
}

/**
 * @tc.name: GoResumeInvalidStateTest
 * @tc.desc: Test GoResume returns early for invalid states and leaves state unchanged
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoResumeInvalidStateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    // state is FOREGROUND after Init, invalid for GoResume
    director->GoResume();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::FOREGROUND);

    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);
    director->GoResume();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);

    director->Destroy();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::DESTROYED);
    director->GoResume();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::DESTROYED);
}

/**
 * @tc.name: GoResumeNormalStateTest
 * @tc.desc: Test GoResume from CREATE and STOP states
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoResumeNormalStateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    auto rootNode = RSRootNode::Create(false, false, director->GetRSUIContext());
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());

    director->GoCreate();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);
    director->GoResume();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::RESUME);

    director->GoForeground();
    director->GoBackground();
    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);
    director->GoResume();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::RESUME);
}

/**
 * @tc.name: RebuildNodeTreeTest
 * @tc.desc: Test RebuildNodeTree branches with and without root node
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, RebuildNodeTreeTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    // no root node, should return directly
    director->RebuildNodeTree();

    auto rootNode = RSRootNode::Create(false, false, director->GetRSUIContext());
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    director->RebuildNodeTree();
    EXPECT_EQ(director->GetRSUIContext()->GetRebuildState(), RebuildState::Normal);
}

/**
 * @tc.name: GoForegroundInvalidStateTest
 * @tc.desc: Test GoForeground returns early for invalid states and leaves state unchanged
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoForegroundInvalidStateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);

    director->GoCreate();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);
    director->GoForeground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    director->GoResume();
    director->GoBackground();
    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);
    director->GoForeground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);

    director->Destroy();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::DESTROYED);
    director->GoForeground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::DESTROYED);
}

/**
 * @tc.name: GoForegroundNormalStateTest
 * @tc.desc: Test GoForeground from RESUME and BACKGROUND
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoForegroundNormalStateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);

    director->GoCreate();
    director->GoResume();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::RESUME);
    director->GoForeground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::FOREGROUND);

    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);
    director->GoForeground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::FOREGROUND);
}

/**
 * @tc.name: ExecuteGoForegroundBranchesTest
 * @tc.desc: Test ExecuteGoForeground with different isActive_/isUniRenderEnabled_/textureExport
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, ExecuteGoForegroundBranchesTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);

    director->isActive_ = false;
    director->isUniRenderEnabled_ = false;
    director->ExecuteGoForeground(false);
    EXPECT_TRUE(director->isActive_);

    director->isActive_ = false;
    director->isUniRenderEnabled_ = true;
    director->ExecuteGoForeground(false);
    EXPECT_TRUE(director->isActive_);

    director->isActive_ = false;
    director->isUniRenderEnabled_ = true;
    director->ExecuteGoForeground(true);
    EXPECT_TRUE(director->isActive_);

    // second call when isActive_ is true should not update again
    director->ExecuteGoForeground(false);
    EXPECT_TRUE(director->isActive_);
}

/**
 * @tc.name: GoBackgroundInvalidStateTest
 * @tc.desc: Test GoBackground returns early when state is not FOREGROUND and leaves state unchanged
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoBackgroundInvalidStateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);

    director->GoCreate();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);
    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);

    director->GoResume();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::RESUME);
    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::RESUME);

    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);
    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);

    director->Destroy();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::DESTROYED);
    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::DESTROYED);
}

/**
 * @tc.name: GoBackgroundNormalStateTest
 * @tc.desc: Test GoBackground from FOREGROUND with various ExecuteGoBackground branches
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoBackgroundNormalStateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);

    // isActive_ false: ExecuteGoBackground does nothing
    director->isActive_ = false;
    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);

    // reset to FOREGROUND and test isUniRenderEnabled_ path
    director->currentUIDirectorState_ = RSUIDirectorLifecycleState::FOREGROUND;
    director->isActive_ = true;
    director->isUniRenderEnabled_ = true;
    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);

    // reset to FOREGROUND and test textureExport path
    director->currentUIDirectorState_ = RSUIDirectorLifecycleState::FOREGROUND;
    director->isActive_ = true;
    director->isUniRenderEnabled_ = false;
    director->GoBackground(true);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);

    // reset to FOREGROUND and test normal buffer clean path
    director->currentUIDirectorState_ = RSUIDirectorLifecycleState::FOREGROUND;
    director->isActive_ = true;
    director->isUniRenderEnabled_ = false;
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    director->SetRSSurfaceNode(surfaceNode);
    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);
}

/**
 * @tc.name: GoStopInvalidStateTest
 * @tc.desc: Test GoStop returns early for invalid states and leaves state unchanged
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoStopInvalidStateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);

    // state is FOREGROUND after Init, invalid for GoStop
    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::FOREGROUND);

    director->GoBackground();
    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);

    director->Destroy();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::DESTROYED);
    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::DESTROYED);
}

/**
 * @tc.name: GoStopNormalStateTest
 * @tc.desc: Test GoStop from BACKGROUND and RESUME
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoStopNormalStateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);

    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);
    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);

    director->currentUIDirectorState_ = RSUIDirectorLifecycleState::RESUME;
    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);
}

/**
 * @tc.name: ReleaseRenderNodeBranchesTest
 * @tc.desc: Test ReleaseRenderNode handles null context, skip flag and node filtering
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, ReleaseRenderNodeBranchesTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    ASSERT_NE(director->rsUIContext_, nullptr);

    // rsUIContext_ is null path
    auto savedContext = director->rsUIContext_;
    director->rsUIContext_ = nullptr;
    director->ReleaseRenderNode();
    director->rsUIContext_ = savedContext;

    // skipDestroyUIContext_ is true path
    director->skipDestroyUIContext_ = true;
    director->ReleaseRenderNode();
    director->skipDestroyUIContext_ = false;

    // normal path with app window surface node and a canvas child
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "AppWindow";
    auto surfaceNode = RSSurfaceNode::Create(config);
    director->SetRSSurfaceNode(surfaceNode);
    auto child = RSCanvasNode::Create(false, false, director->GetRSUIContext());
    auto rootNode = RSRootNode::Create(false, false, director->GetRSUIContext());
    rootNode->AddChild(child, -1);
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    director->ReleaseRenderNode();
}

/**
 * @tc.name: ExecuteGoDestroyBranchesTest
 * @tc.desc: Test ExecuteGoDestroy with root node and skipDestroyUIContext variations
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, ExecuteGoDestroyBranchesTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);

    // no root node and no ui context
    director->ExecuteGoDestroy(false);

    auto rootNode = RSRootNode::Create(false, false, director->GetRSUIContext());
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());

    director->isUniRenderEnabled_ = false;
    director->ExecuteGoDestroy(false);
    EXPECT_EQ(director->rootNode_.lock(), nullptr);

    // re-create root node and test skipDestroyUIContext_
    auto rootNode2 = RSRootNode::Create(false, false, director->GetRSUIContext());
    director->SetRSRootNode(rootNode2->ReinterpretCastTo<RSRootNode>());
    director->skipDestroyUIContext_ = true;
    director->ExecuteGoDestroy(false);
}

/**
 * @tc.name: DestroyStateTest
 * @tc.desc: Test Destroy sets DESTROYED state and executes destroy logic
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, DestroyStateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->Destroy();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::DESTROYED);
}

/**
 * @tc.name: GoStopFromCreateStateTest
 * @tc.desc: Test GoStop from CREATE state sets state to STOP without executing stop logic
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoStopFromCreateStateTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->GoCreate();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::CREATE);
    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);
}

/**
 * @tc.name: GoStopIdempotentTest
 * @tc.desc: Test GoStop is idempotent when called repeatedly from BACKGROUND
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoStopIdempotentTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->GoBackground();
    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);
    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);
}

/**
 * @tc.name: GoDestroyNoCrashTest
 * @tc.desc: Test GoDestroy can be called without crashing
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoDestroyNoCrashTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->GoDestroy();
    SUCCEED();
}

/**
 * @tc.name: GoDestroyStateUnchangedTest
 * @tc.desc: Test GoDestroy does not change current lifecycle state
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GoDestroyStateUnchangedTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    auto stateBefore = director->GetCurrentState();
    director->GoDestroy();
    EXPECT_EQ(director->GetCurrentState(), stateBefore);

    director->GoBackground();
    stateBefore = director->GetCurrentState();
    director->GoDestroy();
    EXPECT_EQ(director->GetCurrentState(), stateBefore);

    director->GoStop();
    stateBefore = director->GetCurrentState();
    director->GoDestroy();
    EXPECT_EQ(director->GetCurrentState(), stateBefore);
}

/**
 * @tc.name: GetCurrentStateInitialTest
 * @tc.desc: Test GetCurrentState returns FOREGROUND after RSUIDirector creation
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GetCurrentStateInitialTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::FOREGROUND);
}

/**
 * @tc.name: GetCurrentStateTransitionTest
 * @tc.desc: Test GetCurrentState reflects lifecycle state transitions
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GetCurrentStateTransitionTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);

    director->GoBackground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::BACKGROUND);

    director->GoStop();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::STOP);

    director->GoResume();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::RESUME);

    director->GoForeground();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::FOREGROUND);

    director->Destroy();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::DESTROYED);
}

/**
 * @tc.name: GetCurrentStateAfterGoDestroyTest
 * @tc.desc: Test GetCurrentState remains unchanged after GoDestroy
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GetCurrentStateAfterGoDestroyTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->GoDestroy();
    EXPECT_EQ(director->GetCurrentState(), RSUIDirectorLifecycleState::FOREGROUND);
}

/**
 * @tc.name: SetRSRootNodeUpdatesRootNodeIdTest
 * @tc.desc: Test SetRSRootNode updates rootNodeId_ in RSUIContext
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, SetRSRootNodeUpdatesRootNodeIdTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    auto uiContext = director->GetRSUIContext();
    ASSERT_NE(uiContext, nullptr);

    auto rootNode = RSRootNode::Create(false, false, uiContext);
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    EXPECT_EQ(uiContext->rootNodeId_, rootNode->GetId());

    // setting the same root node again should return early
    director->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    EXPECT_EQ(uiContext->rootNodeId_, rootNode->GetId());
}

/**
 * @tc.name: GetHybridRenderCanvasEnabledTest
 * @tc.desc: Test GetHybridRenderCanvasEnabled returns system property value
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, GetHybridRenderCanvasEnabledTest, TestSize.Level1)
{
    bool systemPropertiesRet = RSSystemProperties::GetHybridRenderCanvasEnabled();
    bool directorRet = RSUIDirector::GetHybridRenderCanvasEnabled();
    EXPECT_EQ(systemPropertiesRet, directorRet);
}

/**
 * @tc.name: OnCanvasDrawingNodeRenderStartEndTest
 * @tc.desc: Test canvas drawing node render start/end bookkeeping
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, OnCanvasDrawingNodeRenderStartEndTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    NodeId nodeId1 = 1001;
    NodeId nodeId2 = 1002;
    director->OnCanvasDrawingNodeRenderStart(nodeId1);
    director->OnCanvasDrawingNodeRenderStart(nodeId2);
    director->OnCanvasDrawingNodeRenderEnd(nodeId1);
    director->OnCanvasDrawingNodeRenderEnd(nodeId2);
    SUCCEED();
}

/**
 * @tc.name: AnimationDestroyInRenderCallbackProcessorInvalidTest
 * @tc.desc: Test AnimationDestroyInRenderCallbackProcessor with invalid token, invalid node id and valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, AnimationDestroyInRenderCallbackProcessorInvalidTest, TestSize.Level1)
{
    // invalid token
    RSUIDirector::AnimationDestroyInRenderCallbackProcessor(0, 0, 0, 0.0f, false);

    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    auto uiContext = director->GetRSUIContext();
    ASSERT_NE(uiContext, nullptr);
    // valid token but invalid node id
    RSUIDirector::AnimationDestroyInRenderCallbackProcessor(0, 0, uiContext->GetToken(), 0.0f, false);

    // valid token and valid node
    auto node = RSCanvasNode::Create(false, false, uiContext);
    RSUIDirector::AnimationDestroyInRenderCallbackProcessor(
        node->GetId(), 1, uiContext->GetToken(), 0.5f, true);
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
/**
 * @tc.name: InitHybridRenderTest
 * @tc.desc: Test InitHybridRender handles disabled hybrid canvas and null checks
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, InitHybridRenderTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        director->InitHybridRender();
        SUCCEED();
        return;
    }
    director->InitHybridRender();
}

/**
 * @tc.name: FlushCanvasDrawingNodeBuffersTest
 * @tc.desc: Test FlushCanvasDrawingNodeBuffers proxy to RSUIContext
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, FlushCanvasDrawingNodeBuffersTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->FlushCanvasDrawingNodeBuffers();

    director->rsUIContext_ = nullptr;
    director->FlushCanvasDrawingNodeBuffers();
    SUCCEED();
}

/**
 * @tc.name: SetCacheDirHybridCanvasTest
 * @tc.desc: Test SetCacheDir with hybrid canvas paths
 * @tc.type: FUNC
 */
HWTEST_F(RSUIDirectorTest, SetCacheDirHybridCanvasTest, TestSize.Level1)
{
    std::shared_ptr<RSUIDirector> director = CreateRSUIDirector();
    ASSERT_NE(director, nullptr);
    director->SetCacheDir("");
    director->SetCacheDir("/data/cache");

    director->rsUIContext_ = nullptr;
    director->SetCacheDir("/data/cache2");
    SUCCEED();
}
#endif

#endif
} // namespace OHOS::Rosen
