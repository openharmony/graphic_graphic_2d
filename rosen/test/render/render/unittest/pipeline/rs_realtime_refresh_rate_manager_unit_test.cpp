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
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsRealtimeRefreshRateManager : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: EnableStatus01
 * @tc.desc: test RsRealtimeRefreshRateManager.EnableStatus01
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsRealtimeRefreshRateManager, EnableStatus01, TestSize.Level1)
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
 * @tc.name: EnableStatus02
 * @tc.desc: test RsRealtimeRefreshRateManager.EnableStatus02
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsRealtimeRefreshRateManager, EnableStatus02, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();
    uint32_t threadNums = 100;
    std::vector<std::thread> thds;
    for (int i = 0; i < threadNums; i++) {
        thds.emplace_back(std::thread([&] () { instance.SetShowRefreshRateEnabled(true); }));
        thds.emplace_back(std::thread([&] () { instance.SetShowRefreshRateEnabled(false); }));
    }
    for (auto &thd : thds) {
        if (thd.joinable()) {
            thd.join();
        }
    }
}

/**
 * @tc.name: RSInterface01
 * @tc.desc: test RsRealtimeRefreshRateManager.RSInterface01
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsRealtimeRefreshRateManager, RSInterface01, TestSize.Level1)
{
    auto& instance = RSInterfaces::GetInstance();

    instance.SetShowRefreshRateEnabled(true);
    bool ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);

    instance.SetShowRefreshRateEnabled(true);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);

    instance.SetShowRefreshRateEnabled(false);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);

    instance.SetShowRefreshRateEnabled(false);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);
}
}