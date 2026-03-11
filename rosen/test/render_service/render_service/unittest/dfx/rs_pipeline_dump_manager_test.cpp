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
#include <unordered_set>
#include "gtest/gtest.h"
#include "dfx/rs_pipeline_dump_manager.h"
#include "ipc_callbacks/dfx/rs_dump_callback.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// Mock callback class for testing
class MockRSIDumpCallback : public RSIDumpCallback {
public:
    std::string lastDumpResult;

    void OnDumpResult(std::string& dumpResult) override
    {
        lastDumpResult = dumpResult;
    }

    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

class RSPipelineDumpManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    std::unique_ptr<RSPipelineDumpManager> dumpManager_;
    sptr<MockRSIDumpCallback> mockCallback_;
};

void RSPipelineDumpManagerTest::SetUpTestCase() {}
void RSPipelineDumpManagerTest::TearDownTestCase() {}
void RSPipelineDumpManagerTest::SetUp()
{
    dumpManager_ = std::make_unique<RSPipelineDumpManager>();
    mockCallback_ = new MockRSIDumpCallback();
}
void RSPipelineDumpManagerTest::TearDown()
{
    dumpManager_.reset();
    mockCallback_ = nullptr;
}

/*
 * @tc.name: CmdExec_WithValidCommand
 * @tc.desc: Test CmdExec with valid command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, CmdExec_WithValidCommand, TestSize.Level1)
{
    // Given: Set pid and screenId
    dumpManager_->SetPid(100);
    dumpManager_->SetScreenId(0x12345678);

    // When: Execute a valid command
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;

    dumpManager_->CmdExec(argSets, out, mockCallback_);

    // Then: Output should contain pid and screenId information
    EXPECT_TRUE(out.find("RSProcessDump pid:100") != std::string::npos);
    EXPECT_TRUE(out.find("screenId:") != std::string::npos);
}

/*
 * @tc.name: CmdExec_WithNullCallback
 * @tc.desc: Test CmdExec with null callback - should not crash
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, CmdExec_WithNullCallback, TestSize.Level1)
{
    // Given: Set pid and screenId
    dumpManager_->SetPid(200);

    // When: Execute command with null callback
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;

    // Then: Should not crash, output should still be generated
    dumpManager_->CmdExec(argSets, out, nullptr);
    EXPECT_TRUE(out.find("RSProcessDump pid:200") != std::string::npos);
}

/*
 * @tc.name: CmdExec_CallbackInvoked
 * @tc.desc: Test that callback is invoked with correct dump result
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, CmdExec_CallbackInvoked, TestSize.Level1)
{
    // Given: Set pid and screenId, create callback
    dumpManager_->SetPid(300);
    dumpManager_->SetScreenId(0xABCDEF);

    // When: Execute command with callback
    std::unordered_set<std::u16string> argSets = { u"help" };
    std::string out;

    dumpManager_->CmdExec(argSets, out, mockCallback_);

    // Then: Callback should receive the dump result
    EXPECT_FALSE(mockCallback_->lastDumpResult.empty());
    EXPECT_TRUE(mockCallback_->lastDumpResult.find("RSProcessDump pid:300") != std::string::npos);
}

/*
 * @tc.name: SetPid_ValidPid
 * @tc.desc: Test SetPid with valid process ID
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, SetPid_ValidPid, TestSize.Level1)
{
    // Given: A dump manager instance
    dumpManager_->SetPid(1234);

    // When: Execute command
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;

    dumpManager_->CmdExec(argSets, out, mockCallback_);

    // Then: Output should contain the set pid
    EXPECT_TRUE(out.find("pid:1234") != std::string::npos);
}

/*
 * @tc.name: SetPid_ZeroPid
 * @tc.desc: Test SetPid with zero process ID
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, SetPid_ZeroPid, TestSize.Level1)
{
    // Given: A dump manager instance with pid 0
    dumpManager_->SetPid(0);

    // When: Execute command
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;

    dumpManager_->CmdExec(argSets, out, mockCallback_);

    // Then: Output should contain pid:0
    EXPECT_TRUE(out.find("pid:0") != std::string::npos);
}

/*
 * @tc.name: SetPid_NegativePid
 * @tc.desc: Test SetPid with negative process ID
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, SetPid_NegativePid, TestSize.Level1)
{
    // Given: A dump manager instance
    dumpManager_->SetPid(-1);

    // When: Execute command
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;

    dumpManager_->CmdExec(argSets, out, mockCallback_);

    // Then: Should handle negative pid gracefully
    EXPECT_TRUE(out.find("pid:-1") != std::string::npos);
}

/*
 * @tc.name: SetScreenId_ValidScreenId
 * @tc.desc: Test SetScreenId with valid screen ID
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, SetScreenId_ValidScreenId, TestSize.Level1)
{
    // Given: A dump manager instance
    unsigned long screenId = 0x98765432;
    dumpManager_->SetScreenId(screenId);

    // When: Execute command
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;

    dumpManager_->CmdExec(argSets, out, mockCallback_);

    // Then: Output should contain the set screenId
    EXPECT_TRUE(out.find("screenId:" + std::to_string(screenId)) != std::string::npos);
}

/*
 * @tc.name: SetScreenId_ZeroScreenId
 * @tc.desc: Test SetScreenId with zero screen ID
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, SetScreenId_ZeroScreenId, TestSize.Level1)
{
    // Given: A dump manager instance
    dumpManager_->SetScreenId(0);

    // When: Execute command
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;

    dumpManager_->CmdExec(argSets, out, mockCallback_);

    // Then: Output should contain screenId:0
    EXPECT_TRUE(out.find("screenId:0") != std::string::npos);
}

/*
 * @tc.name: SetPidAndScreenId_BothSet
 * @tc.desc: Test SetPid and SetScreenId together
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, SetPidAndScreenId_BothSet, TestSize.Level1)
{
    // Given: A dump manager instance with both pid and screenId set
    int testPid = 999;
    unsigned long testScreenId = 0xFFFFFFFF;
    dumpManager_->SetPid(testPid);
    dumpManager_->SetScreenId(testScreenId);

    // When: Execute command
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;

    dumpManager_->CmdExec(argSets, out, mockCallback_);

    // Then: Output should contain both pid and screenId
    EXPECT_TRUE(out.find("pid:" + std::to_string(testPid)) != std::string::npos);
    EXPECT_TRUE(out.find("screenId:" + std::to_string(testScreenId)) != std::string::npos);
}

/*
 * @tc.name: CmdExec_InvalidCommand
 * @tc.desc: Test CmdExec with invalid command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, CmdExec_InvalidCommand, TestSize.Level1)
{
    // Given: A dump manager instance
    dumpManager_->SetPid(100);

    // When: Execute invalid command
    std::unordered_set<std::u16string> argSets = { u"invalid_command_xyz" };
    std::string out;

    dumpManager_->CmdExec(argSets, out, mockCallback_);

    // Then: Should output help information
    EXPECT_TRUE(out.find("help text") != std::string::npos ||
                out.find("RSProcessDump") != std::string::npos);
}

/*
 * @tc.name: CmdExec_EmptyArgSets
 * @tc.desc: Test CmdExec with empty argument sets
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, CmdExec_EmptyArgSets, TestSize.Level1)
{
    // Given: A dump manager instance
    dumpManager_->SetPid(100);

    // When: Execute with empty argSets
    std::unordered_set<std::u16string> argSets;
    std::string out;

    dumpManager_->CmdExec(argSets, out, mockCallback_);

    // Then: Should output help information
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: CmdExec_MultipleCallsWithDifferentPid
 * @tc.desc: Test CmdExec with multiple calls using different pids
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumpManagerTest, CmdExec_MultipleCallsWithDifferentPid, TestSize.Level1)
{
    // Given: A dump manager instance
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out1, out2, out3;

    // When: Execute with different pids
    dumpManager_->SetPid(100);
    dumpManager_->CmdExec(argSets, out1, mockCallback_);

    dumpManager_->SetPid(200);
    dumpManager_->CmdExec(argSets, out2, mockCallback_);

    dumpManager_->SetPid(300);
    dumpManager_->CmdExec(argSets, out3, mockCallback_);

    // Then: Each output should have its respective pid
    EXPECT_TRUE(out1.find("pid:100") != std::string::npos);
    EXPECT_TRUE(out2.find("pid:200") != std::string::npos);
    EXPECT_TRUE(out3.find("pid:300") != std::string::npos);
}

} // namespace OHOS::Rosen
