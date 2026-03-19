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

#include "gtest/gtest.h"
#include "feature/buffer_reclaim/rs_buffer_reclaim.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
#ifndef ROSEN_CROSS_PLATFORM
constexpr uint32_t ADD_SAME_NODE_NUMS = 5;
class RSBufferReclaimTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    std::shared_ptr<RSBufferReclaim> rsBufferReclaim_ = nullptr;
    sptr<SurfaceBuffer> CreateSurfaceBuffer();
    void RemoveUICaptureNodeForTestCaseAddUICaptureNode004(NodeId nodeId);
    const uint32_t MAX_BUFFER_RECLAIM_NUMS = 50;
};

void RSBufferReclaimTest::RemoveUICaptureNodeForTestCaseAddUICaptureNode004(NodeId nodeId)
{
    auto& nodeIdCountMap5 = rsBufferReclaim_->uiCaptureNodeMap_[ExtractPid(nodeId)];
    for (uint32_t i = 0; i < ADD_SAME_NODE_NUMS; i++) {
        rsBufferReclaim_->RemoveUICaptureNode(nodeId);
        bool hasNode = (nodeIdCountMap5.find(nodeId) != nodeIdCountMap5.end());
        if (hasNode) {
            EXPECT_EQ(nodeIdCountMap5[nodeId], ADD_SAME_NODE_NUMS - i - 1);
        } else {
            EXPECT_EQ(i, ADD_SAME_NODE_NUMS - 1);
        }
    }
}

sptr<SurfaceBuffer> RSBufferReclaimTest::CreateSurfaceBuffer()
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    if (buffer->Alloc(requestConfig) != GSERROR_OK) {
        return nullptr;
    }
    return buffer;
}

void RSBufferReclaimTest::SetUp()
{
    rsBufferReclaim_ = std::make_shared<RSBufferReclaim>();
    ASSERT_NE(rsBufferReclaim_, nullptr);
}

void RSBufferReclaimTest::TearDown()
{
    rsBufferReclaim_ = nullptr;
}
void RSBufferReclaimTest::SetUpTestCase() {}
void RSBufferReclaimTest::TearDownTestCase() {}

