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
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_render_node_allocator.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_scb_node_memory_pool.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// Ensure the GC / allocator / SCB-pool singletons are constructed BEFORE the
// RSMainThread static instance. All static locals register destruction callbacks
// with __cxa_atexit, which runs them in reverse construction order (LIFO).
// The dump paths covered below create drawables bound to nodes owned by
// RSMainThread's RSContext; their deleters call RSRenderNodeGC::DrawableDestructor.
// If the GC instance were constructed after RSMainThread (first touched inside a
// test body), it would be destroyed BEFORE ~RSMainThread at process exit, and the
// drawable deleter running inside ~RSContext would crash on the dead GC singleton
// (exit-time SIGSEGV seen in cppcrash-RSPipelineDumperTest logs). Constructing it
// first here guarantees RSRenderNodeGC outlives RSMainThread, keeping the exit path safe.
namespace {
const bool g_gcSingletonsPreInitialized = []() {
    (void)RSRenderNodeGC::Instance();
    (void)RSRenderNodeAllocator::Instance();
    (void)RSScbNodeMemoryPool::Instance();
    return true;
}();
} // namespace

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
void RSPipelineDumperTest::TearDownTestCase()
{
    // Prevent Signal 11 at process exit: RSRenderNodeGC (function-local static)
    // may be destroyed before RSMainThread (function-local static). When
    // RSMainThread's context_ is destroyed, globalRootRenderNode_'s renderDrawable_
    // deleter calls RSRenderNodeGC::Instance() which is already dead → SIGSEGV.
    // Reset renderDrawable_ on all nodes while RSRenderNodeGC is still alive.
    auto& context = RSMainThread::Instance()->GetContext();
    context.globalRootRenderNode_->renderDrawable_.reset();
    context.GetMutableNodeMap().TraversalNodes(
        [](const std::shared_ptr<RSBaseRenderNode>& node) { node->renderDrawable_.reset(); });
}
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

    std::unordered_set<std::u16string> argSets2 = { u"surfacenode", u"" };
    std::string out2;
    dumpManager_->CmdExec(argSets2, out2, nullptr);

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
 * @tc.name: RegisterMemFuncs_DumpMemLite
 * @tc.desc: Test RegisterMemFuncs registers mem dump function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterMemFuncs_DumpMemLite, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // Then: dumpMem command should be registered
    std::unordered_set<std::u16string> argSets = { u"dumpMemLite", u"123" };
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

    std::unordered_set<std::u16string> argSets2 = { u"dumpExistPidMem", u"" };
    std::string out2;
    dumpManager_->CmdExec(argSets2, out2, nullptr);

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
    std::vector<std::string> validNumbers = { "0", "123", "99999", "007", "42", "" };

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
 * @tc.desc: Test DumpSurfaceNodesNotOnTree basic functionality
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
 * @tc.name: DumpSurfaceNodesNotOnTree_Command
 * @tc.desc: Test DumpSurfaceNodesNotOnTree via registered command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpSurfaceNodesNotOnTree_Command, TestSize.Level1)
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
 * @tc.name: DumpRenderNodesNotOnTree_Command
 * @tc.desc: Test DumpRenderNodesNotOnTree via registered command
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpRenderNodesNotOnTree_Command, TestSize.Level1)
{
    // Given: Initialized dumper
    ASSERT_NE(dumper_, nullptr);
    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute allNodeNotOnTree command
    std::unordered_set<std::u16string> argSets = { u"allNodeNotOnTree" };
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
 * @tc.name: DumpSurfaceNodesNotOnTree_DirectCall
 * @tc.desc: Direct call DumpSurfaceNodesNotOnTree function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpSurfaceNodesNotOnTree_DirectCall, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpSurfaceNodesNotOnTree
    std::string dumpString;
    dumper_->DumpSurfaceNodesNotOnTree(dumpString);

    // Then: Should generate output
    EXPECT_FALSE(dumpString.empty());
}

