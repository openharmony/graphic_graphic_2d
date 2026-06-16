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

#include <memory>

#include "gtest/gtest.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "node_mem_release_param.h"
#endif

#include "consumer_surface.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "ipc_callbacks/rs_frame_stability_callback_stub.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_render_pipeline_agent.h"
#include "rs_render_pipeline.h"
#include "surface_utils.h"
#include "transaction/rs_frame_stability_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint32_t DEFAULT_TIME = 100000; // 100ms
constexpr uint64_t DEFAULT_ID = 10;
constexpr FrameStabilityTarget INVALID_TARGET = { .id = 100, .type = FrameStabilityTargetType::SCREEN };
constexpr FrameStabilityTarget VALID_TARGET = { .id = DEFAULT_ID, .type = FrameStabilityTargetType::SCREEN };
constexpr FrameStabilityConfig DEFAULT_CONFIG = { .stableDuration = 200, .changePercent = 0.0f };
constexpr NodeId FORCE_TUNNEL_NODE_ID = 10001;
const std::string FORCE_TUNNEL_BUNDLE_NAME = "com.ohos.force.tunnel";
const std::string FORCE_TUNNEL_SURFACE_NAME = "ForceTunnelSurface";
const std::string FORCE_TUNNEL_CONFIG_KEY = FORCE_TUNNEL_BUNDLE_NAME + "+" + FORCE_TUNNEL_SURFACE_NAME;

RSSurfaceRenderNodeConfig MakeForceTunnelConfig()
{
    return {
        .id = FORCE_TUNNEL_NODE_ID,
        .name = FORCE_TUNNEL_SURFACE_NAME,
        .bundleName = FORCE_TUNNEL_BUNDLE_NAME,
    };
}

void ClearForceTunnelConfig()
{
    auto surfaceUtils = SurfaceUtils::GetInstance();
    ASSERT_NE(surfaceUtils, nullptr);
    surfaceUtils->RemoveTunnelLayerConfig(FORCE_TUNNEL_CONFIG_KEY);
}

void AddForceTunnelConfig()
{
    auto surfaceUtils = SurfaceUtils::GetInstance();
    ASSERT_NE(surfaceUtils, nullptr);
    surfaceUtils->RemoveTunnelLayerConfig(FORCE_TUNNEL_CONFIG_KEY);
    surfaceUtils->AddTunnelLayerConfig(FORCE_TUNNEL_CONFIG_KEY);
}

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
    void TearDown() override
    {
        ClearForceTunnelConfig();
    }
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
 * @tc.name: ConfigureForceTunnelLayer_NoConfig_ReturnsWithoutSettingInfo
 * @tc.desc: Verify ConfigureForceTunnelLayer returns early when SurfaceUtils has no matching config.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, ConfigureForceTunnelLayer_NoConfig_ReturnsWithoutSettingInfo, TestSize.Level1)
{
    ClearForceTunnelConfig();

    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(FORCE_TUNNEL_SURFACE_NAME);
    ASSERT_NE(surface, nullptr);

    agent->ConfigureForceTunnelLayer(MakeForceTunnelConfig(), surface);
    TunnelLayerState state;
    ASSERT_EQ(surface->GetTunnelLayerInfo(state), GSERROR_OK);
    EXPECT_EQ(state.tunnelLayerInfo.tunnelTypeMask, TUNNEL_TYPE_NONE);
}

