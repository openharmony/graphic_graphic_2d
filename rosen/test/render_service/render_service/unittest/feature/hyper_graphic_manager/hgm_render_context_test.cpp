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

#include "gtest/gtest.h"

#include "feature/hyper_graphic_manager/hgm_render_context.h"
#include "feature/hyper_graphic_manager/rp_hgm_xml_parser.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_surface_render_node.h"
#include "render_server/rs_render_process_manager_agent.h"
#include "render_server/rs_render_service.h"
#include "render_server/rs_render_service_agent.h"
#include "screen_manager/public/rs_screen_manager_agent.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_render_to_service_connection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t delay_110Ms = 110;
constexpr const char* HGM_CONFIG_PATH = "/sys_prod/etc/graphic/hgm_policy_config.xml";
}
class HgmRenderContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void HgmRenderContextTest::SetUpTestCase() {}
void HgmRenderContextTest::TearDownTestCase() {}
void HgmRenderContextTest::SetUp() {}
void HgmRenderContextTest::TearDown() {}

/**
 * @tc.name: InitHgmConfigTest
 * @tc.desc: test HgmRenderContext.InitHgmConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRenderContextTest, InitHgmConfigTest, TestSize.Level1)
{
    sptr<RSIRenderToServiceConnection> renderToServiceConnection = nullptr;
    std::unordered_map<std::string, std::string> sourceTuningConfig;
    std::unordered_map<std::string, std::string> solidLayerConfig;
    std::vector<std::string> appBufferList;
    HgmRenderContext hgmRenderContext(renderToServiceConnection);
    if (auto xmlDocument_ = xmlReadFile(HGM_CONFIG_PATH, nullptr, 0)) {
        EXPECT_EQ(hgmRenderContext.InitHgmConfig(sourceTuningConfig, solidLayerConfig, appBufferList), EXEC_SUCCESS);
    } else {
        EXPECT_EQ(
            hgmRenderContext.InitHgmConfig(sourceTuningConfig, solidLayerConfig, appBufferList), XML_FILE_LOAD_FAIL);
    }
    EXPECT_NE(hgmRenderContext.convertFrameRateFunc_, nullptr);
}

/**
 * @tc.name: NotifyRpHgmFrameRateTest
 * @tc.desc: test HgmRenderContext.NotifyRpHgmFrameRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRenderContextTest, NotifyRpHgmFrameRateTest, TestSize.Level1)
{
    RSRenderService renderService;
    auto& hgmCore = HgmCore::Instance();
    auto mgr = hgmCore.GetFrameRateMgr();
    if (mgr == nullptr) {
        mgr = std::make_unique<HgmFrameRateManager>();
    }
    auto rsDistributor = sptr<VSyncDistributor>::MakeSptr(nullptr, "rs");
    auto appDistributor = sptr<VSyncDistributor>::MakeSptr(nullptr, "app");
    renderService.hgmContext_ = std::make_shared<HgmContext>(nullptr, mgr, nullptr, appDistributor, rsDistributor);
    auto renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderService);
    auto renderProcessManagerAgent = sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService.renderProcessManager_);
    auto screenManagerAgent = sptr<RSScreenManagerAgent>::MakeSptr(renderService.screenManager_);
    auto renderToServiceConnection =
        sptr<RSRenderToServiceConnection>::MakeSptr(renderServiceAgent, renderProcessManagerAgent, screenManagerAgent);
    renderService.hgmContext_->hgmDataChangeTypes_.set(HgmDataChangeType::HGM_CONFIG_DATA);
    renderService.hgmContext_->ltpoEnabled_ = true;
    renderService.hgmContext_->isDelayMode_ = true;
    renderService.hgmContext_->pipelineOffsetPulseNum_ = 1;
    renderService.hgmContext_->isAdaptive_ = true;
    renderService.hgmContext_->gameNodeName_ = "gameNodeName";
    hgmCore.SetPendingScreenRefreshRate(60);
    hgmCore.SetPendingConstraintRelativeTime(2);

    HgmRenderContext hgmRenderContext(renderToServiceConnection);
    auto rsContext = std::make_shared<RSContext>();
    PipelineParam pipelineParam;
    std::unordered_map<NodeId, int> vRateMap = { { 1, 60 }, { 2, 120 } };
    hgmRenderContext.rsCurrRange_.Set(0, 120, 60);
    hgmRenderContext.surfaceData_.emplace_back(std::tuple<std::string, pid_t>({"test", 1}));
    rsContext->GetMutableFrameRateLinkerDestroyIds().insert(1);
    FrameRateLinkerUpdateInfo updateInfo = { { 0, 120, 60 }, 120 };
    rsContext->GetMutableFrameRateLinkerUpdateInfoMap().insert_or_assign(2, updateInfo);
    hgmRenderContext.NotifyRpHgmFrameRate(100, rsContext, vRateMap, true, pipelineParam);
    EXPECT_TRUE(hgmRenderContext.surfaceData_.empty());
    EXPECT_TRUE(rsContext->GetFrameRateLinkerDestroyIds().empty());
    EXPECT_TRUE(rsContext->GetFrameRateLinkerUpdateInfoMap().empty());
    EXPECT_EQ(hgmRenderContext.rsCurrRange_.preferred_, 0);
    EXPECT_EQ(hgmRenderContext.ltpoEnabled_, true);
    EXPECT_EQ(hgmRenderContext.isDelayMode_, true);
    EXPECT_EQ(hgmRenderContext.pipelineOffsetPulseNum_, 1);
    EXPECT_EQ(hgmRenderContext.isAdaptive_, false);
    EXPECT_EQ(pipelineParam.pendingScreenRefreshRate, 60);
    EXPECT_EQ(pipelineParam.pendingConstraintRelativeTime, 2);
    EXPECT_EQ(renderService.hgmContext_->currVsyncId_, 100);
    EXPECT_EQ(renderService.hgmContext_->rsCurrRange_.preferred_, 60);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    mgr->rsFrameRateLinker_ = nullptr;
}

/**
 * @tc.name: HandleAdaptiveVsyncConditionTest
 * @tc.desc: test HgmRenderContext.HandleAdaptiveVsyncCondition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRenderContextTest, HandleAdaptiveVsyncConditionTest, Function | SmallTest | Level1)
{
    sptr<RSIRenderToServiceConnection> renderToServiceConnection = nullptr;
    HgmRenderContext hgmRenderContext(renderToServiceConnection);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    hgmRenderContext.isAdaptive_ = SupportASStatus::SUPPORT_AS;

    hgmRenderContext.HandleAdaptiveVsyncCondition(rsContext);
    EXPECT_EQ(hgmRenderContext.isGameNodeOnTree_, false);

    config.id = 1;
    config.name = "nodeName1";
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode1->SetIsOnTheTree(false);
    rsContext->GetMutableNodeMap().RegisterRenderNode(surfaceNode1);
    hgmRenderContext.gameNodeName_ = "nodeName1";
    hgmRenderContext.HandleAdaptiveVsyncCondition(rsContext);
    EXPECT_EQ(hgmRenderContext.isGameNodeOnTree_, false);

    config.id = 2;
    config.name = "nodeName2";
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode2->SetIsOnTheTree(true);
    rsContext->GetMutableNodeMap().RegisterRenderNode(surfaceNode2);
    hgmRenderContext.gameNodeName_ = "nodeName2";
    hgmRenderContext.HandleAdaptiveVsyncCondition(rsContext);
    EXPECT_EQ(hgmRenderContext.isGameNodeOnTree_, false);

    config.id = 3;
    config.name = "nodeName3";
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    auto surfaceNode3 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode3->SetIsOnTheTree(true);
    rsContext->GetMutableNodeMap().RegisterRenderNode(surfaceNode3);
    hgmRenderContext.HandleAdaptiveVsyncCondition(rsContext);
    EXPECT_EQ(hgmRenderContext.isGameNodeOnTree_, false);

    hgmRenderContext.gameNodeName_ = "nodeName3";
    hgmRenderContext.HandleAdaptiveVsyncCondition(rsContext);
    EXPECT_EQ(hgmRenderContext.isGameNodeOnTree_, true);
    
    hgmRenderContext.isAdaptive_ = SupportASStatus::NOT_SUPPORT;
    hgmRenderContext.HandleAdaptiveVsyncCondition(rsContext);
    EXPECT_EQ(hgmRenderContext.isGameNodeOnTree_, false);
}

/**
 * @tc.name: HandleAdaptiveVsyncConditionTest002
 * @tc.desc: test HgmRenderContext.HandleAdaptiveVsyncCondition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRenderContextTest, HandleAdaptiveVsyncConditionTest002, Function | SmallTest | Level1)
{
    sptr<RSIRenderToServiceConnection> renderToServiceConnection = nullptr;
    HgmRenderContext hgmRenderContext(renderToServiceConnection);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    hgmRenderContext.isAdaptive_ = SupportASStatus::SUPPORT_AS;
    hgmRenderContext.gameNodeName_ = "nodeName1";

    config.id = 1;
    config.name = "nodeName1";
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode1->SetIsOnTheTree(true);
    rsContext->GetMutableNodeMap().RegisterRenderNode(surfaceNode1);
    hgmRenderContext.HandleAdaptiveVsyncCondition(rsContext);
    EXPECT_EQ(hgmRenderContext.isGameNodeOnTree_, true);
    
    config.id = 2;
    config.name = "windowName";
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    auto windowRegion = Occlusion::Region(Occlusion::Rect{0, 0, 1000, 1000});
    surfaceNode2->SetVisibleRegion(windowRegion);
    surfaceNode2->SetIsOnTheTree(true);
    rsContext->GetMutableNodeMap().RegisterRenderNode(surfaceNode2);

    config.id = 3;
    config.name = "nodeName3";
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    auto surfaceNode3 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode3->SetIsOnTheTree(true, 2);
    rsContext->GetMutableNodeMap().RegisterRenderNode(surfaceNode3);
    hgmRenderContext.HandleAdaptiveVsyncCondition(rsContext);
    EXPECT_EQ(hgmRenderContext.isGameNodeOnTree_, false);
}

/**
 * @tc.name: SetServiceToProcessInfoTest
 * @tc.desc: test HgmRenderContext.SetServiceToProcessInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRenderContextTest, SetServiceToProcessInfoTest, TestSize.Level1)
{
    sptr<RSIRenderToServiceConnection> renderToServiceConnection = nullptr;
    HgmRenderContext hgmRenderContext(renderToServiceConnection);
    uint32_t refreshRate = 0;
    uint64_t relativeTime = 0;
    hgmRenderContext.SetServiceToProcessInfo(nullptr, refreshRate, relativeTime);
    EXPECT_EQ(refreshRate, 0);

    sptr<HgmServiceToProcessInfo> serviceToProcessInfo = sptr<HgmServiceToProcessInfo>::MakeSptr();
    serviceToProcessInfo->pendingScreenRefreshRate = 60;
    serviceToProcessInfo->pendingConstraintRelativeTime = 1;
    serviceToProcessInfo->ltpoEnabled = true;
    serviceToProcessInfo->isDelayMode = true;
    serviceToProcessInfo->pipelineOffsetPulseNum = 1;
    serviceToProcessInfo->isAdaptive = true;
    serviceToProcessInfo->gameNodeName = "gameNodeName";
    serviceToProcessInfo->isPowerIdle = true;
    hgmRenderContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(refreshRate, 60);
    EXPECT_EQ(hgmRenderContext.isAdaptive_, false);
    EXPECT_EQ(hgmRenderContext.ltpoEnabled_, false);
    EXPECT_EQ(hgmRenderContext.hgmRPEnergy_->isTouchIdle_, true);

    serviceToProcessInfo->hgmDataChangeTypes.set(HgmDataChangeType::ADAPTIVE_VSYNC);
    hgmRenderContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(hgmRenderContext.isAdaptive_, true);

    serviceToProcessInfo->hgmDataChangeTypes.set(HgmDataChangeType::HGM_CONFIG_DATA);
    hgmRenderContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(hgmRenderContext.ltpoEnabled_, true);
}

/**
 * @tc.name: UpdateSurfaceData001
 * @tc.desc: Test UpdateSurfaceData with xweb framework node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRenderContextTest, UpdateSurfaceData001, TestSize.Level1)
{
    std::string frameworkType = "oh_xweb_1";
    sptr<RSIRenderToServiceConnection> renderToServiceConnection = nullptr;
    HgmRenderContext hgmRenderContext(renderToServiceConnection);

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(MakeNodeId(1, 1));
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(MakeNodeId(2, 2));
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode->GetRSSurfaceHandler()->SetConsumer(IConsumerSurface::Create("SurfaceNode"));

    auto surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler();
    auto surfaceHandler2 = surfaceNode2->GetMutableRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    ASSERT_NE(surfaceHandler2, nullptr);
    surfaceHandler2->consumer_ = nullptr;
    hgmRenderContext.surfaceData_.clear();

    hgmRenderContext.UpdateSurfaceData(surfaceHandler2, surfaceNode2);
    EXPECT_EQ(hgmRenderContext.surfaceData_.size(), 0);

    auto consumer = surfaceHandler->GetConsumer();
    ASSERT_NE(consumer, nullptr);

    consumer->SetSurfaceSourceType(OH_SURFACE_SOURCE_GAME);
    hgmRenderContext.UpdateSurfaceData(surfaceHandler, surfaceNode);
    EXPECT_EQ(hgmRenderContext.surfaceData_.size(), 0);

    consumer->SetSurfaceSourceType(OH_SURFACE_SOURCE_CAMERA);
    hgmRenderContext.UpdateSurfaceData(surfaceHandler, surfaceNode);
    EXPECT_EQ(hgmRenderContext.surfaceData_.size(), 0);

    consumer->SetSurfaceSourceType(OH_SURFACE_SOURCE_VIDEO);
    hgmRenderContext.UpdateSurfaceData(surfaceHandler, surfaceNode);
    EXPECT_EQ(hgmRenderContext.surfaceData_.size(), 0);

    consumer->SetSurfaceSourceType(OH_SURFACE_SOURCE_DEFAULT);
    hgmRenderContext.UpdateSurfaceData(surfaceHandler, surfaceNode);
    EXPECT_EQ(hgmRenderContext.surfaceData_.size(), 1);
    const auto& [surfaceName1, id1] = hgmRenderContext.surfaceData_[hgmRenderContext.surfaceData_.size() - 1];
    EXPECT_NE(surfaceName1, frameworkType);

    consumer->SetSurfaceAppFrameworkType(frameworkType);
    hgmRenderContext.UpdateSurfaceData(surfaceHandler, surfaceNode);
    EXPECT_EQ(hgmRenderContext.surfaceData_.size(), 2);
    const auto& [surfaceName2, id2] = hgmRenderContext.surfaceData_[hgmRenderContext.surfaceData_.size() - 1];
    EXPECT_EQ(surfaceName2, frameworkType);
}
} // namespace OHOS::Rosen