/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <test_header.h>
#include <climits>

#include "common/rs_common_def.h"
#include "hgm_config_callback_manager.h"
#include "hgm_frame_rate_manager.h"
#include "hgm_idle_detector.h"
#include "hgm_test_base.h"
#include "ipc_callbacks/rs_frame_rate_linker_expected_fps_update_callback_stub.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_render_service_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string otherSurface = "Other_SF";
    const std::string settingStrategyName = "99";
    const int32_t HGM_REFRESHRATE_MODE_HIGH = 2;
    constexpr uint64_t  currTime = 200000000;
    constexpr uint64_t  lastTime = 100000000;
    constexpr pid_t appPid = 0;
    constexpr pid_t pid = 1;
    constexpr uint32_t touchCount = 1;
    constexpr uint32_t delay_60Ms = 60;
    constexpr uint32_t delay_110Ms = 110;
    constexpr uint32_t lastTouchUpExpectFps = 90;
    constexpr int32_t OLED_72_HZ = 72;
    constexpr int32_t OLED_50_HZ = 50;
    constexpr int32_t OLED_80_HZ = 80;
    ScreenSize screenSize = {720, 1080, 685, 1218}; // width, height, phyWidth, phyHeight
    constexpr int32_t internalScreenId = 5;
    constexpr int32_t externalScreenId = 0;
    constexpr int32_t frameRateLinkerId1 = 1;
    constexpr int32_t frameRateLinkerId2 = 2;
    constexpr int32_t errorVelocity = -1;
    constexpr int32_t strategy3 = 3;
    constexpr int32_t maxSize = 25;
    const std::string testScene = "TestScene";
    constexpr int32_t STYLUS_LINK_WRITE = 2;
    const GraphicIRect rectF {
        .x = 0,
        .y = 0,
        .w = 2232,
        .h = 1008,
    };
    const GraphicIRect rectM {
        .x = 0,
        .y = 1136,
        .w = 2232,
        .h = 2048,
    };
    const GraphicIRect rectG {
        .x = 0,
        .y = 0,
        .w = 2232,
        .h = 3184,
    };
    const GraphicIRect rectNeg {
        .x = -1,
        .y = -1,
        .w = -1,
        .h = -1,
    };
    const GraphicIRect rectMax {
        .x = INT_MAX,
        .y = INT_MAX,
        .w = INT_MAX,
        .h = INT_MAX,
    };
}

class MyCustomFrameRateLinkerExpectedFpsUpdateCallback : public RSFrameRateLinkerExpectedFpsUpdateCallbackStub {
public:
    explicit MyCustomFrameRateLinkerExpectedFpsUpdateCallback(
        const FrameRateLinkerExpectedFpsUpdateCallback& callback) : cb_(callback) {}
    ~MyCustomFrameRateLinkerExpectedFpsUpdateCallback() override {};

    void OnFrameRateLinkerExpectedFpsUpdate(pid_t dstPid, int32_t expectedFps) override
    {
        ROSEN_LOGD("MyCustomFrameRateLinkerExpectedFpsUpdateCallback::OnFrameRateLinkerExpectedFpsUpdate called,"
            " pid=%{public}d, fps=%{public}d", dstPid, expectedFps);
        if (cb_ != nullptr) {
            cb_(dstPid, expectedFps);
        }
    }

private:
    FrameRateLinkerExpectedFpsUpdateCallback cb_;
};

class HgmFrameRateMgrTest : public HgmTestBase {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void InitHgmFrameRateManager(HgmFrameRateManager &frameRateMgr);
};

void HgmFrameRateMgrTest::SetUpTestCase()
{
    HgmTestBase::SetUpTestCase();
}
void HgmFrameRateMgrTest::TearDownTestCase() {}
void HgmFrameRateMgrTest::SetUp() {}
void HgmFrameRateMgrTest::TearDown() {}

class CustomHgmCallback : public IRemoteStub<RSIHgmConfigChangeCallback> {
public:
    explicit CustomHgmCallback() {}
    ~CustomHgmCallback() override {};

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override {}
    void OnHgmRefreshRateModeChanged(int32_t refreshRateModeName) override {}
    void OnHgmRefreshRateUpdate(int32_t refreshRateUpdate) override {}
};

/**
 * @tc.name: MergeRangeByPriority
 * @tc.desc: Verify the result of MergeRangeByPriority function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, MergeRangeByPriority, Function | SmallTest | Level1)
{
    VoteRange voteRange0 = { OLED_40_HZ, OLED_120_HZ };
    VoteRange voteRange1 = { OLED_30_HZ, OLED_40_HZ };
    VoteRange voteRange2 = { OLED_60_HZ, OLED_90_HZ };
    VoteRange voteRange3 = { OLED_120_HZ, OLED_144_HZ };
    VoteRange voteRange4 = { OLED_30_HZ, OLED_144_HZ };
    VoteRange voteRangeRes;

    voteRangeRes = voteRange0;
    HgmFrameRateManager::MergeRangeByPriority(voteRangeRes, voteRange1);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_40_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_40_HZ);

    voteRangeRes = voteRange0;
    HgmFrameRateManager::MergeRangeByPriority(voteRangeRes, voteRange2);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_60_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_90_HZ);

    voteRangeRes = voteRange0;
    HgmFrameRateManager::MergeRangeByPriority(voteRangeRes, voteRange3);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_120_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_120_HZ);

    voteRangeRes = voteRange0;
    HgmFrameRateManager::MergeRangeByPriority(voteRangeRes, voteRange4);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_40_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_120_HZ);
}

void HgmFrameRateMgrTest::InitHgmFrameRateManager(HgmFrameRateManager &frameRateMgr)
{
    int64_t offset = 0;
    auto& hgmCore = HgmCore::Instance();
    auto vsyncGenerator = CreateVSyncGenerator();
    sptr<Rosen::VSyncController> rsController = new VSyncController(vsyncGenerator, offset);
    sptr<Rosen::VSyncController> appController = new VSyncController(vsyncGenerator, offset);
    auto& configData = hgmCore.GetPolicyConfigData();
    configData->xmlCompatibleMode_ = true;
    frameRateMgr.curScreenStrategyId_ = "";
    frameRateMgr.curRefreshRateMode_ = 0;
    frameRateMgr.Init(nullptr, nullptr, nullptr);
    frameRateMgr.Init(rsController, appController, vsyncGenerator);

    auto strategyConfigs = frameRateMgr.multiAppStrategy_.GetStrategyConfigs();
    auto screenSetting = frameRateMgr.multiAppStrategy_.GetScreenSetting();
    frameRateMgr.HandleAppStrategyConfigEvent(DEFAULT_PID, "", {});
    strategyConfigs[settingStrategyName] = { .min = OLED_NULL_HZ, .max = OLED_120_HZ, .down = OLED_144_HZ,
        .dynamicMode = DynamicModeType::TOUCH_ENABLED, .pointerMode = PointerModeType::POINTER_ENABLED,
        .isFactor = true };
    screenSetting.strategy = settingStrategyName;
    frameRateMgr.multiAppStrategy_.SetStrategyConfigs(strategyConfigs);
    frameRateMgr.multiAppStrategy_.SetScreenSetting(screenSetting);
    for (int i = 0; i < maxSize; ++i) {
        frameRateMgr.frameRateVoteInfoVec_.emplace_back(i, VoteInfo{});
    }
    frameRateMgr.ReportHiSysEvent({ .extInfo = "ON" });
}

/**
 * @tc.name: HandleGameNodeTest
 * @tc.desc: Verify the result of HandleGameNodeTest function
 * @tc.type: FUNC
 * @tc.require:
 */
 HWTEST_F(HgmFrameRateMgrTest, HandleGameNodeTest, Function | SmallTest | Level1)
 {
    HgmFrameRateManager frameRateMgr;
    frameRateMgr.curGameNodeName_ = "gameNode";
    RSRenderNodeMap nodeMap;
    RSSurfaceRenderNodeConfig config;

    PART("HandleGameNodeTest") {
        STEP("1. Test empty surfaceMap") {
            frameRateMgr.HandleGameNode(nodeMap);
            ASSERT_EQ(frameRateMgr.isGameNodeOnTree_.load(), false);
        }
        STEP("2. Test with a normal surfaceNode on tree") {
            config.id = 1;
            config.name = "normalNode";
            auto normalNode = std::make_shared<RSSurfaceRenderNode>(config);
            normalNode->SetIsOnTheTree(true);
            nodeMap.RegisterRenderNode(normalNode);
            frameRateMgr.HandleGameNode(nodeMap);
            ASSERT_EQ(frameRateMgr.isGameNodeOnTree_.load(), false);
        }
        STEP("3. Test with a game surfaceNode not on tree") {
            config.id = 2;
            config.name = "gameNode";
            config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
            auto gameNode1 = std::make_shared<RSSurfaceRenderNode>(config);
            gameNode1->SetIsOnTheTree(false);
            nodeMap.RegisterRenderNode(gameNode1);
            frameRateMgr.HandleGameNode(nodeMap);
            ASSERT_EQ(frameRateMgr.isGameNodeOnTree_.load(), false);
        }
        STEP("4. Test with a game surfaceNode on tree") {
            config.id = 3;
            auto gameNode2 = std::make_shared<RSSurfaceRenderNode>(config);
            gameNode2->SetIsOnTheTree(true);
            nodeMap.RegisterRenderNode(gameNode2);
            frameRateMgr.HandleGameNode(nodeMap);
            ASSERT_EQ(frameRateMgr.isGameNodeOnTree_.load(), true);
        }
        STEP("5. Test with a game surfaceNode on tree and other self node on tree") {
            config.id = 4;
            config.name = "other";
            auto otherNode = std::make_shared<RSSurfaceRenderNode>(config);
            otherNode->SetIsOnTheTree(true);
            nodeMap.RegisterRenderNode(otherNode);
            frameRateMgr.HandleGameNode(nodeMap);
            ASSERT_EQ(frameRateMgr.isGameNodeOnTree_.load(), false);
        }
    }
    sleep(1);
}

