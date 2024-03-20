/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "test_header.h"
#include "pipeline/rs_realtime_refresh_rate_manager.h"
#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRealtimeRefreshRateManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: EnableStatus001
 * @tc.desc: test RSRealtimeRefreshRateManagerTest.EnableStatus001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, EnableStatus001, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(true);
    bool ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, true);

    instance.SetShowRefreshRateEnabled(true);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, true);

    instance.SetShowRefreshRateEnabled(false);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);

    instance.SetShowRefreshRateEnabled(false);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: RSInterface001
 * @tc.desc: test RSRealtimeRefreshRateManagerTest.RSInterface001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, RSInterface001, TestSize.Level1)
{
    auto& instance = RSInterfaces::GetInstance();

    instance.SetShowRefreshRateEnabled(true);
    bool ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, true);

    instance.SetShowRefreshRateEnabled(true);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, true);

    instance.SetShowRefreshRateEnabled(false);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);

    instance.SetShowRefreshRateEnabled(false);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);
}
}