/**
 * @tc.name: ConfigureForceTunnelLayer_NullUtils_ReturnsWithoutSettingInfo
 * @tc.desc: Verify ConfigureForceTunnelLayer returns early when SurfaceUtils is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, ConfigureForceTunnelLayer_NullUtils_ReturnsWithoutSettingInfo, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(FORCE_TUNNEL_SURFACE_NAME);
    ASSERT_NE(surface, nullptr);

    agent->ConfigureForceTunnelLayer(MakeForceTunnelConfig(), surface, nullptr);
    TunnelLayerState state;
    ASSERT_EQ(surface->GetTunnelLayerInfo(state), GSERROR_OK);
    EXPECT_EQ(state.tunnelLayerInfo.tunnelTypeMask, TUNNEL_TYPE_NONE);
}

/**
 * @tc.name: ConfigureForceTunnelLayer_SetTunnelInfoFailed_Returns
 * @tc.desc: Verify ConfigureForceTunnelLayer handles SetTunnelLayerInfo failure after config match.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, ConfigureForceTunnelLayer_SetTunnelInfoFailed_Returns, TestSize.Level1)
{
    AddForceTunnelConfig();

    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    sptr<IConsumerSurface> surface = new ConsumerSurface(FORCE_TUNNEL_SURFACE_NAME);
    ASSERT_NE(surface, nullptr);

    agent->ConfigureForceTunnelLayer(MakeForceTunnelConfig(), surface);
    ClearForceTunnelConfig();
}

/**
 * @tc.name: ConfigureForceTunnelLayer_SetTunnelInfoSucceed
 * @tc.desc: Verify ConfigureForceTunnelLayer sets VIDEO tunnel info when config matches.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, ConfigureForceTunnelLayer_SetTunnelInfoSucceed, TestSize.Level1)
{
    AddForceTunnelConfig();

    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create(FORCE_TUNNEL_SURFACE_NAME);
    ASSERT_NE(surface, nullptr);

    agent->ConfigureForceTunnelLayer(MakeForceTunnelConfig(), surface);
    TunnelLayerState state;
    ASSERT_EQ(surface->GetTunnelLayerInfo(state), GSERROR_OK);
    EXPECT_EQ(state.tunnelLayerInfo.tunnelTypeMask, TUNNEL_TYPE_VIDEO);
    ClearForceTunnelConfig();
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
    constexpr int32_t dropFrameLevel = 1;
    ErrCode ret = agent->DropFrameByPid(pidList, dropFrameLevel);
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
    constexpr int32_t dropFrameLevel = 1;

    ErrCode ret = agent->DropFrameByPid(pidList, dropFrameLevel);
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
 * @tc.name: TakeSurfaceCaptureWindowSync001
 * @tc.desc: Test TakeSurfaceCapture with windowSync enabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderPipelineAgentTest, TakeSurfaceCaptureWindowSync001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->pendingWindowCapTasks_.clear();

    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    NodeId nodeId = 1000300;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    auto& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(surfaceNode);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.windowSync = true;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    RSSurfaceCaptureBlurParam blurParam;
    Drawing::Rect specifiedAreaRect;
    RSSurfaceCapturePermissions permissions;
    permissions.isSystemCalling = true;

    sptr<RSISurfaceCaptureCallback> callback = nullptr;
    agent->TakeSurfaceCapture(nodeId, callback, captureConfig, blurParam, specifiedAreaRect, permissions);
#if defined(RS_ENABLE_UNI_RENDER)
    ASSERT_GE(mainThread->pendingWindowCapTasks_.size(), 0u);
#endif
    nodeMap.UnregisterRenderNode(nodeId);
    mainThread->pendingWindowCapTasks_.clear();
}

/**
 * @tc.name: TakeSurfaceCaptureWindowSync002
 * @tc.desc: Test TakeSurfaceCapture with windowSync disabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderPipelineAgentTest, TakeSurfaceCaptureWindowSync002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->pendingWindowCapTasks_.clear();

    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    NodeId nodeId = 1000301;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    auto& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(surfaceNode);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.windowSync = false;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    RSSurfaceCaptureBlurParam blurParam;
    Drawing::Rect specifiedAreaRect;
    RSSurfaceCapturePermissions permissions;
    permissions.isSystemCalling = true;

    sptr<RSISurfaceCaptureCallback> callback = nullptr;
    agent->TakeSurfaceCapture(nodeId, callback, captureConfig, blurParam, specifiedAreaRect, permissions);
#if defined(RS_ENABLE_UNI_RENDER)
    ASSERT_TRUE(mainThread->pendingWindowCapTasks_.empty());
#endif
    nodeMap.UnregisterRenderNode(nodeId);
    mainThread->pendingWindowCapTasks_.clear();
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

/**
 * @tc.name: UpdateFrameStabilityDetection001
 * @tc.desc: Verify UpdateFrameStabilityDetection returns error when pipeline is null
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSRenderPipelineAgentTest, UpdateFrameStabilityDetection001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    FrameStabilityTarget oldTarget = { .id = 100, .type = FrameStabilityTargetType::SCREEN };
    FrameStabilityTarget newTarget = { .id = 200, .type = FrameStabilityTargetType::WINDOW };
    int32_t ret = agent->UpdateFrameStabilityDetection(1000, oldTarget, newTarget);
    EXPECT_EQ(ret, RENDER_SERVICE_NULL);
}

/**
 * @tc.name: UpdateFrameStabilityDetection002
 * @tc.desc: Verify UpdateFrameStabilityDetection returns error with invalid newTarget ID
 * @tc.type: FUNC
 * @tc.require: issue22896
 */
