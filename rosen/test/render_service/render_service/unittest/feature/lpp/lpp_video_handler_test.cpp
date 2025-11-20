/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "consumer_surface.h"
#include "feature/lpp/lpp_video_handler.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class LppVideoHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode_ = nullptr;
    sptr<ConsumerSurface> consumer_ = nullptr;
    NodeId id = 1;
};

void LppVideoHandlerTest::SetUpTestCase() {}
void LppVideoHandlerTest::TearDownTestCase() {}
void LppVideoHandlerTest::SetUp()
{
    surfaceNode_ = std::make_shared<RSSurfaceRenderNode>(id);
    surfaceNode_->InitRenderParams();
    consumer_ = new ConsumerSurface("LPP");
    consumer_->Init();
    surfaceNode_->surfaceHandler_->SetConsumer(consumer_);
}
void LppVideoHandlerTest::TearDown()
{
    if (surfaceNode_ != nullptr && surfaceNode_->surfaceHandler_ != nullptr) {
        surfaceNode_->surfaceHandler_->SetConsumer(nullptr);
    }
    consumer_ = nullptr;
    surfaceNode_ = nullptr;
}
/**
 * @tc.name: SetHasVirtualMirrorDisplay001
 * @tc.desc: test LppVideoHandlerTest.SetHasVirtualMirrorDisplay001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, SetHasVirtualMirrorDisplay, TestSize.Level1)
{
    bool hasVirtualMirrorDisplay = false;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.SetHasVirtualMirrorDisplay(hasVirtualMirrorDisplay);
    ASSERT_EQ(lppVideoHandler.hasVirtualMirrorDisplay_.load(), hasVirtualMirrorDisplay);
}
/**
 * @tc.name: ConsumeAndUpdateLppBuffer001
 * @tc.desc: ConsumeAndUpdateLppBuffer001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, ConsumeAndUpdateLppBuffer001, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lppConsumerMap_.clear();
    // isInvalidNode is true
    lppVideoHandler.ConsumeAndUpdateLppBuffer(0, nullptr);
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());

    std::shared_ptr<RSSurfaceRenderNode> surfaceNodeTmp = std::make_shared<RSSurfaceRenderNode>(1);
    surfaceNodeTmp->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(1);
    auto& bufferQueue = consumer_->consumer_->bufferQueue_;
    surfaceNodeTmp->surfaceHandler_->SetConsumer(consumer_);
    bufferQueue->sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT;
    // Failed to verify the tag.
    lppVideoHandler.ConsumeAndUpdateLppBuffer(0, surfaceNodeTmp);
    ASSERT_EQ(lppVideoHandler.lppConsumerMap_.size(), 0);

    bufferQueue->sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO;
    lppVideoHandler.lppConsumerMap_.clear();
    for (int64_t vId = 10; vId < 20; vId++) {
        lppVideoHandler.lppConsumerMap_[vId].push_back(consumer_);
    }
    // needRemoveTopNode is true
    lppVideoHandler.ConsumeAndUpdateLppBuffer(0, surfaceNodeTmp);
    ASSERT_EQ(lppVideoHandler.lppConsumerMap_.size(), 10);
    bufferQueue->lppSlotInfo_ = nullptr;
    lppVideoHandler.lppConsumerMap_.clear();
    // buffer == nullptr
    lppVideoHandler.ConsumeAndUpdateLppBuffer(0, surfaceNodeTmp);
    ASSERT_EQ(lppVideoHandler.lppConsumerMap_.size(), 1);
}

/**
 * @tc.name: ConsumeAndUpdateLppBuffer002
 * @tc.desc: ConsumeAndUpdateLppBuffer002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, ConsumeAndUpdateLppBuffer002, TestSize.Level1)
{
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->surfaceHandler_->sourceType_ =
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO);
    ASSERT_NE(consumer_, nullptr);
    auto& bufferQueue = consumer_->consumer_->bufferQueue_;
    bufferQueue->sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO;
    bufferQueue->lppSlotInfo_ = new LppSlotInfo { .readOffset = 0, .writeOffset = 1,
        .slot = { { .seqId = 100, .timestamp = 1000, .crop = { 1, 2, 3, 4 } } },
        .frameRate = 0, .isStopShbDraw = 0 };
    bufferQueue->isRsDrawLpp_ = false;
    auto buffer = SurfaceBuffer::Create();
    BufferHandle* handle = new BufferHandle();
    handle->width = 100;
    handle->height = 100;
    buffer->SetBufferHandle(handle);
    BufferElement ele = {
        .buffer = buffer, .state = BUFFER_STATE_ACQUIRED, .isDeleting = false,
        .config = {}, .fence = SyncFence::InvalidFence()};
    bufferQueue->bufferQueueCache_[100] = ele;

    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lppConsumerMap_.clear();
    lppVideoHandler.ConsumeAndUpdateLppBuffer(0, surfaceNode_);
    EXPECT_FALSE(lppVideoHandler.lppConsumerMap_.empty());

    bufferQueue->lppSlotInfo_->readOffset = 0;
    bufferQueue->lppSlotInfo_->writeOffset = 1;
    bufferQueue->lastLppWriteOffset_ = 0;
    bufferQueue->lppSlotInfo_->slot[0].crop[0] = -1;
    bufferQueue->lppSlotInfo_->slot[0].crop[1] = -1;
    bufferQueue->lppSlotInfo_->slot[0].crop[2] = -1;
    bufferQueue->lppSlotInfo_->slot[0].crop[3] = -1;
    handle->width = -1;
    handle->height = -1;
    buffer->SetBufferHandle(handle);
    bufferQueue->bufferQueueCache_[100] = ele;

    lppVideoHandler.lppConsumerMap_.clear();
    lppVideoHandler.ConsumeAndUpdateLppBuffer(0, surfaceNode_);
    EXPECT_FALSE(lppVideoHandler.lppConsumerMap_.empty());
    delete bufferQueue->lppSlotInfo_;
    bufferQueue->lppSlotInfo_ = nullptr;
    delete handle;
    handle = nullptr;
    lppVideoHandler.lppConsumerMap_.clear();
}

/**
 * @tc.name: JudgeRequestVsyncForLpp001
 * @tc.desc: JudgeRequestVsyncForLpp001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRequestVsyncForLpp001, TestSize.Level1)
{
    // not find vsyncId
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lppConsumerMap_.clear();
    lppVideoHandler.JudgeRequestVsyncForLpp(0);
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());

    // When first awakened
    uint32_t max = LPP_SURFACE_NODE_MAX_SIZE + 1;
    for (uint32_t id = 0; id < max; id++) {
        lppVideoHandler.lppConsumerMap_[id] = {};
    }
    lppVideoHandler.isRequestedVsync_ = false;
    lppVideoHandler.lppRsState_ = LppState::LPP_LAYER;
    lppVideoHandler.JudgeRequestVsyncForLpp(0);
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());

    lppVideoHandler.lppConsumerMap_[0] = {};
    lppVideoHandler.JudgeRequestVsyncForLpp(0);
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());
}

/**
 * @tc.name: JudgeRequestVsyncForLpp002
 * @tc.desc: JudgeRequestVsyncForLpp002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRequestVsyncForLpp002, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lppConsumerMap_.clear();

    ASSERT_NE(surfaceNode_, nullptr);
    ASSERT_NE(consumer_, nullptr);
    surfaceNode_->surfaceHandler_->SetConsumer(nullptr);
    sptr<IConsumerSurface> nullConsumer = nullptr;
    lppVideoHandler.lppConsumerMap_[0].push_back(nullConsumer);
    lppVideoHandler.isRequestedVsync_ = true;
    lppVideoHandler.lppRsState_ = LppState::UNI_RENDER;
    lppVideoHandler.JudgeRequestVsyncForLpp(0);
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());
    lppVideoHandler.lppConsumerMap_.clear();
    surfaceNode_->surfaceHandler_->SetConsumer(consumer_);
}
/**
 * @tc.name: JudgeRequestVsyncForLpp003
 * @tc.desc: JudgeRequestVsyncForLpp003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRequestVsyncForLpp003, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lppConsumerMap_.clear();

    ASSERT_NE(surfaceNode_, nullptr);
    ASSERT_NE(consumer_, nullptr);
    auto& bufferQueue = consumer_->consumer_->bufferQueue_;
    bufferQueue->lppSlotInfo_ = new LppSlotInfo { .readOffset = 0,
        .writeOffset = 1,
        .slot = { { .seqId = 100, .timestamp = 1000, .crop = { 1, 2, 3, 4 } } },
        .frameRate = 0,
        .isStopShbDraw = 0 };
    lppVideoHandler.lppConsumerMap_[0].push_back(consumer_);

    lppVideoHandler.isRequestedVsync_ = true;
    lppVideoHandler.lppRsState_ = LppState::LPP_LAYER;
    lppVideoHandler.lppShbState_ = LppState::LPP_LAYER;

    // ret == GSERROR_NO_CONSUMER
    bufferQueue->lppSlotInfo_->isStopShbDraw = true;
    bufferQueue->lppSkipCount_ = 0;
    bufferQueue->sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO;
    lppVideoHandler.JudgeRequestVsyncForLpp(0);
    EXPECT_FALSE(lppVideoHandler.lppConsumerMap_.empty());

    // ret == GSERROR_OK
    bufferQueue->lppSlotInfo_->isStopShbDraw = false;
    bufferQueue->lastLppWriteOffset_ = 3;
    bufferQueue->lastRsToShbWriteOffset_ = 0;
    lppVideoHandler.JudgeRequestVsyncForLpp(0);
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());

    // ret == GSERROR_OUT_OF_RANGE
    bufferQueue->lppSkipCount_ = 11;
    lppVideoHandler.lppConsumerMap_[0].push_back(consumer_);
    lppVideoHandler.JudgeRequestVsyncForLpp(0);
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());

    bufferQueue->lppSkipCount_ = 0;
    lppVideoHandler.lppConsumerMap_[0].push_back(consumer_);
    lppVideoHandler.lppShbState_ = LppState::UNI_RENDER;
    lppVideoHandler.lppRsState_ = LppState::UNI_RENDER;
    EXPECT_FALSE(lppVideoHandler.lppConsumerMap_.empty());

    delete bufferQueue->lppSlotInfo_;
    bufferQueue->lppSlotInfo_ = nullptr;
    lppVideoHandler.lppConsumerMap_.clear();
}

/**
 * @tc.name: AddLppLayerId001
 * @tc.desc: AddLppLayerId001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, AddLppLayerId001, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    RSLayerPtr ptr = std::make_shared<HdiLayerInfo>();
    ASSERT_NE(ptr, nullptr);
    ptr->SetSurface(nullptr);
    layers.emplace_back(ptr);

    lppVideoHandler.lppLayerId_.clear();
    lppVideoHandler.AddLppLayerId({ nullptr });
    EXPECT_TRUE(lppVideoHandler.lppLayerId_.empty());

    lppVideoHandler.lppLayerId_.clear();
    lppVideoHandler.AddLppLayerId(layers);
    EXPECT_TRUE(lppVideoHandler.lppLayerId_.empty());
}

/**
 * @tc.name: AddLppLayerId002
 * @tc.desc: AddLppLayerId002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, AddLppLayerId002, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    RSLayerPtr ptr = std::make_shared<HdiLayerInfo>();
    ASSERT_NE(ptr, nullptr);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    ptr->SetSurface(csurface);
    ptr->SetTunnelLayerId(0);
    layers.emplace_back(ptr);

    lppVideoHandler.lppLayerId_.clear();
    lppVideoHandler.AddLppLayerId(layers);
    EXPECT_TRUE(lppVideoHandler.lppLayerId_.empty());

    ptr->SetTunnelLayerId(1);
    ptr->SetTunnelLayerProperty(12345);
    lppVideoHandler.lppLayerId_.clear();
    lppVideoHandler.AddLppLayerId(layers);
    EXPECT_TRUE(lppVideoHandler.lppLayerId_.empty());
}

/**
 * @tc.name: AddLppLayerId003
 * @tc.desc: AddLppLayerId003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, AddLppLayerId003, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    RSLayerPtr ptr = std::make_shared<HdiLayerInfo>();
    ASSERT_NE(ptr, nullptr);
    ptr->SetTunnelLayerId(1);
    ptr->SetTunnelLayerProperty(LPP_LAYER_PROPERTY);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    ptr->SetSurface(csurface);
    layers.emplace_back(ptr);

    lppVideoHandler.lppLayerId_.clear();
    lppVideoHandler.AddLppLayerId(layers);
    EXPECT_FALSE(lppVideoHandler.lppLayerId_.empty());
    lppVideoHandler.lppLayerId_.clear();
}

/**
 * @tc.name: RemoveLayerId001
 * @tc.desc: RemoveLayerId001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, RemoveLayerId001, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    RSLayerPtr ptr = std::make_shared<HdiLayerInfo>();
    ASSERT_NE(ptr, nullptr);
    ptr->SetSurface(nullptr);
    layers.emplace_back(ptr);

    lppVideoHandler.lppLayerId_.clear();
    lppVideoHandler.RemoveLayerId(layers);
    EXPECT_TRUE(lppVideoHandler.lppLayerId_.empty());

    lppVideoHandler.lppLayerId_.clear();
    lppVideoHandler.RemoveLayerId({ nullptr });
    EXPECT_TRUE(lppVideoHandler.lppLayerId_.empty());
}

/**
 * @tc.name: RemoveLayerId002
 * @tc.desc: RemoveLayerId002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, RemoveLayerId002, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    RSLayerPtr ptr = std::make_shared<HdiLayerInfo>();
    ASSERT_NE(ptr, nullptr);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    ptr->SetSurface(csurface);
    ptr->SetTunnelLayerId(0);
    layers.emplace_back(ptr);

    lppVideoHandler.lppLayerId_.clear();
    lppVideoHandler.RemoveLayerId(layers);
    EXPECT_TRUE(lppVideoHandler.lppLayerId_.empty());

    ptr->SetTunnelLayerId(0);
    ptr->SetTunnelLayerProperty(12345);
    lppVideoHandler.lppLayerId_.clear();
    lppVideoHandler.RemoveLayerId(layers);
    EXPECT_TRUE(lppVideoHandler.lppLayerId_.empty());
}

/**
 * @tc.name: RemoveLayerId003
 * @tc.desc: RemoveLayerId003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, RemoveLayerId003, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    RSLayerPtr ptr = std::make_shared<HdiLayerInfo>();
    ASSERT_NE(ptr, nullptr);
    ptr->SetTunnelLayerId(1);
    ptr->SetTunnelLayerProperty(LPP_LAYER_PROPERTY);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    ptr->SetSurface(csurface);
    layers.emplace_back(ptr);

    lppVideoHandler.lppLayerId_.clear();
    lppVideoHandler.RemoveLayerId(layers);
    EXPECT_TRUE(lppVideoHandler.lppLayerId_.empty());
    lppVideoHandler.lppLayerId_.clear();
}

/**
 * @tc.name: JudgeLppLayer001
 * @tc.desc: JudgeLppLayer001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeLppLayer001, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lppConsumerMap_.clear();
    lppVideoHandler.JudgeLppLayer(0);
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());
}

/**
 * @tc.name: JudgeLppLayer002
 * @tc.desc: JudgeLppLayer002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeLppLayer002, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lppConsumerMap_.clear();
    lppVideoHandler.lppLayerId_.clear();

    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->surfaceHandler_->SetConsumer(nullptr);
    sptr<IConsumerSurface> nullConsumer = nullptr;
    lppVideoHandler.lppConsumerMap_[0].push_back(nullConsumer);

    lppVideoHandler.lppRsState_ = LppState::LPP_LAYER;
    lppVideoHandler.JudgeLppLayer(0);
    // isInvalidNode is true
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());

    surfaceNode_->SetConsumer(consumer_);
    auto& bufferQueue = consumer_->consumer_->bufferQueue_;
    bufferQueue->lppSlotInfo_ = new LppSlotInfo { .readOffset = 0, .writeOffset = 1,
        .slot = { { .seqId = 100, .timestamp = 1000, .crop = { 1, 2, 3, 4 } } },
        .frameRate = 0, .isStopShbDraw = 0 };
    bufferQueue->sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO;
    bufferQueue->lppSkipCount_ = 11;

    lppVideoHandler.lppConsumerMap_[0].push_back(consumer_);
    lppVideoHandler.JudgeLppLayer(0);
    // ret == GSERROR_OUT_OF_RANGE
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());

    lppVideoHandler.lppConsumerMap_[0].push_back(consumer_);
    bufferQueue->lppSkipCount_ = 0;
    // ret == GSERROR_OK
    lppVideoHandler.JudgeLppLayer(0);
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());

    // lpp -> uni
    lppVideoHandler.lppRsState_ = LppState::LPP_LAYER;
    lppVideoHandler.lppConsumerMap_[0].push_back(consumer_);
    bufferQueue->lppSkipCount_ = 0;
    lppVideoHandler.JudgeLppLayer(0);
    EXPECT_TRUE(lppVideoHandler.lppConsumerMap_.empty());

    // uni->uni
    lppVideoHandler.lppRsState_ = LppState::UNI_RENDER;
    lppVideoHandler.lppConsumerMap_[0].push_back(consumer_);
    lppVideoHandler.lppConsumerMap_[1].push_back(consumer_);
    bufferQueue->lppSkipCount_ = 0;
    lppVideoHandler.JudgeLppLayer(0);
    EXPECT_FALSE(lppVideoHandler.lppConsumerMap_.empty());
    lppVideoHandler.lppConsumerMap_.clear();
    delete bufferQueue->lppSlotInfo_;
    bufferQueue->lppSlotInfo_ = nullptr;
}
} // namespace OHOS::Rosen