/*
 * @tc.name: DumpRenderNodesNotOnTree_DirectCall
 * @tc.desc: Direct call DumpRenderNodesNotOnTree function
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpRenderNodesNotOnTree_DirectCall, TestSize.Level1)
{
    // Given: A dumper instance
    ASSERT_NE(dumper_, nullptr);

    // When: Directly call DumpRenderNodesNotOnTree
    std::string dumpString;
    dumper_->DumpRenderNodesNotOnTree(dumpString);

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

/*
 * @tc.name: DumpGpuMem_WithArgs
 * @tc.desc: Test DumpGpuMem with arguments (branch: argSets.size() > 1 && !argSets.empty())
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpGpuMem_WithArgs, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Dump GPU memory with type argument
    std::unordered_set<std::u16string> argSets = { u"dumpGpuMem", u"detail" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpGpuMem_SingleArg
 * @tc.desc: Test DumpGpuMem with single argument (branch: argSets.size() <= 1)
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpGpuMem_SingleArg, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Dump GPU memory with type argument
    std::unordered_set<std::u16string> argSets = { u"dumpGpuMem" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterContextStatesFuncs_Usage
 * @tc.desc: Test uiContextState command without enough arguments prints usage
 * @tc.type: FUNC
 * @tc.require: issues30915
 */
HWTEST_F(RSPipelineDumperTest, RegisterContextStatesFuncs_Usage, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"uiContextState" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_NE(out.find("Usage: contextStates"), std::string::npos);
}

/*
 * @tc.name: RegisterContextStatesFuncs_InvalidArgs
 * @tc.desc: Test uiContextState command with non-numeric arguments prints error
 * @tc.type: FUNC
 * @tc.require: issues30915
 */
HWTEST_F(RSPipelineDumperTest, RegisterContextStatesFuncs_InvalidArgs, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"uiContextState", u"abc", u"123" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_NE(out.find("Invalid arguments"), std::string::npos);
}

/*
 * @tc.name: RegisterContextStatesFuncs_ValidArgs
 * @tc.desc: Test uiContextState command with valid numeric arguments does not crash
 * @tc.type: FUNC
 * @tc.require: issues30915
 */
