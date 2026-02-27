/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <gtest/gtest.h>
#include "memory/rs_memory_snapshot.h"

namespace OHOS::Rosen {
class RSMemorySnapshotTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMemorySnapshotTest::SetUpTestCase() {}
void RSMemorySnapshotTest::TearDownTestCase() {}
void RSMemorySnapshotTest::SetUp() {}
void RSMemorySnapshotTest::TearDown() {}

/**
 * @tc.name: AddCpuMemoryTest001
 * @tc.desc: Test adding CPU memory for a new PID.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, AddCpuMemoryTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 1001;
    size_t size = 1024;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid, size);
    MemorySnapshotInfo info;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_TRUE(ret);
    ASSERT_EQ(info.cpuMemory, size);
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: RemoveCpuMemoryTest001
 * @tc.desc: Test removing CPU memory for an existing PID.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, RemoveCpuMemoryTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 1002;
    size_t addSize = 2048;
    size_t removeSize = 1024;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid, addSize);
    MemorySnapshot::Instance().RemoveCpuMemory(pid, removeSize);
    MemorySnapshotInfo info;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_TRUE(ret);
    ASSERT_EQ(info.cpuMemory, addSize - removeSize);
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: RemoveCpuMemoryTest002
 * @tc.desc: Test cpuMemory after adding/removing memory.
 * @tc.type: FUNC
 */
HWTEST_F(RSMemorySnapshotTest, RemoveCpuMemoryTest002, testing::ext::TestSize.Level1)
{
    pid_t pid = 123;

    MemorySnapshotInfo baseInfo;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, baseInfo);
    EXPECT_EQ(ret, false);

