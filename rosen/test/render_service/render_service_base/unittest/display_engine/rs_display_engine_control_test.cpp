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

#include <gtest/gtest.h>

#include "display_engine/rs_display_engine_control.h"
#include "display_engine/ipc_callbacks/rs_de_status_change_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplayEngineControlTest : public testing::Test {};

/**
 * @tc.name: GetInstance
 * @tc.desc: Verify GetInstance returns same singleton instance
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlTest, GetInstance, TestSize.Level1)
{
    auto& instance1 = RSDisplayEngineControl::GetInstance();
    auto& instance2 = RSDisplayEngineControl::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: RegisterDEStatusChangeCallbackWithNullptr
 * @tc.desc: Verify RegisterDEStatusChangeCallback returns -1 when callback is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlTest, RegisterDEStatusChangeCallbackWithNullptr, TestSize.Level1)
{
    auto& control = RSDisplayEngineControl::GetInstance();
    sptr<RSIDEStatusChangeCallback> callback = nullptr;
    EXPECT_EQ(control.RegisterDEStatusChangeCallback(callback), -1);
}

/**
 * @tc.name: RegisterDEStatusChangeCallbackWithValidCallback
 * @tc.desc: Verify RegisterDEStatusChangeCallback returns 0 when callback is valid
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlTest, RegisterDEStatusChangeCallbackWithValidCallback, TestSize.Level1)
{
    auto& control = RSDisplayEngineControl::GetInstance();
    sptr<RSIDEStatusChangeCallback> callback = new RSDEStatusChangeCallbackStub();
    EXPECT_EQ(control.RegisterDEStatusChangeCallback(callback), 0);
}

/**
 * @tc.name: UnregisterDEStatusChangeCallback
 * @tc.desc: Verify UnregisterDEStatusChangeCallback returns 0 when no callback is registered
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlTest, UnregisterDEStatusChangeCallback, TestSize.Level1)
{
    auto& control = RSDisplayEngineControl::GetInstance();
    EXPECT_EQ(control.UnregisterDEStatusChangeCallback(), 0);
}

/**
 * @tc.name: NotifyDEStatusChange
 * @tc.desc: Verify NotifyDEStatusChange forwards to RSLuminanceControl
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlTest, NotifyDEStatusChange, TestSize.Level1)
{
    auto& control = RSDisplayEngineControl::GetInstance();
    uint32_t sceneKey = 123;
    std::vector<uint8_t> values = {1, 2, 3};
    EXPECT_EQ(control.NotifyDEStatusChange(sceneKey, values), -1);
}

/**
 * @tc.name: NotifyDEStatusChangeDoneWithoutCallback
 * @tc.desc: Verify NotifyDEStatusChangeDone returns -1 when no callback is registered
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlTest, NotifyDEStatusChangeDoneWithoutCallback, TestSize.Level1)
{
    auto& control = RSDisplayEngineControl::GetInstance();
    uint32_t sceneKey = 123;
    std::vector<uint8_t> result = {1, 2, 3};
    EXPECT_EQ(control.NotifyDEStatusChangeDone(sceneKey, result), -1);
}

/**
 * @tc.name: NotifyDEStatusChangeDoneWithCallback
 * @tc.desc: Verify NotifyDEStatusChangeDone returns 0 when callback is registered
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlTest, NotifyDEStatusChangeDoneWithCallback, TestSize.Level1)
{
    auto& control = RSDisplayEngineControl::GetInstance();
    sptr<RSIDEStatusChangeCallback> callback = new RSDEStatusChangeCallbackStub();
    control.RegisterDEStatusChangeCallback(callback);
 
    uint32_t sceneKey = 123;
    std::vector<uint8_t> result = {1, 2, 3};
    EXPECT_EQ(control.NotifyDEStatusChangeDone(sceneKey, result), 0);
}
} // namespace OHOS::Rosen