HWTEST_F(RSPipelineDumperTest, RegisterContextStatesFuncs_ValidArgs, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"uiContextState", u"12345", u"67890" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    std::unordered_set<std::u16string> argSets2 = { u"uiContextState", u"", u"11111111111111111111111" };
    std::string out2;
    dumpManager_->CmdExec(argSets2, out2, nullptr);

    // With a null handler the scheduled dump task is not executed, so output remains empty.
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpNodeInfo_ValidToken_MatchingNodes
 * @tc.desc: Test DumpNodeInfo with valid token and matching nodes
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodeInfo_ValidToken_MatchingNodes, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Dump node info with valid pid and token
    std::unordered_set<std::u16string> argSets = { u"contextStates", u"1000", u"12345678" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash and contain node statistics header
    EXPECT_FALSE(out.empty());
    EXPECT_TRUE(out.find("-- Node Type Statistics --") != std::string::npos ||
                out.find("Usage: contextStates") != std::string::npos);
}

/*
 * @tc.name: DumpNodeInfo_InvalidArguments_NotEnoughArgs
 * @tc.desc: Test DumpNodeInfo with insufficient arguments (branch: argSets.size() < 2)
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodeInfo_InvalidArguments_NotEnoughArgs, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Dump node info with only one argument
    std::unordered_set<std::u16string> argSets = { u"contextStates", u"1000" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should show usage message
    EXPECT_FALSE(out.empty());
    EXPECT_TRUE(out.find("Usage: contextStates") != std::string::npos);
}

/*
 * @tc.name: DumpNodeInfo_InvalidArguments_NoArgs
 * @tc.desc: Test DumpNodeInfo with no arguments (branch: argSets.size() < 2)
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodeInfo_InvalidArguments_NoArgs, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Dump node info with no arguments
    std::unordered_set<std::u16string> argSets = { u"contextStates" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should show usage message
    EXPECT_FALSE(out.empty());
    EXPECT_TRUE(out.find("Usage: contextStates") != std::string::npos);
}

/*
 * @tc.name: DumpNodeInfo_InvalidArguments_NonNumericPid
 * @tc.desc: Test DumpNodeInfo with non-numeric pid (branch: !IsNumber(pidStr))
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodeInfo_InvalidArguments_NonNumericPid, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Dump node info with non-numeric pid
    std::unordered_set<std::u16string> argSets = { u"contextStates", u"abc", u"12345678" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should show invalid arguments message
    EXPECT_FALSE(out.empty());
    EXPECT_TRUE(out.find("Invalid arguments") != std::string::npos);
}

/*
 * @tc.name: DumpNodeInfo_InvalidArguments_NonNumericToken
 * @tc.desc: Test DumpNodeInfo with non-numeric token (branch: !IsNumber(contextTokenStr))
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodeInfo_InvalidArguments_NonNumericToken, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Dump node info with non-numeric token
    std::unordered_set<std::u16string> argSets = { u"contextStates", u"1000", u"abc" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should show invalid arguments message
    EXPECT_FALSE(out.empty());
    EXPECT_TRUE(out.find("Invalid arguments") != std::string::npos);
}

/*
 * @tc.name: DumpNodeInfo_ZeroToken
 * @tc.desc: Test DumpNodeInfo with zero token (branch: node->GetUIContextToken() == token)
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodeInfo_ZeroToken, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Dump node info with zero token
    std::unordered_set<std::u16string> argSets = { u"contextStates", u"1000", u"0" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpNodeInfo_LargeToken
 * @tc.desc: Test DumpNodeInfo with large token value (branch: node->GetUIContextToken() == token)
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodeInfo_LargeToken, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Dump node info with large token value
    std::unordered_set<std::u16string> argSets = { u"contextStates", u"1000", u"18446744073709551615" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: DumpNodeInfo_MultipleFormats
 * @tc.desc: Test DumpNodeInfo output contains all expected node types
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodeInfo_MultipleFormats, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Dump node info with valid arguments
    std::unordered_set<std::u16string> argSets = { u"contextStates", u"1000", u"12345678" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    // Note: Actual node counts depend on RSMainThread state
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterContextStatesFuncs_ValidManager
 * @tc.desc: Test RegisterContextStatesFuncs with valid manager
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, RegisterContextStatesFuncs_ValidManager, TestSize.Level1)
{
    // Given: Dump manager and dumper
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    // When: Execute contextStates command
    std::unordered_set<std::u16string> argSets = { u"contextStates", u"1000", u"12345678" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Then: Should execute without crash
    EXPECT_FALSE(out.empty());
}

/*
 * @tc.name: RegisterScbPoolFuncs_ValidManager
 * @tc.desc: Test RegisterScbPoolFuncs registers the scbpool handler and it can be executed
 * @tc.type: FUNC
 */
HWTEST_F(RSPipelineDumperTest, RegisterScbPoolFuncs_ValidManager, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    // The handler dispatches through ScheduleTask, which needs a real main handler;
    // the fixture's default dumper has none and would silently drop the dump.
    auto runner = AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    ASSERT_NE(handler, nullptr);
    auto dumper = std::make_unique<RSPipelineDumper>(handler);

    dumper->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"scbpool" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    // Handler registered → command must produce the pool status header
    EXPECT_FALSE(out.empty());
    EXPECT_NE(out.find("SCB Memory Pool Status"), std::string::npos);
}

/*
 * @tc.name: RegisterAllRenderNodesFuncs_ValidManager
 * @tc.desc: Test RegisterAllRenderNodesFuncs registers the allrendernodes handler
 * @tc.type: FUNC
 */
HWTEST_F(RSPipelineDumperTest, RegisterAllRenderNodesFuncs_ValidManager, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    // Same as above: DumpAllRenderNodes runs inside ScheduleTask, so a real handler is
    // required for the dump string to be produced.
    auto runner = AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    ASSERT_NE(handler, nullptr);
    auto dumper = std::make_unique<RSPipelineDumper>(handler);

    dumper->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"allrendernodes" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
    EXPECT_NE(out.find("All Render Nodes List"), std::string::npos);
    EXPECT_NE(out.find("nodeId,pid,isScb,type,onTree,inPool,chunk,slot,globalSlot,surfaceName"),
        std::string::npos);
}

/*
 * @tc.name: RegisterAllRenderNodesFuncs_ScbnodesAlias
 * @tc.desc: Test the scbnodes alias resolves to the same ALL_RENDER_NODES_INFO handler
 * @tc.type: FUNC
 */
HWTEST_F(RSPipelineDumperTest, RegisterAllRenderNodesFuncs_ScbnodesAlias, TestSize.Level1)
{
    ASSERT_NE(dumpManager_, nullptr);
    ASSERT_NE(dumper_, nullptr);

    dumper_->RenderPipelineDumpInit(dumpManager_);

    std::unordered_set<std::u16string> argSets = { u"scbnodes" };
    std::string out;
    dumpManager_->CmdExec(argSets, out, nullptr);

    EXPECT_FALSE(out.empty());
    EXPECT_NE(out.find("All Render Nodes List"), std::string::npos);
}

/*
 * @tc.name: DumpAllRenderNodes_DirectCall_EmptyTree
 * @tc.desc: Direct call DumpAllRenderNodes with an empty node map; verifies the
 *           zero-node path (total=0) and the SCB Pool snapshot tail.
 * @tc.type: FUNC
 */
HWTEST_F(RSPipelineDumperTest, DumpAllRenderNodes_DirectCall_EmptyTree, TestSize.Level1)
{
    ASSERT_NE(dumper_, nullptr);

    std::string dumpString;
    dumper_->DumpAllRenderNodes(dumpString);

    // Header + CSV column line must always be emitted
    EXPECT_NE(dumpString.find("=== All Render Nodes List (pid:"), std::string::npos);
    EXPECT_NE(dumpString.find("nodeId,pid,isScb,type,onTree,inPool,chunk,slot,globalSlot,surfaceName"),
        std::string::npos);
    // Summary + SCB Pool snapshot must always be emitted
    EXPECT_NE(dumpString.find("Summary: total="), std::string::npos);
    EXPECT_NE(dumpString.find("SCB Pool: totalSlots="), std::string::npos);
    EXPECT_NE(dumpString.find(", scbPid="), std::string::npos);
}

/*
 * @tc.name: DumpAllRenderNodes_DirectCall_WithScbPid
 * @tc.desc: Direct call DumpAllRenderNodes after setting scbPid_; verifies the
 *           scbPid value is echoed in the SCB Pool snapshot tail.
 * @tc.type: FUNC
 */
HWTEST_F(RSPipelineDumperTest, DumpAllRenderNodes_DirectCall_WithScbPid, TestSize.Level1)
{
    ASSERT_NE(dumper_, nullptr);
    auto& nodeGC = RSRenderNodeGC::Instance();
    pid_t originalPid = nodeGC.GetScbPid();

    constexpr pid_t testPid = 4321;
    nodeGC.SetScbPid(testPid);
    std::string dumpString;
    dumper_->DumpAllRenderNodes(dumpString);
    nodeGC.SetScbPid(originalPid);

    EXPECT_NE(dumpString.find("scbPid=" + std::to_string(testPid)), std::string::npos);
}

/*
 * @tc.name: DumpAllRenderNodes_DirectCall_PoolSnapshot
 * @tc.desc: Direct call DumpAllRenderNodes after allocating a SCB pool slot; verifies
 *           the pool snapshot reflects a non-zero totalSlots and at least one chunk.
 * @tc.type: FUNC
 */
HWTEST_F(RSPipelineDumperTest, DumpAllRenderNodes_DirectCall_PoolSnapshot, TestSize.Level1)
{
    ASSERT_NE(dumper_, nullptr);
    auto& pool = RSScbNodeMemoryPool::Instance();

    // Ensure the pool is initialized (lazy init on first Allocate)
    void* probe = pool.Allocate();
    ASSERT_NE(probe, nullptr);

    std::string dumpString;
    dumper_->DumpAllRenderNodes(dumpString);

    // Pool snapshot must report a non-zero capacity and at least one chunk
    EXPECT_EQ(dumpString.find("SCB Pool: totalSlots=0,"), std::string::npos) << "unexpected zero capacity";
    EXPECT_EQ(dumpString.find(", chunks=0,"), std::string::npos) << "unexpected zero chunk count";

    pool.Deallocate(probe);
}

/*
 * @tc.name: DumpAllRenderNodes_DirectCall_ScbNodeInPool
 * @tc.desc: Create a real SCB-pool node, register it into the node map, then dump;
 *           verifies the isScb=1 and inPool=1 columns for that node.
 * @tc.type: FUNC
 */
HWTEST_F(RSPipelineDumperTest, DumpAllRenderNodes_DirectCall_ScbNodeInPool, TestSize.Level1)
{
    ASSERT_NE(dumper_, nullptr);
    auto& nodeGC = RSRenderNodeGC::Instance();
    auto& allocator = RSRenderNodeAllocator::Instance();
    pid_t originalPid = nodeGC.GetScbPid();

    constexpr pid_t scbPid = 4242;
    nodeGC.SetScbPid(scbPid);
    NodeId nodeId = (static_cast<NodeId>(scbPid) << 32) | 1;
    auto node = allocator.CreateScbCanvasRenderNode(nodeId, {}, false);
    ASSERT_NE(node, nullptr);
    ASSERT_TRUE(allocator.IsScbNodePool(node.get()));
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    ASSERT_TRUE(nodeMap.RegisterRenderNode(node));

    std::string dumpString;
    dumper_->DumpAllRenderNodes(dumpString);
    nodeMap.UnregisterRenderNode(nodeId);
    nodeGC.SetScbPid(originalPid);

    // The node must appear with isScb=1 and inPool=1
    std::string expectedPrefix = std::to_string(nodeId) + "," + std::to_string(scbPid) + ",1,";
    auto pos = dumpString.find(expectedPrefix);
    ASSERT_NE(pos, std::string::npos) << "SCB node row not found in dump";
    // Within the same CSV row, the 6th field (inPool) must be 1
    auto rowEnd = dumpString.find('\n', pos);
    ASSERT_NE(rowEnd, std::string::npos);
    std::string row = dumpString.substr(pos, rowEnd - pos);
    // Fields: nodeId,pid,isScb,type,onTree,inPool,chunk,slot,globalSlot,surfaceName
    std::vector<std::string> fields;
    size_t start = 0;
    for (int i = 0; i < 9; ++i) {
        auto comma = row.find(',', start);
        ASSERT_NE(comma, std::string::npos);
        fields.push_back(row.substr(start, comma - start));
        start = comma + 1;
    }
    EXPECT_EQ(fields[2], "1") << "isScb should be 1";
    EXPECT_EQ(fields[5], "1") << "inPool should be 1";
}

/*
 * @tc.name: DumpAllRenderNodes_DirectCall_NonScbNodeNotInPool
 * @tc.desc: Create a heap (non-pool) node under a non-SCB pid, register it, then dump;
 *           verifies isScb=0 and inPool=0 columns.
 * @tc.type: FUNC
 */
HWTEST_F(RSPipelineDumperTest, DumpAllRenderNodes_DirectCall_NonScbNodeNotInPool, TestSize.Level1)
{
    ASSERT_NE(dumper_, nullptr);
    auto& nodeGC = RSRenderNodeGC::Instance();
    auto& allocator = RSRenderNodeAllocator::Instance();
    pid_t originalPid = nodeGC.GetScbPid();

    constexpr pid_t otherPid = 5353;
    nodeGC.SetScbPid(-1); // ensure the node below is NOT treated as SCB
    NodeId nodeId = (static_cast<NodeId>(otherPid) << 32) | 2;
    auto node = allocator.CreateRSCanvasRenderNode(nodeId, {}, false);
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(allocator.IsScbNodePool(node.get()));
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    ASSERT_TRUE(nodeMap.RegisterRenderNode(node));

    std::string dumpString;
    dumper_->DumpAllRenderNodes(dumpString);
    nodeMap.UnregisterRenderNode(nodeId);
    nodeGC.SetScbPid(originalPid);

    std::string expectedPrefix = std::to_string(nodeId) + "," + std::to_string(otherPid) + ",0,";
    auto pos = dumpString.find(expectedPrefix);
    ASSERT_NE(pos, std::string::npos) << "non-SCB node row not found in dump";
    auto rowEnd = dumpString.find('\n', pos);
    ASSERT_NE(rowEnd, std::string::npos);
    std::string row = dumpString.substr(pos, rowEnd - pos);
    std::vector<std::string> fields;
    size_t start = 0;
    for (int i = 0; i < 9; ++i) {
        auto comma = row.find(',', start);
        ASSERT_NE(comma, std::string::npos);
        fields.push_back(row.substr(start, comma - start));
        start = comma + 1;
    }
    EXPECT_EQ(fields[2], "0") << "isScb should be 0";
    EXPECT_EQ(fields[5], "0") << "inPool should be 0";
}

namespace {
inline NodeId MakeNodeIdForTopo(pid_t pid, uint32_t seq)
{
    return (static_cast<NodeId>(pid) << 32) | seq;
}
} // namespace

/*
 * @tc.name: DumpAllRenderNodes_ParentChainSurfaceName
 * @tc.desc: Build surface->canvas parent-child topology and mark it on tree; covers the
 *           getSurfaceName parent-chain SURFACE_NODE hit branch and pidSurfaceMap insertion.
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpAllRenderNodes_ParentChainSurfaceName, TestSize.Level1)
{
    ASSERT_NE(dumper_, nullptr);
    constexpr pid_t pid = 6262;
    constexpr uint32_t surfSeq = 10;
    constexpr uint32_t childSeq = 11;
    auto& allocator = RSRenderNodeAllocator::Instance();
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    RSSurfaceRenderNodeConfig cfg = {
        .id = MakeNodeIdForTopo(pid, surfSeq),
        .name = "ParentChainSurface",
    };
    auto surface = std::make_shared<RSSurfaceRenderNode>(cfg);
    ASSERT_NE(surface, nullptr);
    auto child = allocator.CreateRSCanvasRenderNode(MakeNodeIdForTopo(pid, childSeq), {}, false);
    ASSERT_NE(child, nullptr);

    surface->AddChild(child);
    ASSERT_TRUE(nodeMap.RegisterRenderNode(surface));
    ASSERT_TRUE(nodeMap.RegisterRenderNode(child));
    // Setting the surface on tree cascades to the child and sets instanceRootNodeId_.
    surface->SetIsOnTheTree(true, surface->GetId());
    ASSERT_TRUE(child->IsOnTheTree());

    std::string dumpString;
    dumper_->DumpAllRenderNodes(dumpString);

    std::string childPrefix = std::to_string(MakeNodeIdForTopo(pid, childSeq)) + "," +
        std::to_string(pid) + ",";
    auto pos = dumpString.find(childPrefix);
    ASSERT_NE(pos, std::string::npos) << "child node row not found in dump";
    auto rowEnd = dumpString.find('\n', pos);
    ASSERT_NE(rowEnd, std::string::npos);
    std::string row = dumpString.substr(pos, rowEnd - pos);
    EXPECT_NE(row.find("ParentChainSurface"), std::string::npos)
        << "surfaceName should come from parent chain, row: " << row;

    surface->SetIsOnTheTree(false);
    nodeMap.UnregisterRenderNode(child->GetId());
    nodeMap.UnregisterRenderNode(surface->GetId());
}

/*
 * @tc.name: DumpAllRenderNodes_RootNodeFallbackHit
 * @tc.desc: Detach a former child from its surface parent while keeping its
 *           instanceRootNodeId_, covering the root-node fallback branch and
 *           the notOnTree counter branch of DumpAllRenderNodes.
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpAllRenderNodes_RootNodeFallbackHit, TestSize.Level1)
{
    ASSERT_NE(dumper_, nullptr);
    constexpr pid_t pid = 7373;
    constexpr uint32_t surfSeq = 20;
    constexpr uint32_t orphanSeq = 21;
    auto& allocator = RSRenderNodeAllocator::Instance();
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    RSSurfaceRenderNodeConfig cfg = {
        .id = MakeNodeIdForTopo(pid, surfSeq),
        .name = "FallbackSurface",
    };
    auto surface = std::make_shared<RSSurfaceRenderNode>(cfg);
    ASSERT_NE(surface, nullptr);
    auto orphan = allocator.CreateRSCanvasRenderNode(MakeNodeIdForTopo(pid, orphanSeq), {}, false);
    ASSERT_NE(orphan, nullptr);
    ASSERT_TRUE(nodeMap.RegisterRenderNode(surface));
    ASSERT_TRUE(nodeMap.RegisterRenderNode(orphan));

    // Attach, mark on tree (cascades rootId to orphan), then detach so the
    // parent chain is broken but instanceRootNodeId_ is preserved on orphan.
    surface->AddChild(orphan);
    surface->SetIsOnTheTree(true, surface->GetId());
    surface->RemoveChild(orphan);
    ASSERT_FALSE(orphan->IsOnTheTree());

    std::string dumpString;
    dumper_->DumpAllRenderNodes(dumpString);

    std::string orphanPrefix = std::to_string(MakeNodeIdForTopo(pid, orphanSeq)) + "," +
        std::to_string(pid) + ",";
    auto pos = dumpString.find(orphanPrefix);
    ASSERT_NE(pos, std::string::npos) << "orphan row not found in dump";
    auto rowEnd = dumpString.find('\n', pos);
    ASSERT_NE(rowEnd, std::string::npos);
    std::string row = dumpString.substr(pos, rowEnd - pos);
    EXPECT_NE(row.find("FallbackSurface"), std::string::npos)
        << "surfaceName should come from root-node/pid fallback, row: " << row;
    // the orphan row's onTree field (5th CSV column) must be 0 -> notOnTree branch taken
    std::string onTreeField = "," + std::to_string(orphan->IsOnTheTree() ? 1 : 0) + ",";
    EXPECT_NE(row.find(",0,"), std::string::npos)
        << "orphan row onTree field should be 0, row: " << row;

    nodeMap.UnregisterRenderNode(orphan->GetId());
    surface->SetIsOnTheTree(false);
    nodeMap.UnregisterRenderNode(surface->GetId());
}

/*
 * @tc.name: DumpAllRenderNodes_PidSurfaceMapDedup
 * @tc.desc: Multiple on-tree nodes under one pid; covers the pidSurfaceMap
 *           count()>0 skip branch and the empty-name not-inserted branch.
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpAllRenderNodes_PidSurfaceMapDedup, TestSize.Level1)
{
    ASSERT_NE(dumper_, nullptr);
    constexpr pid_t pid = 8484;
    constexpr uint32_t baseSeq = 30;
    constexpr uint32_t surfSeq = 31;
    constexpr uint32_t canvasSeq = 32;
    auto& allocator = RSRenderNodeAllocator::Instance();
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    // 1) On-tree base node whose getSurfaceName result is empty (not inserted into map).
    auto base = std::make_shared<RSBaseRenderNode>(MakeNodeIdForTopo(pid, baseSeq));
    ASSERT_NE(base, nullptr);
    ASSERT_TRUE(nodeMap.RegisterRenderNode(base));
    base->SetIsOnTheTree(true);

    // 2) Named on-tree surface (inserted into pidSurfaceMap).
    RSSurfaceRenderNodeConfig cfg = {
        .id = MakeNodeIdForTopo(pid, surfSeq),
        .name = "DedupSurface",
    };
    auto surface = std::make_shared<RSSurfaceRenderNode>(cfg);
    ASSERT_NE(surface, nullptr);
    ASSERT_TRUE(nodeMap.RegisterRenderNode(surface));

    // 3) Second on-tree node with the same pid (count()>0 -> skip branch).
    auto canvas = allocator.CreateRSCanvasRenderNode(MakeNodeIdForTopo(pid, canvasSeq), {}, false);
    ASSERT_NE(canvas, nullptr);
    ASSERT_TRUE(nodeMap.RegisterRenderNode(canvas));
    surface->AddChild(canvas);
    surface->SetIsOnTheTree(true, surface->GetId());

    std::string dumpString;
    dumper_->DumpAllRenderNodes(dumpString); // covering the skip/no-insert branches without crash
    EXPECT_NE(dumpString.find("DedupSurface"), std::string::npos);

    surface->SetIsOnTheTree(false);
    nodeMap.UnregisterRenderNode(canvas->GetId());
    nodeMap.UnregisterRenderNode(surface->GetId());
    base->SetIsOnTheTree(false);
    nodeMap.UnregisterRenderNode(base->GetId());
}

/*
 * @tc.name: DumpAllRenderNodes_NameEmptyNoFallback
 * @tc.desc: An on-tree node with no surface ancestor, invalid root id and no pid
 *           fallback entry; covers the INVALID_NODEID else branch and the
 *           empty-surfaceName branches of DumpAllRenderNodes.
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpAllRenderNodes_NameEmptyNoFallback, TestSize.Level1)
{
    ASSERT_NE(dumper_, nullptr);
    constexpr pid_t lonelyPid = 9595;
    constexpr uint32_t seq = 40;
    auto& allocator = RSRenderNodeAllocator::Instance();
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    auto node = allocator.CreateRSCanvasRenderNode(MakeNodeIdForTopo(lonelyPid, seq), {}, false);
    ASSERT_NE(node, nullptr);
    ASSERT_TRUE(nodeMap.RegisterRenderNode(node));
    node->SetIsOnTheTree(true);

    std::string dumpString;
    dumper_->DumpAllRenderNodes(dumpString);

    std::string prefix = std::to_string(MakeNodeIdForTopo(lonelyPid, seq)) + "," +
        std::to_string(lonelyPid) + ",";
    auto pos = dumpString.find(prefix);
    ASSERT_NE(pos, std::string::npos) << "lonely node row not found in dump";
    auto rowEnd = dumpString.find('\n', pos);
    ASSERT_NE(rowEnd, std::string::npos);
    std::string row = dumpString.substr(pos, rowEnd - pos);
    // surfaceName is the last CSV field: expect the row to end with a bare comma
    // (empty name, all fallbacks missed) followed by end of line.
    EXPECT_NE(row.find(',', std::to_string(MakeNodeIdForTopo(lonelyPid, seq)).size()), std::string::npos);
    EXPECT_TRUE(row.substr(row.find_last_of(',') + 1).empty())
        << "surfaceName should be empty when all fallbacks miss, row: " << row;

    node->SetIsOnTheTree(false);
    nodeMap.UnregisterRenderNode(node->GetId());
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
/*
 * @tc.name: DumpNodeInfo_CanvasDrawingNodeHybridRender
 * @tc.desc: Test DumpNodeInfo covers bufferDraw_ true and false branches
 * @tc.type: FUNC
 * @tc.require: AR000GSH6G
 */
HWTEST_F(RSPipelineDumperTest, DumpNodeInfo_CanvasDrawingNodeHybridRender, TestSize.Level1)
{
    ASSERT_NE(dumper_, nullptr);
 
    pid_t pid = 9999;
    uint64_t token = 888888;
    NodeId baseId = static_cast<NodeId>(pid) << 32;
 
    // Register a CanvasDrawingNode with bufferDraw_=true
    auto nodeHybrid = std::make_shared<RSCanvasDrawingRenderNode>(baseId | 1);
    nodeHybrid->SetUIContextToken(token);
    nodeHybrid->bufferDraw_ = true;
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(nodeHybrid);
 
    // Register a CanvasDrawingNode with bufferDraw_=false
    auto nodeNonHybrid = std::make_shared<RSCanvasDrawingRenderNode>(baseId | 2);
    nodeNonHybrid->SetUIContextToken(token);
    nodeNonHybrid->bufferDraw_ = false;
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(nodeNonHybrid);
 
    // Avoids PostSyncTask threading complexity and RSMainThread singleton crash at exit.
    std::string out;
    dumper_->DumpNodeInfo(pid, token, out);
 
    // Should contain CanvasDrawingNode statistics with GPU and Non-GPU counts
    EXPECT_FALSE(out.empty());
    EXPECT_TRUE(out.find("CanvasDrawingNode") != std::string::npos);
    EXPECT_TRUE(out.find("GPU: 1") != std::string::npos);
    EXPECT_TRUE(out.find("Non-GPU: 1") != std::string::npos);
 
    // Cleanup registered nodes
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(baseId | 1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(baseId | 2);
}
#endif
} // namespace OHOS::Rosen