/**
 * @tc.name:  BufferReclaimAndResumeTest001
 * @tc.desc: test DoBufferReclaim And DoBufferResume
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, BufferReclaimAndResumeTest001, TestSize.Level2)
{
    auto buffer = CreateSurfaceBuffer();
    EXPECT_TRUE(buffer != nullptr);
    if (buffer) {
        EXPECT_TRUE(rsBufferReclaim_->DoBufferReclaim(buffer));
        // double DoBufferReclaim, should fail
        EXPECT_FALSE(rsBufferReclaim_->DoBufferReclaim(buffer));
        EXPECT_TRUE(rsBufferReclaim_->DoBufferResume(buffer));
        // double DoBufferResume, should fail
        EXPECT_FALSE(rsBufferReclaim_->DoBufferResume(buffer));

        EXPECT_TRUE(buffer->TryReclaim() == GSERROR_OK);
        // buffer has reclaimed, do DoBufferReclaim should fail
        EXPECT_FALSE(rsBufferReclaim_->DoBufferReclaim(buffer));

        EXPECT_TRUE(buffer->TryResumeIfNeeded() == GSERROR_OK);
        // buffer has Resumed, do DoBufferReclaim should ok
        EXPECT_TRUE(rsBufferReclaim_->DoBufferReclaim(buffer));

        EXPECT_TRUE(buffer->TryResumeIfNeeded() == GSERROR_OK);
        // buffer has Resumed, do DoBufferResume should fail
        EXPECT_FALSE(rsBufferReclaim_->DoBufferResume(buffer));
    }
}

/**
 * @tc.name:  BufferReclaimAndResumeTest002
 * @tc.desc: test DoBufferReclaim And DoBufferResume
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, BufferReclaimAndResumeTest002, TestSize.Level2)
{
    auto buffer = CreateSurfaceBuffer();
    EXPECT_TRUE(buffer != nullptr);
    uint64_t bufferId = 0;
    if (buffer) {
        bufferId = buffer->GetBufferId();
        EXPECT_TRUE(rsBufferReclaim_->DoBufferReclaim(buffer));
        EXPECT_TRUE(rsBufferReclaim_->bufferReclaimNumsSet_.find(bufferId) != rsBufferReclaim_->bufferReclaimNumsSet_.end());
        rsBufferReclaim_->RemoveBufferReclaim(bufferId);
    }
    EXPECT_FALSE(rsBufferReclaim_->bufferReclaimNumsSet_.find(bufferId) != rsBufferReclaim_->bufferReclaimNumsSet_.end());
}

/**
 * @tc.name:  BufferReclaimAndResumeTest003
 * @tc.desc: test DoBufferReclaim And DoBufferResume
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, BufferReclaimAndResumeTest003, TestSize.Level2)
{
    for (uint32_t i = 0; i <= MAX_BUFFER_RECLAIM_NUMS; i++) {
        auto buffer = CreateSurfaceBuffer();
        EXPECT_TRUE(buffer != nullptr);
        if (buffer) {
            if (i < MAX_BUFFER_RECLAIM_NUMS) {
                EXPECT_TRUE(rsBufferReclaim_->CheckBufferReclaim());
                EXPECT_TRUE(rsBufferReclaim_->DoBufferReclaim(buffer));
            } else {
                EXPECT_FALSE(rsBufferReclaim_->CheckBufferReclaim());
                EXPECT_FALSE(rsBufferReclaim_->DoBufferReclaim(buffer));
            }
        }
    }
}

/**
 * @tc.name:  AddUICaptureNode001
 * @tc.desc: test AddUICaptureNode
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, AddUICaptureNode001, TestSize.Level1)
{
    rsBufferReclaim_->uiCaptureNodeMap_.clear();
    pid_t testPidBase = 1234;
    uint32_t uidBase = 5678;

    NodeId id1 = MakeNodeId(testPidBase, uidBase++);
    rsBufferReclaim_->AddUICaptureNode(id1);
    EXPECT_TRUE(rsBufferReclaim_->CheckSameProcessUICaptureNode(id1));

    rsBufferReclaim_->RemoveUICaptureNode(id1);
    EXPECT_TRUE(rsBufferReclaim_->uiCaptureNodeMap_.empty());
    EXPECT_FALSE(rsBufferReclaim_->CheckSameProcessUICaptureNode(id1));
}

/**
 * @tc.name:  AddUICaptureNode002
 * @tc.desc: test AddUICaptureNode
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, AddUICaptureNode002, TestSize.Level1)
{
    rsBufferReclaim_->uiCaptureNodeMap_.clear();
    pid_t testPidBase = 1234;
    uint32_t uidBase = 5678;

    NodeId id1 = MakeNodeId(testPidBase, uidBase++);
    rsBufferReclaim_->AddUICaptureNode(id1);

    NodeId id2 = MakeNodeId(testPidBase, uidBase++);
    rsBufferReclaim_->AddUICaptureNode(id2);

    NodeId id3 = MakeNodeId(testPidBase, uidBase++);
    rsBufferReclaim_->AddUICaptureNode(id3);

    bool check = (rsBufferReclaim_->uiCaptureNodeMap_.size() == 1);
    EXPECT_TRUE(check);
    if (check) {
        auto& nodeIdCountMap = rsBufferReclaim_->uiCaptureNodeMap_[testPidBase];
        EXPECT_EQ(nodeIdCountMap.size(), 3);
    }

    rsBufferReclaim_->RemoveUICaptureNode(id1);
    check = (rsBufferReclaim_->uiCaptureNodeMap_.size() == 1);
    EXPECT_TRUE(check);
    if (check) {
        auto& nodeIdCountMap = rsBufferReclaim_->uiCaptureNodeMap_[testPidBase];
        EXPECT_EQ(nodeIdCountMap.size(), 2);
    }

    rsBufferReclaim_->RemoveUICaptureNode(id2);
    check = (rsBufferReclaim_->uiCaptureNodeMap_.size() == 1);
    EXPECT_TRUE(check);
    if (check) {
        auto& nodeIdCountMap = rsBufferReclaim_->uiCaptureNodeMap_[testPidBase];
        EXPECT_EQ(nodeIdCountMap.size(), 1);
    }

    rsBufferReclaim_->RemoveUICaptureNode(id3);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 0);
}

/**
 * @tc.name:  AddUICaptureNode003
 * @tc.desc: test AddUICaptureNode
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, AddUICaptureNode003, TestSize.Level1)
{
    rsBufferReclaim_->uiCaptureNodeMap_.clear();
    pid_t testPidBase = 1234;
    uint32_t uidBase = 5678;

    NodeId id1 = MakeNodeId(testPidBase, uidBase++);
    for (uint32_t i = 1; i <= ADD_SAME_NODE_NUMS; i++) {
        rsBufferReclaim_->AddUICaptureNode(id1);
        bool check = (rsBufferReclaim_->uiCaptureNodeMap_.size() == 1);
        EXPECT_TRUE(check);
        if (check) {
            auto& nodeIdCountMap = rsBufferReclaim_->uiCaptureNodeMap_[testPidBase];
            EXPECT_EQ(nodeIdCountMap.size(), 1);
            auto iter = nodeIdCountMap.begin();
            EXPECT_EQ(iter->first, id1);
            EXPECT_EQ(iter->second, i);
        }
    }
    for (uint32_t i = 1; i <= ADD_SAME_NODE_NUMS; i++) {
        rsBufferReclaim_->RemoveUICaptureNode(id1);
        if (i != ADD_SAME_NODE_NUMS) {
            bool check = (rsBufferReclaim_->uiCaptureNodeMap_.size() == 1);
            EXPECT_TRUE(check);
            if (check) {
                auto& nodeIdCountMap = rsBufferReclaim_->uiCaptureNodeMap_[testPidBase];
                EXPECT_EQ(nodeIdCountMap.size(), 1);
                auto iter = nodeIdCountMap.begin();
                EXPECT_EQ(iter->first, id1);
                EXPECT_EQ(iter->second, ADD_SAME_NODE_NUMS - i);
            }
        } else {
            EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 0);
        }
    }
    EXPECT_TRUE(rsBufferReclaim_->uiCaptureNodeMap_.empty());
}

/**
 * @tc.name:  AddUICaptureNode004
 * @tc.desc: test AddUICaptureNode
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, AddUICaptureNode004, TestSize.Level1)
{
    pid_t testPidBase = 1234;
    uint32_t uidBase = 5678;

    NodeId id1 = MakeNodeId(testPidBase++, uidBase++);
    NodeId id2 = MakeNodeId(testPidBase++, uidBase++);
    NodeId id3 = MakeNodeId(testPidBase, uidBase++);
    NodeId id4 = MakeNodeId(testPidBase, uidBase++);
    NodeId id5 = MakeNodeId(testPidBase++, uidBase++);
    NodeId id6 = MakeNodeId(testPidBase, uidBase++);

    rsBufferReclaim_->AddUICaptureNode(id1);
    rsBufferReclaim_->AddUICaptureNode(id2);
    rsBufferReclaim_->AddUICaptureNode(id3);
    rsBufferReclaim_->AddUICaptureNode(id4);
    rsBufferReclaim_->AddUICaptureNode(id5);
    for (uint32_t i = 0; i < ADD_SAME_NODE_NUMS; i++) {
        rsBufferReclaim_->AddUICaptureNode(id6);
    }
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 4);

    auto& nodeIdCountMap1 = rsBufferReclaim_->uiCaptureNodeMap_[ExtractPid(id1)];
    EXPECT_EQ(nodeIdCountMap1.size(), 1);
    EXPECT_EQ(nodeIdCountMap1[id1], 1);

    auto& nodeIdCountMap2 = rsBufferReclaim_->uiCaptureNodeMap_[ExtractPid(id2)];
    EXPECT_EQ(nodeIdCountMap2.size(), 1);
    EXPECT_EQ(nodeIdCountMap2[id2], 1);

    auto& nodeIdCountMap3 = rsBufferReclaim_->uiCaptureNodeMap_[ExtractPid(id3)];
    EXPECT_EQ(nodeIdCountMap3.size(), 3);
    EXPECT_EQ(nodeIdCountMap3[id3], 1);
    EXPECT_EQ(nodeIdCountMap3[id4], 1);
    EXPECT_EQ(nodeIdCountMap3[id5], 1);

    auto& nodeIdCountMap4 = rsBufferReclaim_->uiCaptureNodeMap_[ExtractPid(id6)];
    EXPECT_EQ(nodeIdCountMap4.size(), 1);
    EXPECT_EQ(nodeIdCountMap4[id6], ADD_SAME_NODE_NUMS);

    rsBufferReclaim_->RemoveUICaptureNode(id1);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 3);
    rsBufferReclaim_->RemoveUICaptureNode(id2);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 2);
    rsBufferReclaim_->RemoveUICaptureNode(id3);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 2);
    rsBufferReclaim_->RemoveUICaptureNode(id4);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 2);
    rsBufferReclaim_->RemoveUICaptureNode(id5);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 1);
    RemoveUICaptureNodeForTestCaseAddUICaptureNode004(id6);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 0);
}

/**
 * @tc.name:  AddUICaptureNode005
 * @tc.desc: test AddUICaptureNode
 * @tc.type: FUNC
 * @tc.require: issue#913
 */
