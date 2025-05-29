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
#include "limit_number.h"
#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class VsyncEnabledScreenIdTest : public testing::Test {
public:
    static constexpr uint32_t SLEEP_TIME_FOR_DELAY = 1000000; // 1000ms

    impl::VSyncSampler* GetVSyncSamplerImplPtr();
    impl::RSScreenManager* GetRSScreenManagerImplPtr();
    void SetScreenPowerStatusDelay(sptr<RSScreenManager> screenManager, ScreenId id, ScreenPowerStatus status);
};

impl::VSyncSampler* VsyncEnabledScreenIdTest::GetVSyncSamplerImplPtr()
{
    auto sampler = CreateVSyncSampler();
    if (sampler == nullptr) {
        return nullptr;
    }
    return static_cast<impl::VSyncSampler*>(sampler.GetRefPtr());
}

impl::RSScreenManager* VsyncEnabledScreenIdTest::GetRSScreenManagerImplPtr()
{
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        return nullptr;
    }
    return static_cast<impl::RSScreenManager*>(screenManager.GetRefPtr());
}

void VsyncEnabledScreenIdTest::SetScreenPowerStatusDelay(
    sptr<RSScreenManager> screenManager, ScreenId id, ScreenPowerStatus status)
{
    screenManager->SetScreenPowerStatus(id, status);
    usleep(SLEEP_TIME_FOR_DELAY);
}

/*
 * @tc.name: ScreenConnectAndDisconnectTest001
 * @tc.desc: test screen disconnect in reverse order.
 * @tc.type: FUNC
 * CaseDescription: 1. preSetup: not fold screen.
 *                  2. operation: simulate connecting 3 screens and disconnecting them in reverse order.
 *                  3. result: vsyncEnabledScreenId_ is UINT64_MAX if there is no screen connected,
 *                             and vsyncEnabledScreenId_ is 0 otherwise.
 */
HWTEST_F(VsyncEnabledScreenIdTest, ScreenConnectAndDisconnectTest001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    // not fold screen
    impl::RSScreenManager* screenManagerImpl = GetRSScreenManagerImplPtr();
    screenManagerImpl->isFoldScreenFlag_ = false;

    auto hdiOutput1 = HdiOutput::CreateHdiOutput(0); // screenId 0
    auto hdiOutput2 = HdiOutput::CreateHdiOutput(1); // screenId 1
    auto hdiOutput3 = HdiOutput::CreateHdiOutput(2); // screenId 2

    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, UINT64_MAX);

    // 1st screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput1, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 2nd screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput2, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 3rd screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput3, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 3rd screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput3, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 2nd screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput2, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 1st screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput1, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, UINT64_MAX);
}

/*
 * @tc.name: ScreenConnectAndDisconnectTest002
 * @tc.desc: test screen disconnect in sequence.
 * @tc.type: FUNC
 * CaseDescription: 1. preSetup: not fold screen.
 *                  2. operation: simulate connecting 3 screens and disconnecting them in sequence.
 *                  3. result: vsyncEnabledScreenId_ is UINT64_MAX if there is no screen connected,
 *                             and vsyncEnabledScreenId_ is not UINT64_MAX otherwise.
 */
HWTEST_F(VsyncEnabledScreenIdTest, ScreenConnectAndDisconnectTest002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    // not fold screen
    impl::RSScreenManager* screenManagerImpl = GetRSScreenManagerImplPtr();
    screenManagerImpl->isFoldScreenFlag_ = false;

    auto hdiOutput1 = HdiOutput::CreateHdiOutput(0); // screenId 0
    auto hdiOutput2 = HdiOutput::CreateHdiOutput(1); // screenId 1
    auto hdiOutput3 = HdiOutput::CreateHdiOutput(2); // screenId 2

    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, UINT64_MAX);

    // 1st screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput1, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 2nd screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput2, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 3rd screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput3, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 1st screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput1, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // 2nd screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput2, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 2);

    // 3rd screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput3, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, UINT64_MAX);
}

/*
 * @tc.name: SetScreenPowerStatusTest001
 * @tc.desc: test SetScreenPowerStatus when isFoldScreenFlag_ is false.
 * @tc.type: FUNC
 * CaseDescription: 1. preSetup: isFoldScreenFlag_ is false.
 *                  2. operation: simulate the connection and disconnection of 3 screens, and test SetScreenPowerStatus.
 *                  3. result: SetScreenPowerStatus will not change the value of vsyncEnabledScreenId_.
 */
