/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "ipc_callbacks/rs_frame_stability_callback_stub.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "rs_render_pipeline_agent.h"
#include "rs_render_pipeline.h"
#include "transaction/rs_frame_stability_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint32_t DEFAULT_TIME = 100000; // 100ms
constexpr uint64_t DEFAULT_ID = 10;
constexpr FrameStabilityTarget INVALID_TARGET = { .id = 100, .type = FrameStabilityTargetType::SCREEN };
constexpr FrameStabilityTarget VALID_TARGET = { .id = DEFAULT_ID, .type = FrameStabilityTargetType::SCREEN };
constexpr FrameStabilityConfig DEFAULT_CONFIG = { .stableDuration = 200, .changePercent = 0.0f };

class RSFrameStabilityCallbackStubMock : public RSFrameStabilityCallbackStub {
public:
    RSFrameStabilityCallbackStubMock() = default;
    virtual ~RSFrameStabilityCallbackStubMock() = default;
    void OnFrameStabilityChanged(bool isStable) override {};
};

class RSRenderPipelineAgentTest : public testing::Test {
public:
    static inline RSMainThread* mainThread_ = nullptr;
    static inline std::shared_ptr<RSScreenRenderNode> screenNode_ = nullptr;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override {}
    void TearDown() override {}
};

void RSRenderPipelineAgentTest::SetUpTestCase()
{
    mainThread_ = RSMainThread::Instance();
    ASSERT_NE(mainThread_, nullptr);
    mainThread_->handler_ =
        std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::AppExecFwk::EventRunner::Create(true));
    mainThread_->handler_->eventRunner_->Run();
    auto context = mainThread_->context_;
    ASSERT_NE(context, nullptr);
    screenNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID, DEFAULT_ID, context);
    ASSERT_NE(screenNode_, nullptr);
    context->nodeMap.RegisterRenderNode(screenNode_);
}

void RSRenderPipelineAgentTest::TearDownTestCase()
{
    usleep(DEFAULT_TIME);
    auto& nodeMap = RSMainThread::Instance()->GetContext().nodeMap;
    nodeMap.UnregisterRenderNode(DEFAULT_ID);
    mainThread_->handler_->RemoveAllEvents();
    mainThread_->handler_->eventRunner_->Stop();
    mainThread_->handler_->eventRunner_ = nullptr;
    mainThread_->handler_ = nullptr;
    mainThread_->context_ = nullptr;
    screenNode_ = nullptr;
}