HWTEST_F(RSBufferReclaimTest, AddUICaptureNode005, TestSize.Level1)
{
    rsBufferReclaim_->uiCaptureNodeMap_.clear();
    pid_t testPidBase = 1234;
    uint32_t uidBase = 5678;

    NodeId id1 = MakeNodeId(testPidBase, uidBase);
    NodeId id2 = MakeNodeId(testPidBase, ++uidBase);
    NodeId id3 = MakeNodeId(++testPidBase, uidBase++);
    NodeId id4 = MakeNodeId(testPidBase++, uidBase++);
    rsBufferReclaim_->AddUICaptureNode(id1);
    EXPECT_TRUE(rsBufferReclaim_->CheckSameProcessUICaptureNode(id1));
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 1);

    EXPECT_TRUE(rsBufferReclaim_->CheckSameProcessUICaptureNode(id2));
    EXPECT_FALSE(rsBufferReclaim_->CheckSameProcessUICaptureNode(id3));
    EXPECT_FALSE(rsBufferReclaim_->CheckSameProcessUICaptureNode(id4));
    rsBufferReclaim_->RemoveUICaptureNode(id2);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 1);
    rsBufferReclaim_->RemoveUICaptureNode(id3);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 1);
    rsBufferReclaim_->RemoveUICaptureNode(id4);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 1);
    rsBufferReclaim_->RemoveUICaptureNode(id1);
    EXPECT_EQ(rsBufferReclaim_->uiCaptureNodeMap_.size(), 0);
}
#endif
} // namespace OHOS::Rosen