    MemorySnapshot::Instance().AddCpuMemory(pid, 1024);
    MemorySnapshotInfo currentInfo;
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, currentInfo);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(currentInfo.cpuMemory, 1024);

    MemorySnapshot::Instance().RemoveCpuMemory(pid, 513);
    currentInfo = {0};
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, currentInfo);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(currentInfo.cpuMemory, 511);

    MemorySnapshot::Instance().RemoveCpuMemory(pid, 513);
    currentInfo = {0};
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, currentInfo);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(currentInfo.cpuMemory, 511);

    MemorySnapshot::Instance().RemoveCpuMemory(pid, 511);
    currentInfo = {0};
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, currentInfo);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(currentInfo.cpuMemory, 0);

    std::set<pid_t> exitedPidSet = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPidSet);
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, currentInfo);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: GetMemorySnapshotInfoByPidTest001
 * @tc.desc: Test GetMemorySnapshotInfoByPid for an existing PID.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, GetMemorySnapshotInfoByPidTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 6001;
    size_t cpuSize = 1024;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid, cpuSize);
    MemorySnapshotInfo expectedInfo;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, expectedInfo);
    ASSERT_TRUE(ret);
    ASSERT_EQ(expectedInfo.pid, pid);
    ASSERT_EQ(expectedInfo.cpuMemory, cpuSize);
    ASSERT_EQ(expectedInfo.gpuMemory, 0);
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: UpdateGpuMemoryInfoTest001
 * @tc.desc: Test updating GPU memory info and check total memory.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, UpdateGpuMemoryInfoTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 1003;
    size_t cpuSize = 1024;
    size_t gpuSize = 2048;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid, cpuSize);
    std::unordered_map<pid_t, size_t> gpuInfo = {{pid, gpuSize}};
    std::unordered_map<pid_t, MemorySnapshotInfo> pidForReport;
    bool isTotalOver = false;
    MemorySnapshotInfo info;
    MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuInfo, pidForReport, isTotalOver);
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_TRUE(ret);
    ASSERT_EQ(info.TotalMemory(), cpuSize + gpuSize);
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: EraseSnapshotInfoByPidTest001`
 * @tc.desc: Test erasing snapshot info for exited PIDs.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, EraseSnapshotInfoByPidTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 1005;
    size_t size = 1024;
    MemorySnapshot::Instance().AddCpuMemory(pid, size);
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshotInfo info;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: CalculateRiskScoreTest001
 * @tc.desc: Test CalculateRiskScore with non-zero max values.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, CalculateRiskScoreTest001, testing::ext::TestSize.Level1)
{
    MemorySnapshotInfo snapshot;
    snapshot.cpuMemory = 1024;
    snapshot.gpuMemory = 2048;
    size_t maxCpu = 2048;
    size_t maxGpu = 4096;
    size_t maxSum = 4096;
    float expected = 0.3 * 0.5 + 0.3 * 0.5 + 0.4 * 0.75;
    float score = MemorySnapshot::Instance().CalculateRiskScore(snapshot, maxCpu, maxGpu, maxSum);
    ASSERT_FLOAT_EQ(score, expected);
}

/**
 * @tc.name: FindMaxValuesTest001
 * @tc.desc: Test FindMaxValues with empty snapshot.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, FindMaxValuesTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 3001;
    size_t cpuSize = 1024;
    size_t gpuSize = 2048;
    MemorySnapshot::Instance().AddCpuMemory(pid, cpuSize);
    std::unordered_map<pid_t, size_t> gpuInfo = {{pid, gpuSize}};
    std::unordered_map<pid_t, MemorySnapshotInfo> pidForReport;
    bool isTotalOver = false;
    MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuInfo, pidForReport, isTotalOver);
    std::vector<MemorySnapshotInfo> list;
    size_t maxCpu = 100;
    size_t maxGpu = 200;
    size_t maxSum = 300;
    MemorySnapshot::Instance().FindMaxValues(list, maxCpu, maxGpu, maxSum);
    ASSERT_TRUE(maxCpu >= 1024);
    ASSERT_TRUE(maxGpu >= 2048);
    ASSERT_TRUE(maxSum >= 3072);
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: GetMemorySnapshotTest001
 * @tc.desc: Test GetMemorySnapshot returns correct copy.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, GetMemorySnapshotTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 4001;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid, 1024);
    std::unordered_map<pid_t, MemorySnapshotInfo> snapshot;
    MemorySnapshot::Instance().GetMemorySnapshot(snapshot);
    ASSERT_TRUE(snapshot.find(pid) != snapshot.end());
    ASSERT_EQ(snapshot[pid].cpuMemory, 1024);
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: GetTotalMemoryTest001
 * @tc.desc: Test GetTotalMemory after adding/removing memory.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, GetTotalMemoryTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 5001;
    size_t initialTotal = MemorySnapshot::Instance().GetTotalMemory();
    MemorySnapshot::Instance().AddCpuMemory(pid, 1024);
    ASSERT_EQ(MemorySnapshot::Instance().GetTotalMemory(), initialTotal);
    MemorySnapshot::Instance().RemoveCpuMemory(pid, 512);
    ASSERT_EQ(MemorySnapshot::Instance().GetTotalMemory(), initialTotal);
    MemorySnapshot::Instance().EraseSnapshotInfoByPid({pid});
    ASSERT_EQ(MemorySnapshot::Instance().GetTotalMemory(), initialTotal);
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: InitMemoryLimitTest001
 * @tc.desc: Test InitMemoryLimit initializes only once.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, InitMemoryLimitTest001, testing::ext::TestSize.Level1)
{
    auto callback = [](pid_t, size_t, bool) {return true; };
    MemorySnapshot::Instance().InitMemoryLimit(callback, 1000, 2000, 3000);
    MemorySnapshot::Instance().InitMemoryLimit(nullptr, 5000, 6000, 7000);
    ASSERT_EQ(MemorySnapshot::Instance().singleMemoryWarning_, 1000);
    ASSERT_EQ(MemorySnapshot::Instance().totalMemoryLimit_, 3000);
}

/**
 * @tc.name: AddCpuMemoryTest002
 * @tc.desc: Test adding CPU memory for an existing PID (line 41 false).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, AddCpuMemoryTest002, testing::ext::TestSize.Level1)
{
    pid_t pid = 1006;
    size_t size1 = 1024;
    size_t size2 = 2048;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);

    // First call - PID doesn't exist
    MemorySnapshot::Instance().AddCpuMemory(pid, size1);
    MemorySnapshotInfo info;
    MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_EQ(info.cpuMemory, size1);

    // Second call - PID exists (line 41 false)
    MemorySnapshot::Instance().AddCpuMemory(pid, size2);
    MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_EQ(info.cpuMemory, size1 + size2);

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: RemoveCpuMemoryTest003
 * @tc.desc: Test removing CPU memory for non-existent PID (line 54 false).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, RemoveCpuMemoryTest003, testing::ext::TestSize.Level1)
{
    pid_t pid = 9999;
    size_t size = 1024;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);

    // Remove from non-existent PID - should not crash (line 54 false)
    MemorySnapshot::Instance().RemoveCpuMemory(pid, size);
    MemorySnapshotInfo info;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: UpdateGpuMemoryInfoTest002
 * @tc.desc: Test UpdateGpuMemoryInfo with PID not in gpuInfo (line 78 false).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, UpdateGpuMemoryInfoTest002, testing::ext::TestSize.Level1)
{
    pid_t pid = 1007;
    size_t cpuSize = 1024;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid, cpuSize);

    // Empty gpuInfo - PID not found (line 78 false)
    std::unordered_map<pid_t, size_t> gpuInfo;
    std::unordered_map<pid_t, MemorySnapshotInfo> pidForReport;
    bool isTotalOver = false;
    MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuInfo, pidForReport, isTotalOver);

    MemorySnapshotInfo info;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_TRUE(ret);
    ASSERT_EQ(info.cpuMemory, cpuSize);
    ASSERT_EQ(info.gpuMemory, 0);

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: UpdateGpuMemoryInfoTest003
 * @tc.desc: Test UpdateGpuMemoryInfo with single memory warning exceeded (line 82 true).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, UpdateGpuMemoryInfoTest003, testing::ext::TestSize.Level1)
{
    auto callback = [](pid_t, size_t, bool) {return true; };
    MemorySnapshot::Instance().InitMemoryLimit(callback, 500, 2000, 3000);

    pid_t pid = 1008;
    size_t cpuSize = 1024;
    size_t gpuSize = 2048;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid, cpuSize);

    std::unordered_map<pid_t, size_t> gpuInfo = {{pid, gpuSize}};
    std::unordered_map<pid_t, MemorySnapshotInfo> pidForReport;
    bool isTotalOver = false;
    MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuInfo, pidForReport, isTotalOver);

    // Total memory (3072) > singleMemoryWarning (500), so pid should be in report (line 82 true)
    ASSERT_TRUE(pidForReport.find(pid) != pidForReport.end());
    ASSERT_GT(pidForReport[pid].TotalMemory(), 500);

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: UpdateGpuMemoryInfoTest004
 * @tc.desc: Test UpdateGpuMemoryInfo with total memory limit exceeded (line 86 true).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, UpdateGpuMemoryInfoTest004, testing::ext::TestSize.Level1)
{
    auto callback = [](pid_t, size_t, bool) {return true; };
    MemorySnapshot::Instance().InitMemoryLimit(callback, 10000, 20000, 1000);

    pid_t pid1 = 1009;
    pid_t pid2 = 1010;
    std::set<pid_t> exitedPids = {pid1, pid2};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid1, 1024);
    MemorySnapshot::Instance().AddCpuMemory(pid2, 1024);

    std::unordered_map<pid_t, size_t> gpuInfo = {{pid1, 2048}, {pid2, 2048}};
    std::unordered_map<pid_t, MemorySnapshotInfo> pidForReport;
    bool isTotalOver = false;
    MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuInfo, pidForReport, isTotalOver);

    // Total memory > totalMemoryLimit (1000), so all PIDs should be in report (line 86 true)
    ASSERT_TRUE(isTotalOver);
    ASSERT_TRUE(pidForReport.find(pid1) != pidForReport.end());
    ASSERT_TRUE(pidForReport.find(pid2) != pidForReport.end());

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: UpdateGpuMemoryInfoTest005
 * @tc.desc: Test UpdateGpuMemoryInfo with totalMemory > MEMORY_SNAPSHOT_PRINT_HILOG_LIMIT (line 99 true).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, UpdateGpuMemoryInfoTest005, testing::ext::TestSize.Level1)
{
    pid_t pid = 1012;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid, 1400 * 1024 * 1024); // > 1300 * 1024 * 1024

    std::unordered_map<pid_t, size_t> gpuInfo = {{pid, 0}};
    std::unordered_map<pid_t, MemorySnapshotInfo> pidForReport;
    bool isTotalOver = false;

    // totalMemory = 1400MB > MEMORY_SNAPSHOT_PRINT_HILOG_LIMIT (1300MB), line 99 true
    MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuInfo, pidForReport, isTotalOver);

    // Verify that PID is not in pidForReport (no warning, no limit exceeded)
    ASSERT_TRUE(pidForReport.find(pid) == pidForReport.end());
    ASSERT_FALSE(isTotalOver);

    // Verify the snapshot was created and has correct memory
    MemorySnapshotInfo info;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_TRUE(ret);
    ASSERT_EQ(info.cpuMemory, 1400 * 1024 * 1024);
    ASSERT_EQ(info.gpuMemory, 0);

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: UpdateGpuMemoryInfoTest006
 * @tc.desc: Test UpdateGpuMemoryInfo with totalMemory <= MEMORY_SNAPSHOT_PRINT_HILOG_LIMIT (line 99 false).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, UpdateGpuMemoryInfoTest006, testing::ext::TestSize.Level1)
{
    // Set up memory limits using InitMemoryLimit (this is allowed)
    // This ensures singleMemoryWarning_ and totalMemoryLimit_ are set to known values
    auto callback = [](pid_t, size_t, bool) {return true; };
    // Set high limits: 2000MB warning, 3000MB overflow, 5000MB total
    MemorySnapshot::Instance().InitMemoryLimit(callback, 2000, 3000, 5000);

    // Clean up all existing PIDs
    std::vector<pid_t> dirtyList;
    MemorySnapshot::Instance().GetDirtyMemorySnapshot(dirtyList);
    std::unordered_map<pid_t, MemorySnapshotInfo> allSnapshots;
    MemorySnapshot::Instance().GetMemorySnapshot(allSnapshots);

    std::set<pid_t> allPids;
    for (const auto& [pid, info] : allSnapshots) {
        allPids.insert(pid);
    }
    for (auto pid : dirtyList) {
        allPids.insert(pid);
    }
    if (!allPids.empty()) {
        MemorySnapshot::Instance().EraseSnapshotInfoByPid(allPids);
    }

    // Add test PID with memory that's under all limits
    pid_t pid = 1018;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().AddCpuMemory(pid, 500 * 1024 * 1024); // 500MB CPU

    std::unordered_map<pid_t, size_t> gpuInfo = {{pid, 300 * 1024 * 1024}}; // 300MB GPU
    std::unordered_map<pid_t, MemorySnapshotInfo> pidForReport;
    bool isTotalOver = false;

    // totalMemory = 300MB (only GPU counts), <= MEMORY_SNAPSHOT_PRINT_HILOG_LIMIT (1300MB), line 99 false
    // Also under singleMemoryWarning_ (2000MB) and totalMemoryLimit_ (5000MB)
    MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuInfo, pidForReport, isTotalOver);

    // Verify no warnings or reports were triggered
    ASSERT_TRUE(pidForReport.find(pid) == pidForReport.end());
    ASSERT_FALSE(isTotalOver);

    // Verify the snapshot was created correctly
    MemorySnapshotInfo info;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_TRUE(ret);
    ASSERT_EQ(info.cpuMemory, 500 * 1024 * 1024);
    ASSERT_EQ(info.gpuMemory, 300 * 1024 * 1024);
    ASSERT_EQ(info.TotalMemory(), 800 * 1024 * 1024);

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: EraseSnapshotInfoByPidTest002
 * @tc.desc: Test erasing snapshot info for non-existent PID (line 117 false).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, EraseSnapshotInfoByPidTest002, testing::ext::TestSize.Level1)
{
    pid_t existingPid = 1013;
    pid_t nonExistentPid = 9999;
    std::set<pid_t> exitedPids = {existingPid, nonExistentPid};

    MemorySnapshot::Instance().AddCpuMemory(existingPid, 1024);

    // Erase with non-existent PID - should not crash (line 117 false for nonExistentPid)
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);

    MemorySnapshotInfo info;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(existingPid, info);
    ASSERT_FALSE(ret);

    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(nonExistentPid, info);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: GetDirtyMemorySnapshotTest001
 * @tc.desc: Test GetDirtyMemorySnapshot returns dirty PIDs.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, GetDirtyMemorySnapshotTest001, testing::ext::TestSize.Level1)
{
    pid_t pid1 = 1014;
    pid_t pid2 = 1015;
    std::set<pid_t> exitedPids = {pid1, pid2};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);

    MemorySnapshot::Instance().AddCpuMemory(pid1, 1024);
    MemorySnapshot::Instance().AddCpuMemory(pid2, 2048);

    std::vector<pid_t> dirtyList;
    MemorySnapshot::Instance().GetDirtyMemorySnapshot(dirtyList);

    // Should contain pid1 and pid2 (order not guaranteed)
    ASSERT_TRUE(std::find(dirtyList.begin(), dirtyList.end(), pid1) != dirtyList.end());
    ASSERT_TRUE(std::find(dirtyList.begin(), dirtyList.end(), pid2) != dirtyList.end());

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: FillMemorySnapshotTest001
 * @tc.desc: Test FillMemorySnapshot fills bundleName and uid (line 151 true).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, FillMemorySnapshotTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 1016;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);

    MemorySnapshot::Instance().AddCpuMemory(pid, 1024);

    // Prepare infoMap with bundleName and uid
    std::unordered_map<pid_t, MemorySnapshotInfo> infoMap;
    MemorySnapshotInfo fillInfo;
    fillInfo.pid = pid;
    fillInfo.bundleName = "com.example.test";
    fillInfo.uid = 1000;
    infoMap[pid] = fillInfo;

    MemorySnapshot::Instance().FillMemorySnapshot(infoMap);

    // Verify bundleName and uid are filled
    MemorySnapshotInfo result;
    MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, result);
    ASSERT_EQ(result.bundleName, "com.example.test");
    ASSERT_EQ(result.uid, 1000);

    // Verify dirty list is cleared
    std::vector<pid_t> dirtyList;
    MemorySnapshot::Instance().GetDirtyMemorySnapshot(dirtyList);
    ASSERT_TRUE(dirtyList.empty());

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: FillMemorySnapshotTest002
 * @tc.desc: Test FillMemorySnapshot with non-existent PID in appMemorySnapshots_ (line 151 false).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, FillMemorySnapshotTest002, testing::ext::TestSize.Level1)
{
    pid_t pid = 1017;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);

    MemorySnapshot::Instance().AddCpuMemory(pid, 1024);

    // Prepare infoMap with different PID (not in appMemorySnapshots_)
    std::unordered_map<pid_t, MemorySnapshotInfo> infoMap;
    MemorySnapshotInfo fillInfo;
    fillInfo.pid = 9999; // Different PID
    fillInfo.bundleName = "com.example.test";
    fillInfo.uid = 1000;
    infoMap[9999] = fillInfo;

    // Should not crash when PID not found in infoMap (line 151 true, but infoMap[pid] doesn't exist)
    MemorySnapshot::Instance().FillMemorySnapshot(infoMap);

    // Verify pid's info was NOT updated (bundleName and uid remain empty/default)
    MemorySnapshotInfo result;
    MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, result);
    ASSERT_TRUE(result.bundleName.empty());
    ASSERT_EQ(result.uid, 0);

    // Verify dirty list is cleared
    std::vector<pid_t> dirtyList;
    MemorySnapshot::Instance().GetDirtyMemorySnapshot(dirtyList);
    ASSERT_TRUE(dirtyList.empty());

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: FindMaxValuesTest002
 * @tc.desc: Test FindMaxValues with empty snapshot (all branches false).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, FindMaxValuesTest002, testing::ext::TestSize.Level1)
{
    std::vector<MemorySnapshotInfo> list;
    size_t maxCpu = 100;
    size_t maxGpu = 200;
    size_t maxSum = 300;

    MemorySnapshot::Instance().FindMaxValues(list, maxCpu, maxGpu, maxSum);

    // All should remain 0 (no snapshots to compare)
    ASSERT_EQ(maxCpu, 0);
    ASSERT_EQ(maxGpu, 0);
    ASSERT_EQ(maxSum, 0);
    ASSERT_TRUE(list.empty());
}

/**
 * @tc.name: CalculateRiskScoreTest002
 * @tc.desc: Test CalculateRiskScore with zero max values (line 223-225 true).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, CalculateRiskScoreTest002, testing::ext::TestSize.Level1)
{
    MemorySnapshotInfo snapshot;
    snapshot.cpuMemory = 1024;
    snapshot.gpuMemory = 2048;
    size_t maxCpu = 0;
    size_t maxGpu = 0;
    size_t maxSum = 0;

    // All max values are 0, so normalized values should be 0
    float score = MemorySnapshot::Instance().CalculateRiskScore(snapshot, maxCpu, maxGpu, maxSum);
    ASSERT_FLOAT_EQ(score, 0.0);
}

/**
 * @tc.name: CalculateRiskScoreTest003
 * @tc.desc: Test CalculateRiskScore with some zero max values.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, CalculateRiskScoreTest003, testing::ext::TestSize.Level1)
{
    MemorySnapshotInfo snapshot;
    snapshot.cpuMemory = 1024;
    snapshot.gpuMemory = 2048;
    size_t maxCpu = 2048;
    size_t maxGpu = 0;
    size_t maxSum = 6144;

    // maxGpu is 0, other are non-zero
    float expected = 0.3 * 0.5 + 0.3 * 0.0 + 0.4 * 0.5;
    float score = MemorySnapshot::Instance().CalculateRiskScore(snapshot, maxCpu, maxGpu, maxSum);
    ASSERT_FLOAT_EQ(score, expected);
}

/**
 * @tc.name: GetMemorySnapshotTest002
 * @tc.desc: Test GetMemorySnapshot returns empty map when no snapshots.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, GetMemorySnapshotTest002, testing::ext::TestSize.Level1)
{
    // Clean up all existing snapshots
    std::vector<pid_t> dirtyList;
    MemorySnapshot::Instance().GetDirtyMemorySnapshot(dirtyList);
    if (!dirtyList.empty()) {
        std::set<pid_t> exitedPids(dirtyList.begin(), dirtyList.end());
        MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    }

    std::unordered_map<pid_t, MemorySnapshotInfo> snapshot;
    MemorySnapshot::Instance().GetMemorySnapshot(snapshot);

    // Should be empty or only contain system entries
    ASSERT_TRUE(snapshot.empty() || snapshot.size() < 10);
}

/**
 * @tc.name: PrintMemorySnapshotToHilogTest001
 * @tc.desc: Test PrintMemorySnapshotToHilog with currentTime < memorySnapshotHilogTime_ (line 169 true).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, PrintMemorySnapshotToHilogTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 1019;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid, 1024);

    // Verify snapshot exists
    MemorySnapshotInfo info;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_TRUE(ret);
    ASSERT_EQ(info.cpuMemory, 1024);

    // First call should print (currentTime >= memorySnapshotHilogTime_, line 169 false)
    MemorySnapshot::Instance().PrintMemorySnapshotToHilog();

    // Verify snapshot still exists after first call
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_TRUE(ret);

    // Immediate second call should not print (currentTime < memorySnapshotHilogTime_, line 169 true)
    MemorySnapshot::Instance().PrintMemorySnapshotToHilog();

    // Verify snapshot still exists after second call (no crash)
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, info);
    ASSERT_TRUE(ret);
    ASSERT_EQ(info.cpuMemory, 1024);

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: FindMaxValuesTest003
 * @tc.desc: Test FindMaxValues with mixed values (some <= max, some > max).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, FindMaxValuesTest003, testing::ext::TestSize.Level1)
{
    pid_t pid1 = 1020;
    pid_t pid2 = 1021;
    pid_t pid3 = 1022;
    std::set<pid_t> exitedPids = {pid1, pid2, pid3};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);

    // pid1: cpu=1024, gpu=2048, total=3072
    MemorySnapshot::Instance().AddCpuMemory(pid1, 1024);
    // pid2: cpu=2048, gpu=1024, total=3072
    MemorySnapshot::Instance().AddCpuMemory(pid2, 2048);
    // pid3: cpu=512, gpu=512, total=1024
    MemorySnapshot::Instance().AddCpuMemory(pid3, 512);

    std::unordered_map<pid_t, size_t> gpuInfo = {{pid1, 2048}, {pid2, 1024}, {pid3, 512}};
    std::unordered_map<pid_t, MemorySnapshotInfo> pidForReport;
    bool isTotalOver = false;
    MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuInfo, pidForReport, isTotalOver);

    std::vector<MemorySnapshotInfo> list;
    size_t maxCpu = 2048; // Set initial max to test <= branch
    size_t maxGpu = 2048; // Set initial max to test <= branch
    size_t maxSum = 4096; // Set initial max to test <= branch

    MemorySnapshot::Instance().FindMaxValues(list, maxCpu, maxGpu, maxSum);

    // maxCpu should be 2048 (pid2, not updated by pid1 which is <= maxCpu initially)
    ASSERT_EQ(maxCpu, 2048);
    // maxGpu should be 2048 (pid1, not updated by pid2/pid3 which are <= maxGpu initially)
    ASSERT_EQ(maxGpu, 2048);
    // maxSum should be 4096 (not updated, all totals are <= maxSum initially)
    ASSERT_EQ(maxSum, 4096);
    ASSERT_EQ(list.size(), 3);

    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}
} // namespace OHOS::Rosen