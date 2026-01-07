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
    instance.GetRealtimeRefreshRateByScreenId(INVALID_SCREEN_ID);
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
        thds.emplace_back(std::thread([&instance]() { instance.SetShowRefreshRateEnabled(true, 1); }));
        thds.emplace_back(std::thread([&instance]() { instance.SetShowRefreshRateEnabled(true, 0); }));
        thds.emplace_back(std::thread([&instance]() { instance.SetShowRefreshRateEnabled(false, 1); }));
        thds.emplace_back(std::thread([&instance]() { instance.SetShowRefreshRateEnabled(false, 0); }));
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
 * @tc.name: EnableStatus003
 * @tc.desc: test SetShowRefreshRateEnabled where type is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, EnableStatus003, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(false, 1);
    instance.SetShowRefreshRateEnabled(false, 0);
    ASSERT_FALSE(instance.GetShowRefreshRateEnabled());
    ASSERT_FALSE(instance.collectEnabled_);
    ASSERT_FALSE(instance.isCollectRefreshRateTaskRunning_);

    instance.SetShowRefreshRateEnabled(true, -1);
    ASSERT_FALSE(instance.GetShowRefreshRateEnabled());
    ASSERT_FALSE(instance.collectEnabled_);
    ASSERT_FALSE(instance.isCollectRefreshRateTaskRunning_);

    instance.SetShowRefreshRateEnabled(true, -2);
    ASSERT_FALSE(instance.GetShowRefreshRateEnabled());
    ASSERT_FALSE(instance.collectEnabled_);
    ASSERT_FALSE(instance.isCollectRefreshRateTaskRunning_);

    instance.SetShowRefreshRateEnabled(true, 2);
    ASSERT_FALSE(instance.GetShowRefreshRateEnabled());
    ASSERT_FALSE(instance.collectEnabled_);
    ASSERT_FALSE(instance.isCollectRefreshRateTaskRunning_);

    instance.SetShowRefreshRateEnabled(true, 3);
    ASSERT_FALSE(instance.GetShowRefreshRateEnabled());
    ASSERT_FALSE(instance.collectEnabled_);
    ASSERT_FALSE(instance.isCollectRefreshRateTaskRunning_);
}

