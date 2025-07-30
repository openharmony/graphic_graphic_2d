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

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
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
    auto renderService(new RSRenderService());
    if (renderService) {
        renderService->mainThread_ = RSMainThread::Instance();
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

std::vector<RSRenderNode::DrawCmdContainer> GetBufferOfDrawCmdModifiersFromTree(
    const std::vector<std::shared_ptr<RSRenderNode>>& tree)
{
    std::vector<RSRenderNode::DrawCmdContainer> bufferOfDrawCmdModifiers;
    bufferOfDrawCmdModifiers.reserve(tree.size());

    for (const auto& node : tree) {
        auto& drawCmdModifiers = node->GetDrawCmdModifiers();
        bufferOfDrawCmdModifiers.emplace_back();
        for (const auto& [modifierType, modifiers] : drawCmdModifiers) {
            auto bufferList = std::list<std::shared_ptr<OHOS::Rosen::RSRenderModifier>>();
            for (const auto& modifier : modifiers) {
                bufferList.push_back(modifier);
            }
            bufferOfDrawCmdModifiers.back().insert({ modifierType, bufferList });
        }
    }
    return bufferOfDrawCmdModifiers;
}

/*
    Will not check that is exactly equal because some elemets for "same" object will be different such
    as uniqueID for Image, checking Desc should be enough.
*/
bool CheckDescsInLists(
    std::list<std::shared_ptr<RSRenderModifier>> firstList, std::list<std::shared_ptr<RSRenderModifier>> secondList)
{
    auto bufferElement = secondList.begin();
    auto patchedElement = firstList.begin();
    while (bufferElement != secondList.end() && patchedElement != firstList.end()) {
        auto patchedPropertyDrawCmdList = (*patchedElement)->GetPropertyDrawCmdList();
        auto bufferPropertyDrawCmdList = (*bufferElement)->GetPropertyDrawCmdList();
        if (!patchedPropertyDrawCmdList || !bufferPropertyDrawCmdList) {
            ++bufferElement;
            ++patchedElement;
            continue;
        }
        auto patchedDesc = patchedPropertyDrawCmdList->GetOpsWithDesc();
        auto bufferDesc = bufferPropertyDrawCmdList->GetOpsWithDesc();
        if (bufferDesc.empty()) {
            /* Empty string suggest that saved node was in IMMEDIATE mode */
            /* Will skip checking DrawCmdList that is in IMMEDIATE mode */
            ++bufferElement;
            ++patchedElement;
            continue;
        }

        if (patchedDesc != bufferDesc) {
            return false;
        }
        ++bufferElement;
        ++patchedElement;
    }
    return true;
}

bool CheckDrawCmdModifiersEqual(const RSContext& context, const std::vector<std::shared_ptr<RSRenderNode>>& tree,
    const std::vector<RSRenderNode::DrawCmdContainer>& bufferOfDrawCmdModifiers)
{
    bool isDrawCmdModifiersEqual = true;

    for (size_t index = 0; index < tree.size(); ++index) {
        const auto& node = tree[index];
        if (!node) {
            continue;
        }
        NodeId nodeId = node->GetId();
        NodeId patchedNodeId = Utils::PatchNodeId(nodeId);
        const auto& patchedNode = context.GetNodeMap().GetRenderNode(patchedNodeId);
        if (!patchedNode) {
            isDrawCmdModifiersEqual = false;
            continue;
        }
        const auto& drawCmdModifiers = patchedNode->GetDrawCmdModifiers();

        for (const auto& [modifierType, modifiers] : drawCmdModifiers) {
            HRPI("TestDoubleReplay: For index %" PRIu64 " and modifierType %{public}hd count in buffer %" PRIu64, index,
                modifierType, bufferOfDrawCmdModifiers[index].count(modifierType));
            if (bufferOfDrawCmdModifiers[index].count(modifierType) == 0) {
                isDrawCmdModifiersEqual = false;
                continue;
            }

            const auto& currentBufferList = bufferOfDrawCmdModifiers[index].at(modifierType);
            HRPI("TestDoubleReplay: For modifierType %{public}hd modifiers size %" PRIu64 " buffer size %" PRIu64,
                modifierType, modifiers.size(), currentBufferList.size());
            if (currentBufferList.size() != modifiers.size()) {
                isDrawCmdModifiersEqual = false;
                continue;
            }

            isDrawCmdModifiersEqual = CheckDescsInLists(modifiers, currentBufferList);
            if (!isDrawCmdModifiersEqual) {
                continue;
            }
        }
    }

    return isDrawCmdModifiersEqual;
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
 * @tc.name: RSTreeTest
 * @tc.desc: Test SecureScreen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, SecureScreen, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;

    sptr<RSRenderService> renderService = GetAndInitRenderService();

    EXPECT_NO_THROW({
        RSProfiler::Init(renderService);
        EXPECT_FALSE(RSProfiler::IsSecureScreen());
    });
}

/*
 * @tc.name: RSDoubleTransformationTest
 * @tc.desc: Test double use of FirstFrameMarshalling & FirstFrameUnmarshalling with test tree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, RSDoubleTransformationTest, Function | Reliability | LargeTest | Level2)
{
    std::string flag = "0x00000012";
    RSLogManager::GetInstance().SetRSLogFlag(flag);

    RSProfiler::testing_ = true;
    const NodeId topId = 54321000;

    sptr<RSRenderService> renderService = GetAndInitRenderService();

    EXPECT_NE(renderService->mainThread_, nullptr);
    RSContext& context = renderService->mainThread_->GetContext();

    EXPECT_NO_THROW({
        RSProfiler::Init(renderService);

        auto testTreeBuilder = TestTreeBuilder();
        auto tree = testTreeBuilder.Build(context, topId, true);

        GenerateFullChildrenListForAll(context);

        auto bufferOfDrawCmdModifiers = GetBufferOfDrawCmdModifiersFromTree(tree);

        auto data = RSProfiler::FirstFrameMarshalling(RSFILE_VERSION_LATEST);
        RSProfiler::FirstFrameUnmarshalling(data, RSFILE_VERSION_LATEST);
        RSProfiler::TestSwitch(ArgList()); // Should be HiddenSpaceOn

        GenerateFullChildrenListForAll(context);

        RSProfiler::TestSwitch(ArgList()); // Should be HiddenSpaceOff

        GenerateFullChildrenListForAll(context);

        data = RSProfiler::FirstFrameMarshalling(RSFILE_VERSION_LATEST);
        RSProfiler::FirstFrameUnmarshalling(data, RSFILE_VERSION_LATEST);
        RSProfiler::TestSwitch(ArgList()); // Should be HiddenSpaceOn

        GenerateFullChildrenListForAll(context);

        bool isDrawCmdModifiersEqual = CheckDrawCmdModifiersEqual(context, tree, bufferOfDrawCmdModifiers);

        RSProfiler::TestSwitch(ArgList()); // Should be HiddenSpaceOff

        GenerateFullChildrenListForAll(context);

        RSProfiler::ClearTestTree(ArgList());

        EXPECT_TRUE(isDrawCmdModifiersEqual);
    });
}

/*
 * @tc.name: IfNeedToSkipDuringReplay
 * @tc.desc: Test IfNeedToSkipDuringReplay method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, IfNeedToSkipDuringReplay, Function | Reliability | LargeTest | Level2)
{
    RSProfiler::testing_ = true;

    RSProfiler::SetMode(Mode::READ_EMUL);

    auto data = std::make_shared<Drawing::Data>();
    constexpr size_t length = 40'000;
    constexpr size_t position = 36;

    void* allocated = malloc(length);
    EXPECT_TRUE(data->BuildFromMalloc(allocated, length));

    auto* buffer = new[std::nothrow] uint8_t[sizeof(MessageParcel) + 1];
    MessageParcel* messageParcel = new (buffer + 1) MessageParcel;

    EXPECT_TRUE(RSMarshallingHelper::Marshalling(*messageParcel, data));

    EXPECT_TRUE(RSProfiler::IfNeedToSkipDuringReplay(*messageParcel, position));
    EXPECT_EQ(messageParcel->GetWritePosition(), position);
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

    ArgList argList;
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

    ArgList argList;
    RSProfiler::RecordStart(argList);

    if (RSProfiler::IsRecording()) {
        std::cout << "RSProfiler is recording right now" << std::endl;
    } else {
        std::cout << "RSProfiler is NOT recording right now" << std::endl;
    }

    uint64_t syncTime = Utils::Now() + Utils::ToNanoseconds(1.0f);
    RSProfiler::LogEventVSync(syncTime);
    RSProfiler::RecordStop(argList);

    while (RSProfiler::GetMode() != Mode::NONE);

    RSFile testFile;

    testFile.Open("RECORD_IN_MEMORY");

    EXPECT_TRUE(testFile.IsOpen());

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

HWTEST_F(RSProfilerTestWithContext, HiddenSpaceTurnOnOff, Level1)
{
    EXPECT_NE(RSProfiler::context_, nullptr);
    NodeId topNodeId = 12345000;
    TestTreeBuilder treeBuilder;
    std::cout << "Preparing to run test..." << std::endl;

    auto originalTree = treeBuilder.Build(*RSProfiler::context_, topNodeId, true, true);
    std::cout << "Builded test tree" << std::endl;

    auto patchedTree = treeBuilder.Build(*RSProfiler::context_, Utils::PatchNodeId(topNodeId), true, true, true);
    std::cout << "Builded patched test tree" << std::endl;

    GenerateFullChildrenListForAll(*RSProfiler::context_);
    std::cout << "Generated children for all" << std::endl;

    checkTree(RSProfiler::context_->GetGlobalRootRenderNode(), topNodeId);
    std::cout << "Checked test tree first time" << std::endl;

    RSProfiler::HiddenSpaceTurnOn();
    std::cout << "Enabled HiddenSpace" << std::endl;

    GenerateFullChildrenListForAll(*RSProfiler::context_);
    std::cout << "Generated children for all" << std::endl;

    checkTree(RSProfiler::context_->GetNodeMap().GetRenderNode(Utils::PatchNodeId(0)), topNodeId, true);
    std::cout << "Checked test tree second time" << std::endl;

    RSProfiler::HiddenSpaceTurnOff();
    std::cout << "Disabled HiddenSpace" << std::endl;

    GenerateFullChildrenListForAll(*RSProfiler::context_);
    std::cout << "Generated children for all" << std::endl;

    checkTree(RSProfiler::context_->GetGlobalRootRenderNode(), topNodeId);
    std::cout << "Checked test tree third time" << std::endl;

    for (auto node : originalTree) {
        RSProfiler::context_->GetMutableNodeMap().UnRegisterUnTreeNode(node->GetId());
    }
    for (auto patchedNode : originalTree) {
        RSProfiler::context_->GetMutableNodeMap().UnRegisterUnTreeNode(patchedNode->GetId());
    }

    std::cout << "End of test" << std::endl;
}
} // namespace OHOS::Rosen