HWTEST_F(VsyncEnabledScreenIdTest, SetScreenPowerStatusTest001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    // not fold screen
    impl::RSScreenManager* screenManagerImpl = GetRSScreenManagerImplPtr();
    screenManagerImpl->isFoldScreenFlag_ = false;

    auto hdiOutput1 = HdiOutput::CreateHdiOutput(0); // screenId 0
    auto hdiOutput2 = HdiOutput::CreateHdiOutput(1); // screenId 1
    auto hdiOutput3 = HdiOutput::CreateHdiOutput(2); // screenId 2

    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, UINT64_MAX);

    // 1st screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput1, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 2nd screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput2, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 3rd screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput3, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 0 power on
    SetScreenPowerStatusDelay(screenManager, 0, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 1 power on
    SetScreenPowerStatusDelay(screenManager, 1, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 2 power on
    SetScreenPowerStatusDelay(screenManager, 2, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 1st screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput1, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // 2nd screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput2, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 2);

    // 3rd screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput3, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, UINT64_MAX);
}

/*
 * @tc.name: SetScreenPowerStatusTest002
 * @tc.desc: test SetScreenPowerStatus when isFoldScreenFlag_ is true.
 * @tc.type: FUNC
 * CaseDescription: 1. preSetup: isFoldScreenFlag_ is true.
 *                  2. operation: simulate the connection and disconnection of 3 screens, and test SetScreenPowerStatus.
 *                  3. result: SetScreenPowerStatus will change the value of vsyncEnabledScreenId_,
 *                             if the screenId is in the first two connected screens.
 *                             Otherwise, the value of vsyncEnabledScreenId_ will not change.
 */
HWTEST_F(VsyncEnabledScreenIdTest, SetScreenPowerStatusTest002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    // fold screen
    impl::RSScreenManager* screenManagerImpl = GetRSScreenManagerImplPtr();
    screenManagerImpl->isFoldScreenFlag_ = true;

    auto hdiOutput1 = HdiOutput::CreateHdiOutput(0); // screenId 0
    auto hdiOutput2 = HdiOutput::CreateHdiOutput(1); // screenId 1
    auto hdiOutput3 = HdiOutput::CreateHdiOutput(2); // screenId 2

    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, UINT64_MAX);

    // 1st screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput1, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 1 power on
    SetScreenPowerStatusDelay(screenManager, 1, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 1 power off
    SetScreenPowerStatusDelay(screenManager, 1, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 2nd screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput2, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 1 power on
    SetScreenPowerStatusDelay(screenManager, 1, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // 3rd screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput3, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // set screen 2 power on
    SetScreenPowerStatusDelay(screenManager, 2, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // set screen 0 power on
    SetScreenPowerStatusDelay(screenManager, 0, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // set screen 1 power off
    SetScreenPowerStatusDelay(screenManager, 1, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 2 power off
    SetScreenPowerStatusDelay(screenManager, 2, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 0 power off
    SetScreenPowerStatusDelay(screenManager, 0, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 1st screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput1, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // 2nd screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput2, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 2);

    // 3rd screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput3, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, UINT64_MAX);
}

/*
 * @tc.name: SetScreenPowerStatusTest003
 * @tc.desc: test SetScreenPowerStatus when isFoldScreenFlag_ is true.
 * @tc.type: FUNC
 * CaseDescription: 1. preSetup: isFoldScreenFlag_ is true.
 *                  2. operation: simulate fold screen switchover.
 *                  3. result: 1) when a screen power off, and this screen is vsyncEnabledScreenId_,
 *                                select a POWER_ON status screen as the new vsyncEnabledScreenId_.
 *                             2) when a screen power off, and this screen is vsyncEnabledScreenId_,
 *                                and all other fold screen is POWER_OFF status, vsyncEnabledScreenId_ won't change.
 *                             3) when a screen power off, and this screen is not vsyncEnabledScreenId_,
 *                                vsyncEnabledScreenId_ won't change.
 *                             4) when a screen power on, and vsyncEnabledScreenId_ is POWER_OFF status,
 *                                select this screen as the new vsyncEnabledScreenId_.
 *                             5) when a screen power on, and vsyncEnabledScreenId_ is POWER_ON status,
 *                                vsyncEnabledScreenId_ won't change.
 */
HWTEST_F(VsyncEnabledScreenIdTest, SetScreenPowerStatusTest003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    // fold screen
    impl::RSScreenManager* screenManagerImpl = GetRSScreenManagerImplPtr();
    screenManagerImpl->isFoldScreenFlag_ = true;

    auto hdiOutput1 = HdiOutput::CreateHdiOutput(0); // screenId 0
    auto hdiOutput2 = HdiOutput::CreateHdiOutput(1); // screenId 1
    auto hdiOutput3 = HdiOutput::CreateHdiOutput(2); // screenId 2

    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, UINT64_MAX);

    // 3 screen connected
    impl::RSScreenManager::OnHotPlug(hdiOutput1, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);
    impl::RSScreenManager::OnHotPlug(hdiOutput2, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);
    impl::RSScreenManager::OnHotPlug(hdiOutput3, true, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 1 power on
    SetScreenPowerStatusDelay(screenManager, 1, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // set screen 0 power on
    SetScreenPowerStatusDelay(screenManager, 0, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // set screen 0 power off
    SetScreenPowerStatusDelay(screenManager, 0, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // set screen 0 power on
    SetScreenPowerStatusDelay(screenManager, 0, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // set screen 1 power off
    SetScreenPowerStatusDelay(screenManager, 1, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 1 power on
    SetScreenPowerStatusDelay(screenManager, 1, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 0 power off
    SetScreenPowerStatusDelay(screenManager, 0, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // set screen 1 power off
    SetScreenPowerStatusDelay(screenManager, 1, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);

    // set screen 0 power on
    SetScreenPowerStatusDelay(screenManager, 0, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 0 power off
    SetScreenPowerStatusDelay(screenManager, 0, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // set screen 0 power on
    SetScreenPowerStatusDelay(screenManager, 0, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 0);

    // 3 screen disconnected
    impl::RSScreenManager::OnHotPlug(hdiOutput1, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 1);
    impl::RSScreenManager::OnHotPlug(hdiOutput2, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, 2);
    impl::RSScreenManager::OnHotPlug(hdiOutput3, false, nullptr);
    screenManager->ProcessScreenHotPlugEvents();
    ASSERT_EQ(GetVSyncSamplerImplPtr()->vsyncEnabledScreenId_, UINT64_MAX);
}
} // namespace OHOS::Rosen