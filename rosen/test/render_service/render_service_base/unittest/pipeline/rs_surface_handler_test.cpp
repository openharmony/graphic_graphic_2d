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
#include "pipeline/rs_surface_handler.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceHandlerTest : public testing::Test, public IBufferConsumerListenerClazz {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
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

} // namespace OHOS::Rosen