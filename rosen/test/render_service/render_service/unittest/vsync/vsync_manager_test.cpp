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

#include "vsync_manager.h"
#include "vsync_manager_agent.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class VSyncManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<RSVsyncManager> vsyncManager_;
    static inline sptr<RSScreenManager> screenManager_;

    static constexpr const int32_t WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS = 2;
};

void VSyncManagerTest::SetUpTestCase()
{
    screenManager_ = sptr<RSScreenManager>::MakeSptr();
    vsyncManager_ = vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
    if (!vsyncManager_->init(screenManager_)) {
        RS_LOGE("vsync manager case::%{public}s: vsync init failed", __func__);
    }
}

void VSyncManagerTest::TearDownTestCase()
{
    sleep(WAIT_SYSTEM_ABILITY_REPORT_DATA_SECONDS);
    screenManager_ = nullptr;
    vsyncManager_ = nullptr;
}

namespace {
/*
* Function: GetVsyncManagerAgent
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetVsyncManagerAgent
 */
HWTEST_F(VSyncManagerTest, GetVsyncManagerAgent, Function | MediumTest| Level0)
{
    auto agent = vsyncManager_->GetVsyncManagerAgent();
    ASSERT_NE(agent, nullptr);
}

/*
* Function: GetVsyncAppDistributor
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetVsyncAppDistributor
 */
HWTEST_F(VSyncManagerTest, GetVsyncAppDistributor, Function | MediumTest| Level0)
{
    auto appDistributor = vsyncManager_->GetVSyncAppDistributor();
    ASSERT_NE(appDistributor, nullptr);
}

/*
* Function: GetVsyncRSDistributor
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetVsyncRSDistributor
 */
HWTEST_F(VSyncManagerTest, GetVsyncRSDistributor, Function | MediumTest| Level0)
{
    auto rsDistributor = vsyncManager_->GetVSyncRSDistributor();
    ASSERT_NE(rsDistributor, nullptr);
}

/*
* Function: GetVsyncGenerator
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetVsyncGenerator
 */
HWTEST_F(VSyncManagerTest, GetVsyncGenerator, Function | MediumTest| Level0)
{
    auto generator = vsyncManager_->GetVSyncGenerator();
    ASSERT_NE(generator, nullptr);
}

/*
* Function: GetVsyncRSController
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetVsyncRSController
 */
HWTEST_F(VSyncManagerTest, GetVsyncRSController, Function | MediumTest| Level0)
{
    auto rsController = vsyncManager_->GetVSyncRSController();
    ASSERT_NE(rsController, nullptr);
}

/*
* Function: GetVsyncAppController
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetVsyncAppController
 */
HWTEST_F(VSyncManagerTest, GetVsyncAppController, Function | MediumTest| Level0)
{
    auto appController = vsyncManager_->GetVSyncAppController();
    ASSERT_NE(appController, nullptr);
}

/*
* Function: GetVsyncSampler
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetVsyncSampler
 */
HWTEST_F(VSyncManagerTest, GetVsyncSampler, Function | MediumTest| Level0)
{
    auto sampler = vsyncManager_->GetVSyncSampler();
    ASSERT_NE(sampler, nullptr);
}

/*
* Function: init
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test init
 */
HWTEST_F(VSyncManagerTest, init, Function | MediumTest| Level0)
{
    auto screenManager = vsyncManager_->screenManager_;
    auto res = vsyncManager_->init(nullptr);
    ASSERT_FALSE(res);
    vsyncManager_->SetScreenManager(screenManager);
}

/*
* Function: VsyncComponentInit
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test VsyncComponentInit
 */
HWTEST_F(VSyncManagerTest, VsyncComponentInit, Function | MediumTest| Level0)
{
    auto screenManager = vsyncManager_->screenManager_;
    vsyncManager_->screenManager_ = nullptr;
    ASSERT_FALSE(vsyncManager_->VsyncComponentInit());
    vsyncManager_->screenManager_ = screenManager;
}

/*
* Function: OnScreenConnected
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnScreenConnected
 */
HWTEST_F(VSyncManagerTest, OnScreenConnected, Function | MediumTest| Level0)
{
    ScreenId testId = 12;
    ScreenId id = vsyncManager_->OnScreenConnected(testId, nullptr);
    ASSERT_EQ(id, 12);
}

/*
* Function: OnScreenDisconnected
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnScreenDisconnected
 */
HWTEST_F(VSyncManagerTest, OnScreenDisconnected, Function | MediumTest| Level0)
{
    ScreenId testId = 12;
    vsyncManager_->physicalScreens_.insert(testId);
    vsyncManager_->OnScreenDisconnected(testId, nullptr);
    bool res = vsyncManager_->physicalScreens_.find(testId) == vsyncManager_->physicalScreens_.end();
    ASSERT_TRUE(res);
}

/*
* Function: OnScreenPropertyChanged
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnScreenPropertyChanged
 */
HWTEST_F(VSyncManagerTest, OnScreenPropertyChanged, Function | MediumTest| Level0)
{
    ScreenId testId = 12;
    ScreenPropertyType type = ScreenPropertyType::POWER_STATUS;
    bool isFoldScreen = false;
    sptr<ScreenPropertyBase> propertyBase = sptr<ScreenProperty<uint32_t>>::MakeSptr();
    vsyncManager_->OnScreenPropertyChanged(testId, type, propertyBase, nullptr, isFoldScreen);
    ASSERT_NE(vsyncManager_->vsyncSampler_, nullptr);
}

/*
* Function: OnScreenPropertyChanged
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnScreenPropertyChanged with rsVSyncDistributor_ nullptr
*/
HWTEST_F(VSyncManagerTest, OnScreenPropertyChanged_RsDistributorNull, Function | MediumTest| Level0)
{
    ScreenId testId = 12;
    ScreenPropertyType type = ScreenPropertyType::POWER_STATUS;
    bool isFoldScreen = false;
    sptr<ScreenProperty<uint32_t>> propertyBase = sptr<ScreenProperty<uint32_t>>::MakeSptr();
    propertyBase->Set(static_cast<uint32_t>(ScreenPowerStatus::POWER_STATUS_ON));

    auto distributor = vsyncManager_->rsVSyncDistributor_;
    vsyncManager_->rsVSyncDistributor_ = nullptr;
    vsyncManager_->OnScreenPropertyChanged(testId, type, propertyBase, nullptr, isFoldScreen);
    vsyncManager_->rsVSyncDistributor_ = distributor;
    ASSERT_NE(vsyncManager_->rsVSyncDistributor_, nullptr);
}

/*
* Function: OnScreenPropertyChanged
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnScreenPropertyChanged with POWER_STATUS_ON and id not in set
*/
HWTEST_F(VSyncManagerTest, OnScreenPropertyChanged_PowerStatusOn_Insert, Function | MediumTest| Level0)
{
    ScreenId testId = 13;
    ScreenPropertyType type = ScreenPropertyType::POWER_STATUS;
    bool isFoldScreen = false;
    sptr<ScreenProperty<uint32_t>> propertyBase = sptr<ScreenProperty<uint32_t>>::MakeSptr();
    propertyBase->Set(static_cast<uint32_t>(ScreenPowerStatus::POWER_STATUS_ON));

    vsyncManager_->physicalScreens_.clear();
    vsyncManager_->OnScreenPropertyChanged(testId, type, propertyBase, nullptr, isFoldScreen);
    ASSERT_EQ(vsyncManager_->physicalScreens_.size(), 1);
    ASSERT_TRUE(vsyncManager_->physicalScreens_.find(testId) != vsyncManager_->physicalScreens_.end());
}

/*
* Function: OnScreenPropertyChanged
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnScreenPropertyChanged with POWER_STATUS_ON and id already in set
*/
HWTEST_F(VSyncManagerTest, OnScreenPropertyChanged_PowerStatusOn_AlreadyInSet, Function | MediumTest| Level0)
{
    ScreenId testId = 14;
    ScreenPropertyType type = ScreenPropertyType::POWER_STATUS;
    bool isFoldScreen = false;
    sptr<ScreenProperty<uint32_t>> propertyBase = sptr<ScreenProperty<uint32_t>>::MakeSptr();
    propertyBase->Set(static_cast<uint32_t>(ScreenPowerStatus::POWER_STATUS_ON));

    vsyncManager_->physicalScreens_.clear();
    vsyncManager_->physicalScreens_.insert(testId);
    size_t prevSize = vsyncManager_->physicalScreens_.size();
    vsyncManager_->OnScreenPropertyChanged(testId, type, propertyBase, nullptr, isFoldScreen);
    ASSERT_EQ(vsyncManager_->physicalScreens_.size(), prevSize);
}

/*
* Function: OnScreenPropertyChanged
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnScreenPropertyChanged with POWER_STATUS_OFF and id in set
*/
HWTEST_F(VSyncManagerTest, OnScreenPropertyChanged_PowerStatusOff_Remove, Function | MediumTest| Level0)
{
    ScreenId testId = 15;
    ScreenPropertyType type = ScreenPropertyType::POWER_STATUS;
    bool isFoldScreen = false;
    sptr<ScreenProperty<uint32_t>> propertyBase = sptr<ScreenProperty<uint32_t>>::MakeSptr();
    propertyBase->Set(static_cast<uint32_t>(ScreenPowerStatus::POWER_STATUS_OFF));

    vsyncManager_->physicalScreens_.clear();
    vsyncManager_->physicalScreens_.insert(testId);
    vsyncManager_->OnScreenPropertyChanged(testId, type, propertyBase, nullptr, isFoldScreen);
    ASSERT_TRUE(vsyncManager_->physicalScreens_.find(testId) == vsyncManager_->physicalScreens_.end());
}

/*
* Function: OnScreenPropertyChanged
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnScreenPropertyChanged with POWER_STATUS_OFF and id not in set
*/
HWTEST_F(VSyncManagerTest, OnScreenPropertyChanged_PowerStatusOff_NotInSet, Function | MediumTest| Level0)
{
    ScreenId testId = 16;
    ScreenPropertyType type = ScreenPropertyType::POWER_STATUS;
    bool isFoldScreen = false;
    sptr<ScreenProperty<uint32_t>> propertyBase = sptr<ScreenProperty<uint32_t>>::MakeSptr();
    propertyBase->Set(static_cast<uint32_t>(ScreenPowerStatus::POWER_STATUS_OFF));

    vsyncManager_->physicalScreens_.clear();
    size_t prevSize = vsyncManager_->physicalScreens_.size();
    vsyncManager_->OnScreenPropertyChanged(testId, type, propertyBase, nullptr, isFoldScreen);
    ASSERT_EQ(vsyncManager_->physicalScreens_.size(), prevSize);
}

/*
* Function: OnScreenPropertyChanged
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnScreenPropertyChanged with other power status
*/
HWTEST_F(VSyncManagerTest, OnScreenPropertyChanged_OtherPowerStatus, Function | MediumTest| Level0)
{
    ScreenId testId = 17;
    ScreenPropertyType type = ScreenPropertyType::POWER_STATUS;
    bool isFoldScreen = false;
    sptr<ScreenProperty<uint32_t>> propertyBase = sptr<ScreenProperty<uint32_t>>::MakeSptr();
    propertyBase->Set(static_cast<uint32_t>(ScreenPowerStatus::POWER_STATUS_SUSPEND));

    vsyncManager_->physicalScreens_.clear();
    vsyncManager_->OnScreenPropertyChanged(testId, type, propertyBase, nullptr, isFoldScreen);
    ASSERT_EQ(vsyncManager_->physicalScreens_.size(), 0);
}

/*
* Function: AddRSVsyncConnection
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test AddRSVsyncConnection
 */
HWTEST_F(VSyncManagerTest, AddRSVsyncConnection, Function | MediumTest| Level0)
{
    auto rsDistributor = vsyncManager_->rsVSyncDistributor_;
    sptr<VSyncIConnectionToken> vsyncToken = new IRemoteStub<VSyncIConnectionToken>();
    sptr<VSyncConnection> conn = vsyncManager_->CreateRSVSyncConnection("test", vsyncToken->AsObject());
    ASSERT_EQ(vsyncManager_->AddRSVsyncConnection(conn), VSYNC_ERROR_OK);
    vsyncManager_->rsVSyncDistributor_ = nullptr;
    ASSERT_EQ(vsyncManager_->AddRSVsyncConnection(conn), VSYNC_ERROR_NULLPTR);
    vsyncManager_->rsVSyncDistributor_ = rsDistributor;
}

/*
* Function: CreateRSVSyncConnection
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test CreateRSVSyncConnection
 */
HWTEST_F(VSyncManagerTest, CreateRSVSyncConnection, Function | MediumTest| Level0)
{
    sptr<VSyncIConnectionToken> vsyncToken = new IRemoteStub<VSyncIConnectionToken>();
    std::string name = "test";
    ASSERT_NE(vsyncManager_->CreateRSVSyncConnection(name, vsyncToken->AsObject()), nullptr);
}

/*
* Function: RegSetScreenVsyncEnabledCallbackForRenderService
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test RegSetScreenVsyncEnabledCallbackForRenderService
 */
HWTEST_F(VSyncManagerTest, RegSetScreenVsyncEnabledCallbackForRenderService, Function | MediumTest| Level0)
{
    ScreenId vsyncId = 12;
    vsyncManager_->RegSetScreenVsyncEnabledCallbackForRenderService(vsyncId, nullptr);
    auto vsyncSamplerImpl = static_cast<impl::VSyncSampler*>(vsyncManager_->vsyncSampler_.GetRefPtr());
    ASSERT_NE(vsyncSamplerImpl->updateVsyncEnabledScreenIdCallback_, nullptr);
}

/*
* Function: JudgeVSyncEnabledScreenWhileHotPlug
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test JudgeVSyncEnabledScreenWhileHotPlug
 */
HWTEST_F(VSyncManagerTest, JudgeVSyncEnabledScreenWhileHotPlug, Function | MediumTest| Level0)
{
    ScreenId vsyncId = 12;
    bool connected = true;
    auto sampler = vsyncManager_->vsyncSampler_;
    ScreenId id = vsyncManager_->JudgeVSyncEnabledScreenWhileHotPlug(vsyncId, connected);
    ASSERT_EQ(id, 12);
    vsyncManager_->vsyncSampler_ = nullptr;
    id = vsyncManager_->JudgeVSyncEnabledScreenWhileHotPlug(vsyncId, connected);
    ASSERT_EQ(id, INVALID_SCREEN_ID);
    vsyncManager_->vsyncSampler_ = sampler;
}

/*
* Function: InitDVSyncParams
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test InitDVSyncParams
 */
HWTEST_F(VSyncManagerTest, InitDVSyncParams, Function | MediumTest| Level0)
{
    auto dvsyncParam = vsyncManager_->InitDVSyncParams();
    ASSERT_NE(dvsyncParam.bufferCountParams[0], 12);
}

/*
* Function: DoSamplerCallBack
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test DoSamplerCallBack
 */
HWTEST_F(VSyncManagerTest, DoSamplerCallBack, Function | MediumTest| Level0)
{
    ScreenId id = 14;
    ScreenId lastVsyncEnabledScreenId = 16;
    ScreenPowerStatus status = ScreenPowerStatus::POWER_STATUS_SUSPEND;
    bool enabled = true;
    auto res = static_cast<impl::VSyncSampler*>(
        vsyncManager_->vsyncSampler_.GetRefPtr())->judgeVSyncEnabledScreenWhilePowerStatusChangedCallback_(
            id, status, lastVsyncEnabledScreenId);
    ASSERT_EQ(res, lastVsyncEnabledScreenId);
    static_cast<impl::VSyncSampler*>(
        vsyncManager_->vsyncSampler_.GetRefPtr())->setScreenVsyncEnableByIdCallback_(
            id, lastVsyncEnabledScreenId, enabled);
    ASSERT_NE(vsyncManager_->vsyncSampler_, nullptr);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
