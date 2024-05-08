/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "platform/common/rs_log.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceHandlerTest : public testing::Test, public IBufferConsumerListenerClazz {
public:
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    RSSurfaceHandler::SurfaceBufferEntry RequestAndFlushBuffer();
    void SetUp() override;
    void TearDown() override;
    void OnBufferAvailable() override {}
private:
    std::unique_ptr<RSSurfaceHandler> rSSurfaceHandlerPtr_;
    sptr<IConsumerSurface> consumerSurfacePtr_;
    sptr<IBufferProducer> bufferProducerPtr_;
    sptr<Surface> surfacePtr_;
    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
};
void RSSurfaceHandlerTest::SetUp()
{
    NodeId id = 1;
    rSSurfaceHandlerPtr_ = std::make_unique<RSSurfaceHandler>(id);
    consumerSurfacePtr_ = IConsumerSurface::Create();
    consumerSurfacePtr_->RegisterConsumerListener(this);
    bufferProducerPtr_ = consumerSurfacePtr_->GetProducer();
    surfacePtr_ = Surface::CreateSurfaceAsProducer(bufferProducerPtr_);
    rSSurfaceHandlerPtr_->SetConsumer(consumerSurfacePtr_);
}
void RSSurfaceHandlerTest::TearDown() {}

static inline void BufferDeleteCbFunc(int32_t seqNum)
{
    ROSEN_LOGI("%{public}s:%{public}d seqNum=%{public}d", __func__, __LINE__, seqNum);
};

RSSurfaceHandler::SurfaceBufferEntry RSSurfaceHandlerTest::RequestAndFlushBuffer()
{
    RSSurfaceHandler::SurfaceBufferEntry buffer;
    if (!surfacePtr_) {
        return buffer;
    }

    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    [[maybe_unused]] GSError ret = surfacePtr_->RequestBuffer(buffer.buffer, requestFence, requestConfig);

    sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
    ret = surfacePtr_->FlushBuffer(buffer.buffer, flushFence, flushConfig);

    return buffer;
}

/**
 * @tc.name: SetDefaultWidthAndHeight001
 * @tc.desc: test set default width and height
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(RSSurfaceHandlerTest, SetDefaultWidthAndHeight001, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    int32_t width = 1920;
    int32_t height = 1024;
    rSSurfaceHandlerPtr_->SetDefaultWidthAndHeight(width, height);
    EXPECT_EQ(rSSurfaceHandlerPtr_->consumer_->GetDefaultWidth(), width);
    EXPECT_EQ(rSSurfaceHandlerPtr_->consumer_->GetDefaultHeight(), height);
#endif
}

/**
 * @tc.name: SetBufferSizeChanged001
 * @tc.desc: test set buffer size changed
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(RSSurfaceHandlerTest, SetBufferSizeChanged001, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence = 0;
    int64_t timestamp = 0;
    Rect damage = {0};
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    // case 1: rSSurfaceHandlerPtr_->preBuffer_->buffer is nullptr
    auto ret = surfacePtr_->RequestBuffer(buffer, releaseFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    rSSurfaceHandlerPtr_->SetBuffer(buffer, acquireFence, damage, timestamp);
    rSSurfaceHandlerPtr_->SetBufferSizeChanged(buffer);
    EXPECT_TRUE(!rSSurfaceHandlerPtr_->bufferSizeChanged_);
    // case 2: rSSurfaceHandlerPtr_->preBuffer_->buffer isn't nullptr
    requestConfig.width = 0x200;
    ret = surfacePtr_->RequestBuffer(buffer, releaseFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    rSSurfaceHandlerPtr_->SetBuffer(buffer, acquireFence, damage, timestamp);
    rSSurfaceHandlerPtr_->SetBufferSizeChanged(buffer);
    EXPECT_TRUE(rSSurfaceHandlerPtr_->bufferSizeChanged_);
    rSSurfaceHandlerPtr_->RegisterDeleteBufferListener(BufferDeleteCbFunc);
#endif
}

/**
 * @tc.name: RegisterDeleteBufferListener001
 * @tc.desc: test register delete buffer listener and 'cleanCache()'
 * @tc.type: FUNC
 * @tc.require: issueI9IUKU
 */