/**
 * @tc.name: SetGlobalDarkColorMode_NullPipeline_ReturnInvalidValue
 * @tc.desc: Verify SetGlobalDarkColorMode returns ERR_INVALID_VALUE when pipeline is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, SetGlobalDarkColorMode_NullPipeline_ReturnInvalidValue, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    ErrCode ret = agent->SetGlobalDarkColorMode(true);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetGlobalDarkColorMode_ValidPipeline_ReturnOk
 * @tc.desc: Verify SetGlobalDarkColorMode returns ERR_OK when pipeline exists.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, SetGlobalDarkColorMode_ValidPipeline_ReturnOk, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    ErrCode retDark = agent->SetGlobalDarkColorMode(true);
    ErrCode retLight = agent->SetGlobalDarkColorMode(false);
    EXPECT_EQ(retDark, ERR_OK);
    EXPECT_EQ(retLight, ERR_OK);
}

/**
 * @tc.name: DropFrameByPid_NullPipeline_ReturnInvalidValue
 * @tc.desc: Verify DropFrameByPid returns ERR_INVALID_VALUE when pipeline is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, DropFrameByPid_NullPipeline_ReturnInvalidValue, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    std::vector<int32_t> pidList = { 1001, 1002 };
    constexpr int32_t DROP_FRAME_LEVEL = 1;
    ErrCode ret = agent->DropFrameByPid(pidList, DROP_FRAME_LEVEL);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: DropFrameByPid_ValidPipeline_ReturnOk
 * @tc.desc: Verify DropFrameByPid returns ERR_OK when pipeline exists.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, DropFrameByPid_ValidPipeline_ReturnOk, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    RSMainThread* mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    renderPipeline->mainThread_ = mainThread;

    std::vector<int32_t> pidList = { 1001, 1002 };
    constexpr int32_t DROP_FRAME_LEVEL = 1;

    ErrCode ret = agent->DropFrameByPid(pidList, DROP_FRAME_LEVEL);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: GetScreenHdrStatusTest001
 * @tc.desc: Verify GetScreenHdrStatus.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, GetScreenHdrStatusTest001, TestSize.Level1)
{
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    auto mainThread = RSMainThread::Instance();
    mainThread->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    mainThread->handler_->eventRunner_->Run();

    auto pipeline = std::make_shared<RSRenderPipeline>();
    pipeline->mainThread_ = mainThread;
    auto agent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);

    ScreenId screenId = 0;
    HdrStatus hdrStatus = HdrStatus::HDR_EFFECT;
    int32_t resCode = 0;
    EXPECT_EQ(agent->GetScreenHDRStatus(screenId, hdrStatus, resCode), ERR_OK);
    EXPECT_EQ(resCode, SCREEN_NOT_FOUND);
    EXPECT_EQ(hdrStatus, HdrStatus::NO_HDR);
}

/**
 * @tc.name: GetScreenHdrStatusTest002
 * @tc.desc: Verify GetScreenHdrStatus.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, GetScreenHdrStatusTest002, TestSize.Level1)
{
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    auto mainThread = RSMainThread::Instance();
    mainThread->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    mainThread->handler_->eventRunner_->Run();
    NodeId nodeId = 1001;
    ScreenId screenId = 1005;
    auto screenNode = std::make_shared<RSScreenRenderNode>(nodeId, screenId);
    auto& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.screenNodeMap_[nodeId] = screenNode;

    auto pipeline = std::make_shared<RSRenderPipeline>();
    pipeline->mainThread_ = mainThread;
    auto agent = sptr<RSRenderPipelineAgent>::MakeSptr(pipeline);

    HdrStatus hdrStatus = HdrStatus::HDR_EFFECT;
    int32_t resCode = 1;
    EXPECT_EQ(agent->GetScreenHDRStatus(screenId, hdrStatus, resCode), ERR_OK);
    EXPECT_EQ(resCode, SUCCESS);
    EXPECT_EQ(hdrStatus, HdrStatus::NO_HDR);
    nodeMap.screenNodeMap_.erase(nodeId);
}

/**
 * @tc.name: GetMaxGpuBufferSize_NullPipeline
 * @tc.desc: Verify GetMaxGpuBufferSize returns ERR_INVALID_VALUE when pipeline is null
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, GetMaxGpuBufferSize_NullPipeline, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    ErrCode ret = agent->GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: RegisterFrameStabilityDetection001
 * @tc.desc: Verify RegisterFrameStabilityDetection returns error when pipeline is null
 * @tc.type: FUNC
 * @tc.require: 22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RegisterFrameStabilityDetection001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    sptr<RSFrameStabilityCallbackStubMock> callback = new RSFrameStabilityCallbackStubMock();
    int32_t ret = agent->RegisterFrameStabilityDetection(1000, INVALID_TARGET, DEFAULT_CONFIG, callback);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.name: RegisterFrameStabilityDetection002
 * @tc.desc: Verify RegisterFrameFrameStabilityDetection returns error with invalid ID
 * @tc.type: FUNC
 * @tc.require: 22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RegisterFrameStabilityDetection002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    renderPipeline->mainThread_ = mainThread_;

    sptr<RSFrameStabilityCallbackStubMock> callback = new RSFrameStabilityCallbackStubMock();
    int32_t ret = agent->RegisterFrameStabilityDetection(1000, INVALID_TARGET, DEFAULT_CONFIG, callback);
    EXPECT_EQ(ret, static_cast<int32_t>(FrameStabilityErrorCode::INVALID_ID));
}

/**
 * @tc.name: RegisterFrameStabilityDetection003
 * @tc.desc: Verify RegisterFrameFrameStabilityDetection returns error with valid ID
 * @tc.type: FUNC
 * @tc.require: 22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RegisterFrameStabilityDetection003, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    renderPipeline->mainThread_ = mainThread_;

    sptr<RSFrameStabilityCallbackStubMock> callback = new RSFrameStabilityCallbackStubMock();
    int32_t ret = agent->RegisterFrameStabilityDetection(1000, VALID_TARGET, DEFAULT_CONFIG, callback);
    EXPECT_NE(ret, static_cast<int32_t>(FrameStabilityErrorCode::INVALID_ID));
}

/**
 * @tc.name: UnregisterFrameStabilityDetection001
 * @tc.desc: Verify UnregisterFrameStabilityDetection returns error when pipeline is null
 * @tc.type: FUNC
 * @tc.require: 22984
 */
