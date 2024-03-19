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

#include "gtest/gtest.h"

#include "ui/rs_frame_rate_policy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSFrameRatePolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFrameRatePolicyTest::SetUpTestCase() {}
void RSFrameRatePolicyTest::TearDownTestCase() {}
void RSFrameRatePolicyTest::SetUp() {}
void RSFrameRatePolicyTest::TearDown() {}

/**
 * @tc.name: interface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSFrameRatePolicyTest, interface, TestSize.Level1)
{
    auto instance = RSFrameRatePolicy::GetInstance();

    instance->RegisterHgmConfigChangeCallback();

    std::string scene = "0";
    float speed = 1.0;
    instance->GetPreferredFps(scene, speed);
}

/**
 * @tc.name: interface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSFrameRatePolicyTest, GetRefreshRateMode_Test, TestSize.Level1)
{
    auto instance = RSFrameRatePolicy::GetInstance();

    instance->RegisterHgmConfigChangeCallback();

    instance->GetRefreshRateModeName();
}

} // namespace Rosen
} // namespace OHOS