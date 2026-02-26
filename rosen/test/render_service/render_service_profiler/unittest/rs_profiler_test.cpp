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

#include "gtest/gtest.h"
#include "rs_profiler.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_command.h"
#include "rs_profiler_file.h"
#include "rs_profiler_log.h"
#include "rs_profiler_network.h"
#include "rs_profiler_utils.h"

#include "common/rs_common_def.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "render_server/rs_render_service.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
namespace ModifierNG {
class TestCustomRenderModifier : public RSRenderModifier {
public:
    TestCustomRenderModifier() = default;
    ~TestCustomRenderModifier() override = default;

    RSModifierType GetType() const override
    {
        return RSModifierType::CONTENT_STYLE;
    }

    bool IsCustom() const override
    {
        return true;
    }

    Drawing::DrawCmdListPtr GetPropertyDrawCmdList() const override
    {
        return Getter<Drawing::DrawCmdListPtr>(RSPropertyType::CONTENT_STYLE, nullptr);
    }

protected:
    void OnSetDirty() override {}
};
} // namespace ModifierNG

class RSB_EXPORT TestRSCanvasDrawingRenderNode : public RSCanvasDrawingRenderNode {
public:
    TestRSCanvasDrawingRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
        : RSCanvasDrawingRenderNode(id, context, isTextureExportNode)
    {}
    ~TestRSCanvasDrawingRenderNode() override = default;

    void ApplyModifiers() override {}
    void QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor) override {}
    void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor) override {}
    void Process(const std::shared_ptr<RSNodeVisitor>& visitor) override {}
    void ProcessTransitionBeforeChildren(RSPaintFilterCanvas& canvas) override {}
    void ProcessAnimatePropertyBeforeChildren(RSPaintFilterCanvas& canvas, bool includeProperty) override {}
    void ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas) override {}
    void ProcessTransitionAfterChildren(RSPaintFilterCanvas& canvas) override {}
    void ProcessAnimatePropertyAfterChildren(RSPaintFilterCanvas& canvas) override {}
    void ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas) override {}
    void OnTreeStateChanged() override {}
    void UpdateNodeColorSpace() override {}
    void MarkNodeColorSpace(bool isP3Color) override {}
};

class RSProfilerTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override
    {
        Network::outgoing_ = {};
        Network::incoming_ = {};
    };
    void TearDown() override
    {
        RSProfiler::SetMode(Mode::NONE);
    };
};

RSRenderService* GetAndInitRenderService()
{
    MemoryTrack::Instance();
    MemorySnapshot::Instance();
    RSRenderNodeGC::Instance();

    auto renderService(new RSRenderService());
    if (renderService) {
        renderService->mainThread_ = new RSMainThread();
    }
    if (renderService->mainThread_) {
        renderService->mainThread_->context_ = std::make_shared<RSContext>();
        renderService->mainThread_->context_->Initialize();
    }
    return renderService;
}

void GenerateFullChildrenListForAll(const RSContext& context)
{
    context.GetGlobalRootRenderNode()->GenerateFullChildrenList();
    context.GetNodeMap().TraversalNodes(
        [](const std::shared_ptr<RSRenderNode>& node) { node->GenerateFullChildrenList(); });
}

/*
 * @tc.name: Interface Test
 * @tc.desc: RSProfiler Interface Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, InterfaceTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    EXPECT_NO_THROW({
        RSProfiler::Init(nullptr);
        RSProfiler::OnFrameBegin();
        RSProfiler::OnRenderBegin();
        RSProfiler::OnRenderEnd();
        RSProfiler::OnFrameEnd();
        RSProfiler::OnProcessCommand();
        Network::Stop();
    });
    RSProfiler::testing_ = false;
    EXPECT_NO_THROW({
        RSProfiler::Init(nullptr);
        RSProfiler::OnFrameBegin();
        RSProfiler::OnRenderBegin();
        RSProfiler::OnRenderEnd();
        RSProfiler::OnFrameEnd();
        RSProfiler::OnProcessCommand();
        Network::Stop();
    });
}
 
/*
 * @tc.name: Info Test
 * @tc.desc: RSProfiler Info Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, InfoTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    std::vector<std::string> cmds {"info", "freq", "fixenv",
        "params", "rscon_print", "rstree_contains", "rstree_pid"};
    EXPECT_NO_THROW({
        Network::PushCommand(cmds);
        for (auto cmd : cmds) {
            RSProfiler::ProcessCommands();
        }
    });
}
 
/*
 * @tc.name: RSTreeTest
 * @tc.desc: RSProfiler RSTree Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, RSTreeTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    std::vector<std::string> cmds {
        "rstree_save_frame 1", "rstree_load_frame 1",
    };
    EXPECT_NO_THROW({
        RSProfiler::Init(nullptr);
        Network::PushCommand(cmds);
        for (auto cmd : cmds) {
            RSProfiler::ProcessCommands();
        }
        Network::Stop();
    });
}

/*
 * @tc.name: MarshalSelfDrawingBuffersTest
 * @tc.desc: Test MarshalSelfDrawingBuffers method
 * @tc.type: FUNC
 * @tc.require: 21645
 */