/**
 * @tc.name: HgmUiFrameworkDirtyNodeTest
 * @tc.desc: Verify the result of HgmUiFrameworkDirtyNodeTest function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmUiFrameworkDirtyNodeTest, Function | SmallTest | Level1)
{
    HgmFrameRateManager frameRateMgr;
    std::vector<std::weak_ptr<RSRenderNode>> uiFwkDirtyNodes;
    PART("HgmUiFrameworkDirtyNodeTest") {
        STEP("1. Test empty uiFwkDirtyNodes") {
            ASSERT_EQ(uiFwkDirtyNodes.size(), 0);
            frameRateMgr.UpdateUIFrameworkDirtyNodes(uiFwkDirtyNodes, 0);
            frameRateMgr.voterTouchEffective_ = true;
            {
                std::shared_ptr<RSRenderNode> renderNode1 = std::make_shared<RSRenderNode>(0);
                uiFwkDirtyNodes.emplace_back(renderNode1);
                ASSERT_EQ(uiFwkDirtyNodes.size(), 1);
            }
            frameRateMgr.UpdateUIFrameworkDirtyNodes(uiFwkDirtyNodes, 0);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 0);
        }
        STEP("2. Test uiFwkDirtyNodes with a clean renderNode") {
            std::shared_ptr<RSRenderNode> renderNode2 = std::make_shared<RSRenderNode>(0);
            uiFwkDirtyNodes.emplace_back(renderNode2);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 1);
            ASSERT_EQ(renderNode2->IsDirty(), false);
            frameRateMgr.UpdateUIFrameworkDirtyNodes(uiFwkDirtyNodes, 0);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 1);
        }
        STEP("3. Test uiFwkDirtyNodes with a dirty renderNode") {
            std::shared_ptr<RSRenderNode> renderNode3 = std::make_shared<RSRenderNode>(0);
            uiFwkDirtyNodes.emplace_back(renderNode3);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 2);

            frameRateMgr.UpdateUIFrameworkDirtyNodes(uiFwkDirtyNodes, 0);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 1);

            renderNode3->SetDirty();
            ASSERT_EQ(renderNode3->IsDirty(), true);
            frameRateMgr.UpdateUIFrameworkDirtyNodes(uiFwkDirtyNodes, 0);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 1);
        }
        STEP("4. other branch") {
            frameRateMgr.surfaceData_.emplace_back(std::tuple<std::string, pid_t, UIFWKType>());
            frameRateMgr.UpdateUIFrameworkDirtyNodes(uiFwkDirtyNodes, 0);
            frameRateMgr.voterGamesEffective_ = true;
            frameRateMgr.UpdateUIFrameworkDirtyNodes(uiFwkDirtyNodes, 0);
            frameRateMgr.voterTouchEffective_ = false;
            frameRateMgr.UpdateUIFrameworkDirtyNodes(uiFwkDirtyNodes, 0);
        }
    }
    sleep(1);
}

/**
 * @tc.name: ProcessPendingRefreshRate
 * @tc.desc: Verify the result of ProcessPendingRefreshRate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, ProcessPendingRefreshRate, Function | SmallTest | Level1)
{
    HgmFrameRateManager frameRateMgr;
    std::vector<std::weak_ptr<RSRenderNode>> uiFwkDirtyNodes;
    bool disableSafeVote = true;
    frameRateMgr.multiAppStrategy_.SetDisableSafeVoteValue(disableSafeVote);

    frameRateMgr.UpdateUIFrameworkDirtyNodes(uiFwkDirtyNodes, 0);
    FrameRateLinkerMap appFrameRateLinkers;
    const std::map<uint64_t, int> vRatesMap;
    frameRateMgr.UniProcessDataForLtpo(currTime, frameRateMgr.rsFrameRateLinker_,
        appFrameRateLinkers, vRatesMap);
    auto& hgmCore = HgmCore::Instance();
    hgmCore.ltpoEnabled_ = false;
    FrameRateLinkerId id;
    frameRateMgr.rsFrameRateLinker_  = std::make_shared<RSRenderFrameRateLinker>(id);

    frameRateMgr.appFrameRateLinkers_.clear();
    frameRateMgr.appFrameRateLinkers_.try_emplace(id, nullptr);
    frameRateMgr.UpdateSoftVSync(true);
    frameRateMgr.appFrameRateLinkers_.try_emplace(id, frameRateMgr.rsFrameRateLinker_);
    frameRateMgr.UpdateSoftVSync(true);
    frameRateMgr.currRefreshRate_.store(OLED_90_HZ);
    frameRateMgr.UpdateSoftVSync(true);
    int64_t vsyncid = 0;
    uint32_t rsRate = 0;
    bool isUiDvsyncOn = false;
    frameRateMgr.pendingRefreshRate_ = nullptr;
    frameRateMgr.ProcessPendingRefreshRate(currTime, vsyncid, rsRate, isUiDvsyncOn);
    frameRateMgr.lastPendingConstraintRelativeTime_ = currTime;
    frameRateMgr.ProcessPendingRefreshRate(currTime, vsyncid, rsRate, isUiDvsyncOn);
    ASSERT_EQ(hgmCore.pendingConstraintRelativeTime_,
        frameRateMgr.lastPendingConstraintRelativeTime_);
    uint32_t pendingRefreshRate = OLED_60_HZ;
    frameRateMgr.pendingRefreshRate_ = std::make_shared<uint32_t>(pendingRefreshRate);
    frameRateMgr.ProcessPendingRefreshRate(currTime, vsyncid, rsRate, isUiDvsyncOn);
    ASSERT_EQ(frameRateMgr.pendingConstraintRelativeTime_, 0);
    ASSERT_EQ(frameRateMgr.lastPendingRefreshRate_, OLED_60_HZ);
    frameRateMgr.pendingRefreshRate_ = nullptr;
    frameRateMgr.ProcessPendingRefreshRate(currTime, vsyncid, rsRate, isUiDvsyncOn);
    frameRateMgr.voterTouchEffective_ = true;
    std::string surfaceName = "surface0";
    size_t oldSize = frameRateMgr.surfaceData_.size();
    frameRateMgr.UpdateSurfaceTime(surfaceName, pid, UIFWKType::FROM_UNKNOWN);
    EXPECT_GE(frameRateMgr.surfaceData_.size(), oldSize);
}

/**
 * @tc.name: HgmConfigCallbackManagerTest001
 * @tc.desc: Verify the result of HgmConfigCallbackManagerTest001 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmConfigCallbackManagerTest001, Function | SmallTest | Level1)
{
    sptr<HgmConfigCallbackManager> hccMgr = HgmConfigCallbackManager::GetInstance();
    PART("HgmConfigCallbackManagerTest") {
        STEP("1. Callback is nullptr") {
            sptr<CustomHgmCallback> cb1 = new CustomHgmCallback();
            hccMgr->RegisterHgmRefreshRateModeChangeCallback(0, nullptr);
            hccMgr->RegisterHgmRefreshRateModeChangeCallback(1, cb1);
        }
        STEP("2. Test SyncHgmConfigChangeCallback without callback") {
            std::unordered_map<pid_t, sptr<RSIHgmConfigChangeCallback>> emptyCallback;
            std::swap(hccMgr->animDynamicCfgCallbacks_, emptyCallback);
            ASSERT_EQ(hccMgr->animDynamicCfgCallbacks_.empty(), true);
            hccMgr->SyncHgmConfigChangeCallback();
        }
    }
}

/**
 * @tc.name: HgmConfigCallbackManagerTest002
 * @tc.desc: Verify the result of HgmConfigCallbackManagerTest002 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmConfigCallbackManagerTest002, Function | SmallTest | Level1)
{
    sptr<HgmConfigCallbackManager> hccMgr = HgmConfigCallbackManager::GetInstance();
    PART("HgmConfigCallbackManagerTest") {
        STEP("1. Test SyncCallback function with callback") {
            sptr<CustomHgmCallback> cb = new CustomHgmCallback();
            hccMgr->animDynamicCfgCallbacks_[0] = cb;
            hccMgr->refreshRateModeCallbacks_[0] = cb;
            hccMgr->SyncHgmConfigChangeCallback();
            hccMgr->SyncRefreshRateModeChangeCallback(0);
            hccMgr->RegisterHgmConfigChangeCallback(0, nullptr);
            auto& hgmCore = HgmCore::Instance();
            auto& configData = hgmCore.GetPolicyConfigData();
            configData->pageNameList_.push_back("com.app10");
            ScreenId id = 0;
            int32_t mode = 0;
            sptr<HgmScreen> screen = new HgmScreen(id, mode, screenSize);
            hgmCore.screenList_.push_back(screen);
            hccMgr->RegisterHgmConfigChangeCallback(pid, cb);
            hccMgr->RegisterHgmRefreshRateUpdateCallback(0, nullptr);
            hccMgr->RegisterHgmRefreshRateUpdateCallback(pid, cb);
            hccMgr->refreshRateUpdateCallbacks_.try_emplace(0, cb);
            hccMgr->RegisterHgmRefreshRateUpdateCallback(0, nullptr);
            hccMgr->RegisterHgmRefreshRateUpdateCallback(pid, cb);
            hccMgr->SyncHgmConfigChangeCallback();
            hccMgr->SyncRefreshRateModeChangeCallback(0);
            hccMgr->refreshRateUpdateCallbacks_ = {
                {0, nullptr},
            };
            hccMgr->SyncRefreshRateUpdateCallback(OLED_60_HZ);
            ASSERT_EQ(hccMgr->animDynamicCfgCallbacks_.empty(), false);
            hccMgr->UnRegisterHgmConfigChangeCallback(pid);
            hccMgr->animDynamicCfgCallbacks_.try_emplace(pid, cb);
            std::shared_ptr<HgmMultiAppStrategy> multiAppStrategy_;
            auto frameRateMgr = hgmCore.GetFrameRateMgr();
            frameRateMgr->GetMultiAppStrategy().GetForegroundPidApp().try_emplace(0,
                std::pair<int32_t, std::string>{0, "com.app10"});
            hccMgr->SyncHgmConfigChangeCallback();
            hccMgr->refreshRateUpdateCallbacks_.try_emplace(0, cb);
            hccMgr->SyncRefreshRateUpdateCallback(OLED_60_HZ);
            hccMgr->SyncHgmConfigChangeCallback();
            hccMgr->refreshRateUpdateCallbacks_.clear();
            hccMgr->refreshRateUpdateCallbacks_.try_emplace(0, cb);
            hccMgr->SyncRefreshRateUpdateCallback(OLED_60_HZ);
        }
    }
}

/**
 * @tc.name: HgmConfigCallbackManagerTest003
 * @tc.desc: Verify the result of HgmConfigCallbackManagerTest003 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmConfigCallbackManagerTest003, Function | SmallTest | Level1)
{
    sptr<HgmConfigCallbackManager> hccMgr = HgmConfigCallbackManager::GetInstance();
    pid_t listenerPid = 1;
    pid_t dstPid = 1;
    vector<int32_t> expectedFrameRates = {60, 0};
    std::string idStr = "";
    const sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> cb = nullptr;
    std::unordered_map<pid_t, sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback>> cbMap;
    cbMap.try_emplace(listenerPid, cb);
    hccMgr->xcomponentExpectedFrameRateCallbacks_.try_emplace(dstPid, cbMap);

    int32_t fps_ = 30;
    std::function<void(int32_t, int32_t)> callback = [&fps_](int32_t dstPid, int32_t fps) {
        fps_ = fps;
    };
    auto temp = new MyCustomFrameRateLinkerExpectedFpsUpdateCallback(callback);
    auto cb1 = iface_cast<RSIFrameRateLinkerExpectedFpsUpdateCallback>(temp);
    for (const auto& expectedFrameRate : expectedFrameRates) {
        PART("HgmConfigCallbackManagerTest") {
            STEP("1. Test RegisterXComponentExpectedFrameRateCallback function with callback") {
                hccMgr->RegisterXComponentExpectedFrameRateCallback(listenerPid, dstPid, cb);
                hccMgr->xcomponentExpectedFrameRate_.clear();
                hccMgr->xcomponentExpectedFrameRate_.try_emplace(dstPid, expectedFrameRate);
                hccMgr->RegisterXComponentExpectedFrameRateCallback(listenerPid, dstPid, cb1);
                ASSERT_EQ(hccMgr->xcomponentExpectedFrameRateCallbacks_[dstPid][listenerPid], cb1);
            }
            STEP("2. Test SyncXComponentExpectedFrameRateCallback function with callback") {
                ASSERT_NE(cb1, nullptr);
                hccMgr->xcomponentExpectedFrameRateCallbacks_.clear();
                cbMap.clear();
                cbMap.try_emplace(listenerPid, cb);
                hccMgr->xcomponentExpectedFrameRateCallbacks_.try_emplace(dstPid, cbMap);
                hccMgr->SyncXComponentExpectedFrameRateCallback(dstPid, idStr, expectedFrameRate);
                hccMgr->xcomponentExpectedFrameRateCallbacks_.clear();
                cbMap.clear();
                cbMap.try_emplace(listenerPid, cb1);
                hccMgr->xcomponentExpectedFrameRateCallbacks_.try_emplace(dstPid, cbMap);
                hccMgr->SyncXComponentExpectedFrameRateCallback(dstPid, idStr, expectedFrameRate);
                ASSERT_EQ(hccMgr->xcomponentExpectedFrameRate_[dstPid], expectedFrameRate);
            }
            ASSERT_EQ(fps_, expectedFrameRate);
        }
    }
}

/**
 * @tc.name: HgmSetPointerActiveFPS
 * @tc.desc: Verify the result of HgmSetPointerActiveFPS function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmSetPointerActiveFPS, Function | SmallTest | Level1)
{
    HgmFrameRateManager frameRateMgr;
    constexpr uint32_t delay_1100Ms = 1100;
    constexpr uint32_t delay_1300Ms = 1300;
    InitHgmFrameRateManager(frameRateMgr);
    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_BUTTON_DOWN, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);

    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_MOVE, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);

    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_BUTTON_UP, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);

    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_MOVE, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_1100Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_1300Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_IDLE_STATE);

    frameRateMgr.pointerManager_.ChangeState(PointerState::POINTER_IDLE_STATE);
    sleep(1); // wait for handler task finished
}

/**
 * @tc.name: MultiThread001
 * @tc.desc: Verify the result of MultiThread001 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, MultiThread001, Function | SmallTest | Level1)
{
    int64_t offset = 0;
    int32_t testThreadNum = 100;
    int32_t touchCnt = 1;
    std::string pkg0 = "com.pkg.other:0:-1";
    std::string pkg1 = "com.ss.hm.ugc.aweme:1001:10067";
    std::string pkg2 = "com.wedobest.fivechess.harm:1002:10110";

    HgmFrameRateManager frameRateMgr;
    auto vsyncGenerator = CreateVSyncGenerator();
    sptr<Rosen::VSyncController> rsController = new VSyncController(vsyncGenerator, offset);
    ASSERT_NE(rsController, nullptr);
    sptr<Rosen::VSyncController> appController = new VSyncController(vsyncGenerator, offset);
    ASSERT_NE(appController, nullptr);
    frameRateMgr.Init(rsController, appController, vsyncGenerator);
    frameRateMgr.forceUpdateCallback_ = [](bool idleTimerExpired, bool forceUpdate) { return; };

    ASSERT_NE(vsyncGenerator, nullptr);
    ASSERT_NE(rsController, nullptr);
    ASSERT_NE(appController, nullptr);
    HgmTaskHandleThread::Instance().PostTask([&]() {
        for (int i = 0; i < testThreadNum; i++) {
            // HandleLightFactorStatus
            frameRateMgr.HandleLightFactorStatus(i, LightFactorStatus::NORMAL_LOW);
            frameRateMgr.HandleLightFactorStatus(i, LightFactorStatus::NORMAL_HIGH);

            // HandlePackageEvent
            frameRateMgr.HandlePackageEvent(i, {pkg0});
            ASSERT_NE(frameRateMgr.multiAppStrategy_.HandlePkgsEvent({pkg0}), EXEC_SUCCESS);
            frameRateMgr.HandlePackageEvent(i, {pkg1});
            ASSERT_NE(frameRateMgr.multiAppStrategy_.HandlePkgsEvent({pkg1}), EXEC_SUCCESS);
            frameRateMgr.HandlePackageEvent(i, {pkg2});
            ASSERT_NE(frameRateMgr.multiAppStrategy_.HandlePkgsEvent({pkg2}), EXEC_SUCCESS);
            frameRateMgr.HandlePackageEvent(i, {pkg0, pkg1});
            ASSERT_NE(frameRateMgr.multiAppStrategy_.HandlePkgsEvent({pkg0, pkg1}), EXEC_SUCCESS);

            // HandleRefreshRateEvent
            frameRateMgr.HandleRefreshRateEvent(i, {});

            // HandleTouchEvent
            frameRateMgr.HandleTouchEvent(i, TouchStatus::TOUCH_DOWN, touchCnt);
            frameRateMgr.HandleTouchEvent(i, TouchStatus::TOUCH_UP, touchCnt);

            // HandleRefreshRateMode
            // param -1、0、1、2、3：refresh rate mode
            frameRateMgr.HandleRefreshRateMode(-1);
            frameRateMgr.HandleRefreshRateMode(0);
            frameRateMgr.HandleRefreshRateMode(1);
            frameRateMgr.HandleRefreshRateMode(2);
            frameRateMgr.HandleRefreshRateMode(3);

            // HandleScreenPowerStatus
            frameRateMgr.HandleScreenPowerStatus(i, ScreenPowerStatus::POWER_STATUS_ON);
            frameRateMgr.HandleScreenPowerStatus(i, ScreenPowerStatus::POWER_STATUS_OFF);
        }
    });
    sleep(2); // wait for handler task finished
}

/**
 * @tc.name: HandleScreenRectFrameRate
 * @tc.desc: Verify the result of HandleScreenRectFrameRate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HandleScreenRectFrameRate, Function | SmallTest | Level1)
{
    int32_t testThreadNum = 100;
    HgmFrameRateManager frameRateMgr;
    HgmTaskHandleThread::Instance().PostTask([&]() {
        for (int i = 0; i < testThreadNum; i++) {
            // HandleScreenRectFrameRate
            frameRateMgr.HandleScreenRectFrameRate(i, rectF);
            frameRateMgr.HandleScreenRectFrameRate(i, rectM);
            frameRateMgr.HandleScreenRectFrameRate(i, rectG);
            frameRateMgr.HandleScreenRectFrameRate(i, rectNeg);
            auto& hgmCore = HgmCore::Instance();
            auto screen = hgmCore.GetScreen(0);
            screen->isSelfOwnedScreenFlag_.store(true);
            auto& configData = hgmCore.GetPolicyConfigData();
            configData->virtualDisplayConfigs_.clear();
            frameRateMgr.HandleScreenRectFrameRate(i, rectMax);
            EXPECT_EQ(frameRateMgr.curScreenStrategyId_, "LTPO-DEFAULT");
        }
    });
    sleep(2); // wait for handler task finished
}

/**
 * @tc.name: UpdateGuaranteedPlanVoteTest
 * @tc.desc: Verify the result of UpdateGuaranteedPlanVote
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, UpdateGuaranteedPlanVoteTest, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();

    mgr->idleDetector_.SetAppSupportedState(false);
    mgr->UpdateGuaranteedPlanVote(currTime);

    mgr->idleDetector_.SetAppSupportedState(true);
    mgr->UpdateGuaranteedPlanVote(currTime);

    mgr->HandleTouchEvent(appPid, TouchStatus::TOUCH_DOWN, touchCount);
    mgr->HandleTouchEvent(appPid, TouchStatus::TOUCH_UP, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    mgr->UpdateGuaranteedPlanVote(currTime);

    mgr->idleDetector_.bufferFpsMap_["AceAnimato"] = 90;
    mgr->HandleTouchEvent(appPid, TouchStatus::TOUCH_DOWN, touchCount);
    mgr->HandleTouchEvent(appPid, TouchStatus::TOUCH_UP, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    mgr->UpdateGuaranteedPlanVote(currTime);

    mgr->idleDetector_.SetAceAnimatorIdleState(false);
    mgr->HandleTouchEvent(appPid, TouchStatus::TOUCH_DOWN, touchCount);
    mgr->HandleTouchEvent(appPid, TouchStatus::TOUCH_UP, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    mgr->UpdateGuaranteedPlanVote(currTime);
    EXPECT_FALSE(mgr->idleDetector_.GetAceAnimatorIdleState());
    mgr->lastTouchUpExpectFps_ = OLED_90_HZ;
    EXPECT_EQ(mgr->idleDetector_.GetTouchUpExpectedFPS(), mgr->lastTouchUpExpectFps_);
}

/**
 * @tc.name: CleanPidCallbackTest
 * @tc.desc: Verify the result of CleanPidCallbackTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, CleanPidCallbackTest, Function | SmallTest | Level2)
{
    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();
    int32_t defaultPid = 0;
    int32_t gamePid = 10024;
    uint32_t undefinedCallbackType = 0xff;
    std::string defaultScreenStrategyId = "LTPO-DEFAULT";
    std::string invalidScreenStrategyId = "DEFAULT-INVALID";
    auto &hgm = HgmCore::Instance();

    mgr->CleanVote(defaultPid);
    mgr->cleanPidCallback_[gamePid].insert(CleanPidCallbackType::LIGHT_FACTOR);
    mgr->cleanPidCallback_[gamePid].insert(CleanPidCallbackType::PACKAGE_EVENT);
    mgr->cleanPidCallback_[gamePid].insert(CleanPidCallbackType::TOUCH_EVENT);
    mgr->cleanPidCallback_[gamePid].insert(CleanPidCallbackType::GAMES);
    mgr->cleanPidCallback_[gamePid].insert(static_cast<CleanPidCallbackType>(undefinedCallbackType));
    mgr->CleanVote(gamePid);
    mgr->pidRecord_.emplace(defaultPid);
    mgr->CleanVote(gamePid);
    mgr->pidRecord_.emplace(gamePid);
    mgr->CleanVote(gamePid);

    ASSERT_EQ(mgr->sceneStack_.empty(), true);
    mgr->sceneStack_.push_back(std::make_pair("sceneName", 0));
    ASSERT_EQ(mgr->sceneStack_.empty(), false);

    std::string savedScreenStrategyId = mgr->curScreenStrategyId_;
    ASSERT_EQ(savedScreenStrategyId, defaultScreenStrategyId);
    mgr->curScreenStrategyId_ = invalidScreenStrategyId;
    mgr->UpdateVoteRule();
    if (hgm.mPolicyConfigData_ == nullptr) {
        return;
    }
    EXPECT_NE(hgm.mPolicyConfigData_, nullptr);
    std::shared_ptr<PolicyConfigData> cachedPolicyConfigData = nullptr;
    std::swap(hgm.mPolicyConfigData_, cachedPolicyConfigData);
    EXPECT_EQ(hgm.mPolicyConfigData_, nullptr);
    mgr->UpdateVoteRule();
    mgr->curScreenStrategyId_ = savedScreenStrategyId;
    std::swap(hgm.mPolicyConfigData_, cachedPolicyConfigData);
    EXPECT_NE(hgm.mPolicyConfigData_, nullptr);
    auto screenSetting = mgr->multiAppStrategy_.GetScreenSetting();
    screenSetting.sceneList.insert(make_pair(testScene, PolicyConfigData::SceneConfig{"1", "1"}));
    screenSetting.gameSceneList.insert(make_pair(testScene, "1"));
    screenSetting.ancoSceneList.insert(make_pair(testScene, PolicyConfigData::SceneConfig{"1", "1"}));
    mgr->multiAppStrategy_.SetScreenSetting(screenSetting);
    EventInfo eventInfo2 = { .eventName = "VOTER_SCENE", .eventStatus = true, .description = testScene };
    mgr->HandleRefreshRateEvent(0, eventInfo2);
    mgr->UpdateVoteRule();
    mgr->sceneStack_.push_back(std::make_pair("sceneName1", 0));
    mgr->UpdateVoteRule();
    sleep(1);
}

/**
 * @tc.name: HandleEventTest
 * @tc.desc: Verify the result of HandleEventTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HandleEventTest, Function | SmallTest | Level2)
{
    std::string pkg0 = "com.pkg.other:0:-1";
    std::string pkg1 = "com.pkg.other:1:-1";

    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();
    auto &hgm = HgmCore::Instance();
    mgr->DeliverRefreshRateVote({"VOTER_GAMES", 120, 90, 0}, true);

    mgr->GetExpectedFrameRate(static_cast<RSPropertyUnit>(RSPropertyUnit::PIXEL_POSITION), 100.f, 0, 0);
    mgr->GetExpectedFrameRate(static_cast<RSPropertyUnit>(0xff), 100.f, 0, 0);
    if (hgm.mPolicyConfigData_ == nullptr) {
        return;
    }
    EXPECT_NE(hgm.mPolicyConfigData_, nullptr);
    std::shared_ptr<PolicyConfigData> cachedPolicyConfigData = nullptr;
    std::swap(hgm.mPolicyConfigData_, cachedPolicyConfigData);
    EXPECT_EQ(hgm.mPolicyConfigData_, nullptr);
    ASSERT_EQ(nullptr, hgm.GetPolicyConfigData());
    mgr->GetPreferredFps("translate", 100.f, 0.f, 0.f);

    EventInfo eventInfo = { .eventName = "VOTER_GAMES", .eventStatus = false,
        .description = pkg0,
    };
    mgr->HandleRefreshRateEvent(0, eventInfo);
    mgr->HandleGamesEvent(0, eventInfo);
    eventInfo.eventStatus = true;
    mgr->HandleGamesEvent(0, eventInfo);
    eventInfo.description = pkg1;
    mgr->HandleGamesEvent(0, eventInfo);
    mgr->HandleGamesEvent(1, eventInfo);
    mgr->HandleIdleEvent(true);
    mgr->HandleIdleEvent(false);
    auto screenSetting = mgr->multiAppStrategy_.GetScreenSetting();
    screenSetting.sceneList.insert(make_pair(testScene, PolicyConfigData::SceneConfig{"1", "1"}));
    screenSetting.gameSceneList.insert(make_pair(testScene, "1"));
    screenSetting.ancoSceneList.insert(make_pair(testScene, PolicyConfigData::SceneConfig{"1", "1"}));
    mgr->multiAppStrategy_.SetScreenSetting(screenSetting);
    EventInfo eventInfo2 = { .eventName = "VOTER_SCENE", .eventStatus = true, .description = testScene };
    mgr->HandleRefreshRateEvent(0, eventInfo2);
    eventInfo2.eventStatus = false;
    mgr->HandleRefreshRateEvent(0, eventInfo2);

    std::swap(hgm.mPolicyConfigData_, cachedPolicyConfigData);
    EXPECT_NE(hgm.mPolicyConfigData_, nullptr);
    eventInfo2.eventName = "VOTER_VIDEO_CALL";
    mgr->HandleRefreshRateEvent(0, eventInfo2);
    eventInfo2.eventName = "VOTER_VIRTUALDISPLAY";
    mgr->HandleRefreshRateEvent(0, eventInfo2);
    eventInfo2.eventName = "VOTER_MULTISELFOWNEDSCREEN";
    mgr->HandleRefreshRateEvent(0, eventInfo2);
}


/**
 * @tc.name: GetDrawingFrameRateTest
 * @tc.desc: Verify the result of GetDrawingFrameRateTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, GetDrawingFrameRateTest, Function | SmallTest | Level2)
{
    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();
    uint32_t refreshRate_60 = 60;
    uint32_t refreshRate_120 = 120;
    FrameRateRange dynamic_120(0, 120, 120);
    EXPECT_EQ(dynamic_120.IsDynamic(), true);
    FrameRateRange static_120(120, 120, 120);
    EXPECT_EQ(static_120.IsDynamic(), false);
    auto &hgmCore = HgmCore::Instance();
    HgmFrameRateManager frameRateMgr;
    hgmCore.adaptiveSync_ = ADAPTIVE_SYNC_ENABLED;
    frameRateMgr.isAdaptive_.store(true);
    frameRateMgr.ProcessAdaptiveSync("VOTER_GAMES");
    frameRateMgr.ProcessAdaptiveSync("VOTER_SCENE");
    frameRateMgr.isAdaptive_.store(false);
    frameRateMgr.ProcessAdaptiveSync("VOTER_GAMES");
    EXPECT_EQ(mgr->GetDrawingFrameRate(refreshRate_60, dynamic_120), 60);
    EXPECT_EQ(mgr->GetDrawingFrameRate(refreshRate_60, static_120), 60);
    EXPECT_EQ(mgr->GetDrawingFrameRate(refreshRate_120, dynamic_120), 120);
    EXPECT_EQ(mgr->GetDrawingFrameRate(refreshRate_120, static_120), 120);
}


/**
 * @tc.name: ProcessRefreshRateVoteTest
 * @tc.desc: Verify the result of ProcessRefreshRateVoteTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, ProcessRefreshRateVoteTest, Function | SmallTest | Level2)
{
    HgmFrameRateManager frameRateMgr;
    VoteInfo resultVoteInfo;
    VoteRange voteRange = { OLED_MIN_HZ, OLED_MAX_HZ };
    bool voterGamesEffective = false;
    auto voterIter = std::find(frameRateMgr.voters_.begin(), frameRateMgr.voters_.end(), "VOTER_GAMES");
    frameRateMgr.ProcessRefreshRateVote(voterIter, resultVoteInfo, voteRange, voterGamesEffective);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_GAMES", OLED_120_HZ, OLED_90_HZ, OLED_NULL_HZ}, true);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_THERMAL", OLED_120_HZ, OLED_90_HZ, OLED_NULL_HZ}, true);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_MULTISELFOWNEDSCREEN", OLED_120_HZ, OLED_90_HZ, OLED_NULL_HZ}, true);
    auto screenSetting = frameRateMgr.multiAppStrategy_.GetScreenSetting();
    screenSetting.sceneList.insert(make_pair(testScene, PolicyConfigData::SceneConfig{"1", "1"}));
    screenSetting.gameSceneList.insert(make_pair(testScene, "1"));
    screenSetting.ancoSceneList.insert(make_pair(testScene, PolicyConfigData::SceneConfig{"1", "1"}));
    frameRateMgr.multiAppStrategy_.SetScreenSetting(screenSetting);
    EventInfo eventInfo2 = { .eventName = "VOTER_SCENE", .eventStatus = true, .description = testScene };
    frameRateMgr.HandleRefreshRateEvent(0, eventInfo2);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_ANCO", OLED_120_HZ, OLED_90_HZ, OLED_60_HZ}, true);
    auto resVoteInfo = frameRateMgr.ProcessRefreshRateVote();
    EXPECT_EQ(resVoteInfo.min, OLED_MIN_HZ);
    frameRateMgr.ancoScenes_.emplace("VOTER_ANCO");
    resVoteInfo = frameRateMgr.ProcessRefreshRateVote();
    EXPECT_EQ(resVoteInfo.min, OLED_MIN_HZ);
    frameRateMgr.voters_.push_back("VOTER_ANCO");
    auto voterIter1 = std::find(frameRateMgr.voters_.begin(), frameRateMgr.voters_.end(), "VOTER_ANCO");
    EXPECT_FALSE(frameRateMgr.ProcessRefreshRateVote(voterIter1, resultVoteInfo, voteRange, voterGamesEffective));
    frameRateMgr.voters_.push_back("VOTER_VIDEO");
    voterIter1 = std::find(frameRateMgr.voters_.begin(), frameRateMgr.voters_.end(), "VOTER_VIDEO");
    EXPECT_FALSE(frameRateMgr.ProcessRefreshRateVote(voterIter1, resultVoteInfo, voteRange, voterGamesEffective));
}

/**
 * @tc.name: ProcessRefreshRateVoteTest2
 * @tc.desc: Verify the result of ProcessRefreshRateVote when idle 30 enabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, ProcessRefreshRateVoteTest2, Function | SmallTest | Level2)
{
    HgmFrameRateManager frameRateMgr;
    VoteRange voteRange = { OLED_MIN_HZ, OLED_MAX_HZ };
    frameRateMgr.DeliverRefreshRateVote({"VOTER_PACKAGES", OLED_60_HZ, OLED_120_HZ, 0}, true);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_LTPO", OLED_120_HZ, OLED_120_HZ, 0}, true);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_IDLE", OLED_30_HZ, OLED_30_HZ, 0}, true);
    auto resVoteInfo = frameRateMgr.ProcessRefreshRateVote();
    EXPECT_EQ(resVoteInfo.min, OLED_120_HZ);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_LTPO", OLED_120_HZ, OLED_120_HZ, 0}, false);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_LTPO", OLED_60_HZ, OLED_60_HZ, 0}, true);
    resVoteInfo = frameRateMgr.ProcessRefreshRateVote();
    EXPECT_EQ(resVoteInfo.min, OLED_60_HZ);
    EXPECT_EQ(resVoteInfo.max, OLED_60_HZ);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_LTPO", OLED_60_HZ, OLED_60_HZ, 0}, false);
    resVoteInfo = frameRateMgr.ProcessRefreshRateVote();
    EXPECT_EQ(resVoteInfo.min, OLED_60_HZ);
    EXPECT_EQ(resVoteInfo.max, OLED_60_HZ);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_PACKAGES", OLED_60_HZ, OLED_120_HZ, 0}, false);
    frameRateMgr.DeliverRefreshRateVote({"VOTER_PACKAGES", OLED_30_HZ, OLED_120_HZ, 0}, true);
    resVoteInfo = frameRateMgr.ProcessRefreshRateVote();
    EXPECT_EQ(resVoteInfo.max, OLED_30_HZ);
    sleep(1); // wait for handler task finished
}

/**
 * @tc.name: SetAceAnimatorVoteTest
 * @tc.desc: Verify the result of SetAceAnimatorVoteTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, SetAceAnimatorVoteTest, Function | SmallTest | Level2)
{
    HgmFrameRateManager frameRateMgr;
    frameRateMgr.SetAceAnimatorVote(nullptr);
    std::shared_ptr<RSRenderFrameRateLinker> linker = std::make_shared<RSRenderFrameRateLinker>();
    ASSERT_NE(linker, nullptr);
    frameRateMgr.SetAceAnimatorVote(linker);
    linker->SetAnimatorExpectedFrameRate(OLED_60_HZ);
    frameRateMgr.SetAceAnimatorVote(linker);
}

/**
 * @tc.name: HgmSimpleTimerTest
 * @tc.desc: Verify the result of HgmSimpleTimerTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmSimpleTimerTest, Function | SmallTest | Level2)
{
    auto timer = HgmSimpleTimer("HgmSimpleTimer", std::chrono::milliseconds(delay_60Ms), nullptr, nullptr);
    ASSERT_NE(timer.handler_, nullptr);
    ASSERT_EQ(timer.name_, "HgmSimpleTimer");
    ASSERT_EQ(timer.interval_.load(), std::chrono::milliseconds(delay_60Ms));
    ASSERT_EQ(timer.startCallback_, nullptr);
    ASSERT_EQ(timer.expiredCallback_, nullptr);
    timer.Start();
    timer.Reset();
    timer.Stop();
    sleep(1); // wait for timer stop
}

/**
 * @tc.name: FrameRateReportTest
 * @tc.desc: Verify the result of FrameRateReportTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, FrameRateReportTest, Function | SmallTest | Level2)
{
    HgmFrameRateManager mgr;
    mgr.FrameRateReport();
    mgr.curRefreshRateMode_ = HGM_REFRESHRATE_MODE_HIGH;
    mgr.FrameRateReport();
    EXPECT_EQ(mgr.schedulePreferredFpsChange_, false);
    mgr.schedulePreferredFps_ = OLED_60_HZ;
    mgr.currRefreshRate_ = OLED_60_HZ;
    mgr.FrameRateReport();
    mgr.currRefreshRate_ = OLED_90_HZ;
    mgr.FrameRateReport();
    mgr.curRefreshRateMode_ = 0;
    mgr.schedulePreferredFps_ = 0;
    mgr.schedulePreferredFpsChange_ = true;
    mgr.FrameRateReport();
    EXPECT_EQ(mgr.schedulePreferredFpsChange_, false);
}

/**
 * @tc.name: CollectFrameRateChange
 * @tc.desc: Verify the result of CollectFrameRateChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, CollectFrameRateChange, Function | SmallTest | Level2)
{
    HgmFrameRateManager mgr;
    InitHgmFrameRateManager(mgr);
    FrameRateRange finalRange = {OLED_60_HZ, OLED_120_HZ, OLED_60_HZ};
    FrameRateRange appExpectedRange = {OLED_60_HZ, OLED_120_HZ, OLED_72_HZ};
    std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker = std::make_shared<RSRenderFrameRateLinker>();
    std::shared_ptr<RSRenderFrameRateLinker> appFrameRateLinker = std::make_shared<RSRenderFrameRateLinker>();
    appFrameRateLinker->SetExpectedRange(appExpectedRange);

    FrameRateLinkerMap appFrameRateLinkers = {
        {frameRateLinkerId1, nullptr},
        {frameRateLinkerId2, appFrameRateLinker}
    };
    EXPECT_EQ(mgr.CollectFrameRateChange(finalRange, rsFrameRateLinker, appFrameRateLinkers), false);
    mgr.currRefreshRate_.store(OLED_60_HZ);
    EXPECT_GT(mgr.GetDrawingFrameRate(mgr.currRefreshRate_, finalRange), 0);
    EXPECT_EQ(mgr.CollectFrameRateChange(finalRange, rsFrameRateLinker, appFrameRateLinkers), true);
    mgr.controller_ = nullptr;
    EXPECT_EQ(mgr.CollectFrameRateChange(finalRange, rsFrameRateLinker, appFrameRateLinkers), false);
}

/**
 * @tc.name: CollectVRateChange
 * @tc.desc: Verify the result of CollectVRateChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, CollectVRateChange, Function | SmallTest | Level2)
{
    HgmFrameRateManager mgr;
    InitHgmFrameRateManager(mgr);
    FrameRateRange finalRange = {OLED_60_HZ, OLED_120_HZ, OLED_60_HZ};
    mgr.vRatesMap_ = {
        {0, 0},
        {1, 1},
        {2, 2}
    };
    uint64_t linkerId = 2;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_60_HZ);
    EXPECT_EQ(finalRange.max_, OLED_120_HZ);
    EXPECT_EQ(finalRange.preferred_, OLED_60_HZ);

    linkerId = 0;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_60_HZ);
    EXPECT_EQ(finalRange.max_, OLED_120_HZ);
    EXPECT_EQ(finalRange.preferred_, OLED_60_HZ);

    linkerId = 1;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_60_HZ);
    EXPECT_EQ(finalRange.max_, OLED_120_HZ);
    EXPECT_EQ(finalRange.preferred_, OLED_60_HZ);
    
    linkerId = 2;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_60_HZ);
    EXPECT_EQ(finalRange.max_, OLED_120_HZ);
    EXPECT_EQ(finalRange.preferred_, OLED_60_HZ);

    finalRange.preferred_ = 0;
    mgr.controllerRate_ = 0;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_NULL_HZ);
    EXPECT_EQ(finalRange.max_, OLED_144_HZ);
    EXPECT_EQ(finalRange.preferred_, 1);

    finalRange.preferred_ = 0;
    mgr.controllerRate_ = 100;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_NULL_HZ);
    EXPECT_EQ(finalRange.max_, OLED_144_HZ);
    EXPECT_EQ(finalRange.preferred_, 50);
}

/**
 * @tc.name: HandleFrameRateChangeForLTPO
 * @tc.desc: Verify the result of HandleFrameRateChangeForLTPO
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HandleFrameRateChangeForLTPO, Function | SmallTest | Level2)
{
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    hgmCore.SetPendingScreenRefreshRate(OLED_30_HZ);
    frameRateMgr->currRefreshRate_ = OLED_120_HZ;
    hgmCore.lowRateToHighQuickSwitch_.store(false);
    frameRateMgr->HandleFrameRateChangeForLTPO(0, false);
    hgmCore.lowRateToHighQuickSwitch_.store(true);
    frameRateMgr->HandleFrameRateChangeForLTPO(0, false);
    frameRateMgr->forceUpdateCallback_ = nullptr;
    frameRateMgr->HandleFrameRateChangeForLTPO(0, false);
    frameRateMgr->forceUpdateCallback_ = [](bool idleTimerExpired, bool forceUpdate) { return; };
    frameRateMgr->HandleFrameRateChangeForLTPO(0, false);
    EXPECT_EQ(frameRateMgr->GetPreferredFps("translate", errorVelocity, 0, 0), 0);
    hgmCore.lowRateToHighQuickSwitch_.store(true);
    VSyncController* rsController;
    VSyncController* appController;
    VSyncGenerator* vsyncGenerator;
    frameRateMgr->controller_ = std::make_shared<HgmVSyncGeneratorController>(rsController,
        appController, vsyncGenerator);
    frameRateMgr->HandleFrameRateChangeForLTPO(0, false);
}

/**
 * @tc.name: DVSyncTaskProcess
 * @tc.desc: Verify the result of DVSyncTaskProcess
 * @tc.type: FUNC
 * @tc.require: issueIBX8OW
 */