HWTEST_F(RSRenderPipelineAgentTest, UpdateFrameStabilityDetection002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    renderPipeline->mainThread_ = mainThread_;

    FrameStabilityTarget oldTarget = VALID_TARGET;
    FrameStabilityTarget newTarget = INVALID_TARGET;
    int32_t ret = agent->UpdateFrameStabilityDetection(1000, oldTarget, newTarget);
    EXPECT_EQ(ret, static_cast<int32_t>(FrameStabilityErrorCode::INVALID_ID));
}

/**
 * @tc.name: CleanTest_NullPipeline
 * @tc.desc: Verify Clean returns early when rsRenderPipeline_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, CleanTest_NullPipeline, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    ASSERT_EQ(agent->rsRenderPipeline_.lock(), nullptr);

    pid_t testPid = 12345;
    // Should return early without crash when rsRenderPipeline_ is nullptr
    agent->Clean(testPid, false);
    agent->Clean(testPid, true);
    // Verify agent state remains unchanged after Clean
    EXPECT_EQ(agent->rsRenderPipeline_.lock(), nullptr);
}

/**
 * @tc.name: CleanTest_ForRefreshFalse
 * @tc.desc: Verify Clean with forRefresh=false schedules CleanResources correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, CleanTest_ForRefreshFalse, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    ASSERT_NE(mainThread_, nullptr);
    renderPipeline->mainThread_ = mainThread_;

    pid_t testPid = 12345;
    ASSERT_NE(agent->rsRenderPipeline_.lock(), nullptr);
    ASSERT_NE(agent->rsRenderPipeline_.lock()->mainThread_, nullptr);

    // Execute Clean with forRefresh=false
    agent->Clean(testPid, false);
    // Verify agent and pipeline remain valid after Clean
    EXPECT_NE(agent->rsRenderPipeline_.lock(), nullptr);
    EXPECT_NE(agent->rsRenderPipeline_.lock()->mainThread_, nullptr);
}

/**
 * @tc.name: CleanTest_ForRefreshTrue
 * @tc.desc: Verify Clean with forRefresh=true schedules CleanResources correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, CleanTest_ForRefreshTrue, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    ASSERT_NE(mainThread_, nullptr);
    renderPipeline->mainThread_ = mainThread_;

    pid_t testPid = 12345;
    ASSERT_NE(agent->rsRenderPipeline_.lock(), nullptr);
    ASSERT_NE(agent->rsRenderPipeline_.lock()->mainThread_, nullptr);

    // Execute Clean with forRefresh=true
    agent->Clean(testPid, true);
    // Verify agent and pipeline remain valid after Clean
    EXPECT_NE(agent->rsRenderPipeline_.lock(), nullptr);
    EXPECT_NE(agent->rsRenderPipeline_.lock()->mainThread_, nullptr);
}

/**
 * @tc.name: RemoveConnection001
 * @tc.desc: Verify RemoveConnection when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RemoveConnection001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    auto token = sptr<RSIConnectionToken>();
    auto ret = agent->RemoveConnection(getpid(), token);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: RemoveConnection002
 * @tc.desc: Verify RemoveConnection when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RemoveConnection002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto token = sptr<RSIConnectionToken>();
    auto ret = agent->RemoveConnection(getpid(), token);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: UnRegisterApplicationAgent001
 * @tc.desc: Verify UnRegisterApplicationAgent when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, UnRegisterApplicationAgent001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    auto app = sptr<IApplicationAgent>();
    agent->UnRegisterApplicationAgent(app);
}

/**
 * @tc.name: UnRegisterApplicationAgent002
 * @tc.desc: Verify UnRegisterApplicationAgent when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, UnRegisterApplicationAgent002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto app = sptr<IApplicationAgent>();
    agent->UnRegisterApplicationAgent(app);
}

/**
 * @tc.name: CreateDisplayNode001
 * @tc.desc: Verify CreateDisplayNode when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, CreateDisplayNode001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    RSDisplayNodeConfig displayNodeConfig;
    bool success = true;
    auto ret = agent->CreateDisplayNode(displayNodeConfig, 0, success);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: CreateNode001
 * @tc.desc: Verify CreateNode when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, CreateNode001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    RSSurfaceRenderNodeConfig config;
    bool success = true;
    auto ret = agent->CreateNode(config, success);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: ForceRefreshOneFrameWithNextVSync001
 * @tc.desc: Verify ForceRefreshOneFrameWithNextVSync when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, ForceRefreshOneFrameWithNextVSync001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    agent->ForceRefreshOneFrameWithNextVSync();
}

/**
 * @tc.name: RegisterApplicationAgent001
 * @tc.desc: Verify RegisterApplicationAgent when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RegisterApplicationAgent001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto app = sptr<IApplicationAgent>();
    agent->RegisterApplicationAgent(0, app);
}

/**
 * @tc.name: RegisterApplicationAgent002
 * @tc.desc: Verify RegisterApplicationAgent when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RegisterApplicationAgent002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto app = sptr<IApplicationAgent>();
    agent->RegisterApplicationAgent(0, app);
}

/**
 * @tc.name: RegisterBufferClearListener001
 * @tc.desc: Verify RegisterBufferClearListener when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RegisterBufferClearListener001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSIBufferClearCallback>();
    agent->RegisterBufferClearListener(0, callback);
}

/**
 * @tc.name: RegisterBufferClearListener002
 * @tc.desc: Verify RegisterBufferClearListener when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RegisterBufferClearListener002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSIBufferClearCallback>();
    agent->RegisterBufferClearListener(0, callback);
}

/**
 * @tc.name: RegisterBufferAvailableListener001
 * @tc.desc: Verify RegisterBufferAvailableListener when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RegisterBufferAvailableListener001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSIBufferAvailableCallback>();
    auto ret = agent->RegisterBufferAvailableListener(0, callback, true);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: RegisterBufferAvailableListener002
 * @tc.desc: Verify RegisterBufferAvailableListener when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RegisterBufferAvailableListener002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSIBufferAvailableCallback>();
    auto ret = agent->RegisterBufferAvailableListener(0, callback, true);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetSystemAnimatedScenes001
 * @tc.desc: Verify SetSystemAnimatedScenes when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetSystemAnimatedScenes001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto systemAnimatedScenes = SystemAnimatedScenes();
    bool success = true;
    auto ret = agent->SetSystemAnimatedScenes(systemAnimatedScenes, true, success);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetHardwareEnabled001
 * @tc.desc: Verify SetHardwareEnabled when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetHardwareEnabled001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto selfDrawingType = SelfDrawingNodeType();
    auto ret = agent->SetHardwareEnabled(0, true, selfDrawingType, true);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetHardwareEnabled002
 * @tc.desc: Verify SetHardwareEnabled when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetHardwareEnabled002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto selfDrawingType = SelfDrawingNodeType();
    auto ret = agent->SetHardwareEnabled(0, true, selfDrawingType, true);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetHidePrivacyContent001
 * @tc.desc: Verify SetHidePrivacyContent when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetHidePrivacyContent001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    uint32_t resCode = 0;
    auto ret = agent->SetHidePrivacyContent(0, true, resCode);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetHidePrivacyContent002
 * @tc.desc: Verify SetHidePrivacyContent when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetHidePrivacyContent002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    uint32_t resCode = 0;
    auto ret = agent->SetHidePrivacyContent(0, true, resCode);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetFocusAppInfo001
 * @tc.desc: Verify SetFocusAppInfo when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetFocusAppInfo001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    FocusAppInfo info;
    int32_t repCode = 0;
    auto ret = agent->SetFocusAppInfo(info, repCode);
    EXPECT_EQ(ret, INVALID_ARGUMENTS);
}

/**
 * @tc.name: SetFocusAppInfo002
 * @tc.desc: Verify SetFocusAppInfo when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetFocusAppInfo002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    FocusAppInfo info;
    int32_t repCode = 0;
    auto ret = agent->SetFocusAppInfo(info, repCode);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: TakeSurfaceCapture001
 * @tc.desc: Verify TakeSurfaceCapture when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, TakeSurfaceCapture001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSISurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureBlurParam blurParam;
    Drawing::Rect specifiedAreaRect;
    RSSurfaceCapturePermissions permissions;
    agent->TakeSurfaceCapture(0, callback, captureConfig, blurParam, specifiedAreaRect, permissions);
}

/**
 * @tc.name: TakeSurfaceCapture002
 * @tc.desc: Verify TakeSurfaceCapture when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, TakeSurfaceCapture002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSISurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureBlurParam blurParam;
    Drawing::Rect specifiedAreaRect;
    RSSurfaceCapturePermissions permissions;
    agent->TakeSurfaceCapture(0, callback, captureConfig, blurParam, specifiedAreaRect, permissions);
}

/**
 * @tc.name: TakeSelfSurfaceCapture001
 * @tc.desc: Verify TakeSelfSurfaceCapture when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, TakeSelfSurfaceCapture001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSISurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    agent->TakeSelfSurfaceCapture(0, callback, captureConfig, true);
}

/**
 * @tc.name: TakeSelfSurfaceCapture002
 * @tc.desc: Verify TakeSelfSurfaceCapture when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, TakeSelfSurfaceCapture002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSISurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    agent->TakeSelfSurfaceCapture(0, callback, captureConfig, true);
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindows001
 * @tc.desc: Verify TakeSurfaceCaptureWithAllWindows when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, TakeSurfaceCaptureWithAllWindows001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSISurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCapturePermissions permissions;
    auto ret = agent->TakeSurfaceCaptureWithAllWindows(0, callback, captureConfig, true, permissions);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindows002
 * @tc.desc: Verify TakeSurfaceCaptureWithAllWindows when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, TakeSurfaceCaptureWithAllWindows002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSISurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCapturePermissions permissions;
    auto ret = agent->TakeSurfaceCaptureWithAllWindows(0, callback, captureConfig, true, permissions);
    ASSERT_EQ(ret, ERR_PERMISSION_DENIED);
}

/**
 * @tc.name: FreezeScreen001
 * @tc.desc: Verify FreezeScreen when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, FreezeScreen001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto ret = agent->FreezeScreen(0, true, true);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: FreezeScreen002
 * @tc.desc: Verify FreezeScreen when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, FreezeScreen002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto ret = agent->FreezeScreen(0, true, true);
    ASSERT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetWindowFreezeImmediately001
 * @tc.desc: Verify SetWindowFreezeImmediately when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetWindowFreezeImmediately001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSISurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureBlurParam blurParam;
    agent->SetWindowFreezeImmediately(0, true, callback, captureConfig, blurParam, true);
}

/**
 * @tc.name: SetWindowFreezeImmediately002
 * @tc.desc: Verify SetWindowFreezeImmediately when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetWindowFreezeImmediately002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSISurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureBlurParam blurParam;
    agent->SetWindowFreezeImmediately(0, true, callback, captureConfig, blurParam, true);
}

/**
 * @tc.name: TakeUICaptureInRange001
 * @tc.desc: Verify TakeUICaptureInRange when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, TakeUICaptureInRange001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSISurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCapturePermissions permissions;
    agent->TakeUICaptureInRange(0, callback, captureConfig, permissions);
}

/**
 * @tc.name: TakeUICaptureInRange002
 * @tc.desc: Verify TakeUICaptureInRange when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, TakeUICaptureInRange002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSISurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCapturePermissions permissions;
    agent->TakeUICaptureInRange(0, callback, captureConfig, permissions);
}

/**
 * @tc.name: SetHwcNodeBounds001
 * @tc.desc: Verify SetHwcNodeBounds when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetHwcNodeBounds001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto ret = agent->SetHwcNodeBounds(0, 0, 0, 0, 0);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetHwcNodeBounds002
 * @tc.desc: Verify SetHwcNodeBounds when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetHwcNodeBounds002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto ret = agent->SetHwcNodeBounds(0, 0, 0, 0, 0);
    ASSERT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: GetScreenHDRStatus001
 * @tc.desc: Verify GetScreenHDRStatus when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, GetScreenHDRStatus001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    HdrStatus hdrStatus;
    int32_t resCode = 0;
    auto ret = agent->GetScreenHDRStatus(0, hdrStatus, resCode);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetAncoForceDoDirect001
 * @tc.desc: Verify SetAncoForceDoDirect when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetAncoForceDoDirect001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    bool res = true;
    auto ret = agent->SetAncoForceDoDirect(true, res);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetAncoForceDoDirect002
 * @tc.desc: Verify SetAncoForceDoDirect when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetAncoForceDoDirect002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    bool res = true;
    auto ret = agent->SetAncoForceDoDirect(true, res);
    ASSERT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetLayerTopForHWC001
 * @tc.desc: Verify SetLayerTopForHWC when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetLayerTopForHWC001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto ret = agent->SetLayerTopForHWC(0, true, 0);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetLayerTopForHWC002
 * @tc.desc: Verify SetLayerTopForHWC when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetLayerTopForHWC002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto ret = agent->SetLayerTopForHWC(0, true, 0);
    ASSERT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetWindowContainer001
 * @tc.desc: Verify SetWindowContainer when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetWindowContainer001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    agent->SetWindowContainer(0, true);
}

/**
 * @tc.name: SetWindowContainer002
 * @tc.desc: Verify SetWindowContainer when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetWindowContainer002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    agent->SetWindowContainer(0, true);
}

/**
 * @tc.name: ClearUifirstCache001
 * @tc.desc: Verify ClearUifirstCache when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, ClearUifirstCache001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    agent->ClearUifirstCache(0);
}

/**
 * @tc.name: ClearUifirstCache002
 * @tc.desc: Verify ClearUifirstCache when pipeline isn't null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, ClearUifirstCache002, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    ASSERT_NE(renderPipeline, nullptr);
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    agent->ClearUifirstCache(0);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus001
 * @tc.desc: Verify SetCurtainScreenUsingStatus when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetCurtainScreenUsingStatus001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto ret = agent->SetCurtainScreenUsingStatus(false);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetBitmap001
 * @tc.desc: Verify GetBitmap when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, GetBitmap001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    Drawing::Bitmap bitmap;
    bool success = false;
    auto ret = agent->GetBitmap(0, bitmap, success);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: ReportJankStats001
 * @tc.desc: Verify ReportJankStats when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, ReportJankStats001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto ret = agent->ReportJankStats();
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: ReportEventResponse001
 * @tc.desc: Verify ReportEventResponse when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, ReportEventResponse001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    DataBaseRs info;
    auto ret = agent->ReportEventResponse(info);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: ReportEventComplete001
 * @tc.desc: Verify ReportEventComplete when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, ReportEventComplete001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    DataBaseRs info;
    auto ret = agent->ReportEventComplete(info);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: ReportEventJankFrame001
 * @tc.desc: Verify ReportEventJankFrame when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, ReportEventJankFrame001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    DataBaseRs info;
    auto ret = agent->ReportEventJankFrame(info);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: UpdateAnimationOcclusionStatus001
 * @tc.desc: Verify UpdateAnimationOcclusionStatus when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, UpdateAnimationOcclusionStatus001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    std::string sceneId = "0";
    agent->UpdateAnimationOcclusionStatus(sceneId, false);
}

/**
 * @tc.name: ReportRsSceneJankStart001
 * @tc.desc: Verify ReportRsSceneJankStart when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, ReportRsSceneJankStart001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    AppInfo info;
    auto ret = agent->ReportRsSceneJankStart(info);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: ReportRsSceneJankEnd001
 * @tc.desc: Verify ReportRsSceneJankEnd when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, ReportRsSceneJankEnd001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    AppInfo info;
    auto ret = agent->ReportRsSceneJankEnd(info);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoStop001
 * @tc.desc: Verify AvcodecVideoStop when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, AvcodecVideoStop001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    std::vector<uint64_t> uniqueIdList;
    std::vector<std::string> surfaceNameList;
    auto ret = agent->AvcodecVideoStop(uniqueIdList, surfaceNameList, 0);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoStart001
 * @tc.desc: Verify AvcodecVideoStart when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, AvcodecVideoStart001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    std::vector<uint64_t> uniqueIdList;
    std::vector<std::string> surfaceNameList;
    auto ret = agent->AvcodecVideoStart(uniqueIdList, surfaceNameList, 0, 0);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoGet001
 * @tc.desc: Verify AvcodecVideoGet when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, AvcodecVideoGet001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto ret = agent->AvcodecVideoGet(0);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: AvcodecVideoGetRecent001
 * @tc.desc: Verify AvcodecVideoGetRecent when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, AvcodecVideoGetRecent001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto ret = agent->AvcodecVideoGetRecent();
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetBrightnessInfoChangeCallback001
 * @tc.desc: Verify SetBrightnessInfoChangeCallback when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, SetBrightnessInfoChangeCallback001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSIBrightnessInfoChangeCallback>();
    auto ret = agent->SetBrightnessInfoChangeCallback(0, callback);
    ASSERT_EQ(ret, StatusCode::INVALID_ARGUMENTS);
}

/**
 * @tc.name: RegisterOcclusionChangeCallback001
 * @tc.desc: Verify RegisterOcclusionChangeCallback when pipeline is null
 * @tc.type: FUNC
 * @tc.require:  issue22984
 */
