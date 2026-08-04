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

#include <memory>
#include <vector>
#include <thread>
#include <chrono>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "feature/frame_stability/rs_frame_stability_manager.h"
#include "ipc_callbacks/rs_frame_stability_callback_stub.h"
#include "transaction/rs_frame_stability_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class MockRSFrameStabilityCallbackStub : public RSFrameStabilityCallbackStub {
public:
    MockRSFrameStabilityCallbackStub() = default;
    virtual ~MockRSFrameStabilityCallbackStub() noexcept = default;

    MOCK_METHOD(void, OnFrameStabilityChanged, (bool isStable), (override));
};

class RSFrameStabilityManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    static constexpr uint64_t TEST_TARGET_ID_1 = 1;
    static constexpr uint64_t TEST_TARGET_ID_2 = 2;
    static constexpr pid_t TEST_PID = 1000;
    static constexpr uint32_t TEST_STABLE_DURATION_MS = 100;
    static constexpr float TEST_CHANGE_PERCENT = 0.5f;
};

void RSFrameStabilityManagerTest::SetUpTestCase() {}

void RSFrameStabilityManagerTest::TearDownTestCase() {}

void RSFrameStabilityManagerTest::SetUp() {}

void RSFrameStabilityManagerTest::TearDown() {}

} // namespace OHOS::Rosen