HWTEST_F(RSRenderPipelineAgentTest, UnregisterFrameStabilityDetection001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    int32_t ret = agent->UnregisterFrameStabilityDetection(1000, INVALID_TARGET);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.name: UnregisterFrameStabilityDetection002
 * @tc.desc: Verify UnregisterFrameStabilityDetection returns error with valid ID
 * @tc.type: FUNC
 * @tc.require: 22984
 */
HWTEST_F(RSRenderPipelineAgentTest, UnregisterFrameStabilityDetection002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    renderPipeline->mainThread_ = mainThread_;

    int32_t ret = agent->UnregisterFrameStabilityDetection(1000, VALID_TARGET);
    EXPECT_NE(ret, static_cast<int32_t>(FrameStabilityErrorCode::INVALID_ID));
}

/**
 * @tc.name: StartFrameStabilityCollection001
 * @tc.desc: Verify StartFrameStabilityCollection returns error when pipeline is null
 * @tc.type: FUNC
 * @tc.require: 22984
 */
HWTEST_F(RSRenderPipelineAgentTest, StartFrameStabilityCollection001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    int32_t ret = agent->StartFrameStabilityCollection(1000, INVALID_TARGET, DEFAULT_CONFIG);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.name: StartFrameStabilityCollection002
 * @tc.desc: Verify StartFrameStabilityCollection returns error with invalid ID
 * @tc.type: FUNC
 * @tc.require: 22984
 */
HWTEST_F(RSRenderPipelineAgentTest, StartFrameStabilityCollection002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    renderPipeline->mainThread_ = mainThread_;

    int32_t ret = agent->StartFrameStabilityCollection(1000, INVALID_TARGET, DEFAULT_CONFIG);
    EXPECT_EQ(ret, static_cast<int32_t>(FrameStabilityErrorCode::INVALID_ID));
}

/**
 * @tc.name: StartFrameStabilityCollection003
 * @tc.desc: Verify StartFrameStabilityCollection returns error with valid ID
 * @tc.type: FUNC
 * @tc.require: 22984
 */
HWTEST_F(RSRenderPipelineAgentTest, StartFrameStabilityCollection003, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    renderPipeline->mainThread_ = mainThread_;

    int32_t ret = agent->StartFrameStabilityCollection(1000, VALID_TARGET, DEFAULT_CONFIG);
    EXPECT_NE(ret, static_cast<int32_t>(FrameStabilityErrorCode::INVALID_ID));
}

/**
 * @tc.name: GetFrameStabilityResult_NullPipeline
 * @tc.desc: Verify GetFrameStabilityResult returns error when pipeline is null
 * @tc.type: FUNC
 * @tc.require: 22984
 */
HWTEST_F(RSRenderPipelineAgentTest, GetFrameStabilityResult001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    bool result = false;
    int32_t ret = agent->GetFrameStabilityResult(1000, INVALID_TARGET, result);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.name: GetFrameStabilityResult002
 * @tc.desc: Verify GetFrameStabilityResult returns error with valid ID
 * @tc.type: FUNC
 * @tc.require: 22984
 */
HWTEST_F(RSRenderPipelineAgentTest, GetFrameStabilityResult002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    renderPipeline->mainThread_ = mainThread_;

    bool result = false;
    int32_t ret = agent->GetFrameStabilityResult(1000, VALID_TARGET, result);
    EXPECT_NE(ret, static_cast<int32_t>(FrameStabilityErrorCode::INVALID_ID));
}
} // namespace OHOS::Rosen
