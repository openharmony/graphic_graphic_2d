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

    instance.SetShowRefreshRateEnabled(true, 1);
    bool ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, true);
    ASSERT_EQ(instance.collectEnabled_, false);
    ASSERT_EQ(instance.isCollectRefreshRateTaskRunning_, true);

    instance.SetShowRefreshRateEnabled(true, 0);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, true);
    ASSERT_EQ(instance.collectEnabled_, true);
    ASSERT_EQ(instance.isCollectRefreshRateTaskRunning_, true);

    instance.SetShowRefreshRateEnabled(true, 1);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, true);
    ASSERT_EQ(instance.collectEnabled_, true);
    ASSERT_EQ(instance.isCollectRefreshRateTaskRunning_, true);

    instance.SetShowRefreshRateEnabled(true, 0);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, true);
    ASSERT_EQ(instance.collectEnabled_, true);
    ASSERT_EQ(instance.isCollectRefreshRateTaskRunning_, true);

    instance.SetShowRefreshRateEnabled(false, 1);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);
    ASSERT_EQ(instance.collectEnabled_, true);
    ASSERT_EQ(instance.isCollectRefreshRateTaskRunning_, true);

    instance.SetShowRefreshRateEnabled(false, 0);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);
    ASSERT_EQ(instance.collectEnabled_, false);
    ASSERT_EQ(instance.isCollectRefreshRateTaskRunning_, false);

    instance.SetShowRefreshRateEnabled(false, 1);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);
    ASSERT_EQ(instance.collectEnabled_, false);
    ASSERT_EQ(instance.isCollectRefreshRateTaskRunning_, false);

    instance.SetShowRefreshRateEnabled(false, 0);
    ret = instance.GetShowRefreshRateEnabled();
    ASSERT_EQ(ret, false);
    ASSERT_EQ(instance.collectEnabled_, false);
    ASSERT_EQ(instance.isCollectRefreshRateTaskRunning_, false);
    instance.isCollectRefreshRateTaskRunning_ = false;
    instance.SetShowRefreshRateEnabled(true, 0);
    instance.showEnabled_ = true;
    instance.GetRealtimeRefreshRate(INVALID_SCREEN_ID);
}

/**
 * @tc.name: EnableStatus002
 * @tc.desc: test RSRealtimeRefreshRateManagerTest.EnableStatus002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, EnableStatus002, TestSize.Level1)
{
    constexpr int32_t waitTaskFinishNs = 100000;
    auto& instance = RSRealtimeRefreshRateManager::Instance();
    uint32_t threadNums = 100;
    std::vector<std::thread> thds;
    for (int i = 0; i < threadNums; i++) {
        thds.emplace_back(std::thread([&] () { instance.SetShowRefreshRateEnabled(true, 1); }));
        thds.emplace_back(std::thread([&] () { instance.SetShowRefreshRateEnabled(true, 0); }));
        thds.emplace_back(std::thread([&] () { instance.SetShowRefreshRateEnabled(false, 1); }));
        thds.emplace_back(std::thread([&] () { instance.SetShowRefreshRateEnabled(false, 0); }));
    }
    for (auto &thd : thds) {
        if (thd.joinable()) {
            thd.join();
        }
    }
    usleep(waitTaskFinishNs);
    instance.SetShowRefreshRateEnabled(false, 1);
    instance.SetShowRefreshRateEnabled(false, 0);
    ASSERT_EQ(instance.showEnabled_, false);
    ASSERT_EQ(instance.collectEnabled_, false);
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