HWTEST_F(RSProfilerTest, MarshalSelfDrawingBuffersTest, Level1)
{
    auto context = std::make_shared<RSContext>();
    RSProfiler::context_ = context.get();
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    config.name = "surface1";
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    auto& nodeMap = context->GetMutableNodeMap();
    nodeMap.RegisterRenderNode(rsSurfaceRenderNode);
    std::stringstream stream;
    RSProfiler::MarshalSelfDrawingBuffers(stream, false);
    ASSERT_TRUE(rsSurfaceRenderNode->IsSelfDrawingType());
    ASSERT_TRUE(rsSurfaceRenderNode->GetAbsRect().IsEmpty());
}

/*
 * @tc.name: UnmarshalSelfDrawingBuffersTest
 * @tc.desc: Test UnmarshalSelfDrawingBuffers method
 * @tc.type: FUNC
 * @tc.require: 21645
 */
HWTEST_F(RSProfilerTest, UnmarshalSelfDrawingBuffersTest, Level1)
{
    auto context = std::make_shared<RSContext>();
    RSProfiler::context_ = context.get();
    RSSurfaceRenderNodeConfig config;
    config.id = Utils::PatchNodeId(1);
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    config.name = "patchSurface";
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    auto& nodeMap = context->GetMutableNodeMap();
    nodeMap.RegisterRenderNode(rsSurfaceRenderNode);
    RSProfiler::UnmarshalSelfDrawingBuffers();
    ASSERT_TRUE(Utils::IsNodeIdPatched(rsSurfaceRenderNode->GetId()));
    ASSERT_TRUE(rsSurfaceRenderNode->IsSelfDrawingType());
    ASSERT_TRUE(rsSurfaceRenderNode->GetAbsRect().IsEmpty());
}

class RSProfilerTestWithContext : public testing::Test {
    static RSRenderService* renderService;

public:
    static void SetUpTestCase()
    {
        renderService = GetAndInitRenderService();
        RSProfiler::Init(renderService);
    };
    static void TearDownTestCase()
    {
        delete renderService;
        renderService = nullptr;
        RSProfiler::Init(nullptr);
    };
    void SetUp() override {};
    void TearDown() override {};
};

RSRenderService* RSProfilerTestWithContext::renderService;

void checkTree(std::shared_ptr<RSBaseRenderNode> rootNode, NodeId topNodeId, bool isPatched = false)
{
    if (isPatched) {
        EXPECT_EQ(rootNode->GetId(), Utils::PatchNodeId(0));
    } else {
        EXPECT_EQ(rootNode->GetId(), 0);
    }

    auto children = rootNode->GetChildren();
    ASSERT_EQ(children->size(), 1);
    auto screenNode = children->at(0);
    auto shouldBeId = topNodeId;
    if (isPatched) {
        shouldBeId = Utils::PatchNodeId(topNodeId);
    }
    EXPECT_EQ(screenNode->GetId(), shouldBeId);
    children = screenNode->GetChildren();
    ASSERT_EQ(children->size(), 1);
    auto displayNode = children->at(0);
    shouldBeId++;
    EXPECT_EQ(displayNode->GetId(), shouldBeId);
}

