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

#include "info_collection/rs_frame_stats_collection.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSFrameStatsCollectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFrameStatsCollectionTest::SetUpTestCase() {}

void RSFrameStatsCollectionTest::TearDownTestCase() {}

void RSFrameStatsCollectionTest::SetUp()
{
    RSFrameStatsCollection::GetInstance().ResetFrameStats();
}

void RSFrameStatsCollectionTest::TearDown()
{
    RSFrameStatsCollection::GetInstance().ResetFrameStats();
}

HWTEST_F(RSFrameStatsCollectionTest, Singleton001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest Singleton001 start";

    auto& instance1 = RSFrameStatsCollection::GetInstance();
    auto& instance2 = RSFrameStatsCollection::GetInstance();
    EXPECT_EQ(&instance1, &instance2);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest Singleton001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, GetLevel001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetLevel001 start";

    FrameStatsLevel level = RSFrameStatsCollection::GetLevel();
    EXPECT_TRUE(level == FrameStatsLevel::Disabled || level == FrameStatsLevel::Basic ||
        level == FrameStatsLevel::Full);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetLevel001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, IsEnabled001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IsEnabled001 start";

    FrameStatsLevel level = RSFrameStatsCollection::GetLevel();
    bool enabled = RSFrameStatsCollection::IsEnabled();
    if (level > FrameStatsLevel::Disabled) {
        EXPECT_TRUE(enabled);
    } else {
        EXPECT_FALSE(enabled);
    }

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IsEnabled001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, IsLevelAtLeast001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IsLevelAtLeast001 start";

    EXPECT_TRUE(RSFrameStatsCollection::IsLevelAtLeast(FrameStatsLevel::Disabled));

    FrameStatsLevel level = RSFrameStatsCollection::GetLevel();
    bool atLeastBasic = RSFrameStatsCollection::IsLevelAtLeast(FrameStatsLevel::Basic);
    bool atLeastFull = RSFrameStatsCollection::IsLevelAtLeast(FrameStatsLevel::Full);
    if (level >= FrameStatsLevel::Basic) {
        EXPECT_TRUE(atLeastBasic);
    } else {
        EXPECT_FALSE(atLeastBasic);
    }
    if (level >= FrameStatsLevel::Full) {
        EXPECT_TRUE(atLeastFull);
    } else {
        EXPECT_FALSE(atLeastFull);
    }

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IsLevelAtLeast001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, Increment001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest Increment001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    size_t idx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSMainThread::TotalFrames);

    instance.Increment(idx);
    uint64_t value = instance.GetCounterValue(idx);
    EXPECT_EQ(value, 1u);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest Increment001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, Increment002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest Increment002 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    size_t idx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSMainThread::DirectCompositionFrames);

    for (int i = 0; i < 10; ++i) {
        instance.Increment(idx);
    }

    uint64_t value = instance.GetCounterValue(idx);
    EXPECT_EQ(value, 10u);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest Increment002 end";
}

HWTEST_F(RSFrameStatsCollectionTest, Increment003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest Increment003 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    size_t invalidIdx = 10000;

    instance.Increment(invalidIdx);
    uint64_t value = instance.GetCounterValue(invalidIdx);
    EXPECT_EQ(value, 0u);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest Increment003 end";
}

