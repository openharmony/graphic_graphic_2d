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

#include "gtest/gtest.h"

#include "feature/hyper_graphic_manager/hgm_context.h"
#include "feature/vrate/rp_vsync_rate_reduce_manager.h"
#include "hgm_core.h"
#include "screen_manager/rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t delay_110Ms = 110;

auto& hgmCore = HgmCore::Instance();
auto frameRateMgr = hgmCore.GetFrameRateMgr();
// used for ProcessHgmFrameRateTest to prevent crash
std::shared_ptr<HgmContext> hgmContextForProcess = nullptr;
}

class HgmContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void HgmContextTest::SetUpTestCase()
{
    auto rsVSyncDistributor = sptr<VSyncDistributor>::MakeSptr(nullptr, "rs");
    hgmContextForProcess = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, rsVSyncDistributor);
}

void HgmContextTest::TearDownTestCase() {}

void HgmContextTest::SetUp() {}

void HgmContextTest::TearDown() {}

class CustomHgmCallback : public IRemoteStub<RSIHgmConfigChangeCallback> {
public:
    explicit CustomHgmCallback() {}
    ~CustomHgmCallback() override {};

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override {}
    void OnHgmRefreshRateModeChanged(int32_t refreshRateModeName) override {}
    void OnHgmRefreshRateUpdate(int32_t refreshRateUpdate) override {}
};