HWTEST_F(HgmFrameRateMgrTest, DVSyncTaskProcessor, Function | SmallTest | Level2)
{
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    hgmCore.SetPendingScreenRefreshRate(OLED_144_HZ);
    auto lastRefreshRate = hgmCore.GetPendingScreenRefreshRate();
    frameRateMgr->controllerRate_ = OLED_30_HZ;
    frameRateMgr->currRefreshRate_ = OLED_120_HZ;
    VSyncController* rsController = nullptr;
    VSyncController* appController = nullptr;
    VSyncGenerator* vsyncGenerator = nullptr;
    frameRateMgr->controller_ = std::make_shared<HgmVSyncGeneratorController>(rsController,
        appController, vsyncGenerator);
    ASSERT_NE(frameRateMgr->controller_, nullptr);
    int64_t delayTime = 10;
    uint64_t targetTime = 0;
    frameRateMgr->DVSyncTaskProcessor(delayTime, targetTime);
}

/**
 * @tc.name: GetLowBrightVec
 * @tc.desc: Verify the result of GetLowBrightVec
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, GetLowBrightVec, Function | SmallTest | Level2)
{
    HgmFrameRateManager mgr;
    std::shared_ptr<PolicyConfigData> configData = std::make_shared<PolicyConfigData>();

    std::vector<std::string> screenConfigs = {"LTPO-DEFAULT", "LTPO-internal", "LTPO-external"};
    PolicyConfigData::SupportedModeConfig supportedMode = {{"LowBright", {}}};
    PolicyConfigData::SupportedModeConfig supportedMode1 = {{"LowBright", {OLED_30_HZ, OLED_60_HZ, OLED_120_HZ}}};
    PolicyConfigData::SupportedModeConfig supportedMode2 = {{"LowBright", {OLED_MIN_HZ}}};
    for (const auto& screenConfig : screenConfigs) {
        if (configData->screenStrategyConfigs_.find(screenConfig) == configData->screenStrategyConfigs_.end()) {
            continue;
        }
        configData->supportedModeConfigs_[screenConfig] = supportedMode;
        mgr.GetLowBrightVec(configData);
        ASSERT_EQ(mgr.isAmbientEffect_, false);
        ASSERT_TRUE(mgr.lowBrightVec_.empty());

        configData->supportedModeConfigs_[screenConfig].clear();
        configData->supportedModeConfigs_[screenConfig] = supportedMode1;
        mgr.GetLowBrightVec(configData);
        ASSERT_EQ(mgr.isAmbientEffect_, true);
        ASSERT_TRUE(!mgr.lowBrightVec_.empty());

        configData->supportedModeConfigs_[screenConfig].clear();
        configData->supportedModeConfigs_[screenConfig] = supportedMode2;
        mgr.GetLowBrightVec(configData);
        ASSERT_EQ(mgr.isAmbientEffect_, false);
        ASSERT_TRUE(mgr.lowBrightVec_.empty());
    }
}

/**
 * @tc.name: SetTimeoutParamsFromConfig
 * @tc.desc: Verify the result of SetTimeoutParamsFromConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, SetTimeoutParamsFromConfig, Function | SmallTest | Level2)
{
    HgmFrameRateManager mgr;
    std::shared_ptr<PolicyConfigData> configData = std::make_shared<PolicyConfigData>();

    auto time1 = mgr.touchManager_.upTimeoutTimer_.interval_.load();
    auto time2 = mgr.touchManager_.rsIdleTimeoutTimer_.interval_.load();
    ASSERT_EQ(time1, std::chrono::milliseconds(3000));
    ASSERT_EQ(time2, std::chrono::milliseconds(600));

    int32_t upTimeoutMs = 2000;
    int32_t rsIdleTimeoutMs = 300;
    configData->timeoutStrategyConfig_["up_timeout_ms"] = "2000";
    configData->timeoutStrategyConfig_["rs_idle_timeout_ms"] = "300";

    mgr.SetTimeoutParamsFromConfig(configData);
    auto time3 = mgr.touchManager_.upTimeoutTimer_.interval_.load();
    auto time4 = mgr.touchManager_.rsIdleTimeoutTimer_.interval_.load();
    ASSERT_EQ(time3, std::chrono::milliseconds(upTimeoutMs));
    ASSERT_EQ(time4, std::chrono::milliseconds(rsIdleTimeoutMs));
}

/**
 * @tc.name: GetStylusVec
 * @tc.desc: Verify the result of GetStylusVec
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, GetStylusVec, Function | SmallTest | Level2)
{
    HgmFrameRateManager mgr;
    std::shared_ptr<PolicyConfigData> configData = std::make_shared<PolicyConfigData>();
 
    std::vector<std::string> screenConfigs = {"LTPO-DEFAULT", "LTPS-DEFAULT"};
    for (const auto& screenConfig : screenConfigs) {
        auto iter = configData->supportedModeConfigs_.find(screenConfig);
        if (iter == configData-> supportedModeConfigs_.end()) {
            continue;
        }
 
        auto& supportedModeConfig = iter->second;
        auto it = supportedModeConfig.find("StylusPen");
        if (it == supportedModeConfig.end()) {
            continue;
        }
 
        supportedModeConfig["StylusPen"].clear();
        mgr.GetStylusVec(configData);
        ASSERT_TRUE(mgr.stylusVec_.empty());
 
        std::vector<uint32_t> expectedVec = {OLED_60_HZ, OLED_120_HZ};
        supportedModeConfig["StylusPen"] = expectedVec;
        mgr.GetStylusVec(configData);
        ASSERT_EQ(mgr.stylusVec_, expectedVec);
    }
}

/**
 * @tc.name: GetDrawingFrameRate
 * @tc.desc: Verify the result of GetDrawingFrameRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, GetDrawingFrameRate, Function | SmallTest | Level2)
{
    std::vector<std::pair<std::pair<uint32_t, FrameRateRange>, uint32_t>> inputAndOutput = {
        {{0, {0, 120, 60}}, 0},
        {{60, {0, 120, 0}}, 0},
        {{60, {0, 90, 120}}, 60},
        {{60, {0, 120, 120}}, 60},
        {{90, {0, 120, 30}}, 30},
        {{80, {0, 120, 30}}, 40},
        {{70, {0, 120, 30}}, 35},
        {{60, {0, 120, 30}}, 30},
        {{50, {0, 120, 30}}, 50}
    };

    for (const auto& [input, output] : inputAndOutput) {
        EXPECT_EQ(HgmFrameRateManager::GetDrawingFrameRate(input.first, input.second), output);
    }
}

/**
 * @tc.name: HandleScreenPowerStatus
 * @tc.desc: Verify the result of HandleScreenPowerStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HandleScreenPowerStatus, Function | SmallTest | Level1)
{
    ScreenId extraScreenId = 1;
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    auto configData = hgmCore.GetPolicyConfigData();
    if (configData == nullptr || frameRateMgr == nullptr) {
        return;
    }
    // init
    configData->screenStrategyConfigs_["screen0_LTPS"] = "LTPS-DEFAULT";
    configData->screenStrategyConfigs_["screen0_LTPO"] = "LTPO-DEFAULT";
    configData->screenStrategyConfigs_["screen5_LTPS"] = "LTPS-DEFAULT";
    configData->screenStrategyConfigs_["screen5_LTPO"] = "LTPO-DEFAULT";
    EXPECT_EQ(hgmCore.AddScreen(externalScreenId, 0, screenSize), EXEC_SUCCESS);
    EXPECT_EQ(hgmCore.AddScreen(internalScreenId, 0, screenSize), EXEC_SUCCESS);

    // fold -> expand -> fold
    frameRateMgr->curScreenStrategyId_.clear();
    frameRateMgr->HandleScreenPowerStatus(internalScreenId, ScreenPowerStatus::POWER_STATUS_SUSPEND);
    frameRateMgr->HandleScreenPowerStatus(externalScreenId, ScreenPowerStatus::POWER_STATUS_ON);
    EXPECT_EQ(frameRateMgr->curScreenId_, externalScreenId);
    EXPECT_EQ(hgmCore.AddScreen(extraScreenId, 0, screenSize), EXEC_SUCCESS);
    EXPECT_EQ(frameRateMgr->curScreenId_, externalScreenId);
    EXPECT_EQ(hgmCore.RemoveScreen(extraScreenId), EXEC_SUCCESS);
    EXPECT_EQ(frameRateMgr->curScreenId_, externalScreenId);

    EXPECT_EQ(hgmCore.AddScreen(extraScreenId, 0, screenSize), EXEC_SUCCESS);
    frameRateMgr->HandleScreenPowerStatus(externalScreenId, ScreenPowerStatus::POWER_STATUS_SUSPEND);
    frameRateMgr->HandleScreenPowerStatus(internalScreenId, ScreenPowerStatus::POWER_STATUS_ON);
    EXPECT_EQ(frameRateMgr->curScreenId_, internalScreenId);
    EXPECT_EQ(hgmCore.RemoveScreen(extraScreenId), EXEC_SUCCESS);
    EXPECT_EQ(frameRateMgr->curScreenId_, internalScreenId);

    EXPECT_EQ(hgmCore.AddScreen(extraScreenId, 0, screenSize), EXEC_SUCCESS);
    EXPECT_EQ(frameRateMgr->curScreenId_, internalScreenId);
    EXPECT_EQ(hgmCore.RemoveScreen(extraScreenId), EXEC_SUCCESS);

    EXPECT_EQ(hgmCore.AddScreen(extraScreenId, 0, screenSize), EXEC_SUCCESS);
    frameRateMgr->HandleScreenPowerStatus(internalScreenId, ScreenPowerStatus::POWER_STATUS_SUSPEND);
    frameRateMgr->HandleScreenPowerStatus(externalScreenId, ScreenPowerStatus::POWER_STATUS_ON);
    EXPECT_EQ(frameRateMgr->curScreenId_, externalScreenId);
    EXPECT_EQ(hgmCore.RemoveScreen(extraScreenId), EXEC_SUCCESS);
    EXPECT_EQ(frameRateMgr->curScreenId_, externalScreenId);

    // expand -> multiScreen -> expand
    frameRateMgr->HandleScreenPowerStatus(externalScreenId, ScreenPowerStatus::POWER_STATUS_SUSPEND);
    frameRateMgr->HandleScreenPowerStatus(internalScreenId, ScreenPowerStatus::POWER_STATUS_ON);
    EXPECT_EQ(frameRateMgr->curScreenId_, internalScreenId);

    hgmCore.SetMultiSelfOwnedScreenEnable(true);
    frameRateMgr->HandleScreenPowerStatus(externalScreenId, ScreenPowerStatus::POWER_STATUS_ON);
    EXPECT_EQ(frameRateMgr->curScreenId_, internalScreenId);

    hgmCore.SetMultiSelfOwnedScreenEnable(false);
    frameRateMgr->HandleScreenPowerStatus(externalScreenId, ScreenPowerStatus::POWER_STATUS_SUSPEND);
    EXPECT_EQ(frameRateMgr->curScreenId_, internalScreenId);
}

/**
 * @tc.name: NotifyScreenRectFrameRateChange
 * @tc.desc: Verify the result of NotifyScreenRectFrameRateChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, NotifyScreenRectFrameRateChange, Function | SmallTest | Level1)
{
    auto &hgmCore = HgmCore::Instance();
    Rect activeRect;
    ScreenId id = 0;
    GraphicIRect dstActiveRect {
        .x = activeRect.x,
        .y = activeRect.y,
        .w = activeRect.w,
        .h = activeRect.h,
    };
    hgmCore.NotifyScreenRectFrameRateChange(id, dstActiveRect);
    EXPECT_NE(hgmCore.hgmFrameRateMgr_, nullptr);
}

/**
 * @tc.name: HandleScreenExtStrategyChange
 * @tc.desc: Verify the result of HandleScreenExtStrategyChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HandleScreenExtStrategyChange, Function | SmallTest | Level1)
{
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    if (frameRateMgr == nullptr || hgmCore.mPolicyConfigData_ == nullptr) {
        return;
    }
    frameRateMgr->HandleScreenExtStrategyChange(true, "_THERMAL");
    frameRateMgr->HandleScreenExtStrategyChange(false, "test");
    EXPECT_NE(frameRateMgr, nullptr);
}

/**
 * @tc.name: HandlePackageEvent
 * @tc.desc: Verify the result of HandlePackageEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HandlePackageEvent, Function | SmallTest | Level1)
{
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    std::string pkgName0 = "com.pkg0";
    std::string pkgName1 = "com.pkg1";
    std::string scene0 = "SCENE0";
    std::string scene1 = "SCENE1";
    std::string scene2 = "SCENE2";

    auto sceneListConfig = frameRateMgr->GetMultiAppStrategy().GetScreenSetting();
    sceneListConfig.sceneList[scene0] = {"1", "1", false};
    sceneListConfig.sceneList[scene1] = {"1", "1", true};
    sceneListConfig.gameSceneList[scene0] = {"1", "1"};

    frameRateMgr->GetMultiAppStrategy().SetScreenSetting(sceneListConfig);

    auto checkFunc = [frameRateMgr, scene0, scene1] (bool scene0Existed, bool scene1Existed, bool gameScene0Existed,
                                                     bool gameScene1Existed) {
        auto sceneStack = frameRateMgr->sceneStack_;
        EXPECT_EQ(std::find(sceneStack.begin(), sceneStack.end(),
            std::pair<std::string, pid_t>({scene0, DEFAULT_PID})) != sceneStack.end(), scene0Existed);
        EXPECT_EQ(std::find(sceneStack.begin(), sceneStack.end(),
            std::pair<std::string, pid_t>({scene1, DEFAULT_PID})) != sceneStack.end(), scene1Existed);

        auto gameScenes = frameRateMgr->gameScenes_;
        EXPECT_EQ(gameScenes.find(scene0) != gameScenes.end(), gameScene0Existed);
        EXPECT_EQ(gameScenes.find(scene1) != gameScenes.end(), gameScene1Existed);
    };

    frameRateMgr->HandleSceneEvent(DEFAULT_PID, {"VOTER_SCENE", true, OLED_NULL_HZ, OLED_MAX_HZ, scene0});
    checkFunc(true, false, true, false);

    frameRateMgr->HandlePackageEvent(DEFAULT_PID, {pkgName0});
    checkFunc(false, false, false, false);

    // multi scene
    frameRateMgr->HandleSceneEvent(DEFAULT_PID, {"VOTER_SCENE", true, OLED_NULL_HZ, OLED_MAX_HZ, scene0});
    frameRateMgr->HandleSceneEvent(DEFAULT_PID, {"VOTER_SCENE", true, OLED_NULL_HZ, OLED_MAX_HZ, scene1});
    checkFunc(true, true, true, false);

    frameRateMgr->HandlePackageEvent(DEFAULT_PID, {pkgName1});
    checkFunc(false, true, false, false);
    frameRateMgr->currRefreshRate_ = OLED_30_HZ;
    FrameRateRange finalRange = {OLED_60_HZ, OLED_120_HZ, OLED_60_HZ};
    EXPECT_EQ(frameRateMgr->CalcRefreshRate(frameRateMgr->curScreenId_.load(), finalRange),
        frameRateMgr->currRefreshRate_);
    frameRateMgr->stylusMode_ = STYLUS_LINK_WRITE;
    frameRateMgr->stylusVec_.clear();
    ASSERT_EQ(frameRateMgr->CalcRefreshRate(frameRateMgr->curScreenId_.load(), finalRange),
        frameRateMgr->currRefreshRate_);
}

/**
 * @tc.name: AvoidChangeRateFrequent
 * @tc.desc: Verify the result of AvoidChangeRateFrequent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, AvoidChangeRateFrequent, Function | SmallTest | Level1)
{
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    
    frameRateMgr->isDragScene_ = false;
    ASSERT_EQ(frameRateMgr->AvoidChangeRateFrequent(120), 120);
    ASSERT_EQ(frameRateMgr->AvoidChangeRateFrequent(72), 72);

    frameRateMgr->isDragScene_ = true;
    ASSERT_EQ(frameRateMgr->AvoidChangeRateFrequent(120), 120);
    ASSERT_EQ(frameRateMgr->AvoidChangeRateFrequent(72), 120);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_EQ(frameRateMgr->AvoidChangeRateFrequent(72), 72);
}

/**
 * @tc.name: ChangePriority
 * @tc.desc: Verify the result of ChangePriority
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, ChangePriority, Function | SmallTest | Level1)
{
    constexpr uint32_t DEFAULT_PRIORITY = 0;
    constexpr uint32_t VOTER_SCENE_PRIORITY_BEFORE_PACKAGES = 1;
    constexpr uint32_t VOTER_LTPO_PRIORITY_BEFORE_PACKAGES = 2;
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    
    frameRateMgr->ChangePriority(DEFAULT_PRIORITY);
    auto packagesPos = find(frameRateMgr->voters_.begin(), frameRateMgr->voters_.end(), "VOTER_PACKAGES");
    auto ltpoPos = find(frameRateMgr->voters_.begin(), frameRateMgr->voters_.end(), "VOTER_LTPO");
    auto scenePos = find(frameRateMgr->voters_.begin(), frameRateMgr->voters_.end(), "VOTER_SCENE");
    ASSERT_LT(packagesPos, ltpoPos);
    ASSERT_LT(ltpoPos, scenePos);
    frameRateMgr->ChangePriority(VOTER_SCENE_PRIORITY_BEFORE_PACKAGES);
    auto packagesPos1 = find(frameRateMgr->voters_.begin(), frameRateMgr->voters_.end(), "VOTER_PACKAGES");
    auto ltpoPos1 = find(frameRateMgr->voters_.begin(), frameRateMgr->voters_.end(), "VOTER_LTPO");
    auto scenePos1 = find(frameRateMgr->voters_.begin(), frameRateMgr->voters_.end(), "VOTER_SCENE");
    ASSERT_LT(scenePos1, packagesPos1);
    ASSERT_LT(packagesPos1, ltpoPos1);
    frameRateMgr->ChangePriority(VOTER_LTPO_PRIORITY_BEFORE_PACKAGES);
    auto packagesPos2 = find(frameRateMgr->voters_.begin(), frameRateMgr->voters_.end(), "VOTER_PACKAGES");
    auto ltpoPos2 = find(frameRateMgr->voters_.begin(), frameRateMgr->voters_.end(), "VOTER_LTPO");
    auto scenePos2 = find(frameRateMgr->voters_.begin(), frameRateMgr->voters_.end(), "VOTER_SCENE");
    ASSERT_LT(scenePos2, ltpoPos2);
    ASSERT_LT(ltpoPos2, packagesPos2);
}

/**
 * @tc.name: HandleDynamicModeEvent
 * @tc.desc: Verify the result of HandleDynamicModeEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HandleDynamicModeEvent, Function | SmallTest | Level1)
{
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    frameRateMgr->HandleDynamicModeEvent(true);
    EXPECT_EQ(hgmCore.enableDynamicMode_, true);
}

/**
 * @tc.name: ProcessPageUrlVote
 * @tc.desc: Verify the result of ProcessPageUrlVote
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, ProcessPageUrlVote, Function | SmallTest | Level1)
{
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    pid_t pid = 0;
    std::string strategy = "";
    bool isAddVote = true;
    std::string packageName = "";
    std::string pageName = "";
    bool isEnter = true;
    frameRateMgr->ProcessPageUrlVote(pid, strategy, isAddVote);
    frameRateMgr->CleanPageUrlVote(pid);
    frameRateMgr->NotifyPageName(pid, packageName, pageName, isEnter);
    EXPECT_EQ(frameRateMgr->appPageUrlStrategy_.pageUrlConfig_,
        frameRateMgr->multiAppStrategy_.screenSettingCache_.pageUrlConfig);
}

/**
 * @tc.name: CollectGameRateDiscountChange
 * @tc.desc: Verify the result of CollectGameRateDiscountChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, CollectGameRateDiscountChange, Function | SmallTest | Level2)
{
    HgmFrameRateManager mgr;
    InitHgmFrameRateManager(mgr);
    FrameRateRange appExpectedRange = {OLED_60_HZ, OLED_120_HZ, OLED_60_HZ};
    FrameRateRange appZeroExpectedRange = {OLED_MIN_HZ, OLED_120_HZ, OLED_MIN_HZ};
    FrameRateRange appErrExpectedRange = {OLED_60_HZ, OLED_120_HZ, OLED_MIN_HZ};

    EXPECT_EQ(mgr.CollectGameRateDiscountChange(frameRateLinkerId1, appExpectedRange), false);
    std::vector<uint64_t> linkerIds = {frameRateLinkerId1, frameRateLinkerId2};

    mgr.currRefreshRate_ = OLED_60_HZ;
    uint32_t rateDiscount = 0;
    EXPECT_EQ(mgr.SetVsyncRateDiscountLTPO(linkerIds, rateDiscount), true);
    EXPECT_EQ(mgr.CollectGameRateDiscountChange(frameRateLinkerId1, appExpectedRange), false);
    rateDiscount = 1;
    EXPECT_EQ(mgr.SetVsyncRateDiscountLTPO(linkerIds, rateDiscount), true);
    EXPECT_EQ(mgr.CollectGameRateDiscountChange(frameRateLinkerId1, appExpectedRange), false);
    rateDiscount = 2;
    EXPECT_EQ(mgr.SetVsyncRateDiscountLTPO(linkerIds, rateDiscount), true);
    mgr.CollectGameRateDiscountChange(frameRateLinkerId1, appExpectedRange);
    mgr.CollectGameRateDiscountChange(frameRateLinkerId1, appZeroExpectedRange);
    EXPECT_EQ(mgr.CollectGameRateDiscountChange(frameRateLinkerId1, appErrExpectedRange), false);

    mgr.currRefreshRate_ = OLED_MIN_HZ;
    mgr.CollectGameRateDiscountChange(frameRateLinkerId1, appExpectedRange);
    EXPECT_EQ(mgr.CollectGameRateDiscountChange(frameRateLinkerId1, appZeroExpectedRange), false);

    mgr.EraseGameRateDiscountMap(frameRateLinkerId1);
}
} // namespace Rosen
} // namespace OHOS