/*
 * @tc.name: LogEventStart
 * @tc.desc: Test LogEventStart
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, LogEventStart, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    sptr<RSRenderService> renderService = GetAndInitRenderService();
    RSProfiler::Init(renderService);

    std::vector<std::string> args = { "WAITFORFINISH" };
    ArgList argList(args);
    RSProfiler::RecordStart(argList);

    uint64_t curTime = 123;
    double timeSinceRecordStart = 456.2f;
    RSCaptureData captureData;
    if (RSProfiler::IsRecording()) {
        std::cout << "RSProfiler is recording right now" << std::endl;
    } else {
        std::cout << "RSProfiler is NOT recording right now" << std::endl;
    }

    RSProfiler::LogEventStart(curTime, captureData, timeSinceRecordStart);

    uint64_t readTime = captureData.GetTime();
    std::string readProperty = captureData.GetProperty(RSCaptureData::KEY_EVENT_TYPE);

    RSProfiler::RecordStop(argList);
    RSProfiler::SetMode(Mode::NONE);

    EXPECT_DOUBLE_EQ(static_cast<double>(readTime), timeSinceRecordStart);
    EXPECT_EQ(readProperty, RSCaptureData::VAL_EVENT_TYPE_VSYNC);
}

/*
 * @tc.name: SecureScreen
 * @tc.desc: Test SecureScreen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTestWithContext, SecureScreen, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    EXPECT_FALSE(RSProfiler::IsSecureScreen());
}

/*
 * @tc.name: LogEventVSync
 * @tc.desc: Test LogEventVSync
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, LogEventVSync, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    sptr<RSRenderService> renderService = GetAndInitRenderService();
    RSProfiler::Init(renderService);

    std::vector<std::string> args = { "WAITFORFINISH" };
    ArgList argList(args);
    RSProfiler::RecordStart(argList);

    if (RSProfiler::IsRecording()) {
        std::cout << "RSProfiler is recording right now" << std::endl;
    } else {
        std::cout << "RSProfiler is NOT recording right now" << std::endl;
    }

    uint64_t syncTime = Utils::Now() + Utils::ToNanoseconds(1.0f);
    RSProfiler::LogEventVSync(syncTime);
    RSProfiler::RecordStop(argList);

    RSFile testFile;

    std::string error;
    testFile.Open("RECORD_IN_MEMORY", error);

    EXPECT_TRUE(testFile.IsOpen());
    EXPECT_TRUE(error.empty());

    std::vector<uint8_t> data;
    double readTime;
    EXPECT_EQ(testFile.ReadLogEvent(1e10f, 0, data, readTime), true);

    testFile.Close();

    std::vector<char> cdata;
    cdata.assign(data.begin() + 1, data.end());

    RSCaptureData captureData;
    captureData.Deserialize(cdata);

    float readCDTime = captureData.GetTime();
    std::string readCDProperty = captureData.GetProperty(RSCaptureData::KEY_EVENT_TYPE);

    EXPECT_EQ(readCDProperty, RSCaptureData::VAL_EVENT_TYPE_VSYNC);
    EXPECT_NEAR(readCDTime, 1.f, 1e-2);

    RSProfiler::SetMode(Mode::NONE);
}

/*
 * @tc.name: UnmarshalNodeModifiersTest
 * @tc.desc: Test UnmarshalNodeModifiers
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, UnmarshalNodeModifiersTest, testing::ext::TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    drawCmdList->SetWidth(100); // DrawCmdList width for test is 100
    drawCmdList->SetHeight(100); // DrawCmdList height for test is 100
    auto modifier = std::make_shared<ModifierNG::TestCustomRenderModifier>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    modifier->AttachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE, property);
    std::shared_ptr<RSContext> context = nullptr;
    TestRSCanvasDrawingRenderNode node(1, context, false);
    node.AddModifier(modifier);
    node.stagingRenderParams_ = std::make_unique<RSRenderParams>(node.GetId());
    std::stringstream data;
    auto instanceRootNodeId = node.instanceRootNodeId_;
    auto firstLevelNodeId = node.firstLevelNodeId_;
    int32_t modifierCount = 0;
    data.write(reinterpret_cast<const char*>(&instanceRootNodeId), sizeof(instanceRootNodeId));
    data.write(reinterpret_cast<const char*>(&firstLevelNodeId), sizeof(firstLevelNodeId));
    data.write(reinterpret_cast<const char*>(&modifierCount), sizeof(modifierCount));
    auto ret = RSProfiler::UnmarshalNodeModifiers(node, data, 0, node.GetType());
    EXPECT_EQ(ret, "");
}

/*
 * @tc.name: PlaybackDeltaTimeNanoTest
 * @tc.desc: Test PlaybackDeltaTimeNano
 * @tc.type: FUNC
 * @tc.require: 22491
 */
HWTEST_F(RSProfilerTest, PlaybackDeltaTimeNanoTest, TestSize.Level1)
{
    RSProfiler::SetReplayStartTimeNano(0);
    ASSERT_EQ(RSProfiler::GetReplayStartTimeNano(), 0);
    auto deltaTimePrev = RSProfiler::PlaybackDeltaTimeNano();
    auto deltaTimeNext = RSProfiler::PlaybackDeltaTimeNano();
    ASSERT_TRUE(deltaTimeNext >= deltaTimePrev);
}

class RSProfilerFileTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/*
 * @tc.name: RSFileOffsetVersionTest
 * @tc.desc: Test RSFileOffsetVersion
 * @tc.type: FUNC
 * @tc.require: 22491
 */