HWTEST_F(RSRenderPipelineAgentTest, RegisterOcclusionChangeCallback001, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    auto callback = sptr<RSIOcclusionChangeCallback>();
    auto ret = agent->RegisterOcclusionChangeCallback(0, callback);
    ASSERT_EQ(ret, StatusCode::INVALID_ARGUMENTS);
}

/**
 * @tc.name: SetHdrForceHwcEnabled_NullPipeline
 * @tc.desc: Test SetHdrForceHwcEnabled when rsRenderPipeline_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderPipelineAgentTest, SetHdrForceHwcEnabled_NullPipeline, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    std::string nodeIdStr = "test_node";
    ErrCode ret = agent->SetHdrForceHwcEnabled(nodeIdStr, true);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetHdrForceHwcEnabled_NullMainThread
 * @tc.desc: Test SetHdrForceHwcEnabled when GetMainThread() returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderPipelineAgentTest, SetHdrForceHwcEnabled_NullMainThread, TestSize.Level1)
{
    auto renderPipeline = std::make_shared<RSRenderPipeline>();
    renderPipeline->mainThread_ = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    std::string nodeIdStr = "test_node";
    ErrCode ret = agent->SetHdrForceHwcEnabled(nodeIdStr, true);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetHdrForceHwcEnabled_NormalCase
 * @tc.desc: Test SetHdrForceHwcEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderPipelineAgentTest, SetHdrForceHwcEnabled_NormalCase, TestSize.Level1)
{
    auto renderPipeline = std::make_shared<RSRenderPipeline>();
    auto mainThread = RSMainThread::Instance();
    renderPipeline->mainThread_ = mainThread;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    std::string nodeIdStr = "test_node";
    ErrCode ret = agent->SetHdrForceHwcEnabled(nodeIdStr, true);
    EXPECT_EQ(ret, ERR_OK);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: SubmitCanvasPreAllocatedBufferTest
 * @tc.desc: SubmitCanvasPreAllocatedBuffer Test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentTest, SubmitCanvasPreAllocatedBufferTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = std::make_shared<RSRenderPipeline>();
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    ASSERT_NE(mainThread_, nullptr);
    renderPipeline->mainThread_ = mainThread_;
    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(false);
    auto ret = agent->SubmitCanvasPreAllocatedBuffer(1, 1, nullptr, 1);
    EXPECT_NE(ret, 0);
    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(true);
    ret = agent->SubmitCanvasPreAllocatedBuffer(1, 1, nullptr, 1);
    EXPECT_NE(ret, 0);
}
#endif
} // namespace OHOS::Rosen
