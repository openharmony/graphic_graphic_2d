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

#include "feature/hyper_graphic_manager/hgm_rp_context.h"
#include "feature/hyper_graphic_manager/rp_hgm_xml_parser.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_surface_render_node.h"
#include "render_server/rs_render_service.h"
#include "render_server/rs_render_process_manager_agent.h"
#include "render_server/rs_render_service_agent.h"
#include "screen_manager/public/rs_screen_manager_agent.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_render_to_service_connection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr const char* HGM_CONFIG_PATH = "/sys_prod/etc/graphic/hgm_policy_config.xml";
}
class HgmRPContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void HgmRPContextTest::SetUpTestCase() {}
void HgmRPContextTest::TearDownTestCase() {}
void HgmRPContextTest::SetUp() {}
void HgmRPContextTest::TearDown() {}

/**
 * @tc.name: InitHgmConfigTest
 * @tc.desc: test HgmRPContext.InitHgmConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPContextTest, InitHgmConfigTest, TestSize.Level1)
{
    sptr<RSIRenderToServiceConnection> renderToServiceConnection = nullptr;
    std::unordered_map<std::string, std::string> sourceTuningConfig;
    std::unordered_map<std::string, std::string> solidLayerConfig;
    std::vector<std::string> appBufferList;
    HgmRPContext hgmRPContext(renderToServiceConnection);
    if (auto xmlDocument_ = xmlReadFile(HGM_CONFIG_PATH, nullptr, 0)) {
        EXPECT_EQ(hgmRPContext.InitHgmConfig(sourceTuningConfig, solidLayerConfig, appBufferList), EXEC_SUCCESS);
    } else {
        EXPECT_EQ(hgmRPContext.InitHgmConfig(sourceTuningConfig, solidLayerConfig, appBufferList), XML_FILE_LOAD_FAIL);
    }
    EXPECT_NE(hgmRPContext.convertFrameRateFunc_, nullptr);
}

/**
 * @tc.name: NotifyRpHgmFrameRateTest
 * @tc.desc: test HgmRPContext.NotifyRpHgmFrameRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPContextTest, NotifyRpHgmFrameRateTest, TestSize.Level1)
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

    HgmRPContext hgmRPContext(renderToServiceConnection);
    auto rsContext = std::make_shared<RSContext>();
    PipelineParam pipelineParam;
    std::unordered_map<NodeId, int> vRateMap = { { 1, 60 }, { 2, 120 } };
    hgmRPContext.rsCurrRange_.Set(0, 120, 60);
    hgmRPContext.UpdateSurfaceData("test", 1);
    rsContext->GetMutableFrameRateLinkerDestroyIds().insert(1);
    FrameRateLinkerUpdateInfo updateInfo = { { 0, 120, 60 }, 120 };
    rsContext->GetMutableFrameRateLinkerUpdateInfoMap().insert_or_assign(2, updateInfo);
    hgmRPContext.NotifyRpHgmFrameRate(100, rsContext, vRateMap, true, pipelineParam);
    EXPECT_EQ(hgmRPContext.rsCurrRange_.preferred_, 0);
    EXPECT_EQ(hgmRPContext.surfaceData_.size(), 0);
    EXPECT_EQ(rsContext->GetFrameRateLinkerDestroyIds().size(), 0);
    EXPECT_EQ(rsContext->GetFrameRateLinkerUpdateInfoMap().size(), 0);
    EXPECT_EQ(hgmRPContext.ltpoEnabled_, true);
    EXPECT_EQ(hgmRPContext.isDelayMode_, true);
    EXPECT_EQ(hgmRPContext.pipelineOffsetPulseNum_, 1);
    EXPECT_EQ(hgmRPContext.isAdaptive_, false);
    EXPECT_EQ(pipelineParam.pendingScreenRefreshRate, 60);
    EXPECT_EQ(pipelineParam.pendingConstraintRelativeTime, 2);
    EXPECT_EQ(renderService.hgmContext_->currVsyncId_, 100);
    EXPECT_EQ(renderService.hgmContext_->rsCurrRange_.preferred_, 60);
}

/**
 * @tc.name: HandleGameNodeTest
 * @tc.desc: test HgmRPContext.HandleGameNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPContextTest, HandleGameNodeTest, Function | SmallTest | Level1)
{
    sptr<RSIRenderToServiceConnection> renderToServiceConnection = nullptr;
    HgmRPContext hgmRPContext(renderToServiceConnection);
    RSRenderNodeMap nodeMap;
    RSSurfaceRenderNodeConfig config;
    hgmRPContext.isAdaptive_ = SupportASStatus::SUPPORT_AS;

    hgmRPContext.HandleGameNode(nodeMap);
    EXPECT_EQ(hgmRPContext.isGameNodeOnTree_, false);

    config.id = 1;
    config.name = "nodeName1";
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode1->SetIsOnTheTree(false);
    nodeMap.RegisterRenderNode(surfaceNode1);
    hgmRPContext.gameNodeName_ = "nodeName1";
    hgmRPContext.HandleGameNode(nodeMap);
    EXPECT_EQ(hgmRPContext.isGameNodeOnTree_, false);

    config.id = 2;
    config.name = "nodeName2";
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode2->SetIsOnTheTree(true);
    nodeMap.RegisterRenderNode(surfaceNode2);
    hgmRPContext.gameNodeName_ = "nodeName2";
    hgmRPContext.HandleGameNode(nodeMap);
    EXPECT_EQ(hgmRPContext.isGameNodeOnTree_, false);

    config.id = 3;
    config.name = "nodeName3";
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    auto surfaceNode3 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode3->SetIsOnTheTree(true);
    nodeMap.RegisterRenderNode(surfaceNode3);
    hgmRPContext.HandleGameNode(nodeMap);
    EXPECT_EQ(hgmRPContext.isGameNodeOnTree_, false);

    hgmRPContext.gameNodeName_ = "nodeName3";
    hgmRPContext.HandleGameNode(nodeMap);
    EXPECT_EQ(hgmRPContext.isGameNodeOnTree_, true);
    
    hgmRPContext.isAdaptive_ = SupportASStatus::NOT_SUPPORT;
    hgmRPContext.HandleGameNode(nodeMap);
    EXPECT_EQ(hgmRPContext.isGameNodeOnTree_, false);
}

/**
 * @tc.name: HandleGameNodeTest002
 * @tc.desc: test HgmRPContext.HandleGameNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPContextTest, HandleGameNodeTest002, Function | SmallTest | Level1)
{
    sptr<RSIRenderToServiceConnection> renderToServiceConnection = nullptr;
    HgmRPContext hgmRPContext(renderToServiceConnection);
    RSRenderNodeMap nodeMap;
    RSSurfaceRenderNodeConfig config;
    hgmRPContext.isAdaptive_ = SupportASStatus::SUPPORT_AS;
    hgmRPContext.gameNodeName_ = "nodeName1";

    config.id = 1;
    config.name = "nodeName1";
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode1->SetIsOnTheTree(true);
    nodeMap.RegisterRenderNode(surfaceNode1);
    hgmRPContext.HandleGameNode(nodeMap);
    EXPECT_EQ(hgmRPContext.isGameNodeOnTree_, true);
    
    config.id = 2;
    config.name = "windowName";
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    auto windowRegion = Occlusion::Region(Occlusion::Rect{0, 0, 1000, 1000});
    surfaceNode2->SetVisibleRegion(windowRegion);
    surfaceNode2->SetIsOnTheTree(true);
    nodeMap.RegisterRenderNode(surfaceNode2);

    config.id = 3;
    config.name = "nodeName3";
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    auto surfaceNode3 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode3->SetIsOnTheTree(true, 2);
    nodeMap.RegisterRenderNode(surfaceNode3);
    hgmRPContext.HandleGameNode(nodeMap);
    EXPECT_EQ(hgmRPContext.isGameNodeOnTree_, false);
}

/**
 * @tc.name: TestSetServiceToProcessInfo
 * @tc.desc: test HgmRPContext.SetServiceToProcessInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPContextTest, TestSetServiceToProcessInfo, TestSize.Level1)
{
    sptr<RSIRenderToServiceConnection> renderToServiceConnection = nullptr;
    HgmRPContext hgmRPContext(renderToServiceConnection);
    uint32_t refreshRate = 0;
    uint64_t relativeTime = 0;
    hgmRPContext.SetServiceToProcessInfo(nullptr, refreshRate, relativeTime);
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
    hgmRPContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(refreshRate, 60);
    EXPECT_EQ(hgmRPContext.isAdaptive_, false);
    EXPECT_EQ(hgmRPContext.ltpoEnabled_, false);
    EXPECT_EQ(hgmRPContext.hgmRPEnergy_->isTouchIdle_, true);

    serviceToProcessInfo->hgmDataChangeTypes.set(HgmDataChangeType::ADAPTIVE_VSYNC);
    hgmRPContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(hgmRPContext.isAdaptive_, true);

    serviceToProcessInfo->hgmDataChangeTypes.set(HgmDataChangeType::HGM_CONFIG_DATA);
    hgmRPContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(hgmRPContext.ltpoEnabled_, true);
}
} // namespace OHOS::Rosen