HWTEST_F(RSSurfaceHandlerTest, RegisterDeleteBufferListener001, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    int32_t releaseFence = 0;
    auto ret = surfacePtr_->RequestBuffer(rSSurfaceHandlerPtr_->preBuffer_.buffer, releaseFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    rSSurfaceHandlerPtr_->RegisterDeleteBufferListener(BufferDeleteCbFunc);
    EXPECT_TRUE(rSSurfaceHandlerPtr_->preBuffer_.bufferDeleteCb_ != nullptr);
    rSSurfaceHandlerPtr_->CleanCache();
#endif
}

#ifndef ROSEN_CROSS_PLATFORM
/**
 * @tc.name: ReleaseBuffer001
 * @tc.desc: test ReleaseBuffer
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSSurfaceHandlerTest, ReleaseBuffer001, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer = RequestAndFlushBuffer();
    ASSERT_NE(rSSurfaceHandlerPtr_, nullptr);
    rSSurfaceHandlerPtr_->ReleaseBuffer(buffer);
    ASSERT_EQ(buffer.buffer, nullptr);
}

/**
 * @tc.name: ConsumeAndUpdateBuffer001
 * @tc.desc: test ConsumeAndUpdateBuffer while buffer is empty
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSSurfaceHandlerTest, ConsumeAndUpdateBuffer001, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer;
    ASSERT_NE(rSSurfaceHandlerPtr_, nullptr);
    rSSurfaceHandlerPtr_->ConsumeAndUpdateBuffer(buffer);
    ASSERT_FALSE(rSSurfaceHandlerPtr_->IsCurrentFrameBufferConsumed());
    rSSurfaceHandlerPtr_->ReleaseBuffer(buffer);
}

/**
 * @tc.name: ConsumeAndUpdateBuffer002
 * @tc.desc: test ConsumeAndUpdateBuffer while buffer isn't empty
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSSurfaceHandlerTest, ConsumeAndUpdateBuffer002, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer = RequestAndFlushBuffer();
    ASSERT_NE(rSSurfaceHandlerPtr_, nullptr);
    rSSurfaceHandlerPtr_->ConsumeAndUpdateBuffer(buffer);
    ASSERT_TRUE(rSSurfaceHandlerPtr_->IsCurrentFrameBufferConsumed());
    rSSurfaceHandlerPtr_->ReleaseBuffer(buffer);
}

/**
 * @tc.name: CacheBuffer001
 * @tc.desc: test CacheBuffer for add cache
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSSurfaceHandlerTest, CacheBuffer01, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer;
    ASSERT_NE(rSSurfaceHandlerPtr_, nullptr);
    rSSurfaceHandlerPtr_->CacheBuffer(buffer);
    ASSERT_TRUE(rSSurfaceHandlerPtr_->HasBufferCache());
}

/**
 * @tc.name: GetBufferFromCache001
 * @tc.desc: test GetBufferFromCache while no cache satisfy render time
 * @tc.type:FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSSurfaceHandlerTest, GetBufferFromCache001, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer1 = RequestAndFlushBuffer();
    RSSurfaceHandler::SurfaceBufferEntry buffer2 = RequestAndFlushBuffer();
    buffer1.timestamp = 100; // timestamps of two buffers can be any different number
    buffer2.timestamp = 200;

    ASSERT_NE(rSSurfaceHandlerPtr_, nullptr);
    rSSurfaceHandlerPtr_->bufferCache_.clear();
    rSSurfaceHandlerPtr_->CacheBuffer(buffer1);
    rSSurfaceHandlerPtr_->CacheBuffer(buffer2);

    uint64_t vsyncTimestamp = 50; // let vsyncTimestamp's timestamp smller than any buffer in cache
    ASSERT_EQ(rSSurfaceHandlerPtr_->GetBufferFromCache(vsyncTimestamp).buffer, nullptr);

    rSSurfaceHandlerPtr_->ReleaseBuffer(buffer1);
    rSSurfaceHandlerPtr_->ReleaseBuffer(buffer2);
}

/**
 * @tc.name: GetBufferFromCache002
 * @tc.desc: test GetBufferFromCache while only one cache satisfy render time
 * @tc.type:FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSSurfaceHandlerTest, GetBufferFromCache002, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer1 = RequestAndFlushBuffer();
    RSSurfaceHandler::SurfaceBufferEntry buffer2 = RequestAndFlushBuffer();
    buffer1.timestamp = 100; // timestamps of two buffers can be any different number
    buffer2.timestamp = 200;

    ASSERT_NE(rSSurfaceHandlerPtr_, nullptr);
    rSSurfaceHandlerPtr_->bufferCache_.clear();
    rSSurfaceHandlerPtr_->CacheBuffer(buffer1);
    rSSurfaceHandlerPtr_->CacheBuffer(buffer2);

    uint64_t vsyncTimestamp = 150; // let vsyncTimestamp's timestamp only bigger than one buffer in cache
    ASSERT_EQ(rSSurfaceHandlerPtr_->GetBufferFromCache(vsyncTimestamp).buffer, buffer1.buffer);

    rSSurfaceHandlerPtr_->ReleaseBuffer(buffer1);
    rSSurfaceHandlerPtr_->ReleaseBuffer(buffer2);
}

/**
 * @tc.name: GetBufferFromCache003
 * @tc.desc: test GetBufferFromCache while more than one cache satisfy render time
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSSurfaceHandlerTest, GetBufferFromCache003, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer1 = RequestAndFlushBuffer();
    RSSurfaceHandler::SurfaceBufferEntry buffer2 = RequestAndFlushBuffer();
    buffer1.timestamp = 100; // timestamps of two buffers can be any different number
    buffer2.timestamp = 200;

    ASSERT_NE(rSSurfaceHandlerPtr_, nullptr);
    rSSurfaceHandlerPtr_->bufferCache_.clear();
    rSSurfaceHandlerPtr_->CacheBuffer(buffer1);
    rSSurfaceHandlerPtr_->CacheBuffer(buffer2);

    uint64_t vsyncTimestamp = 250; // let vsyncTimestamp's timestamp bigger than more than one buffer in cache
    ASSERT_EQ(rSSurfaceHandlerPtr_->GetBufferFromCache(vsyncTimestamp).buffer, buffer2.buffer);

    rSSurfaceHandlerPtr_->ReleaseBuffer(buffer1);
    rSSurfaceHandlerPtr_->ReleaseBuffer(buffer2);
}
#endif
} // namespace OHOS::Rosen