/**
 * @tc.name: CountRealtimeFrame001
 * @tc.desc: test CountRealtimeFrame where showEnabled_ and collectEnabled_ are false and false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, CountRealtimeFrame001, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(false, 1);
    instance.SetShowRefreshRateEnabled(false, 0);
    ASSERT_FALSE(instance.GetShowRefreshRateEnabled());
    ASSERT_FALSE(instance.collectEnabled_);

    instance.realtimeFrameCountMap_.clear();
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());

    instance.CountRealtimeFrame(0);
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());
}

/**
 * @tc.name: CountRealtimeFrame002
 * @tc.desc: test CountRealtimeFrame where showEnabled_ and collectEnabled_ are true and false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, CountRealtimeFrame002, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(true, 1);
    instance.SetShowRefreshRateEnabled(false, 0);
    ASSERT_TRUE(instance.GetShowRefreshRateEnabled());
    ASSERT_FALSE(instance.collectEnabled_);

    instance.realtimeFrameCountMap_.clear();
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());

    instance.CountRealtimeFrame(0);
    auto iter = instance.realtimeFrameCountMap_.find(0);
    ASSERT_NE(iter, instance.realtimeFrameCountMap_.end());
    ASSERT_EQ(iter->second, 1);

    instance.CountRealtimeFrame(0);
    ASSERT_EQ(iter->second, 2);
}

/**
 * @tc.name: CountRealtimeFrame003
 * @tc.desc: test CountRealtimeFrame where showEnabled_ and collectEnabled_ are false and true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, CountRealtimeFrame003, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(false, 1);
    instance.SetShowRefreshRateEnabled(true, 0);
    ASSERT_FALSE(instance.GetShowRefreshRateEnabled());
    ASSERT_TRUE(instance.collectEnabled_);

    instance.realtimeFrameCountMap_.clear();
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());

    instance.CountRealtimeFrame(0);
    auto iter = instance.realtimeFrameCountMap_.find(0);
    ASSERT_NE(iter, instance.realtimeFrameCountMap_.end());
    ASSERT_EQ(iter->second, 1);

    instance.CountRealtimeFrame(0);
    ASSERT_EQ(iter->second, 2);
}

/**
 * @tc.name: CountRealtimeFrame004
 * @tc.desc: test CountRealtimeFrame where showEnabled_ and collectEnabled_ are true and true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, CountRealtimeFrame004, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(true, 1);
    instance.SetShowRefreshRateEnabled(true, 0);
    ASSERT_TRUE(instance.GetShowRefreshRateEnabled());
    ASSERT_TRUE(instance.collectEnabled_);

    instance.realtimeFrameCountMap_.clear();
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());

    instance.CountRealtimeFrame(0);
    auto iter = instance.realtimeFrameCountMap_.find(0);
    ASSERT_NE(iter, instance.realtimeFrameCountMap_.end());
    ASSERT_EQ(iter->second, 1);

    instance.CountRealtimeFrame(0);
    ASSERT_EQ(iter->second, 2);
}

/**
 * @tc.name: UpdateScreenRefreshRate001
 * @tc.desc: test UpdateScreenRefreshRate where showEnabled_ is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, UpdateScreenRefreshRate001, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(false, 1);
    ASSERT_FALSE(instance.GetShowRefreshRateEnabled());

    instance.screenRefreshRateMap_.clear();
    ASSERT_TRUE(instance.screenRefreshRateMap_.empty());

    instance.UpdateScreenRefreshRate(0, 90);
    auto iter = instance.screenRefreshRateMap_.find(0);
    ASSERT_NE(iter, instance.screenRefreshRateMap_.end());
    ASSERT_EQ(iter->second, 90);

    instance.UpdateScreenRefreshRate(0, 90);
    ASSERT_EQ(iter->second, 90);

    instance.UpdateScreenRefreshRate(0, 120);
    ASSERT_EQ(iter->second, 120);
}

/**
 * @tc.name: UpdateScreenRefreshRate002
 * @tc.desc: test UpdateScreenRefreshRate where showEnabled_ is true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, UpdateScreenRefreshRate002, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(true, 1);
    ASSERT_TRUE(instance.GetShowRefreshRateEnabled());

    instance.screenRefreshRateMap_.clear();
    ASSERT_TRUE(instance.screenRefreshRateMap_.empty());

    instance.UpdateScreenRefreshRate(0, 90);
    auto iter = instance.screenRefreshRateMap_.find(0);
    ASSERT_NE(iter, instance.screenRefreshRateMap_.end());
    ASSERT_EQ(iter->second, 90);

    instance.UpdateScreenRefreshRate(0, 90);
    ASSERT_EQ(iter->second, 90);

    instance.UpdateScreenRefreshRate(0, 120);
    ASSERT_EQ(iter->second, 120);
}

/**
 * @tc.name: GetRefreshRate001
 * @tc.desc: test GetRefreshRate where showEnabled_ and collectEnabled_ are false and false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, GetRefreshRate001, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(false, 1);
    instance.SetShowRefreshRateEnabled(false, 0);
    ASSERT_FALSE(instance.GetShowRefreshRateEnabled());
    ASSERT_FALSE(instance.collectEnabled_);

    auto [currentRefreshRate, realtimeRefreshRate] = instance.GetRefreshRateByScreenId(0);
    ASSERT_EQ(currentRefreshRate, 0);
    ASSERT_EQ(realtimeRefreshRate, 0);
    uint32_t realtimeRefreshRateByScreenId = instance.GetRealtimeRefreshRateByScreenId(0);
    ASSERT_EQ(realtimeRefreshRateByScreenId, 0);
}

/**
 * @tc.name: GetRefreshRate002
 * @tc.desc: test GetRefreshRate where showEnabled_ and collectEnabled_ are true and false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, GetRefreshRate002, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(true, 1);
    instance.SetShowRefreshRateEnabled(false, 0);
    ASSERT_TRUE(instance.GetShowRefreshRateEnabled());
    ASSERT_FALSE(instance.collectEnabled_);

    instance.screenRefreshRateMap_.clear();
    instance.realtimeFrameCountMap_.clear();
    ASSERT_TRUE(instance.screenRefreshRateMap_.empty());
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());

    auto [currentRefreshRate1, realtimeRefreshRate1] = instance.GetRefreshRateByScreenId(0);
    ASSERT_EQ(currentRefreshRate1, instance.DEFAULT_SCREEN_REFRESH_RATE);
    ASSERT_EQ(realtimeRefreshRate1, instance.DEFAULT_REALTIME_REFRESH_RATE);
    uint32_t realtimeRefreshRateByScreenId1 = instance.GetRealtimeRefreshRateByScreenId(0);
    ASSERT_EQ(realtimeRefreshRateByScreenId1, instance.DEFAULT_REALTIME_REFRESH_RATE);

    instance.screenRefreshRateMap_.emplace(0, 120);
    instance.realtimeFrameCountMap_.emplace(0, 90);
    auto [currentRefreshRate2, realtimeRefreshRate2] = instance.GetRefreshRateByScreenId(0);
    ASSERT_EQ(currentRefreshRate2, 120);
    ASSERT_EQ(realtimeRefreshRate2, 90);
    uint32_t realtimeRefreshRateByScreenId2 = instance.GetRealtimeRefreshRateByScreenId(0);
    ASSERT_EQ(realtimeRefreshRateByScreenId2, 90);

    instance.screenRefreshRateMap_.clear();
    instance.realtimeFrameCountMap_.clear();
    ASSERT_TRUE(instance.screenRefreshRateMap_.empty());
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());

    instance.screenRefreshRateMap_.emplace(0, 90);
    instance.realtimeFrameCountMap_.emplace(0, 120);
    auto [currentRefreshRate3, realtimeRefreshRate3] = instance.GetRefreshRateByScreenId(0);
    ASSERT_EQ(currentRefreshRate3, 90);
    ASSERT_EQ(realtimeRefreshRate3, 90);
    uint32_t realtimeRefreshRateByScreenId3 = instance.GetRealtimeRefreshRateByScreenId(0);
    ASSERT_EQ(realtimeRefreshRateByScreenId3, 90);
}

/**
 * @tc.name: GetRefreshRate003
 * @tc.desc: test GetRefreshRate where showEnabled_ and collectEnabled_ are false and true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, GetRefreshRate003, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(false, 1);
    instance.SetShowRefreshRateEnabled(true, 0);
    ASSERT_FALSE(instance.GetShowRefreshRateEnabled());
    ASSERT_TRUE(instance.collectEnabled_);

    instance.screenRefreshRateMap_.clear();
    instance.realtimeFrameCountMap_.clear();
    ASSERT_TRUE(instance.screenRefreshRateMap_.empty());
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());

    auto [currentRefreshRate1, realtimeRefreshRate1] = instance.GetRefreshRateByScreenId(0);
    ASSERT_EQ(currentRefreshRate1, instance.DEFAULT_SCREEN_REFRESH_RATE);
    ASSERT_EQ(realtimeRefreshRate1, instance.DEFAULT_REALTIME_REFRESH_RATE);
    uint32_t realtimeRefreshRateByScreenId1 = instance.GetRealtimeRefreshRateByScreenId(0);
    ASSERT_EQ(realtimeRefreshRateByScreenId1, instance.DEFAULT_REALTIME_REFRESH_RATE);

    instance.screenRefreshRateMap_.emplace(0, 120);
    instance.realtimeFrameCountMap_.emplace(0, 90);
    auto [currentRefreshRate2, realtimeRefreshRate2] = instance.GetRefreshRateByScreenId(0);
    ASSERT_EQ(currentRefreshRate2, 120);
    ASSERT_EQ(realtimeRefreshRate2, 90);
    uint32_t realtimeRefreshRateByScreenId2 = instance.GetRealtimeRefreshRateByScreenId(0);
    ASSERT_EQ(realtimeRefreshRateByScreenId2, 90);

    instance.screenRefreshRateMap_.clear();
    instance.realtimeFrameCountMap_.clear();
    ASSERT_TRUE(instance.screenRefreshRateMap_.empty());
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());

    instance.screenRefreshRateMap_.emplace(0, 90);
    instance.realtimeFrameCountMap_.emplace(0, 120);
    auto [currentRefreshRate3, realtimeRefreshRate3] = instance.GetRefreshRateByScreenId(0);
    ASSERT_EQ(currentRefreshRate3, 90);
    ASSERT_EQ(realtimeRefreshRate3, 90);
    uint32_t realtimeRefreshRateByScreenId3 = instance.GetRealtimeRefreshRateByScreenId(0);
    ASSERT_EQ(realtimeRefreshRateByScreenId3, 90);
}

/**
 * @tc.name: GetRefreshRate004
 * @tc.desc: test GetRefreshRate where showEnabled_ and collectEnabled_ are true and true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRealtimeRefreshRateManagerTest, GetRefreshRate004, TestSize.Level1)
{
    auto& instance = RSRealtimeRefreshRateManager::Instance();

    instance.SetShowRefreshRateEnabled(false, 1);
    instance.SetShowRefreshRateEnabled(true, 0);
    ASSERT_TRUE(instance.GetShowRefreshRateEnabled());
    ASSERT_TRUE(instance.collectEnabled_);

    instance.screenRefreshRateMap_.clear();
    instance.realtimeFrameCountMap_.clear();
    ASSERT_TRUE(instance.screenRefreshRateMap_.empty());
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());

    auto [currentRefreshRate1, realtimeRefreshRate1] = instance.GetRefreshRateByScreenId(0);
    ASSERT_EQ(currentRefreshRate1, instance.DEFAULT_SCREEN_REFRESH_RATE);
    ASSERT_EQ(realtimeRefreshRate1, instance.DEFAULT_REALTIME_REFRESH_RATE);
    uint32_t realtimeRefreshRateByScreenId1 = instance.GetRealtimeRefreshRateByScreenId(0);
    ASSERT_EQ(realtimeRefreshRateByScreenId1, instance.DEFAULT_REALTIME_REFRESH_RATE);

    instance.screenRefreshRateMap_.emplace(0, 120);
    instance.realtimeFrameCountMap_.emplace(0, 90);
    auto [currentRefreshRate2, realtimeRefreshRate2] = instance.GetRefreshRateByScreenId(0);
    ASSERT_EQ(currentRefreshRate2, 120);
    ASSERT_EQ(realtimeRefreshRate2, 90);
    uint32_t realtimeRefreshRateByScreenId2 = instance.GetRealtimeRefreshRateByScreenId(0);
    ASSERT_EQ(realtimeRefreshRateByScreenId2, 90);

    instance.screenRefreshRateMap_.clear();
    instance.realtimeFrameCountMap_.clear();
    ASSERT_TRUE(instance.screenRefreshRateMap_.empty());
    ASSERT_TRUE(instance.realtimeFrameCountMap_.empty());

    instance.screenRefreshRateMap_.emplace(0, 90);
    instance.realtimeFrameCountMap_.emplace(0, 120);
    auto [currentRefreshRate3, realtimeRefreshRate3] = instance.GetRefreshRateByScreenId(0);
    ASSERT_EQ(currentRefreshRate3, 90);
    ASSERT_EQ(realtimeRefreshRate3, 90);
    uint32_t realtimeRefreshRateByScreenId3 = instance.GetRealtimeRefreshRateByScreenId(0);
    ASSERT_EQ(realtimeRefreshRateByScreenId3, 90);
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