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
 * @tc.name: EraseSnapshotInfoByPidTest001
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
 * @tc.name: InitMemoryLimitTest001
 * @tc.desc: Test InitMemoryLimit initializes only once.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, InitMemoryLimitTest001, testing::ext::TestSize.Level1)
{
    auto callback = [](pid_t, size_t, bool) { return true; };
    MemorySnapshot::Instance().InitMemoryLimit(callback, 1000, 2000, 3000);
    MemorySnapshot::Instance().InitMemoryLimit(nullptr, 5000, 6000, 7000);
    ASSERT_EQ(MemorySnapshot::Instance().singleMemoryWarning_, 1000);
    ASSERT_EQ(MemorySnapshot::Instance().totalMemoryLimit_, 3000);
}

/**
 * @tc.name: InstanceSingletonTest001
 * @tc.desc: Test that Instance() returns the same singleton instance.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, InstanceSingletonTest001, testing::ext::TestSize.Level1)
{
    MemorySnapshot& instance1 = MemorySnapshot::Instance();
    MemorySnapshot& instance2 = MemorySnapshot::Instance();
    ASSERT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: GetDirtyMemorySnapshotTest001
 * @tc.desc: Test getting dirty memory snapshot list.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, GetDirtyMemorySnapshotTest001, testing::ext::TestSize.Level1)
{
    pid_t pid1 = 2001;
    pid_t pid2 = 2002;
    std::set<pid_t> exitedPids = {pid1, pid2};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid1, 1024);
    MemorySnapshot::Instance().AddCpuMemory(pid2, 2048);
    std::vector<pid_t> dirtyList;
    MemorySnapshot::Instance().GetDirtyMemorySnapshot(dirtyList);
    ASSERT_TRUE(std::find(dirtyList.begin(), dirtyList.end(), pid1) != dirtyList.end());
    ASSERT_TRUE(std::find(dirtyList.begin(), dirtyList.end(), pid2) != dirtyList.end());
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: FillMemorySnapshotTest001
 * @tc.desc: Test filling memory snapshot with bundle info.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, FillMemorySnapshotTest001, testing::ext::TestSize.Level1)
{
    pid_t pid = 2003;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid, 1024);
    std::unordered_map<pid_t, MemorySnapshotInfo> infoMap;
    MemorySnapshotInfo bundleInfo;
    bundleInfo.pid = pid;
    bundleInfo.bundleName = "com.example.test";
    bundleInfo.uid = 10001;
    infoMap[pid] = bundleInfo;
    MemorySnapshot::Instance().FillMemorySnapshot(infoMap);
    MemorySnapshotInfo actualInfo;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid, actualInfo);
    ASSERT_TRUE(ret);
    ASSERT_EQ(actualInfo.bundleName, "com.example.test");
    ASSERT_EQ(actualInfo.uid, 10001);
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: GetTotalMemoryTest001
 * @tc.desc: Test GetTotalMemory returns correct total memory.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, GetTotalMemoryTest001, testing::ext::TestSize.Level1)
{
    pid_t pid1 = 7001;
    pid_t pid2 = 7002;
    std::set<pid_t> exitedPids = {pid1, pid2};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    MemorySnapshot::Instance().AddCpuMemory(pid1, 1024);
    MemorySnapshot::Instance().AddCpuMemory(pid2, 2048);
    std::unordered_map<pid_t, size_t> gpuInfo = {{pid1, 512}, {pid2, 1024}};
    std::unordered_map<pid_t, MemorySnapshotInfo> pidForReport;
    bool isTotalOver = false;
    MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuInfo, pidForReport, isTotalOver);
    size_t totalMemory = MemorySnapshot::Instance().GetTotalMemory();
    ASSERT_GE(totalMemory, 1000);
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: UpdateGpuMemoryInfoTest002
 * @tc.desc: Test UpdateGpuMemoryInfo detects total memory overflow.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemorySnapshotTest, UpdateGpuMemoryInfoTest002, testing::ext::TestSize.Level1)
{
    pid_t pid = 8001;
    std::set<pid_t> exitedPids = {pid};
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
    auto callback = [](pid_t, uint64_t, bool) { return true; };
    MemorySnapshot::Instance().InitMemoryLimit(callback, 1000, 2000, 1);
    MemorySnapshot::Instance().AddCpuMemory(pid, 50);
    std::unordered_map<pid_t, size_t> gpuInfo = {{pid, 60}};
    std::unordered_map<pid_t, MemorySnapshotInfo> pidForReport;
    bool isTotalOver = false;
    MemorySnapshot::Instance().UpdateGpuMemoryInfo(gpuInfo, pidForReport, isTotalOver);
    ASSERT_TRUE(isTotalOver);
    ASSERT_TRUE(pidForReport.find(pid) != pidForReport.end());
    MemorySnapshot::Instance().EraseSnapshotInfoByPid(exitedPids);
}

/**
 * @tc.name: CalculateRiskScoreTest002
 * @tc.desc: Test CalculateRiskScore with zero max values.
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
    float score = MemorySnapshot::Instance().CalculateRiskScore(snapshot, maxCpu, maxGpu, maxSum);
    ASSERT_FLOAT_EQ(score, 0.0f);
}
} // namespace OHOS::Rosen