/**
 * @tc.name: InitHgmTaskHandleThreadTest001
 * @tc.desc: test InitHgmTaskHandleThread
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, InitHgmTaskHandleThreadTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto runner = AppExecFwk::EventRunner::Create(true);
        auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
        auto hgmContext = std::make_shared<HgmContext>(handler, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        auto orgHgmConfigUpdateCallback = frameRateMgr->hgmConfigUpdateCallback_;
        auto orgAdaptiveVsyncUpdateCallback = frameRateMgr->adaptiveVsyncUpdateCallback_;

        frameRateMgr->hgmConfigUpdateCallback_ = nullptr;
        frameRateMgr->adaptiveVsyncUpdateCallback_ = nullptr;
        ASSERT_EQ(frameRateMgr->hgmConfigUpdateCallback_, nullptr);
        ASSERT_EQ(frameRateMgr->adaptiveVsyncUpdateCallback_, nullptr);

        hgmContext->InitHgmTaskHandleThread(nullptr, nullptr, nullptr);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
        ASSERT_NE(frameRateMgr->hgmConfigUpdateCallback_, nullptr);
        ASSERT_NE(frameRateMgr->adaptiveVsyncUpdateCallback_, nullptr);

        auto orgHgmDataChangeTypes = hgmContext->hgmDataChangeTypes_;
        auto orgLtpoEnabled = hgmContext->ltpoEnabled_;
        auto orgIsDelayMode = hgmContext->isDelayMode_;
        auto orgPipelineOffsetPulseNum = hgmContext->pipelineOffsetPulseNum_;

        hgmContext->hgmDataChangeTypes_.reset();
        hgmContext->ltpoEnabled_ = false;
        hgmContext->isDelayMode_ = false;
        hgmContext->pipelineOffsetPulseNum_ = 0;

        frameRateMgr->hgmConfigUpdateCallback_(nullptr, true, true, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
        EXPECT_TRUE(hgmContext->hgmDataChangeTypes_.test(HgmDataChangeType::HGM_CONFIG_DATA));
        EXPECT_TRUE(hgmContext->ltpoEnabled_);
        EXPECT_TRUE(hgmContext->isDelayMode_);
        EXPECT_EQ(hgmContext->pipelineOffsetPulseNum_, 1);

        hgmContext->hgmDataChangeTypes_ = orgHgmDataChangeTypes;
        hgmContext->ltpoEnabled_ = orgLtpoEnabled;
        hgmContext->isDelayMode_ = orgIsDelayMode;
        hgmContext->pipelineOffsetPulseNum_ = orgPipelineOffsetPulseNum;

        auto orgIsAdaptive = hgmContext->isAdaptive_;
        auto orgGameNodeName = hgmContext->gameNodeName_;

        hgmContext->hgmDataChangeTypes_.reset();
        hgmContext->isAdaptive_ = false;
        hgmContext->gameNodeName_ = "";

        frameRateMgr->adaptiveVsyncUpdateCallback_(true, "testGameNode");
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
        EXPECT_TRUE(hgmContext->hgmDataChangeTypes_.test(HgmDataChangeType::ADAPTIVE_VSYNC));
        EXPECT_TRUE(hgmContext->isAdaptive_);
        EXPECT_EQ(hgmContext->gameNodeName_, "testGameNode");

        hgmContext->hgmDataChangeTypes_ = orgHgmDataChangeTypes;
        hgmContext->isAdaptive_ = orgIsAdaptive;
        hgmContext->gameNodeName_ = orgGameNodeName;

        frameRateMgr->hgmConfigUpdateCallback_ = orgHgmConfigUpdateCallback;
        frameRateMgr->adaptiveVsyncUpdateCallback_ = orgAdaptiveVsyncUpdateCallback;
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: HandleHgmProcessInfoTest001
 * @tc.desc: test HandleHgmProcessInfo when info is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, HandleHgmProcessInfoTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);
        hgmContext->HandleHgmProcessInfo(nullptr);
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: HandleHgmProcessInfoTest002
 * @tc.desc: test HandleHgmProcessInfo when info is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, HandleHgmProcessInfoTest002, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        auto orgIsGameNodeOnTree = frameRateMgr->IsGameNodeOnTree();

        frameRateMgr->SetIsGameNodeOnTree(false);
        ASSERT_FALSE(frameRateMgr->isGameNodeOnTree_);

        auto processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
        processToServiceInfo->frameRateLinkerDestroyIds = { 1 };
        FrameRateRange range(0, 120, 60);
        processToServiceInfo->frameRateLinkerUpdateInfoMap[2] = { range, 120 };
        processToServiceInfo->rsCurrRange = range;
        processToServiceInfo->surfaceData = { std::tuple<std::string, pid_t>({ "test", 1 }) };
        processToServiceInfo->isGameNodeOnTree = true;

        hgmContext->HandleHgmProcessInfo(processToServiceInfo);
        EXPECT_EQ(hgmContext->rsCurrRange_.preferred_, 60);
        EXPECT_TRUE(frameRateMgr->isGameNodeOnTree_);

        frameRateMgr->SetIsGameNodeOnTree(orgIsGameNodeOnTree);
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: SetServiceToProcessInfoTest001
 * @tc.desc: test SetServiceToProcessInfo where serviceToProcessInfo is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SetServiceToProcessInfoTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);
    hgmContext->SetServiceToProcessInfo(nullptr);
}

/**
 * @tc.name: SetServiceToProcessInfoTest002
 * @tc.desc: test SetServiceToProcessInfo where type contains ADAPTIVE_VSYNC
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SetServiceToProcessInfoTest002, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    auto orgPendingScreenRefreshRate = hgmCore.GetPendingScreenRefreshRate();
    auto orgPendingConstraintRelativeTime = hgmCore.GetPendingConstraintRelativeTime();

    hgmCore.SetPendingScreenRefreshRate(1);
    hgmCore.SetPendingConstraintRelativeTime(1);
    ASSERT_EQ(hgmCore.GetPendingScreenRefreshRate(), 1);
    ASSERT_EQ(hgmCore.GetPendingConstraintRelativeTime(), 1);

    hgmContext->hgmDataChangeTypes_.set(HgmDataChangeType::ADAPTIVE_VSYNC);
    hgmContext->isAdaptive_ = true;
    hgmContext->gameNodeName_ = "testGameNode";

    auto serviceToProcessInfo = sptr<HgmServiceToProcessInfo>::MakeSptr();
    hgmContext->SetServiceToProcessInfo(serviceToProcessInfo);

    EXPECT_EQ(serviceToProcessInfo->pendingScreenRefreshRate, 1);
    EXPECT_EQ(serviceToProcessInfo->pendingConstraintRelativeTime, 1);

    EXPECT_TRUE(serviceToProcessInfo->hgmDataChangeTypes.test(HgmDataChangeType::ADAPTIVE_VSYNC));
    EXPECT_TRUE(serviceToProcessInfo->isAdaptive);
    EXPECT_EQ(serviceToProcessInfo->gameNodeName, "testGameNode");

    hgmCore.SetPendingScreenRefreshRate(orgPendingScreenRefreshRate);
    hgmCore.SetPendingConstraintRelativeTime(orgPendingConstraintRelativeTime);
}

/**
 * @tc.name: SetServiceToProcessInfoTest003
 * @tc.desc: test SetServiceToProcessInfo where type contains HGM_CONFIG_DATA
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SetServiceToProcessInfoTest003, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    auto orgPendingScreenRefreshRate = hgmCore.GetPendingScreenRefreshRate();
    auto orgPendingConstraintRelativeTime = hgmCore.GetPendingConstraintRelativeTime();

    hgmCore.SetPendingScreenRefreshRate(1);
    hgmCore.SetPendingConstraintRelativeTime(1);
    ASSERT_EQ(hgmCore.GetPendingScreenRefreshRate(), 1);
    ASSERT_EQ(hgmCore.GetPendingConstraintRelativeTime(), 1);

    hgmContext->hgmDataChangeTypes_.set(HgmDataChangeType::HGM_CONFIG_DATA);
    hgmContext->ltpoEnabled_ = true;
    hgmContext->isDelayMode_ = true;
    hgmContext->pipelineOffsetPulseNum_ = 1;

    auto serviceToProcessInfo = sptr<HgmServiceToProcessInfo>::MakeSptr();
    hgmContext->SetServiceToProcessInfo(serviceToProcessInfo);

    EXPECT_EQ(serviceToProcessInfo->pendingScreenRefreshRate, 1);
    EXPECT_EQ(serviceToProcessInfo->pendingConstraintRelativeTime, 1);

    EXPECT_TRUE(serviceToProcessInfo->hgmDataChangeTypes.test(HgmDataChangeType::HGM_CONFIG_DATA));
    EXPECT_TRUE(serviceToProcessInfo->ltpoEnabled);
    EXPECT_TRUE(serviceToProcessInfo->isDelayMode);
    EXPECT_EQ(serviceToProcessInfo->pipelineOffsetPulseNum, 1);

    hgmCore.SetPendingScreenRefreshRate(orgPendingScreenRefreshRate);
    hgmCore.SetPendingConstraintRelativeTime(orgPendingConstraintRelativeTime);
}

/**
 * @tc.name: SetServiceToProcessInfoTest004
 * @tc.desc: test SetServiceToProcessInfo where type contains ADAPTIVE_VSYNC HGM_CONFIG_DATA
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SetServiceToProcessInfoTest004, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    auto orgPendingScreenRefreshRate = hgmCore.GetPendingScreenRefreshRate();
    auto orgPendingConstraintRelativeTime = hgmCore.GetPendingConstraintRelativeTime();

    hgmCore.SetPendingScreenRefreshRate(1);
    hgmCore.SetPendingConstraintRelativeTime(1);
    ASSERT_EQ(hgmCore.GetPendingScreenRefreshRate(), 1);
    ASSERT_EQ(hgmCore.GetPendingConstraintRelativeTime(), 1);

    hgmContext->hgmDataChangeTypes_.set(HgmDataChangeType::ADAPTIVE_VSYNC);
    hgmContext->isAdaptive_ = true;
    hgmContext->gameNodeName_ = "testGameNode";

    hgmContext->hgmDataChangeTypes_.set(HgmDataChangeType::HGM_CONFIG_DATA);
    hgmContext->ltpoEnabled_ = true;
    hgmContext->isDelayMode_ = true;
    hgmContext->pipelineOffsetPulseNum_ = 1;

    auto serviceToProcessInfo = sptr<HgmServiceToProcessInfo>::MakeSptr();
    hgmContext->SetServiceToProcessInfo(serviceToProcessInfo);

    EXPECT_EQ(serviceToProcessInfo->pendingScreenRefreshRate, 1);
    EXPECT_EQ(serviceToProcessInfo->pendingConstraintRelativeTime, 1);

    EXPECT_TRUE(serviceToProcessInfo->hgmDataChangeTypes.test(HgmDataChangeType::ADAPTIVE_VSYNC));
    EXPECT_TRUE(serviceToProcessInfo->isAdaptive);
    EXPECT_EQ(serviceToProcessInfo->gameNodeName, "testGameNode");

    EXPECT_TRUE(serviceToProcessInfo->hgmDataChangeTypes.test(HgmDataChangeType::HGM_CONFIG_DATA));
    EXPECT_TRUE(serviceToProcessInfo->ltpoEnabled);
    EXPECT_TRUE(serviceToProcessInfo->isDelayMode);
    EXPECT_EQ(serviceToProcessInfo->pipelineOffsetPulseNum, 1);

    hgmCore.SetPendingScreenRefreshRate(orgPendingScreenRefreshRate);
    hgmCore.SetPendingConstraintRelativeTime(orgPendingConstraintRelativeTime);
}

/**
 * @tc.name: ProcessHgmFrameRateTest001
 * @tc.desc: test ProcessHgmFrameRate where rsCurrRange_ is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, ProcessHgmFrameRateTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        ASSERT_NE(hgmContextForProcess, nullptr);

        auto orgRsCurrRange = hgmContextForProcess->rsCurrRange_;

        FrameRateRange range(0, 0, 0);
        hgmContextForProcess->rsCurrRange_ = range;

        auto processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
        hgmContextForProcess->ProcessHgmFrameRate(1, 1, processToServiceInfo, nullptr);

        EXPECT_EQ(hgmContextForProcess->GetCurrVsyncId(), 1);
        EXPECT_EQ(frameRateMgr->timestamp_, 1);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));

        hgmContextForProcess->rsCurrRange_ = orgRsCurrRange;
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: ProcessHgmFrameRateTest002
 * @tc.desc: test ProcessHgmFrameRate where rsCurrRange_ is valid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, ProcessHgmFrameRateTest002, TestSize.Level1)
{
    if (frameRateMgr) {
        ASSERT_NE(hgmContextForProcess, nullptr);

        auto orgRsCurrRange = hgmContextForProcess->rsCurrRange_;

        FrameRateRange range(0, 120, 60);
        hgmContextForProcess->rsCurrRange_ = range;

        auto processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
        hgmContextForProcess->ProcessHgmFrameRate(1, 1, processToServiceInfo, nullptr);

        EXPECT_EQ(hgmContextForProcess->GetCurrVsyncId(), 1);
        EXPECT_EQ(frameRateMgr->timestamp_, 1);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));

        hgmContextForProcess->rsCurrRange_ = orgRsCurrRange;
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: ProcessHgmFrameRateTest003
 * @tc.desc: test ProcessHgmFrameRate when no need refresh
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, ProcessHgmFrameRateTest003, TestSize.Level1)
{
    if (frameRateMgr) {
        ASSERT_NE(hgmContextForProcess, nullptr);

        auto orgVoterTouchEffective = frameRateMgr->voterTouchEffective_.load();
        auto orgPostHgmTaskFlag = hgmCore.postHgmTaskFlag_.load();
        auto orgNeedPostTask = RSVsyncRateReduceManager::needPostTask_.load();
        auto orgIsVideoCallVsyncChange = HgmEnergyConsumptionPolicy::Instance().isVideoCallVsyncChange_.load();
        auto orgPendingScreenRefreshRate = hgmCore.GetPendingScreenRefreshRate();

        frameRateMgr->voterTouchEffective_ = false;
        hgmCore.SetHgmTaskFlag(false);
        RSVsyncRateReduceManager::SetVSyncRatesChangeStatus(false);
        HgmEnergyConsumptionPolicy::Instance().GetVideoCallVsyncChange();
        hgmCore.SetPendingScreenRefreshRate(frameRateMgr->GetCurrRefreshRate());

        auto processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
        hgmContextForProcess->ProcessHgmFrameRate(1, 1, processToServiceInfo, nullptr);

        EXPECT_EQ(hgmContextForProcess->GetCurrVsyncId(), 1);
        EXPECT_EQ(frameRateMgr->timestamp_, 1);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));

        frameRateMgr->voterTouchEffective_ = orgVoterTouchEffective;
        hgmCore.SetHgmTaskFlag(orgPostHgmTaskFlag);
        RSVsyncRateReduceManager::SetVSyncRatesChangeStatus(orgNeedPostTask);
        HgmEnergyConsumptionPolicy::Instance().isVideoCallVsyncChange_ = orgIsVideoCallVsyncChange;
        hgmCore.SetPendingScreenRefreshRate(orgPendingScreenRefreshRate);
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: ProcessHgmFrameRateTest004
 * @tc.desc: test ProcessHgmFrameRate when need refresh
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, ProcessHgmFrameRateTest004, TestSize.Level1)
{
    if (frameRateMgr) {
        ASSERT_NE(hgmContextForProcess, nullptr);

        auto orgPostHgmTaskFlag = hgmCore.postHgmTaskFlag_.load();

        hgmCore.SetHgmTaskFlag(true);

        auto processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
        hgmContextForProcess->ProcessHgmFrameRate(1, 1, processToServiceInfo, nullptr);

        EXPECT_EQ(hgmContextForProcess->GetCurrVsyncId(), 1);
        EXPECT_EQ(frameRateMgr->timestamp_, 1);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));

        hgmCore.SetHgmTaskFlag(orgPostHgmTaskFlag);
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: ProcessHgmFrameRateTest005
 * @tc.desc: test ProcessHgmFrameRate where rsFrameRateLinker_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, ProcessHgmFrameRateTest005, TestSize.Level1)
{
    if (frameRateMgr) {
        ASSERT_NE(hgmContextForProcess, nullptr);

        auto orgRsFrameRateLinker = hgmContextForProcess->rsFrameRateLinker_;

        hgmContextForProcess->rsFrameRateLinker_ = nullptr;

        auto processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
        hgmContextForProcess->ProcessHgmFrameRate(1, 1, processToServiceInfo, nullptr);

        EXPECT_EQ(hgmContextForProcess->GetCurrVsyncId(), 1);
        EXPECT_EQ(frameRateMgr->timestamp_, 1);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));

        hgmContextForProcess->rsFrameRateLinker_ = orgRsFrameRateLinker;
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: AddScreenToHgmTest001
 * @tc.desc: test AddScreenToHgm
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, AddScreenToHgmTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        hgmCore.screenIds_.clear();
        hgmCore.screenList_.clear();
        ASSERT_TRUE(hgmCore.screenIds_.empty());
        ASSERT_TRUE(hgmCore.screenList_.empty());

        auto screenManager = sptr<RSScreenManager>::MakeSptr();
        hgmCore.SetScreenManager(screenManager.GetRefPtr());

        ScreenId id = 1;
        auto screenProperty = sptr<RSScreenProperty>::MakeSptr();
        screenProperty->Set<ScreenPropertyType::ID>(id);
        hgmContext->AddScreenToHgm(screenProperty);

        ASSERT_FALSE(hgmCore.screenIds_.empty());
        EXPECT_EQ(hgmCore.screenIds_.back(), id);
        ASSERT_FALSE(hgmCore.screenList_.empty());
        EXPECT_EQ(hgmCore.screenList_.back()->GetId(), id);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: RemoveScreenFromHgmTest001
 * @tc.desc: test RemoveScreenFromHgm
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, RemoveScreenFromHgmTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        hgmCore.screenIds_.clear();
        hgmCore.screenList_.clear();
        ASSERT_TRUE(hgmCore.screenIds_.empty());
        ASSERT_TRUE(hgmCore.screenList_.empty());

        ScreenId id = 1;
        auto screenProperty = sptr<RSScreenProperty>::MakeSptr();
        screenProperty->Set<ScreenPropertyType::ID>(id);
        hgmContext->AddScreenToHgm(screenProperty);

        ASSERT_FALSE(hgmCore.screenIds_.empty());
        EXPECT_EQ(hgmCore.screenIds_.back(), id);
        ASSERT_FALSE(hgmCore.screenList_.empty());
        EXPECT_EQ(hgmCore.screenList_.back()->GetId(), id);

        hgmContext->RemoveScreenFromHgm(id);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
        EXPECT_TRUE(hgmCore.screenIds_.empty());
        EXPECT_TRUE(hgmCore.screenList_.empty());
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: CreateAndUnregisterFrameRateLinkerTest001
 * @tc.desc: test CreateFrameRateLinker and UnregisterFrameRateLinker
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, CreateAndUnregisterFrameRateLinkerTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    const string name = "testLinker";
    FrameRateLinkerId id = 1;
    NodeId windowNodeId = 1;
    hgmContext->CreateFrameRateLinker(name, id, windowNodeId);
    EXPECT_NE(hgmContext->frameRateLinkerMap_.GetFrameRateLinker(id), nullptr);

    hgmContext->UnregisterFrameRateLinker(id);
    EXPECT_EQ(hgmContext->frameRateLinkerMap_.GetFrameRateLinker(id), nullptr);
}

/**
 * @tc.name: SyncFrameRateRangeTest001
 * @tc.desc: test SyncFrameRateRange when linker does not exist
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SyncFrameRateRangeTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    FrameRateLinkerId id = 1;
    FrameRateRange range(0, 120, 60);
    int32_t animatorExpectedFrameRate = 60;
    hgmContext->SyncFrameRateRange(id, range, animatorExpectedFrameRate);

    EXPECT_EQ(hgmContext->frameRateLinkerMap_.GetFrameRateLinker(id), nullptr);
}

/**
 * @tc.name: SyncFrameRateRangeTest002
 * @tc.desc: test SyncFrameRateRange when range.type is not NATIVE_VSYNC_FRAME_RATE_TYPE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SyncFrameRateRangeTest002, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    const string name = "testLinker";
    FrameRateLinkerId id = 1;
    NodeId windowNodeId = 1;
    hgmContext->CreateFrameRateLinker(name, id, windowNodeId);
    auto linker = hgmContext->frameRateLinkerMap_.GetFrameRateLinker(id);
    ASSERT_NE(linker, nullptr);

    FrameRateRange range(0, 120, 60, DISPLAY_SOLOIST_FRAME_RATE_TYPE);
    int32_t animatorExpectedFrameRate = 60;
    hgmContext->SyncFrameRateRange(id, range, animatorExpectedFrameRate);

    EXPECT_EQ(linker->expectedRange_, range);
    EXPECT_EQ(linker->animatorExpectedFrameRate_, animatorExpectedFrameRate);
}

/**
 * @tc.name: SyncFrameRateRangeTest003
 * @tc.desc: test SyncFrameRateRange when range.type is NATIVE_VSYNC_FRAME_RATE_TYPE, where vsync conn is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SyncFrameRateRangeTest003, TestSize.Level1)
{
    auto vsyncGenerator = CreateVSyncGenerator();
    auto appVSyncController = sptr<VSyncController>::MakeSptr(vsyncGenerator, 0);
    auto appVSyncDistributor = sptr<VSyncDistributor>::MakeSptr(appVSyncController, "app");
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, appVSyncDistributor, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    const string name = "testLinker";
    FrameRateLinkerId id = 1;
    NodeId windowNodeId = 1;
    hgmContext->CreateFrameRateLinker(name, id, windowNodeId);
    auto linker = hgmContext->frameRateLinkerMap_.GetFrameRateLinker(id);
    ASSERT_NE(linker, nullptr);

    FrameRateRange range(0, 120, 60, NATIVE_VSYNC_FRAME_RATE_TYPE);
    int32_t animatorExpectedFrameRate = 60;
    hgmContext->SyncFrameRateRange(id, range, animatorExpectedFrameRate);

    EXPECT_EQ(linker->expectedRange_, range);
    EXPECT_EQ(linker->animatorExpectedFrameRate_, animatorExpectedFrameRate);
}

/**
 * @tc.name: SyncFrameRateRangeTest004
 * @tc.desc: test SyncFrameRateRange when range.type is NATIVE_VSYNC_FRAME_RATE_TYPE, where vsync conn is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SyncFrameRateRangeTest004, TestSize.Level1)
{
    auto vsyncGenerator = CreateVSyncGenerator();
    auto appVSyncController = sptr<VSyncController>::MakeSptr(vsyncGenerator, 0);
    auto appVSyncDistributor = sptr<VSyncDistributor>::MakeSptr(appVSyncController, "appTest");
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, appVSyncDistributor, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    const string name = "testLinker";
    FrameRateLinkerId id = 1;
    NodeId windowNodeId = 1;
    hgmContext->CreateFrameRateLinker(name, id, windowNodeId);
    auto linker = hgmContext->frameRateLinkerMap_.GetFrameRateLinker(id);
    ASSERT_NE(linker, nullptr);

    auto connServerApp = sptr<VSyncConnection>::MakeSptr(appVSyncDistributor, "appTest", nullptr, id);
    appVSyncDistributor->AddConnection(connServerApp);

    FrameRateRange range(0, 120, 60, NATIVE_VSYNC_FRAME_RATE_TYPE);
    int32_t animatorExpectedFrameRate = 60;
    hgmContext->SyncFrameRateRange(id, range, animatorExpectedFrameRate);

    EXPECT_EQ(linker->expectedRange_, range);
    EXPECT_EQ(linker->animatorExpectedFrameRate_, animatorExpectedFrameRate);
}

/**
 * @tc.name: NotifyXComponentExpectedFrameRateTest001
 * @tc.desc: test NotifyXComponentExpectedFrameRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyXComponentExpectedFrameRateTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);
    hgmContext->NotifyXComponentExpectedFrameRate(0, "1", 60);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
}

/**
 * @tc.name: RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest001
 * @tc.desc: test RegisterFrameRateLinkerExpectedFpsUpdateCallback when dstpid is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);
    EXPECT_EQ(
        hgmContext->RegisterFrameRateLinkerExpectedFpsUpdateCallback(0, 0, nullptr), StatusCode::INVALID_ARGUMENTS);
}

/**
 * @tc.name: RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest002
 * @tc.desc: test RegisterFrameRateLinkerExpectedFpsUpdateCallback when dstpid is not 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest002, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);
    EXPECT_EQ(hgmContext->RegisterFrameRateLinkerExpectedFpsUpdateCallback(0, 1, nullptr), StatusCode::SUCCESS);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
}

/**
 * @tc.name: GetRSFrameRateLinkerTest001
 * @tc.desc: test GetRSFrameRateLinker
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, GetRSFrameRateLinkerTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);
    EXPECT_NE(hgmContext->GetRSFrameRateLinker(), nullptr);
}

/**
 * @tc.name: GetScreenCurrentRefreshRateTest001
 * @tc.desc: test GetScreenCurrentRefreshRate when id does not exist
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, GetScreenCurrentRefreshRateTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    hgmCore.screenIds_.clear();
    hgmCore.screenList_.clear();
    ASSERT_TRUE(hgmCore.screenIds_.empty());
    ASSERT_TRUE(hgmCore.screenList_.empty());

    EXPECT_EQ(hgmContext->GetScreenCurrentRefreshRate(1), 0);
}

/**
 * @tc.name: SetScreenRefreshRateTest001
 * @tc.desc: test SetScreenRefreshRate when id does not exist
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SetScreenRefreshRateTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    hgmCore.screenIds_.clear();
    hgmCore.screenList_.clear();
    ASSERT_TRUE(hgmCore.screenIds_.empty());
    ASSERT_TRUE(hgmCore.screenList_.empty());

    hgmContext->SetScreenRefreshRate(1, 1, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
}

/**
 * @tc.name: SetRefreshRateModeTest001
 * @tc.desc: test SetRefreshRateMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SetRefreshRateModeTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);
    hgmContext->SetRefreshRateMode(1);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
}

/**
 * @tc.name: GetCurrentRefreshRateModeTest001
 * @tc.desc: test GetCurrentRefreshRateMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, GetCurrentRefreshRateModeTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);
    hgmContext->GetCurrentRefreshRateMode();
}

/**
 * @tc.name: GetScreenSupportedRefreshRatesTest001
 * @tc.desc: test GetScreenSupportedRefreshRates
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, GetScreenSupportedRefreshRatesTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);
    hgmContext->GetScreenSupportedRefreshRates(1);
}

/**
 * @tc.name: NotifyDynamicModeEventTest001
 * @tc.desc: test NotifyDynamicModeEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyDynamicModeEventTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);
        hgmContext->NotifyDynamicModeEvent(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: NotifyRefreshRateEventTest001
 * @tc.desc: test NotifyRefreshRateEvent when eventName is VOTER_SCENE_BLUR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyRefreshRateEventTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    pid_t pid = 0;
    EventInfo eventInfo;
    eventInfo.eventName = "VOTER_SCENE_BLUR";
    hgmContext->NotifyRefreshRateEvent(pid, eventInfo);
}

/**
 * @tc.name: NotifyRefreshRateEventTest002
 * @tc.desc: test NotifyRefreshRateEvent when eventName is VOTER_SCENE_GPU
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyRefreshRateEventTest002, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    pid_t pid = 0;
    EventInfo eventInfo;
    eventInfo.eventName = "VOTER_SCENE_GPU";
    hgmContext->NotifyRefreshRateEvent(pid, eventInfo);
}

/**
 * @tc.name: NotifyRefreshRateEventTest003
 * @tc.desc: test NotifyRefreshRateEvent when eventName is not VOTER_SCENE_BLUR or VOTER_SCENE_GPU
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyRefreshRateEventTest003, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        pid_t pid = 0;
        EventInfo eventInfo;
        hgmContext->NotifyRefreshRateEvent(pid, eventInfo);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: NotifyLightFactorStatusTest001
 * @tc.desc: test NotifyLightFactorStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyLightFactorStatusTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        pid_t pid = 0;
        int32_t lightFactorStatus = 0;
        EXPECT_EQ(hgmContext->NotifyLightFactorStatus(pid, lightFactorStatus), ERR_OK);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: NotifyAppStrategyConfigChangeEventTest001
 * @tc.desc: test NotifyAppStrategyConfigChangeEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyAppStrategyConfigChangeEventTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        pid_t pid = 0;
        std::string pkgName = "testPkg";
        std::vector<std::pair<std::string, std::string>> newConfig;

        EXPECT_EQ(hgmContext->NotifyAppStrategyConfigChangeEvent(pid, pkgName, newConfig), ERR_OK);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: HandleTouchEventTest001
 * @tc.desc: test HandleTouchEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, HandleTouchEventTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        pid_t pid = 0;
        int32_t touchStatus = 1;
        int32_t touchCnt = 0;
        hgmContext->HandleTouchEvent(pid, touchStatus, touchCnt);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: NotifyPackageEventTouchEventTest001
 * @tc.desc: test NotifyPackageEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyPackageEventTouchEventTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        pid_t pid = 0;
        std::vector<std::string> packageList;
        hgmContext->NotifyPackageEvent(pid, packageList);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: NotifyHgmConfigEventTest001
 * @tc.desc: test NotifyHgmConfigEvent when eventName is HGMCONFIG_HIGH_TEMP
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyHgmConfigEventTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        auto orgScreenExtStrategyMap = frameRateMgr->screenExtStrategyMap_;

        frameRateMgr->screenExtStrategyMap_ = HGM_CONFIG_SCREENEXT_STRATEGY_MAP;
        auto screenExtStrategyMap = frameRateMgr->screenExtStrategyMap_;
        auto screenExtStrategyMapIter = frameRateMgr->screenExtStrategyMap_.find(HGM_CONFIG_TYPE_THERMAL_SUFFIX);
        ASSERT_NE(screenExtStrategyMapIter, screenExtStrategyMap.end());
        screenExtStrategyMapIter->second.second = false;
        ASSERT_FALSE(screenExtStrategyMapIter->second.second);

        std::string eventName = "HGMCONFIG_HIGH_TEMP";
        bool state = true;
        hgmContext->NotifyHgmConfigEvent(eventName, state);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
        EXPECT_EQ(screenExtStrategyMapIter->second.second, state);

        frameRateMgr->screenExtStrategyMap_ = orgScreenExtStrategyMap;
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: NotifyHgmConfigEventTest002
 * @tc.desc: test NotifyHgmConfigEvent when eventName is IA_DRAG_SLIDE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyHgmConfigEventTest002, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        auto orgScreenExtStrategyMap = frameRateMgr->screenExtStrategyMap_;

        frameRateMgr->screenExtStrategyMap_ = HGM_CONFIG_SCREENEXT_STRATEGY_MAP;
        auto screenExtStrategyMap = frameRateMgr->screenExtStrategyMap_;
        auto screenExtStrategyMapIter = frameRateMgr->screenExtStrategyMap_.find(HGM_CONFIG_TYPE_DRAGSLIDE_SUFFIX);
        ASSERT_NE(screenExtStrategyMapIter, screenExtStrategyMap.end());
        screenExtStrategyMapIter->second.second = false;
        ASSERT_FALSE(screenExtStrategyMapIter->second.second);

        std::string eventName = "IA_DRAG_SLIDE";
        bool state = true;
        hgmContext->NotifyHgmConfigEvent(eventName, state);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
        EXPECT_EQ(screenExtStrategyMapIter->second.second, state);

        frameRateMgr->screenExtStrategyMap_ = orgScreenExtStrategyMap;
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: NotifyHgmConfigEventTest003
 * @tc.desc: test NotifyHgmConfigEvent when eventName is IA_THROW_SLIDE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifyHgmConfigEventTest003, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        auto orgScreenExtStrategyMap = frameRateMgr->screenExtStrategyMap_;

        frameRateMgr->screenExtStrategyMap_ = HGM_CONFIG_SCREENEXT_STRATEGY_MAP;
        auto screenExtStrategyMap = frameRateMgr->screenExtStrategyMap_;
        auto screenExtStrategyMapIter = frameRateMgr->screenExtStrategyMap_.find(HGM_CONFIG_TYPE_THROWSLIDE_SUFFIX);
        ASSERT_NE(screenExtStrategyMapIter, screenExtStrategyMap.end());
        screenExtStrategyMapIter->second.second = false;
        ASSERT_FALSE(screenExtStrategyMapIter->second.second);

        std::string eventName = "IA_THROW_SLIDE";
        bool state = true;
        hgmContext->NotifyHgmConfigEvent(eventName, state);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
        EXPECT_EQ(screenExtStrategyMapIter->second.second, state);

        frameRateMgr->screenExtStrategyMap_ = orgScreenExtStrategyMap;
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: RegisterHgmRefreshRateUpdateCallbackTest001
 * @tc.desc: test RegisterHgmRefreshRateUpdateCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, RegisterHgmRefreshRateUpdateCallbackTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    pid_t pid = 0;
    sptr<RSIHgmConfigChangeCallback> callback = nullptr;
    EXPECT_EQ(hgmContext->RegisterHgmRefreshRateUpdateCallback(pid, callback), StatusCode::SUCCESS);
}

/**
 * @tc.name: RegisterHgmConfigChangeCallbackTest001
 * @tc.desc: test RegisterHgmConfigChangeCallback when callback is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, RegisterHgmConfigChangeCallbackTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    pid_t pid = 0;
    sptr<RSIHgmConfigChangeCallback> callback = nullptr;
    EXPECT_EQ(hgmContext->RegisterHgmConfigChangeCallback(pid, callback), StatusCode::INVALID_ARGUMENTS);
}

/**
 * @tc.name: RegisterHgmConfigChangeCallbackTest002
 * @tc.desc: test RegisterHgmConfigChangeCallback when callback is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, RegisterHgmConfigChangeCallbackTest002, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    pid_t pid = 0;
    sptr<CustomHgmCallback> callback = new CustomHgmCallback();
    EXPECT_EQ(hgmContext->RegisterHgmConfigChangeCallback(pid, callback), StatusCode::SUCCESS);
}

/**
 * @tc.name: RegisterHgmRefreshRateModeChangeCallbackTest001
 * @tc.desc: test RegisterHgmRefreshRateModeChangeCallback when callback is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, RegisterHgmRefreshRateModeChangeCallbackTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    pid_t pid = 0;
    sptr<RSIHgmConfigChangeCallback> callback = nullptr;
    EXPECT_EQ(hgmContext->RegisterHgmRefreshRateModeChangeCallback(pid, callback), StatusCode::INVALID_ARGUMENTS);
}

/**
 * @tc.name: RegisterHgmRefreshRateModeChangeCallbackTest002
 * @tc.desc: test RegisterHgmRefreshRateModeChangeCallback when callback is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, RegisterHgmRefreshRateModeChangeCallbackTest002, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    pid_t pid = 0;
    sptr<CustomHgmCallback> callback = new CustomHgmCallback();
    EXPECT_EQ(hgmContext->RegisterHgmRefreshRateModeChangeCallback(pid, callback), StatusCode::SUCCESS);
}

/**
 * @tc.name: SetWindowExpectedRefreshRateTest001
 * @tc.desc: test SetWindowExpectedRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SetWindowExpectedRefreshRateTest001, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        pid_t pid = 0;
        std::unordered_map<uint64_t, EventInfo> eventInfos;
        hgmContext->SetWindowExpectedRefreshRate(pid, eventInfos);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: SetWindowExpectedRefreshRateTest002
 * @tc.desc: test SetWindowExpectedRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, SetWindowExpectedRefreshRateTest002, TestSize.Level1)
{
    if (frameRateMgr) {
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, nullptr, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        pid_t pid = 0;
        std::unordered_map<std::string, EventInfo> eventInfos;
        hgmContext->SetWindowExpectedRefreshRate(pid, eventInfos);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: NotifySoftVsyncRateDiscountEventTest001
 * @tc.desc: test NotifySoftVsyncRateDiscountEvent when linkerIds is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifySoftVsyncRateDiscountEventTest001, TestSize.Level1)
{
    auto vsyncGenerator = CreateVSyncGenerator();
    auto appVSyncController = sptr<VSyncController>::MakeSptr(vsyncGenerator, 0);
    auto appVSyncDistributor = sptr<VSyncDistributor>::MakeSptr(appVSyncController, "app");
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, appVSyncDistributor, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    uint32_t pid = 0;
    std::string name = "testVsync";
    uint32_t rateDiscount = 0;
    ASSERT_TRUE(appVSyncDistributor->GetVsyncNameLinkerIds(pid, name).empty());
    EXPECT_FALSE(hgmContext->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount));
}

/**
 * @tc.name: NotifySoftVsyncRateDiscountEventTest002
 * @tc.desc: test NotifySoftVsyncRateDiscountEvent when linkerIds is not empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, NotifySoftVsyncRateDiscountEventTest002, TestSize.Level1)
{
    if (frameRateMgr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto appVSyncController = sptr<VSyncController>::MakeSptr(vsyncGenerator, 0);
        auto appVSyncDistributor = sptr<VSyncDistributor>::MakeSptr(appVSyncController, "app");
        auto hgmContext = std::make_shared<HgmContext>(nullptr, frameRateMgr, nullptr, appVSyncDistributor, nullptr);
        ASSERT_NE(hgmContext, nullptr);

        uint64_t id = 4294967296;
        uint32_t pid = 1;
        std::string name = "testVsync";
        uint32_t rateDiscount = 0;
        auto connServerApp = sptr<VSyncConnection>::MakeSptr(appVSyncDistributor, name, nullptr, id);
        appVSyncDistributor->AddConnection(connServerApp);
        ASSERT_FALSE(appVSyncDistributor->GetVsyncNameLinkerIds(pid, name).empty());
        EXPECT_TRUE(hgmContext->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount));
        appVSyncDistributor->RemoveConnection(connServerApp);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    } else {
        EXPECT_EQ(hgmCore.mPolicyConfigData_, nullptr);
    }
}

/**
 * @tc.name: LastForceUpdateVsyncIdTest001
 * @tc.desc: test SetLastForceUpdateVsyncId and GetLastForceUpdateVsyncId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, LastForceUpdateVsyncIdTest001, TestSize.Level1)
{
    auto hgmContext = std::make_shared<HgmContext>(nullptr, nullptr, nullptr, nullptr, nullptr);
    ASSERT_NE(hgmContext, nullptr);

    uint64_t lastForceUpdateVsyncId = 1;
    hgmContext->SetLastForceUpdateVsyncId(lastForceUpdateVsyncId);

    EXPECT_EQ(hgmContext->GetLastForceUpdateVsyncId(), lastForceUpdateVsyncId);
}
} // namespace OHOS::Rosen