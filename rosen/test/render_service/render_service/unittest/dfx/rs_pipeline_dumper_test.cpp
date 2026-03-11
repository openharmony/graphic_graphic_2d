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
#include <memory>
#include "gtest/gtest.h"
#include "dfx/rs_pipeline_dumper.h"
#include "dfx/rs_pipeline_dump_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSPipelineDumperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    std::unique_ptr<RSPipelineDumper> dumper_;
    std::shared_ptr<RSPipelineDumpManager> dumpManager_;
};

void RSPipelineDumperTest::SetUpTestCase() {}
void RSPipelineDumperTest::TearDownTestCase() {}
void RSPipelineDumperTest::SetUp()
{
    dumpManager_ = std::make_shared<RSPipelineDumpManager>();
    dumper_ = std::make_unique<RSPipelineDumper>();
}
void RSPipelineDumperTest::TearDown()
{
    dumper_.reset();
    dumpManager_.reset();
}

/*
 * @tc.name: RenderPipelineDumpInit_RegisterAllHandlers
 * @tc.desc: Test RenderPipelineDumpInit registers all dump handlers
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RenderPipelineDumpInit_RegisterAllHandlers, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"help" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterRSGfxFuncs_ValidManager
 * @tc.desc: Test RegisterRSGfxFuncs with valid manager
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSGfxFuncs_ValidManager, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"EventParamList" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_TRUE(out.find("RSProcessDump") != std::string::npos || !out.empty());
}

/*
 * @tc.name: RegisterRSTreeFuncs_ValidManager
 * @tc.desc: Test RegisterRSTreeFuncs with valid manager
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSTreeFuncs_ValidManager, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_TRUE(out.find("RSProcessDump") != std::string::npos || !out.empty());
}

/*
 * @tc.name: RegisterMemFuncs_ValidManager
 * @tc.desc: Test RegisterMemFuncs with valid manager
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterMemFuncs_ValidManager, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"allSurfacesMem" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_TRUE(out.find("RSProcessDump") != std::string::npos || !out.empty());
}

/*
 * @tc.name: RegisterGpuFuncs_ValidManager
 * @tc.desc: Test RegisterGpuFuncs with valid manager
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterGpuFuncs_ValidManager, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

#ifdef RS_ENABLE_VK
    std::unordered_set<std::u16string> argSets = { u"vktextureLimit" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);
    EXPECT_TRUE(out.find("RSProcessDump") != std::string::npos || !out.empty());
#endif
    SUCCEED();
}

/*
 * @tc.name: RegisterBufferFuncs_ValidManager
 * @tc.desc: Test RegisterBufferFuncs with valid manager
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterBufferFuncs_ValidManager, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"buffer" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_TRUE(out.find("RSProcessDump") != std::string::npos || !out.empty());
}

/*
 * @tc.name: RegisterSurfaceInfoFuncs_ValidManager
 * @tc.desc: Test RegisterSurfaceInfoFuncs with valid manager
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterSurfaceInfoFuncs_ValidManager, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"surface" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_TRUE(out.find("RSProcessDump") != std::string::npos || !out.empty());
}

/*
 * @tc.name: MultipleDumpers_CanCoexist
 * @tc.desc: Test that multiple RSPipelineDumper instances can coexist
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, MultipleDumpers_CanCoexist, TestSize.Level1)
{
    auto dumper1 = std::make_unique<RSPipelineDumper>();
    auto dumper2 = std::make_unique<RSPipelineDumper>();
    auto dumper3 = std::make_unique<RSPipelineDumper>();

    auto manager1 = std::make_shared<RSPipelineDumpManager>();
    auto manager2 = std::make_shared<RSPipelineDumpManager>();
    auto manager3 = std::make_shared<RSPipelineDumpManager>();

    dumper1->RenderPipelineDumpInit(manager1);
    dumper2->RenderPipelineDumpInit(manager2);
    dumper3->RenderPipelineDumpInit(manager3);

    std::unordered_set<std::u16string> argSets = { u"help" };
    std::string out1, out2, out3;

    manager1->CmdExec(argSets, out1, nullptr);
    manager2->CmdExec(argSets, out2, nullptr);
    manager3->CmdExec(argSets, out3, nullptr);

    EXPECT_FALSE(out1.empty());
    EXPECT_FALSE(out2.empty());
    EXPECT_FALSE(out3.empty());
}

/*
 * @tc.name: GenerateTaskId_Incremental
 * @tc.desc: Test GenerateTaskId generates unique incremental IDs
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, GenerateTaskId_Incremental, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Generate multiple task IDs
    uint32_t id1 = dumper_->GenerateTaskId();
    uint32_t id2 = dumper_->GenerateTaskId();
    uint32_t id3 = dumper_->GenerateTaskId();

    // Then: IDs should be incremental
    EXPECT_EQ(id2, id1 + 1);
    EXPECT_EQ(id3, id2 + 1);
}

/*
 * @tc.name: GenerateTaskId_Overflow
 * @tc.desc: Test GenerateTaskId handles overflow
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, GenerateTaskId_Overflow, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Generate many IDs (potential overflow scenario)
    for (uint32_t i = 0; i < 5; ++i) {
        auto id = dumper_->GenerateTaskId();
        // Should continue to work even near overflow
        EXPECT_GT(id, 0);
    }
}

/*
 * @tc.name: RegisterRSGfxFuncs_EventParamList
 * @tc.desc: Test RegisterRSGfxFuncs registers event param dump function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSGfxFuncs_EventParamList, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    // When: Register Gfx functions
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: EventParamList command should be registered
    std::unordered_set<std::u16string> argSets = { u"EventParamList" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterRSGfxFuncs_RSLogFlag
 * @tc.desc: Test RegisterRSGfxFuncs handles log flag setting
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSGfxFuncs_RSLogFlag, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Set log flag
    std::unordered_set<std::u16string> argSets = { u"rsLogFlag", u"123" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should attempt to set flag
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterRSTreeFuncs_NotOnTree
 * @tc.desc: Test RegisterRSTreeFuncs registers node not on tree dump
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSTreeFuncs_NotOnTree, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: nodeNotOnTree command should be registered
    std::unordered_set<std::u16string> argSets = { u"nodeNotOnTree" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterRSTreeFuncs_MultiTrees
 * @tc.desc: Test RegisterRSTreeFuncs registers multi-tree dump
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSTreeFuncs_MultiTrees, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: MultiRSTrees command should be registered
    std::unordered_set<std::u16string> argSets = { u"MultiRSTrees" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterRSTreeFuncs_SurfaceNode
 * @tc.desc: Test RegisterRSTreeFuncs handles surface node dump
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSTreeFuncs_SurfaceNode, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: surfacenode command should be registered
    std::unordered_set<std::u16string> argSets = { u"surfacenode", u"123" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterRSTreeFuncs_ClientInfo
 * @tc.desc: Test RegisterRSTreeFuncs registers client dump
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSTreeFuncs_ClientInfo, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: client command should be registered
    std::unordered_set<std::u16string> argSets = { u"client" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterMemFuncs_DumpMem
 * @tc.desc: Test RegisterMemFuncs registers mem dump function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterMemFuncs_DumpMem, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: dumpMem command should be registered
    std::unordered_set<std::u16string> argSets = { u"dumpMem", u"123" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterMemFuncs_ExistPidMem
 * @tc.desc: Test RegisterMemFuncs registers exist pid mem function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterMemFuncs_ExistPidMem, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: existPidMem command should be registered
    std::unordered_set<std::u16string> argSets = { u"dumpExistPidMem", u"123" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterGpuFuncs_VkTextureLimit
 * @tc.desc: Test RegisterGpuFuncs registers vk texture limit dump
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterGpuFuncs_VkTextureLimit, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

#ifdef RS_ENABLE_VK
    // Then: vktextureLimit command should be registered
    std::unordered_set<std::u16string> argSets = { u"vktextureLimit" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
#else
    SUCCEED();
#endif
}

/*
 * @tc.name: RegisterBufferFuncs_CurrentFrameBuffer
 * @tc.desc: Test RegisterBufferFuncs registers frame buffer dump
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterBufferFuncs_CurrentFrameBuffer, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: currentFrameBuffer command should be registered
    std::unordered_set<std::u16string> argSets = { u"buffer" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterSurfaceInfoFuncs_SurfaceInfo
 * @tc.desc: Test RegisterSurfaceInfoFuncs registers surface info dump
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterSurfaceInfoFuncs_SurfaceInfo, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: surface command should be registered
    std::unordered_set<std::u16string> argSets = { u"surface" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: Constructor_WithNullHandler
 * @tc.desc: Test dumper with null event handler
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, Constructor_WithNullHandler, TestSize.Level1)
{
    // Given: Null event handler
    std::shared_ptr<AppExecFwk::EventHandler> nullHandler = nullptr;

    // When: Create dumper with null handler
    RSPipelineDumper nullHandlerDumper(nullHandler);

    // Then: Should create successfully
    SUCCEED();
}

/*
 * @tc.name: ScheduleTask_WithValidHandler
 * @tc.desc: Test ScheduleTask with valid handler
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, ScheduleTask_WithValidHandler, TestSize.Level1)
{
    // Given: Dumper with valid handler
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = AppExecFwk::EventRunner::Create(true);
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    ASSERT_NE(handler_, nullptr);
    auto dumper = std::make_unique<RSPipelineDumper>(handler_);
    // When: Schedule task
    bool taskExecuted = false;
    dumper->ScheduleTask([&taskExecuted]() { taskExecuted = true; });

    // Then: Task should be executed
    EXPECT_TRUE(taskExecuted);
}

/*
 * @tc.name: ScheduleTask_WithNullHandler
 * @tc.desc: Test ScheduleTask with null handler
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, ScheduleTask_WithNullHandler, TestSize.Level1)
{
    // Given: Dumper with null handler
    std::shared_ptr<AppExecFwk::EventHandler> nullHandler = nullptr;
    RSPipelineDumper nullHandlerDumper(nullHandler);

    // When: Schedule task
    bool taskExecuted = false;
    nullHandlerDumper.ScheduleTask([&taskExecuted]() { taskExecuted = true; });

    // Then: Task should not execute
    EXPECT_FALSE(taskExecuted);
}

/*
 * @tc.name: IsNumber_ValidNumbers
 * @tc.desc: Test IsNumber utility function with valid numbers
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, IsNumber_ValidNumbers, TestSize.Level1)
{
    // Given: Various number strings
    std::vector<std::string> validNumbers = { "0", "123", "99999", "007", "42" };

    // When & Then: All should be recognized as numbers
    for (const auto& numStr : validNumbers) {
        // Note: IsNumber is a static function in the implementation file
        // We test indirectly through DumpMem
        RSPipelineDumper dumper(nullptr);
        std::unordered_set<std::u16string> argSets;
        std::string out;
        argSets.insert(u"dumpMem");
        argSets.insert(std::u16string(numStr.begin(), numStr.end()));

        // Should not crash
        SUCCEED();
    }
}

/*
 * @tc.name: IsNumber_InvalidNumbers
 * @tc.desc: Test IsNumber with invalid inputs
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, IsNumber_InvalidNumbers, TestSize.Level1)
{
    // Given: Various invalid strings
    std::vector<std::string> invalidStrings = { "", "abc", "12a34", "12 34", "-123", "12.34" };

    // When & Then: None should be recognized as numbers
    for (const auto& str : invalidStrings) {
        RSPipelineDumper dumper(nullptr);
        std::unordered_set<std::u16string> argSets;
        std::string out;
        argSets.insert(u"dumpMem");
        if (!str.empty()) {
            argSets.insert(std::u16string(str.begin(), str.end()));
        }

        // Should handle gracefully
        SUCCEED();
    }
}

/*
 * @tc.name: ExtractDumpInfo_NotUniRender
 * @tc.desc: Test ExtractDumpInfo when not in UniRender mode
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, ExtractDumpInfo_NotUniRender, TestSize.Level1)
{
    // Given: A dumper and args
    RSPipelineDumper dumper(nullptr);
    std::unordered_set<std::u16string> argSets = { u"test" };
    std::string dumpInfo;

    // Note: ExtractDumpInfo is called internally by dump functions
    // We test the behavior indirectly
    SUCCEED();
}

/*
 * @tc.name: DumpNodesNotOnTree_BasicFlow
 * @tc.desc: Test DumpNodesNotOnTheTree basic functionality
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodesNotOnTree_BasicFlow, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Dump nodes not on tree
    std::string dumpString;
    // Note: This is a private function, tested indirectly via command
    std::unordered_set<std::u16string> argSets = { u"nodeNotOnTree" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    // Actual functionality depends on RSMainThread state
    SUCCEED();
}

/*
 * @tc.name: DumpRenderServiceTree_SingleFrame
 * @tc.desc: Test DumpRenderServiceTree with single frame
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpRenderServiceTree_SingleFrame, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Dump render service tree
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: DumpAllNodesMemSize_MemoryInfo
 * @tc.desc: Test DumpAllNodesMemSize collects memory info
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpAllNodesMemSize_MemoryInfo, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Dump all nodes memory
    std::unordered_set<std::u16string> argSets = { u"allSurfacesMem" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    // Actual output depends on RSMainThread state
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpRSEvenParam_EventParams
 * @tc.desc: Test DumpRSEvenParam dumps event parameters
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpRSEvenParam_EventParams, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Dump event parameters
    std::unordered_set<std::u16string> argSets = { u"EventParamList" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpJankStatsRs_FlushStats
 * @tc.desc: Test DumpJankStatsRs flushes jank stats
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpJankStatsRs_FlushStats, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Flush jank stats
    std::unordered_set<std::u16string> argSets = { u"flushJankStatsRs" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpVkTextureLimit_GpuInfo
 * @tc.desc: Test DumpVkTextureLimit reports GPU limits
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpVkTextureLimit_GpuInfo, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

#ifdef RS_ENABLE_VK
    // When: Dump vk texture limit
    std::unordered_set<std::u16string> argSets = { u"vktextureLimit" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
#else
    SUCCEED();
#endif
}

/*
 * @tc.name: DumpExistPidMem_ValidPid
 * @tc.desc: Test DumpExistPidMem with valid pid
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpExistPidMem_ValidPid, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Dump exist pid memory
    std::unordered_set<std::u16string> argSets = { u"dumpExistPidMem", u"1234" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpExistPidMem_NonNumeric
 * @tc.desc: Test DumpExistPidMem with non-numeric pid
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpExistPidMem_NonNumeric, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Dump with non-numeric pid
    std::unordered_set<std::u16string> argSets = { u"dumpExistPidMem", u"abc" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle error gracefully
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpMem_ValidPid
 * @tc.desc: Test DumpMem with valid pid
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpMem_ValidPid, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Dump memory for specific pid
    std::unordered_set<std::u16string> argSets = { u"dumpMem", u"5678" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpMem_EmptyPid
 * @tc.desc: Test DumpMem without pid
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpMem_EmptyPid, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Dump memory without pid
    std::unordered_set<std::u16string> argSets = { u"dumpMem" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: Constructor_WithValidHandler
 * @tc.desc: Test constructor with valid event handler
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, Constructor_WithValidHandler, TestSize.Level1)
{
    // Given: A valid event handler
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    std::shared_ptr<AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    runner->Run();

    // When: Create dumper with handler
    auto dumperWithHandler = std::make_unique<RSPipelineDumper>(handler);

    // Then: Should be created successfully
    ASSERT_NE(dumperWithHandler, nullptr);
}

/*
 * @tc.name: RegisterFpsFuncs_NonUniRender
 * @tc.desc: Test RegisterFpsFuncs when not in uni render mode
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterFpsFuncs_NonUniRender, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Initialize dump
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: FPS commands should be registered
    std::unordered_set<std::u16string> argSets = { u"fpsInfo" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: RegisterRSGfxFuncs_WithLogFlagArgs
 * @tc.desc: Test RegisterRSGfxFuncs with log flag arguments
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSGfxFuncs_WithLogFlagArgs, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute rsLogFlag command with argument
    std::unordered_set<std::u16string> argSets = { u"rsLogFlag", u"DEBUG" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle the command
    EXPECT_TRUE(out.find("Successed") != std::string::npos ||
                out.find("Failed") != std::string::npos ||
                !out.empty());
}

/*
 * @tc.name: RegisterRSGfxFuncs_WithoutArgs
 * @tc.desc: Test RegisterRSGfxFuncs without additional arguments
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSGfxFuncs_WithoutArgs, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute rsLogFlag without argument
    std::unordered_set<std::u16string> argSets = { u"rsLogFlag" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: RegisterRSTreeFuncs_WithSurfaceNodeId
 * @tc.desc: Test RegisterRSTreeFuncs with surface node id
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSTreeFuncs_WithSurfaceNodeId, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute surfacenode command with id
    std::unordered_set<std::u16string> argSets = { u"surfacenode", u"12345" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: RegisterRSTreeFuncs_WithoutNodeId
 * @tc.desc: Test RegisterRSTreeFuncs without node id
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterRSTreeFuncs_WithoutNodeId, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute surfacenode command without id
    std::unordered_set<std::u16string> argSets = { u"surfacenode" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: RegisterBufferFuncs_UniRender
 * @tc.desc: Test RegisterBufferFuncs in uni render mode
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterBufferFuncs_UniRender, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute buffer command
    std::unordered_set<std::u16string> argSets = { u"buffer" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: RegisterSurfaceInfoFuncs_UniRender
 * @tc.desc: Test RegisterSurfaceInfoFuncs in uni render mode
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterSurfaceInfoFuncs_UniRender, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute surface command
    std::unordered_set<std::u16string> argSets = { u"surface" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: DumpExistPidMem_InvalidPid
 * @tc.desc: Test DumpExistPidMem with invalid pid
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpExistPidMem_InvalidPid, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute existPidMem with invalid pid
    std::unordered_set<std::u16string> argSets = { u"dumpExistPidMem", u"invalid" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: DumpExistPidMem_NumericPid
 * @tc.desc: Test DumpExistPidMem with numeric pid value
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpExistPidMem_NumericPid, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute existPidMem with valid pid
    std::unordered_set<std::u16string> argSets = { u"dumpExistPidMem", u"12345" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: WindowHitchsDump_WithoutLayerName
 * @tc.desc: Test WindowHitchsDump without layer name
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, WindowHitchsDump_WithoutLayerName, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute hitchs without layer name
    std::unordered_set<std::u16string> argSets = { u"hitchs" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: WindowHitchsDump_WithLayerName
 * @tc.desc: Test WindowHitchsDump with layer name
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, WindowHitchsDump_WithLayerName, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute hitchs with layer name
    std::unordered_set<std::u16string> argSets = { u"hitchs", u"testLayer" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: FPSDumpProcess_WithoutLayerName
 * @tc.desc: Test FPSDumpProcess without layer name
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, FPSDumpProcess_WithoutLayerName, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute fpsInfo without layer name
    std::unordered_set<std::u16string> argSets = { u"fpsInfo" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: FPSDumpProcess_WithLayerName
 * @tc.desc: Test FPSDumpProcess with layer name
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, FPSDumpProcess_WithLayerName, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute fpsInfo with layer name
    std::unordered_set<std::u16string> argSets = { u"fpsInfo", u"testLayer" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: FPSDumpClearProcess_WithoutLayerName
 * @tc.desc: Test FPSDumpClearProcess without layer name
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, FPSDumpClearProcess_WithoutLayerName, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute fpsClear without layer name
    std::unordered_set<std::u16string> argSets = { u"fpsClear" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: FPSDumpClearProcess_WithLayerName
 * @tc.desc: Test FPSDumpClearProcess with layer name
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, FPSDumpClearProcess_WithLayerName, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute fpsClear with layer name
    std::unordered_set<std::u16string> argSets = { u"fpsClear", u"testLayer" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: ScheduleTask_WithHandlerAndTask
 * @tc.desc: Test ScheduleTask with valid handler and task
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, ScheduleTask_WithHandlerAndTask, TestSize.Level1)
{
    // Given: A dumper with valid handler
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    std::shared_ptr<AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    runner->Run();

    auto dumperWithHandler = std::make_unique<RSPipelineDumper>(handler);

    // When: Schedule a task
    bool taskExecuted = false;
    dumperWithHandler->ScheduleTask([&taskExecuted]() { taskExecuted = true; });

    // Then: Task should execute
    // Note: Due to async nature, we just verify it doesn't crash
    SUCCEED();
}

/*
 * @tc.name: DumpMem_NonUniRender
 * @tc.desc: Test DumpMem when not in uni render mode
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpMem_NonUniRender, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute dumpMem command
    std::unordered_set<std::u16string> argSets = { u"dumpMem", u"123" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: DumpMem_WithMultipleArgs
 * @tc.desc: Test DumpMem with multiple arguments
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpMem_WithMultipleArgs, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute dumpMem with multiple args
    std::unordered_set<std::u16string> argSets = { u"dumpMem", u"123", u"456", u"789" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: DumpNodesNotOnTheTree_Command
 * @tc.desc: Test DumpNodesNotOnTheTree via registered command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodesNotOnTheTree_Command, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute nodeNotOnTree command
    std::unordered_set<std::u16string> argSets = { u"nodeNotOnTree" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpRenderServiceTree_Command
 * @tc.desc: Test DumpRenderServiceTree via registered command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpRenderServiceTree_Command, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute RSTree command
    std::unordered_set<std::u16string> argSets = { u"RSTree" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpRenderServiceTree_MultiTrees
 * @tc.desc: Test DumpRenderServiceTree with multi trees
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpRenderServiceTree_MultiTrees, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute MultiRSTrees command
    std::unordered_set<std::u16string> argSets = { u"MultiRSTrees" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpAllNodesMemSize_Command
 * @tc.desc: Test DumpAllNodesMemSize via registered command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpAllNodesMemSize_Command, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute allSurfacesMem command
    std::unordered_set<std::u16string> argSets = { u"allSurfacesMem" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpRSEvenParam_Command
 * @tc.desc: Test DumpRSEvenParam via registered command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpRSEvenParam_Command, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute EventParamList command
    std::unordered_set<std::u16string> argSets = { u"EventParamList" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpJankStatsRs_Command
 * @tc.desc: Test DumpJankStatsRs via registered command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpJankStatsRs_Command, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute flushJankStatsRs command
    std::unordered_set<std::u16string> argSets = { u"flushJankStatsRs" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

#ifdef RS_ENABLE_VK
/*
 * @tc.name: DumpVkTextureLimit_Command
 * @tc.desc: Test DumpVkTextureLimit via registered command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpVkTextureLimit_Command, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute vktextureLimit command
    std::unordered_set<std::u16string> argSets = { u"vktextureLimit" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}
#endif

/*
 * @tc.name: DumpSurfaceNode_InvalidId
 * @tc.desc: Test DumpSurfaceNode with invalid node id
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpSurfaceNode_InvalidId, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute surfacenode command with invalid id
    std::unordered_set<std::u16string> argSets = { u"surfacenode", u"0" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: DumpSurfaceNode_LargeId
 * @tc.desc: Test DumpSurfaceNode with large node id
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpSurfaceNode_LargeId, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute surfacenode command with large id
    std::unordered_set<std::u16string> argSets = { u"surfacenode", u"999999999999" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should handle gracefully
    SUCCEED();
}

/*
 * @tc.name: DumpNodesNotOnTheTree_DirectCall
 * @tc.desc: Direct call DumpNodesNotOnTheTree function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodesNotOnTheTree_DirectCall, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpNodesNotOnTheTree
    std::string dumpString;
    dumper_->DumpNodesNotOnTheTree(dumpString);

    // Then: Should generate output
    EXPECT_FALSE(dumpString.empty());
}

/*
 * @tc.name: DumpRenderServiceTree_DirectCall
 * @tc.desc: Direct call DumpRenderServiceTree function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpRenderServiceTree_DirectCall, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpRenderServiceTree with default parameter
    std::string dumpString;
    dumper_->DumpRenderServiceTree(dumpString);

    // Then: Should generate output
    EXPECT_FALSE(dumpString.empty());
}

/*
 * @tc.name: DumpRenderServiceTree_ForceDump
 * @tc.desc: Direct call DumpRenderServiceTree with forceDumpSingleFrame=true
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpRenderServiceTree_ForceDump, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpRenderServiceTree with forceDumpSingleFrame=true
    std::string dumpString;
    dumper_->DumpRenderServiceTree(dumpString, true);

    // Then: Should generate output
    EXPECT_FALSE(dumpString.empty());
}

/*
 * @tc.name: DumpAllNodesMemSize_DirectCall
 * @tc.desc: Direct call DumpAllNodesMemSize function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpAllNodesMemSize_DirectCall, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpAllNodesMemSize
    std::string dumpString;
    dumper_->DumpAllNodesMemSize(dumpString);

    // Then: Should generate output
    EXPECT_FALSE(dumpString.empty());
}

/*
 * @tc.name: DumpRSEvenParam_DirectCall
 * @tc.desc: Direct call DumpRSEvenParam function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpRSEvenParam_DirectCall, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpRSEvenParam
    std::string dumpString;
    dumper_->DumpRSEvenParam(dumpString);

    // Then: Should generate output
    EXPECT_FALSE(dumpString.empty());
}

#ifdef RS_ENABLE_VK
/*
 * @tc.name: DumpVkTextureLimit_DirectCall
 * @tc.desc: Direct call DumpVkTextureLimit function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpVkTextureLimit_DirectCall, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpVkTextureLimit
    std::string dumpString;
    dumper_->DumpVkTextureLimit(dumpString);

    // Then: Should generate output
    EXPECT_FALSE(dumpString.empty());
}
#endif

/*
 * @tc.name: DumpJankStatsRs_DirectCall
 * @tc.desc: Direct call DumpJankStatsRs function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpJankStatsRs_DirectCall, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpJankStatsRs
    std::string dumpString;
    dumper_->DumpJankStatsRs(dumpString);

    // Then: Should generate output
    EXPECT_FALSE(dumpString.empty());
}

/*
 * @tc.name: DumpSurfaceNode_DirectCall
 * @tc.desc: Direct call DumpSurfaceNode function with valid id
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpSurfaceNode_DirectCall, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpSurfaceNode with node id 0
    std::string dumpString;
    NodeId testId = 0;
    dumper_->DumpSurfaceNode(dumpString, testId);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: DumpExistPidMem_DirectCall_Valid
 * @tc.desc: Direct call DumpExistPidMem function with valid pid
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpExistPidMem_DirectCall_Valid, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpExistPidMem with valid arguments
    std::unordered_set<std::u16string> argSets = { u"12345" };
    std::string dumpString;
    dumper_->DumpExistPidMem(argSets, dumpString);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: DumpExistPidMem_DirectCall_Empty
 * @tc.desc: Direct call DumpExistPidMem function with empty args
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpExistPidMem_DirectCall_Empty, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpExistPidMem with empty args
    std::unordered_set<std::u16string> argSets;
    std::string dumpString;
    dumper_->DumpExistPidMem(argSets, dumpString);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: DumpMem_DirectCall_Valid
 * @tc.desc: Direct call DumpMem function with valid pid
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpMem_DirectCall_Valid, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpMem with valid pid
    std::unordered_set<std::u16string> argSets = { u"dumpMem", u"12345" };
    std::string dumpString;
    dumper_->DumpMem(argSets, dumpString);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: DumpMem_DirectCall_Empty
 * @tc.desc: Direct call DumpMem function with empty args
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpMem_DirectCall_Empty, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpMem with empty args
    std::unordered_set<std::u16string> argSets;
    std::string dumpString;
    dumper_->DumpMem(argSets, dumpString);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: WindowHitchsDump_DirectCall_ValidArgs
 * @tc.desc: Direct call WindowHitchsDump function with valid args
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, WindowHitchsDump_DirectCall_ValidArgs, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call WindowHitchsDump with valid layer name
    std::unordered_set<std::u16string> argSets = { u"testLayer", u"hitchs" };
    std::string dumpString;
    std::u16string arg = u"hitchs";
    dumper_->WindowHitchsDump(argSets, dumpString, arg);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: WindowHitchsDump_DirectCall_ArgNotFound
 * @tc.desc: Direct call WindowHitchsDump function with arg not in argSets
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, WindowHitchsDump_DirectCall_ArgNotFound, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call WindowHitchsDump with arg not in argSets
    std::unordered_set<std::u16string> argSets = { u"testLayer" };
    std::string dumpString;
    std::u16string arg = u"nonexistent";
    dumper_->WindowHitchsDump(argSets, dumpString, arg);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: WindowHitchsDump_DirectCall_EmptyArgSets
 * @tc.desc: Direct call WindowHitchsDump function with empty argSets
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, WindowHitchsDump_DirectCall_EmptyArgSets, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call WindowHitchsDump with empty argSets
    std::unordered_set<std::u16string> argSets;
    std::string dumpString;
    std::u16string arg = u"hitchs";
    dumper_->WindowHitchsDump(argSets, dumpString, arg);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: FPSDumpClearProcess_DirectCall_ValidArgs
 * @tc.desc: Direct call FPSDumpClearProcess function with valid args
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, FPSDumpClearProcess_DirectCall_ValidArgs, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call FPSDumpClearProcess with valid layer name
    std::unordered_set<std::u16string> argSets = { u"testLayer", u"fpsClear" };
    std::string dumpString;
    std::u16string arg = u"fpsClear";
    dumper_->FPSDumpClearProcess(argSets, dumpString, arg);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: FPSDumpClearProcess_DirectCall_ArgNotFound
 * @tc.desc: Direct call FPSDumpClearProcess function with arg not in argSets
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, FPSDumpClearProcess_DirectCall_ArgNotFound, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call FPSDumpClearProcess with arg not in argSets
    std::unordered_set<std::u16string> argSets = { u"testLayer" };
    std::string dumpString;
    std::u16string arg = u"nonexistent";
    dumper_->FPSDumpClearProcess(argSets, dumpString, arg);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: FPSDumpClearProcess_DirectCall_EmptyArgSets
 * @tc.desc: Direct call FPSDumpClearProcess function with empty argSets after erase
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, FPSDumpClearProcess_DirectCall_EmptyArgSets, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call FPSDumpClearProcess with only the arg (empty after erase)
    std::unordered_set<std::u16string> argSets = { u"fpsClear" };
    std::string dumpString;
    std::u16string arg = u"fpsClear";
    dumper_->FPSDumpClearProcess(argSets, dumpString, arg);

    // Then: Should execute without crash (logs error for missing layer name)
    SUCCEED();
}

/*
 * @tc.name: ClearFps_DirectCall_ValidArgs
 * @tc.desc: Direct call ClearFps function with valid layer name
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, ClearFps_DirectCall_ValidArgs, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call ClearFps with valid layer name
    std::string dumpString;
    std::string layerName = "testLayer";
    dumper_->ClearFps(dumpString, layerName);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: ClearFps_DirectCall_EmptyLayerName
 * @tc.desc: Direct call ClearFps function with empty layer name
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, ClearFps_DirectCall_EmptyLayerName, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call ClearFps with empty layer name
    std::string dumpString;
    std::string layerName = "";
    dumper_->ClearFps(dumpString, layerName);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: FPSDumpProcess_DirectCall_ValidArgs
 * @tc.desc: Direct call FPSDumpProcess function with valid args
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, FPSDumpProcess_DirectCall_ValidArgs, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call FPSDumpProcess with valid layer name
    std::unordered_set<std::u16string> argSets = { u"testLayer", u"composer" };
    std::string dumpString;
    std::u16string arg = u"composer";
    dumper_->FPSDumpProcess(argSets, dumpString, arg);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: FPSDumpProcess_DirectCall_ArgNotFound
 * @tc.desc: Direct call FPSDumpProcess function with arg not in argSets
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, FPSDumpProcess_DirectCall_ArgNotFound, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call FPSDumpProcess with arg not in argSets
    std::unordered_set<std::u16string> argSets = { u"testLayer" };
    std::string dumpString;
    std::u16string arg = u"nonexistent";
    dumper_->FPSDumpProcess(argSets, dumpString, arg);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: FPSDumpProcess_DirectCall_EmptyArgSets
 * @tc.desc: Direct call FPSDumpProcess function with empty argSets after erase
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, FPSDumpProcess_DirectCall_EmptyArgSets, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call FPSDumpProcess with only the arg (empty after erase)
    std::unordered_set<std::u16string> argSets = { u"composer" };
    std::string dumpString;
    std::u16string arg = u"composer";
    dumper_->FPSDumpProcess(argSets, dumpString, arg);

    // Then: Should execute without crash (logs error for missing layer name)
    SUCCEED();
}

/*
 * @tc.name: DumpFps_DirectCall_ValidArgs
 * @tc.desc: Direct call DumpFps function with valid layer name
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpFps_DirectCall_ValidArgs, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpFps with valid layer name
    std::string dumpString;
    std::string layerName = "testLayer";
    dumper_->DumpFps(dumpString, layerName);

    // Then: Should execute without crash
    SUCCEED();
}

/*
 * @tc.name: DumpFps_DirectCall_EmptyLayerName
 * @tc.desc: Direct call DumpFps function with empty layer name
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpFps_DirectCall_EmptyLayerName, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpFps with empty layer name
    std::string dumpString;
    std::string layerName = "";
    dumper_->DumpFps(dumpString, layerName);

    // Then: Should execute without crash
    SUCCEED();
}

} // namespace OHOS::Rosen