HWTEST_F(RSFrameStatsCollectionTest, IncrementBySurfaceNode001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementBySurfaceNode001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    instance.ResetFrameStats();

    instance.IncrementBySurfaceNode("TestSurfaceNode", "DrawImageGPUCount");
    instance.IncrementBySurfaceNode("TestSurfaceNode", "DrawImageGPUCount");
    instance.IncrementBySurfaceNode("TestSurfaceNode", "TotalFrames");

    std::string dump = instance.DumpFrameStats();
    EXPECT_FALSE(dump.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementBySurfaceNode001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, IncrementBySurfaceNode002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementBySurfaceNode002 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    instance.ResetFrameStats();

    instance.IncrementBySurfaceNode("", "DrawImageGPUCount");
    instance.IncrementBySurfaceNode("TestSurfaceNode", "");

    std::string dump = instance.DumpFrameStats();
    EXPECT_FALSE(dump.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementBySurfaceNode002 end";
}

HWTEST_F(RSFrameStatsCollectionTest, IncrementBySurfaceNode003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementBySurfaceNode003 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    instance.ResetFrameStats();

    instance.IncrementBySurfaceNode("TestSurfaceNode", "CustomCounter", 5);
    instance.IncrementBySurfaceNode("TestSurfaceNode", "CustomCounter", 3);

    std::string dump = instance.DumpFrameStats();
    EXPECT_FALSE(dump.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementBySurfaceNode003 end";
}

HWTEST_F(RSFrameStatsCollectionTest, IncrementBySurfaceNode004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementBySurfaceNode004 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    instance.ResetFrameStats();

    instance.IncrementBySurfaceNode("TestSurfaceNode", "DrawImageGPUCount", 1,
        FrameStatsDetail::UniRenderThread);
    instance.IncrementBySurfaceNode("TestSurfaceNode", "UpdateCacheSurface", 2,
        FrameStatsDetail::MainThread);

    std::string dump = instance.DumpFrameStats();
    EXPECT_FALSE(dump.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementBySurfaceNode004 end";
}

HWTEST_F(RSFrameStatsCollectionTest, IncrementBySurfaceNode005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementBySurfaceNode005 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    instance.ResetFrameStats();

    instance.IncrementBySurfaceNode("ScreenNode", "DrawImageGPUCount");
    instance.IncrementBySurfaceNode("AnimationTrace", "TotalFrames");

    std::string dump = instance.DumpFrameStats();
    EXPECT_FALSE(dump.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementBySurfaceNode005 end";
}

HWTEST_F(RSFrameStatsCollectionTest, IncrementDrawableBySurfaceNode001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementDrawableBySurfaceNode001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    instance.ResetFrameStats();

    instance.IncrementDrawableBySurfaceNode("TestSurfaceNode", 0);
    instance.IncrementDrawableBySurfaceNode("TestSurfaceNode", 7);
    instance.IncrementDrawableBySurfaceNode("TestSurfaceNode", 52);

    std::string dump = instance.DumpFrameStats();
    EXPECT_FALSE(dump.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementDrawableBySurfaceNode001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, IncrementDrawableBySurfaceNode002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementDrawableBySurfaceNode002 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    instance.ResetFrameStats();

    instance.IncrementDrawableBySurfaceNode("TestSurfaceNode", -1);
    instance.IncrementDrawableBySurfaceNode("TestSurfaceNode", -1, 2);

    std::string dump = instance.DumpFrameStats();
    EXPECT_FALSE(dump.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementDrawableBySurfaceNode002 end";
}

HWTEST_F(RSFrameStatsCollectionTest, IncrementDrawableBySurfaceNode003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementDrawableBySurfaceNode003 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    instance.ResetFrameStats();

    instance.IncrementDrawableBySurfaceNode("", 0);
    instance.IncrementDrawableBySurfaceNode("", 7);

    std::string dump = instance.DumpFrameStats();
    EXPECT_FALSE(dump.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest IncrementDrawableBySurfaceNode003 end";
}

HWTEST_F(RSFrameStatsCollectionTest, GetFrameStats001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetFrameStats001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    auto entries = instance.GetFrameStats();

    EXPECT_FALSE(entries.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetFrameStats001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, GetCounterValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetCounterValue001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    size_t idx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSUniRenderThread::TotalFrames);

    instance.Increment(idx);
    instance.Increment(idx);

    uint64_t value = instance.GetCounterValue(idx);
    EXPECT_EQ(value, 2u);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetCounterValue001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, GetCounterValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetCounterValue002 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    size_t invalidIdx = 100000;

    uint64_t value = instance.GetCounterValue(invalidIdx);
    EXPECT_EQ(value, 0u);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetCounterValue002 end";
}

HWTEST_F(RSFrameStatsCollectionTest, ResetFrameStats001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest ResetFrameStats001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    size_t idx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSSubThread::TotalFrames);

    instance.Increment(idx);
    instance.Increment(idx);
    instance.ResetFrameStats();

    uint64_t value = instance.GetCounterValue(idx);
    EXPECT_EQ(value, 0u);

    std::string app = instance.GetForegroundApp();
    EXPECT_TRUE(app.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest ResetFrameStats001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, ResetFrameStatsClearsDetail001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest ResetFrameStatsClearsDetail001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();

    instance.IncrementBySurfaceNode("TestSurfaceNode", "DrawImageGPUCount");
    instance.IncrementDrawableBySurfaceNode("TestSurfaceNode", 7);
    instance.ResetFrameStats();

    std::string dump = instance.DumpFrameStats();
    EXPECT_TRUE(dump.find("TestSurfaceNode") == std::string::npos ||
        dump.find("DrawImageGPUCount") == std::string::npos);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest ResetFrameStatsClearsDetail001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, DumpFrameStats001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest DumpFrameStats001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    std::string dump = instance.DumpFrameStats();

    EXPECT_FALSE(dump.empty());
    EXPECT_TRUE(dump.find("FrameStats") != std::string::npos);
    EXPECT_TRUE(dump.find("Level:") != std::string::npos);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest DumpFrameStats001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, SetForegroundApp001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest SetForegroundApp001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    std::vector<std::string> params = { "com.test.app:1234:normal" };

    instance.SetForegroundApp(params);
    std::string app = instance.GetForegroundApp();

    EXPECT_EQ(app, "com.test.app");

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest SetForegroundApp001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, SetForegroundApp002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest SetForegroundApp002 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    std::vector<std::string> params1 = { "com.test.app1:1234:normal" };
    std::vector<std::string> params2 = { "com.test.app2:5678:normal" };

    instance.SetForegroundApp(params1);
    size_t idx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSMainThread::TotalFrames);
    instance.Increment(idx);
    instance.Increment(idx);

    instance.SetForegroundApp(params2);
    instance.Increment(idx);

    auto entries = instance.GetFrameStatsByApp("com.test.app1");
    bool found = false;
    for (const auto& entry : entries) {
        if (entry.name == "RSMainThread|TotalFrames") {
            EXPECT_EQ(entry.count, 2u);
            found = true;
        }
    }
    EXPECT_TRUE(found);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest SetForegroundApp002 end";
}

HWTEST_F(RSFrameStatsCollectionTest, GetForegroundApp001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetForegroundApp001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    instance.ResetFrameStats();

    std::string app = instance.GetForegroundApp();
    EXPECT_TRUE(app.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetForegroundApp001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, GetFrameStatsByApp001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetFrameStatsByApp001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    auto entries = instance.GetFrameStatsByApp("nonexistent.app");

    EXPECT_TRUE(entries.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest GetFrameStatsByApp001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, PerLayerRateIndex001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest PerLayerRateIndex001 start";

    size_t idx1 = FrameStatsCounter::PerLayerRateIndex(1, 60);
    size_t idx2 = FrameStatsCounter::PerLayerRateIndex(2, 60);
    size_t idx3 = FrameStatsCounter::PerLayerRateIndex(5, 90);

    EXPECT_NE(idx1, idx2);
    EXPECT_NE(idx2, idx3);
    EXPECT_NE(idx1, idx3);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest PerLayerRateIndex001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, PerLayerRateIndex002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest PerLayerRateIndex002 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    size_t idx = FrameStatsCounter::PerLayerRateIndex(3, 60);

    instance.Increment(idx);
    instance.Increment(idx);

    uint64_t value = instance.GetCounterValue(idx);
    EXPECT_EQ(value, 2u);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest PerLayerRateIndex002 end";
}

HWTEST_F(RSFrameStatsCollectionTest, RateSlot001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest RateSlot001 start";

    size_t slot60 = FrameStatsCounter::RateSlot(60);
    size_t slot90 = FrameStatsCounter::RateSlot(90);
    size_t slotInvalid = FrameStatsCounter::RateSlot(999);

    EXPECT_NE(slot60, slot90);
    EXPECT_EQ(slotInvalid, 0u);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest RateSlot001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, LayerSlot001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest LayerSlot001 start";

    size_t slot1 = FrameStatsCounter::LayerSlot(1);
    size_t slot2 = FrameStatsCounter::LayerSlot(2);
    size_t slot5 = FrameStatsCounter::LayerSlot(5);
    size_t slot10 = FrameStatsCounter::LayerSlot(10);

    EXPECT_EQ(slot1, 0u);
    EXPECT_EQ(slot2, 1u);
    EXPECT_EQ(slot5, 4u);
    EXPECT_EQ(slot10, 4u);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest LayerSlot001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, ToIndex001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest ToIndex001 start";

    size_t mainIdx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSMainThread::TotalFrames);
    size_t uniIdx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSUniRenderThread::TotalFrames);
    size_t subIdx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSSubThread::TotalFrames);
    size_t composerIdx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSRenderComposer::GpuComposeRedrawFrames);

    EXPECT_LT(mainIdx, uniIdx);
    EXPECT_LT(uniIdx, subIdx);
    EXPECT_LT(subIdx, composerIdx);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest ToIndex001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, AppSwitchMerge001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest AppSwitchMerge001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    size_t idx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSMainThread::TotalFrames);

    instance.SetForegroundApp({ "com.test.app1:1234:normal" });
    instance.Increment(idx);
    instance.Increment(idx);

    instance.SetForegroundApp({ "com.test.app2:5678:normal" });
    instance.Increment(idx);

    instance.SetForegroundApp({ "com.test.app1:1234:normal" });

    uint64_t value = instance.GetCounterValue(idx);
    EXPECT_EQ(value, 2u);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest AppSwitchMerge001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, AppSwitchMerge002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest AppSwitchMerge002 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    size_t idx = FrameStatsCounter::ToIndex(FrameStatsCounter::RSMainThread::TotalFrames);

    instance.SetForegroundApp({ "com.test.app1:1234:normal" });
    instance.Increment(idx);
    instance.Increment(idx);

    instance.SetForegroundApp({ "com.test.app2:5678:normal" });
    instance.Increment(idx);

    instance.SetForegroundApp({ "com.test.app1:1234:normal" });
    instance.Increment(idx);

    uint64_t value = instance.GetCounterValue(idx);
    EXPECT_EQ(value, 3u);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest AppSwitchMerge002 end";
}

HWTEST_F(RSFrameStatsCollectionTest, SetForegroundAppMultiParams001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest SetForegroundAppMultiParams001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    std::vector<std::string> params = {
        "com.ohos.sceneboard:100:normal",
        "com.test.app:1234:normal"
    };

    instance.SetForegroundApp(params);
    std::string app = instance.GetForegroundApp();

    EXPECT_EQ(app, "com.test.app");

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest SetForegroundAppMultiParams001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, SetForegroundAppMultiParams002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest SetForegroundAppMultiParams002 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    std::vector<std::string> params = {
        "com.ohos.sceneboard:100:normal",
        "com.ohos.sceneboard:200:normal"
    };

    instance.SetForegroundApp(params);
    std::string app = instance.GetForegroundApp();

    EXPECT_EQ(app, "com.ohos.sceneboard");

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest SetForegroundAppMultiParams002 end";
}

HWTEST_F(RSFrameStatsCollectionTest, SetForegroundAppEmptyParams001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest SetForegroundAppEmptyParams001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    std::vector<std::string> params;

    instance.SetForegroundApp(params);
    std::string app = instance.GetForegroundApp();

    EXPECT_TRUE(app.empty());

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest SetForegroundAppEmptyParams001 end";
}

HWTEST_F(RSFrameStatsCollectionTest, AppSwitchWithDetailStats001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest AppSwitchWithDetailStats001 start";

    auto& instance = RSFrameStatsCollection::GetInstance();
    instance.ResetFrameStats();

    instance.SetForegroundApp({ "com.test.app1:1234:normal" });
    instance.IncrementBySurfaceNode("NodeA", "DrawImageGPUCount");
    instance.IncrementBySurfaceNode("NodeA", "DrawImageGPUCount");
    instance.IncrementBySurfaceNode("NodeB", "TotalFrames");

    instance.SetForegroundApp({ "com.test.app2:5678:normal" });
    instance.IncrementBySurfaceNode("NodeC", "DrawImageGPUCount");

    std::string dump = instance.DumpFrameStats();
    EXPECT_FALSE(dump.empty());
    EXPECT_TRUE(dump.find("Per-App History") != std::string::npos ||
        dump.find("com.test.app1") != std::string::npos);

    GTEST_LOG_(INFO) << "RSFrameStatsCollectionTest AppSwitchWithDetailStats001 end";
}

} // namespace Rosen
} // namespace OHOS