HWTEST_F(RSProfilerFileTest, RSFileOffsetVersionTest, TestSize.Level1)
{
    RSFileOffsetVersion foVersion;
    foVersion.SetVersion(RSFILE_VERSION_TRACE3D_ADDED);
    const char* fileName = "rs_profiler_test.data";

    FILE* file = fopen(fileName, "wb");
    ASSERT_TRUE(file);
    uint64_t wValue = 1;
    foVersion.WriteU64(file, wValue);
    fclose(file);

    uint64_t rValue = 0;
    file = fopen(fileName, "rb");
    ASSERT_TRUE(file);
    foVersion.ReadU64(file, rValue);
    fclose(file);
    ASSERT_EQ(rValue, wValue);
}

/*
 * @tc.name: ReadAnimationStartTimeTest
 * @tc.desc: Test ReadAnimationStartTime
 * @tc.type: FUNC
 * @tc.require: 22491
 */
HWTEST_F(RSProfilerFileTest, ReadAnimationStartTimeTest, TestSize.Level1)
{
    std::string fileName = "rs_profiler_test.data";
    RSFile rsFile;
    rsFile.SetVersion(RSFILE_VERSION_RENDER_ANIMESTARTTIMES_ADDED);
    ASSERT_TRUE(rsFile.Create(fileName));
    rsFile.WriteAnimationStartTime();
    rsFile.Close();

    std::string error = "";
    ASSERT_TRUE(rsFile.Open(fileName, error));
    EXPECT_TRUE(rsFile.ReadAnimationStartTime());
    rsFile.Close();
}

/*
 * @tc.name: ReadHeaderTest
 * @tc.desc: Test ReadHeader
 * @tc.type: FUNC
 * @tc.require: 22491
 */
HWTEST_F(RSProfilerFileTest, ReadHeaderTest, TestSize.Level1)
{
    std::string fileName = "rs_profiler_test.data";
    RSFile rsFile;
    rsFile.SetVersion(RSFILE_VERSION_RENDER_ANIMESTARTTIMES_ADDED);
    ASSERT_TRUE(rsFile.Create(fileName));
    rsFile.WriteHeader();
    rsFile.Close();

    std::string error = "HasError";
    ASSERT_TRUE(rsFile.Open(fileName, error));
    EXPECT_TRUE(error.empty());
    error = rsFile.ReadHeader();
    EXPECT_TRUE(error.empty());
    rsFile.Close();
}

/*
 * @tc.name: GetEOFTimeTest
 * @tc.desc: Test GetEOFTime
 * @tc.type: FUNC
 * @tc.require: 22491
 */
HWTEST_F(RSProfilerFileTest, GetEOFTimeTest, TestSize.Level1)
{
    std::string fileName = "rs_profiler_test.data";
    RSFile rsFile;
    rsFile.SetVersion(RSFILE_VERSION_RENDER_ANIMESTARTTIMES_ADDED);
    ASSERT_TRUE(rsFile.Create(fileName));
    EXPECT_EQ(rsFile.AddLayer(), 0);
    double time = 100;
    std::vector<uint8_t> data = { 1, 1, 1, 1 };
    size_t size = data.size();
    rsFile.WriteRSData(time, data.data(), size);
    rsFile.Close();

    std::string error = "HasError";
    ASSERT_TRUE(rsFile.Open(fileName, error));
    EXPECT_TRUE(error.empty());
    EXPECT_EQ(rsFile.GetEOFTime(), time);
    rsFile.Close();
}

/*
 * @tc.name: WriteTrace3DMetricsTest
 * @tc.desc: Test WriteTrace3DMetrics
 * @tc.type: FUNC
 * @tc.require: 22491
 */
HWTEST_F(RSProfilerFileTest, WriteTrace3DMetricsTest, TestSize.Level1)
{
    std::string fileName = "rs_profiler_test.data";
    RSFile rsFile;
    rsFile.SetVersion(RSFILE_VERSION_RENDER_ANIMESTARTTIMES_ADDED);
    ASSERT_TRUE(rsFile.Create(fileName));
    auto layerId = rsFile.AddLayer();
    EXPECT_EQ(layerId, 0);
    double time = 100;
    uint32_t frame = 1;
    std::vector<uint8_t> wData = { 1, 0, 0, 1 };
    size_t size = wData.size();
    rsFile.WriteTrace3DMetrics(layerId, time, frame, wData.data(), size);
    rsFile.Close();

    std::string error = "HasError";
    ASSERT_TRUE(rsFile.Open(fileName, error));
    EXPECT_TRUE(error.empty());
    std::vector<uint8_t> rData = {};
    double readTime = 0;
    EXPECT_TRUE(rsFile.ReadTrace3DMetrics(time, layerId, rData, readTime));
    EXPECT_EQ(readTime, time);
    rsFile.Close();
}
} // namespace OHOS::Rosen