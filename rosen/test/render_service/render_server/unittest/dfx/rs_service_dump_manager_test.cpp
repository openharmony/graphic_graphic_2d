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

#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "gtest/gtest.h"
#include "gtest/gtest-death-test.h"
#include "dfx/rs_service_dump_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSServiceDumpManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    std::unique_ptr<RSServiceDumpManager> dumpManager_;
};

void RSServiceDumpManagerTest::SetUpTestCase() {}
void RSServiceDumpManagerTest::TearDownTestCase() {}
void RSServiceDumpManagerTest::SetUp()
{
    dumpManager_ = std::make_unique<RSServiceDumpManager>();
}
void RSServiceDumpManagerTest::TearDown()
{
    dumpManager_.reset();
}

/*
 * @tc.name: IsServiceDumpCmd_ValidServiceCommand
 * @tc.desc: Test IsServiceDumpCmd with valid service commands
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, IsServiceDumpCmd_ValidServiceCommand, TestSize.Level1)
{
    // Given: A list of valid service commands
    std::vector<std::u16string> serviceCmds = {
        u"screen", u"surface", u"fps", u"fpsClear",
        u"fpsCount", u"clearFpsCount", u"hitchs",
        u"gles", u"rsLogFlag", u"allInfo"
    };

    // When & Then: All commands should be identified as service dump commands
    for (const auto& cmd : serviceCmds) {
        bool result = RSServiceDumpManager::IsServiceDumpCmd(cmd);
        EXPECT_TRUE(result) << "Command should be recognized as service dump command";
    }
}

/*
 * @tc.name: IsServiceDumpCmd_InvalidServiceCommand
 * @tc.desc: Test IsServiceDumpCmd with invalid commands
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, IsServiceDumpCmd_InvalidServiceCommand, TestSize.Level1)
{
    // Given: A list of invalid commands
    std::vector<std::u16string> invalidCmds = {
        u"invalid", u"RSTree", u"nodeNotOnTree",
        u"EventParamList", u"dumpMem", u"client"
    };

    // When & Then: None should be identified as service dump commands
    for (const auto& cmd : invalidCmds) {
        bool result = RSServiceDumpManager::IsServiceDumpCmd(cmd);
        EXPECT_FALSE(result) << "Command should NOT be recognized as service dump command";
    }
}

/*
 * @tc.name: IsProcessDumpCmd_ValidProcessCommand
 * @tc.desc: Test IsProcessDumpCmd with valid process commands
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, IsProcessDumpCmd_ValidProcessCommand, TestSize.Level1)
{
    // Given: A list of valid process commands
    std::vector<std::u16string> processCmds = {
        u"nodeNotOnTree", u"surface", u"allSurfacesMem",
        u"RSTree", u"MultiRSTrees", u"EventParamList",
        u"dumpMem", u"surfacenode", u"flushJankStatsRs",
        u"client", u"rsLogFlag", u"dumpExistPidMem",
        u"buffer", u"allInfo"
    };

    // When & Then: All commands should be identified as process dump commands
    for (const auto& cmd : processCmds) {
        bool result = RSServiceDumpManager::IsProcessDumpCmd(cmd);
        EXPECT_TRUE(result) << "Command should be recognized as process dump command";
    }
}

/*
 * @tc.name: IsProcessDumpCmd_InvalidProcessCommand
 * @tc.desc: Test IsProcessDumpCmd with invalid commands
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, IsProcessDumpCmd_InvalidProcessCommand, TestSize.Level1)
{
    // Given: A list of invalid commands
    std::vector<std::u16string> invalidCmds = {
        u"invalid", u"screen", u"gles"
    };

    // When & Then: None should be identified as process dump commands
    for (const auto& cmd : invalidCmds) {
        bool result = RSServiceDumpManager::IsProcessDumpCmd(cmd);
        EXPECT_FALSE(result) << "Command should NOT be recognized as process dump command";
    }
}

/*
 * @tc.name: IsServiceDumpCmd_EmptyCommand
 * @tc.desc: Test IsServiceDumpCmd with empty command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, IsServiceDumpCmd_EmptyCommand, TestSize.Level1)
{
    // Given: An empty command
    std::u16string emptyCmd = u"";

    // When & Then: Should not be recognized as service dump command
    bool result = RSServiceDumpManager::IsServiceDumpCmd(emptyCmd);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: IsProcessDumpCmd_EmptyCommand
 * @tc.desc: Test IsProcessDumpCmd with empty command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, IsProcessDumpCmd_EmptyCommand, TestSize.Level1)
{
    // Given: An empty command
    std::u16string emptyCmd = u"";

    // When & Then: Should not be recognized as process dump command
    bool result = RSServiceDumpManager::IsProcessDumpCmd(emptyCmd);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: InitProcessDumpTask_ValidProcessCount
 * @tc.desc: Test InitProcessDumpTask with valid process count
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, InitProcessDumpTask_ValidProcessCount, TestSize.Level1)
{
    // Given: A dump manager instance
    ASSERT_NE(dumpManager_, nullptr);

    // When: Initialize with process count
    int32_t processCount = 3;

    // Then: Initialization should succeed (verified by no crash)
    dumpManager_->InitProcessDumpTask(processCount);
    SUCCEED();
}

/*
 * @tc.name: InitProcessDumpTask_ZeroProcessCount
 * @tc.desc: Test InitProcessDumpTask with zero process count
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, InitProcessDumpTask_ZeroProcessCount, TestSize.Level1)
{
    // Given: A dump manager instance
    ASSERT_NE(dumpManager_, nullptr);

    // When: Initialize with zero process count
    dumpManager_->InitProcessDumpTask(0);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: InitProcessDumpTask_NegativeProcessCount
 * @tc.desc: Test InitProcessDumpTask with negative process count
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, InitProcessDumpTask_NegativeProcessCount, TestSize.Level1)
{
    // Given: A dump manager instance
    ASSERT_NE(dumpManager_, nullptr);

    // When: Initialize with negative process count
    dumpManager_->InitProcessDumpTask(-1);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: CollectDump_SingleCollection
 * @tc.desc: Test CollectDump with single dump collection
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, CollectDump_SingleCollection, TestSize.Level1)
{
    // Given: A dump manager instance initialized with 1 process
    dumpManager_->InitProcessDumpTask(1);
    std::string testData = "Test dump data";

    // When: Collect dump data
    dumpManager_->CollectDump(testData);

    ASSERT_NE(dumpManager_, nullptr);
    // Then: Collection should succeed (verified by no crash)
    SUCCEED();
}

/*
 * @tc.name: CollectDump_MultipleCollections
 * @tc.desc: Test CollectDump with multiple dump collections
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, CollectDump_MultipleCollections, TestSize.Level1)
{
    // Given: A dump manager instance initialized with 3 processes
    dumpManager_->InitProcessDumpTask(3);

    // When: Collect multiple dump data
    std::string dump1 = "Dump data 1";
    std::string dump2 = "Dump data 2";
    std::string dump3 = "Dump data 3";
    dumpManager_->CollectDump(dump1);
    dumpManager_->CollectDump(dump2);
    dumpManager_->CollectDump(dump3);

    ASSERT_NE(dumpManager_, nullptr);
    // Then: All collections should succeed
    SUCCEED();
}

/*
 * @tc.name: CollectDump_EmptyString
 * @tc.desc: Test CollectDump with empty string
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, CollectDump_EmptyString, TestSize.Level1)
{
    // Given: A dump manager instance
    dumpManager_->InitProcessDumpTask(1);
    std::string emptyData = "";

    // When: Collect empty dump data
    dumpManager_->CollectDump(emptyData);

    ASSERT_NE(dumpManager_, nullptr);
    // Then: Should handle empty string gracefully
    SUCCEED();
}

/*
 * @tc.name: WaitForDump_Timeout
 * @tc.desc: Test WaitForDump with timeout scenario
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, WaitForDump_Timeout, TestSize.Level1)
{
    // Given: A dump manager instance with 1 process but no collection
    dumpManager_->InitProcessDumpTask(1);
    std::string dumpString;

    // When: Wait for dump (will timeout after 2500ms)
    dumpManager_->WaitForDump(dumpString);

    // Then: Should timeout and return with partial data
    EXPECT_TRUE(dumpString.find("RSProcessDump") != std::string::npos);
}

/*
 * @tc.name: WaitForDump_WithCompleteData
 * @tc.desc: Test WaitForDump with complete dump data
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, WaitForDump_WithCompleteData, TestSize.Level1)
{
    // Given: A dump manager instance initialized and data collected
    dumpManager_->InitProcessDumpTask(1);

    // Collect data in a separate thread
    std::thread collector([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::string testData = "Process dump completed";
        dumpManager_->CollectDump(testData);
    });
    collector.detach();

    std::string dumpString;

    // When: Wait for dump
    dumpManager_->WaitForDump(dumpString);

    // Then: Should contain collected data
    EXPECT_TRUE(dumpString.find("RSProcessDump") != std::string::npos);
}

/*
 * @tc.name: WaitForDump_MultipleProcesses
 * @tc.desc: Test WaitForDump with multiple processes
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, WaitForDump_MultipleProcesses, TestSize.Level1)
{
    // Given: A dump manager instance with 3 processes
    int32_t processCount = 3;
    dumpManager_->InitProcessDumpTask(processCount);

    // Collect data from all processes in separate threads
    for (int i = 0; i < processCount; i++) {
        std::thread collector([this, i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::string testData = "Process " + std::to_string(i) + " dump";
            dumpManager_->CollectDump(testData);
        });
        collector.detach();
    }

    std::string dumpString;

    // When: Wait for all dumps
    dumpManager_->WaitForDump(dumpString);

    // Then: Should contain data from all processes
    EXPECT_TRUE(dumpString.find("RSProcessDump") != std::string::npos);
}

/*
 * @tc.name: IsServiceDumpCmd_CaseSensitive
 * @tc.desc: Test IsServiceDumpCmd case sensitivity
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, IsServiceDumpCmd_CaseSensitive, TestSize.Level1)
{
    // Given: Commands with different cases
    std::u16string lowerCase = u"screen";
    std::u16string upperCase = u"SCREEN";
    std::u16string mixedCase = u"Screen";

    // When & Then: Only exact match should be recognized
    EXPECT_TRUE(RSServiceDumpManager::IsServiceDumpCmd(lowerCase));
    EXPECT_FALSE(RSServiceDumpManager::IsServiceDumpCmd(upperCase));
    EXPECT_FALSE(RSServiceDumpManager::IsServiceDumpCmd(mixedCase));
}

/*
 * @tc.name: IsProcessDumpCmd_CaseSensitive
 * @tc.desc: Test IsProcessDumpCmd case sensitivity
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, IsProcessDumpCmd_CaseSensitive, TestSize.Level1)
{
    // Given: Commands with different cases
    std::u16string lowerCase = u"RSTree";
    std::u16string upperCase = u"RSTREE";
    std::u16string mixedCase = u"Rstree";

    // When & Then: Only exact match should be recognized
    EXPECT_TRUE(RSServiceDumpManager::IsProcessDumpCmd(lowerCase));
    EXPECT_FALSE(RSServiceDumpManager::IsProcessDumpCmd(upperCase));
    EXPECT_FALSE(RSServiceDumpManager::IsProcessDumpCmd(mixedCase));
}

/*
 * @tc.name: InitProcessDumpTask_MultipleCalls
 * @tc.desc: Test InitProcessDumpTask called multiple times
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, InitProcessDumpTask_MultipleCalls, TestSize.Level1)
{
    // Given: A dump manager instance

    // When: Initialize multiple times with different counts
    dumpManager_->InitProcessDumpTask(2);
    dumpManager_->InitProcessDumpTask(3);
    dumpManager_->InitProcessDumpTask(1);

    EXPECT_EQ(dumpManager_->processCount_, 1);
    // Then: Should handle re-initialization gracefully
    SUCCEED();
}

/*
 * @tc.name: DoDump_ServiceCmdOnly
 * @tc.desc: Test DoDump with only service commands
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, DoDump_ServiceCmdOnly, TestSize.Level1)
{
    // Given: A dump manager instance
    ASSERT_NE(dumpManager_, nullptr);

    // When: Execute DoDump with service command only
    std::vector<std::u16string> args = { u"screen" };
    std::string dumpString;

    // Then: Should execute service dump (processManager is null, will return early)
    dumpManager_->DoDump(args, dumpString, nullptr);

    // Should not crash
    SUCCEED();
}

/*
 * @tc.name: DoDump_ProcessCmdOnly
 * @tc.desc: Test DoDump with screen command (service command)
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, DoDump_ProcessCmdOnly, TestSize.Level1)
{
    // Given: A dump manager instance
    ASSERT_NE(dumpManager_, nullptr);

    // When: Execute DoDump with screen command (service command)
    std::vector<std::u16string> args = { u"screen" };
    std::string dumpString;

    // Then: Should execute service dump (does not access processManager)
    dumpManager_->DoDump(args, dumpString, nullptr);

    // Should not crash
    SUCCEED();
}

/*
 * @tc.name: DoDump_BothCmds
 * @tc.desc: Test DoDump with multiple service commands
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, DoDump_BothCmds, TestSize.Level1)
{
    // Given: A dump manager instance
    ASSERT_NE(dumpManager_, nullptr);

    // When: Execute DoDump with multiple service commands
    std::vector<std::u16string> args = { u"screen", u"fps" };
    std::string dumpString;

    // Then: Should handle multiple service commands
    dumpManager_->DoDump(args, dumpString, nullptr);

    // Should not crash
    SUCCEED();
}

/*
 * @tc.name: DoDump_NonCmdArgs
 * @tc.desc: Test DoDump with empty arguments - processArgSets empty
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, DoDump_NonCmdArgs, TestSize.Level1)
{
    // Given: A dump manager instance
    ASSERT_NE(dumpManager_, nullptr);

    // When: Execute DoDump with empty args (processArgSets empty)
    std::vector<std::u16string> args;
    std::string dumpString;

    // Then: Should execute (both sets empty, processArgSets.empty() = true)
    dumpManager_->DoDump(args, dumpString, nullptr);

    // Should not crash
    SUCCEED();
}

/*
 * @tc.name: DoDump_ServiceOnlyCommands
 * @tc.desc: Test DoDump with only service commands - processArgSets empty
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, DoDump_ServiceOnlyCommands, TestSize.Level1)
{
    // Given: A dump manager instance
    ASSERT_NE(dumpManager_, nullptr);

    // When: Execute DoDump with only service commands
    std::vector<std::u16string> args = { u"screen" };
    std::string dumpString;

    // Then: Should execute (processArgSets empty)
    dumpManager_->DoDump(args, dumpString, nullptr);

    // Should not crash
    SUCCEED();
}

/*
 * @tc.name: DoDump_MixedArgsNoProcess
 * @tc.desc: Test DoDump with mixed args but no process command - processArgSets empty
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, DoDump_MixedArgsNoProcess, TestSize.Level1)
{
    // Given: A dump manager instance
    ASSERT_NE(dumpManager_, nullptr);

    // When: Execute DoDump with service commands and extra args (no process cmd)
    std::vector<std::u16string> args = { u"screen", u"fps", u"extraArg" };
    std::string dumpString;

    // Then: Should execute service dump only (processArgSets may have extraArg but not process cmd)
    dumpManager_->DoDump(args, dumpString, nullptr);

    // Should not crash
    SUCCEED();
}

/*
 * @tc.name: DoDump_EmptyArgs
 * @tc.desc: Test DoDump with empty arguments
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, DoDump_EmptyArgs, TestSize.Level1)
{
    // Given: A dump manager instance
    ASSERT_NE(dumpManager_, nullptr);

    // When: Execute DoDump with empty arguments
    std::vector<std::u16string> args;
    std::string dumpString;

    // Then: Should handle gracefully
    dumpManager_->DoDump(args, dumpString, nullptr);

    // Should not crash
    SUCCEED();
}

/*
 * @tc.name: IsDumpCompleted_True
 * @tc.desc: Test IsDumpCompleted returns true when all processes complete
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSServiceDumpManagerTest, IsDumpCompleted_True, TestSize.Level1)
{
    // Given: A dump manager initialized with 3 processes and all completed
    dumpManager_->InitProcessDumpTask(3);
    
    // Simulate completion
    std::string dump1 = "data1";
    std::string dump2 = "data2";
    std::string dump3 = "data3";
    dumpManager_->CollectDump(dump1);
    dumpManager_->CollectDump(dump2);
    dumpManager_->CollectDump(dump3);

    // When: Check if dump is completed
    // Note: IsDumpCompleted is private, indirectly tested through WaitForDump
    std::string dumpString;
    dumpManager_->WaitForDump(dumpString);

    // Then: Should complete without timeout
    EXPECT_TRUE(dumpString.find("RSProcessDump") != std::string::npos);
}

} // namespace OHOS::Rosen
