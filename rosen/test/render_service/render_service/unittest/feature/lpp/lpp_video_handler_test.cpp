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
 * @tc.name: ClearLppSurfaceNode001
 * @tc.desc: test LppVideoHandlerTest.ClearLppSurfaceNode001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, ClearLppSurfaceNode001, TestSize.Level1)
{
    bool hasVirtualMirrorDisplay = false;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.ClearLppSurfaceNode();
    ASSERT_EQ(lppVideoHandler.lowPowerSurfaceNode_.size(), 0);
}
/**
 * @tc.name: JudgeRsDrawLppState001
 * @tc.desc: JudgeRsDrawLppState001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState001, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    bool needDrawFrame = false;
    bool doDirectComposition = false;
 
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.JudgeRsDrawLppState(needDrawFrame, doDirectComposition);
    ASSERT_EQ(needDrawFrame, false);
}
 
/**
 * @tc.name: JudgeRsDrawLppState002
 * @tc.desc: JudgeRsDrawLppState002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState002, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    bool needDrawFrame = false;
    bool doDirectComposition = false;
 
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.lowPowerSurfaceNode_.push_back(nullptr);
    lppVideoHandler.JudgeRsDrawLppState(needDrawFrame, doDirectComposition);
    ASSERT_EQ(needDrawFrame, false);
}
 
/**
 * @tc.name: JudgeRsDrawLppState003
 * @tc.desc: JudgeRsDrawLppState003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState003, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    bool needDrawFrame = false;
    bool doDirectComposition = false;
 
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->SetAbilityState(RSSurfaceNodeAbilityState::BACKGROUND);
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode_);
    lppVideoHandler.JudgeRsDrawLppState(needDrawFrame, doDirectComposition);
    ASSERT_EQ(needDrawFrame, false);
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
 
/**
 * @tc.name: JudgeRsDrawLppState004
 * @tc.desc: JudgeRsDrawLppState004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState004, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    bool needDrawFrame = false;
    bool doDirectComposition = false;
 
    ASSERT_NE(surfaceNode_, nullptr);
    auto surfaceHandler = surfaceNode_->surfaceHandler_;
    surfaceNode_->surfaceHandler_ = nullptr;
    surfaceNode_->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
 
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode_);
    lppVideoHandler.JudgeRsDrawLppState(needDrawFrame, doDirectComposition);
    ASSERT_EQ(needDrawFrame, false);
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    surfaceNode_->surfaceHandler_ = surfaceHandler;
}
 
/**
 * @tc.name: JudgeRsDrawLppState005
 * @tc.desc: JudgeRsDrawLppState005
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState005, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    bool needDrawFrame = false;
    bool doDirectComposition = false;
 
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    surfaceNode_->surfaceHandler_->SetConsumer(nullptr);
 
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode_);
    lppVideoHandler.JudgeRsDrawLppState(needDrawFrame, doDirectComposition);
    ASSERT_EQ(needDrawFrame, false);
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    surfaceNode_->surfaceHandler_->SetConsumer(consumer_);
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
/**
 * @tc.name: JudgeRsDrawLppState006
 * @tc.desc: JudgeRsDrawLppState006
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState006, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
 
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    surfaceNode_->isOnTheTree_ = false;
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    surfaceNode_->SetHardwareForcedDisabledState(true);
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode_);
 
    bool needDrawFrame = false;
    bool doDirectComposition = false;
 
    lppVideoHandler.JudgeRsDrawLppState(needDrawFrame, doDirectComposition);
    ASSERT_EQ(needDrawFrame, false);
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
/**
 * @tc.name: JudgeRsDrawLppState007
 * @tc.desc: JudgeRsDrawLppState007
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState007, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
 
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    surfaceNode_->isOnTheTree_ = false;
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    surfaceNode_->SetHardwareForcedDisabledState(true);
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode_);
 
    bool needDrawFrame = true;
    bool doDirectComposition = false;
    lppVideoHandler.JudgeRsDrawLppState(needDrawFrame, doDirectComposition);
    ASSERT_EQ(needDrawFrame, true);
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
 
/**
 * @tc.name: JudgeRsDrawLppState008
 * @tc.desc: JudgeRsDrawLppState008
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState008, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
 
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    surfaceNode_->isOnTheTree_ = false;
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.hasVirtualMirrorDisplay_.store(true);
    surfaceNode_->SetHardwareForcedDisabledState(false);
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode_);
 
    bool needDrawFrame = true;
    bool doDirectComposition = true;
    lppVideoHandler.JudgeRsDrawLppState(needDrawFrame, doDirectComposition);
    ASSERT_EQ(needDrawFrame, true);
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
 
/**
 * @tc.name: JudgeRsDrawLppState009
 * @tc.desc: JudgeRsDrawLppState009
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState009, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
 
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    surfaceNode_->surfaceHandler_->SetConsumer(consumer_);
    surfaceNode_->isOnTheTree_ = false;
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.hasVirtualMirrorDisplay_.store(true);
    surfaceNode_->SetHardwareForcedDisabledState(false);
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode_);
 
    bool needDrawFrame = true;
    bool doDirectComposition = false;
 
    lppVideoHandler.JudgeRsDrawLppState(needDrawFrame, doDirectComposition);
    ASSERT_EQ(needDrawFrame, true);
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
 
/**
 * @tc.name: JudgeRsDrawLppState010
 * @tc.desc: JudgeRsDrawLppState010
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState010, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
 
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    ASSERT_NE(consumer_, nullptr);
    sptr<BufferQueue>& bufferQueue = consumer_->consumer_->bufferQueue_;
    bufferQueue->sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO;
    bufferQueue->lppSlotInfo_ = new LppSlotInfo {};
    bufferQueue->lppSkipCount_ = 0;
    surfaceNode_->surfaceHandler_->SetConsumer(consumer_);
    surfaceNode_->isOnTheTree_ = false;
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.hasVirtualMirrorDisplay_.store(true);
    surfaceNode_->SetHardwareForcedDisabledState(true);
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode_);
 
    bool needDrawFrame = true;
    bool doDirectComposition = true;
 
    lppVideoHandler.JudgeRsDrawLppState(needDrawFrame, doDirectComposition);
    ASSERT_EQ(needDrawFrame, true);
    lppVideoHandler.lowPowerSurfaceNode_.clear();
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
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.ConsumeAndUpdateLppBuffer(nullptr);
    EXPECT_TRUE(lppVideoHandler.lowPowerSurfaceNode_.empty());
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
    auto surfaceHandler = surfaceNode_->surfaceHandler_;
    surfaceNode_->surfaceHandler_ = nullptr;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lowPowerSurfaceNode_.clear();
 
    lppVideoHandler.ConsumeAndUpdateLppBuffer(surfaceNode_);
    EXPECT_TRUE(lppVideoHandler.lowPowerSurfaceNode_.empty());
    surfaceNode_->surfaceHandler_ = surfaceHandler;
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
 
/**
 * @tc.name: ConsumeAndUpdateLppBuffer003
 * @tc.desc: ConsumeAndUpdateLppBuffer003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, ConsumeAndUpdateLppBuffer003, TestSize.Level1)
{
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->surfaceHandler_->sourceType_ = static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT);
 
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.ConsumeAndUpdateLppBuffer(surfaceNode_);
    EXPECT_TRUE(lppVideoHandler.lowPowerSurfaceNode_.empty());
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
 
/**
 * @tc.name: ConsumeAndUpdateLppBuffer004
 * @tc.desc: ConsumeAndUpdateLppBuffer004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, ConsumeAndUpdateLppBuffer004, TestSize.Level1)
{
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->surfaceHandler_->sourceType_ =
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO);
    surfaceNode_->surfaceHandler_->SetConsumer(nullptr);
 
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.ConsumeAndUpdateLppBuffer(surfaceNode_);
    EXPECT_TRUE(lppVideoHandler.lowPowerSurfaceNode_.empty());
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    surfaceNode_->surfaceHandler_->SetConsumer(consumer_);
}
 
/**
 * @tc.name: ConsumeAndUpdateLppBuffer005
 * @tc.desc: ConsumeAndUpdateLppBuffer005
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, ConsumeAndUpdateLppBuffer005, TestSize.Level1)
{
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->surfaceHandler_->sourceType_ =
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO);
    ASSERT_NE(consumer_, nullptr);
    auto& bufferQueue = consumer_->consumer_->bufferQueue_;
    bufferQueue->sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT;
 
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.ConsumeAndUpdateLppBuffer(surfaceNode_);
    EXPECT_FALSE(lppVideoHandler.lowPowerSurfaceNode_.empty());
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
 
/**
 * @tc.name: ConsumeAndUpdateLppBuffer006
 * @tc.desc: ConsumeAndUpdateLppBuffer006
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, ConsumeAndUpdateLppBuffer006, TestSize.Level1)
{
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->surfaceHandler_->sourceType_ =
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO);
    ASSERT_NE(consumer_, nullptr);
    auto& bufferQueue = consumer_->consumer_->bufferQueue_;
    bufferQueue->sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO;
    bufferQueue->lppSlotInfo_ = new LppSlotInfo { .readOffset = 0,
        .writeOffset = 1,
        .slot = { { .seqId = 100, .timestamp = 1000, .crop = { 1, 2, 3, 4 } } },
        .frameRate = 0,
        .isStopShbDraw = 0 };
    bufferQueue->isRsDrawLpp_ = false;
    auto buffer = SurfaceBuffer::Create();
    BufferHandle handle;
    handle.width = 100;
    handle.height = 100;
    buffer->SetBufferHandle(&handle);
    bufferQueue->lppBufferCache_[100] = buffer;
 
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.ConsumeAndUpdateLppBuffer(surfaceNode_);
    EXPECT_FALSE(lppVideoHandler.lowPowerSurfaceNode_.empty());
 
    bufferQueue->lppSlotInfo_->slot[0].crop[0] = -1;
    bufferQueue->lppSlotInfo_->slot[0].crop[1] = -1;
    bufferQueue->lppSlotInfo_->slot[0].crop[2] = -1;
    bufferQueue->lppSlotInfo_->slot[0].crop[3] = -1;
    handle.width = -1;
    handle.height = -1;
    buffer->SetBufferHandle(&handle);
    bufferQueue->lppBufferCache_[100] = buffer;
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.ConsumeAndUpdateLppBuffer(surfaceNode_);
    EXPECT_FALSE(lppVideoHandler.lowPowerSurfaceNode_.empty());
    delete bufferQueue->lppSlotInfo_;
    bufferQueue->lppSlotInfo_ = nullptr;
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
 
/**
 * @tc.name: ConsumeAndUpdateLppBuffer007
 * @tc.desc: ConsumeAndUpdateLppBuffer007
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, ConsumeAndUpdateLppBuffer007, TestSize.Level1)
{
    ASSERT_NE(surfaceNode_, nullptr);
    surfaceNode_->surfaceHandler_->sourceType_ =
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO);
    ASSERT_NE(consumer_, nullptr);
    auto& bufferQueue = consumer_->consumer_->bufferQueue_;
    bufferQueue->sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO;
    bufferQueue->lppSlotInfo_ = new LppSlotInfo { .readOffset = 0,
        .writeOffset = 1,
        .slot = { { .seqId = 100, .timestamp = 1000, .crop = { -1, -1, -1, -1 } } },
        .frameRate = 0,
        .isStopShbDraw = 0 };
    bufferQueue->isRsDrawLpp_ = false;
    auto buffer = SurfaceBuffer::Create();
    BufferHandle handle;
    handle.width = -1;
    handle.height = -1;
    buffer->SetBufferHandle(&handle);
    bufferQueue->lppBufferCache_[100] = buffer;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.ConsumeAndUpdateLppBuffer(surfaceNode_);
    EXPECT_FALSE(lppVideoHandler.lowPowerSurfaceNode_.empty());
    delete bufferQueue->lppSlotInfo_;
    bufferQueue->lppSlotInfo_ = nullptr;
    lppVideoHandler.lowPowerSurfaceNode_.clear();
}
} // namespace OHOS::Rosen