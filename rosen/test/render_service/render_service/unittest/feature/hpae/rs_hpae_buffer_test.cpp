/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common/rs_common_def.h"
#include "gtest/gtest.h"
#include "feature/hpae/rs_hpae_buffer.h"
#include "pipeline/rs_surface_handler.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_irender_client.h"
#include "feature/hpae/rs_hpae_render_listener.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "iconsumer_surface.h"
#include "surface_buffer_impl.h"
#include "feature/mock/mock_iconsumer_surface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSHpaeBufferTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHpaeBufferTest::SetUpTestCase()
{
}

void RSHpaeBufferTest::TearDownTestCase()
{}
void RSHpaeBufferTest::SetUp()
{}
void RSHpaeBufferTest::TearDown()
{}

BufferRequestConfig bufferConfig = {.width = 1000,
    .height = 2000,
    .strideAlignment = 0,
    .format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP,
    .usage = 0x1234567890,
    .timeout = 0,
    .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3,
    .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE};

/**
 * @tc.name: GetBufferHandleTest
 * @tc.desc: Test GetBufferHandle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBufferTest, GetBufferHandleTest, TestSize.Level1)
{
    auto hpaeBuffer0 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 555);
    ASSERT_EQ(hpaeBuffer0->GetBufferHandle(), nullptr);

    auto hpaeBuffer = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 111);
    auto consumer= IConsumerSurface::Create("layer00");
    auto producer = consumer->GetProducer();
    auto producerSurface = Surface::CreateSurfaceAsProducer(producer);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    auto surface = client->CreateRSSurface(producerSurface);
    auto rsSurface = std::static_pointer_cast<RSSurfaceOhos>(surface);
    hpaeBuffer->rsSurface_ = rsSurface;
    ASSERT_EQ(hpaeBuffer->GetBufferHandle(), nullptr);
}

/**
 * @tc.name: RequestFrameTest
 * @tc.desc: Test RequestFrame
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBufferTest, RequestFrameTest, TestSize.Level1)
{
    auto hpaeBuffer1 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 666);
    ASSERT_EQ(hpaeBuffer1->RequestFrame(bufferConfig, true), nullptr);

    hpaeBuffer1->grContext_ = nullptr;
    ASSERT_EQ(hpaeBuffer1->RequestFrame(bufferConfig, true), nullptr);

    auto consumer = IConsumerSurface::Create("layer01");
    auto producer = consumer->GetProducer();
    auto producerSurface = Surface::CreateSurfaceAsProducer(producer);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    auto surface = client->CreateRSSurface(producerSurface);
    auto rsSurface = std::static_pointer_cast<RSSurfaceOhos>(surface);
    auto hpaeBuffer3 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 22);
    hpaeBuffer3->rsSurface_ = rsSurface;
    hpaeBuffer3->surfaceCreated_= false;
    std::shared_ptr<Drawing::GPUContext> gpuContext = std::make_shared<Drawing::GPUContext>();
    auto renderEngine = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    hpaeBuffer3->grContext_ = nullptr;
    hpaeBuffer3->RequestFrame(bufferConfig, false);
    hpaeBuffer3->grContext_ = gpuContext;
    hpaeBuffer3->RequestFrame(bufferConfig, false);
    hpaeBuffer3->surfaceCreated_= true;
    hpaeBuffer3->RequestFrame(bufferConfig, false);
    hpaeBuffer3->rsSurface_ = nullptr;
    hpaeBuffer3->RequestFrame(bufferConfig, false);
    renderEngine->Init();
    hpaeBuffer3->RequestFrame(bufferConfig, false);
    hpaeBuffer3->surfaceCreated_= false;
    ASSERT_NE(hpaeBuffer3->grContext_, nullptr);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: FlushFrameTest
 * @tc.desc: Test FlushFrame
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBufferTest, FlushFrameTest, TestSize.Level1)
{
    auto hpaeBuffer1 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 33);
    ASSERT_EQ(hpaeBuffer1->FlushFrame(), false);

    auto hpaeBuffer2 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 44);
    auto consumer = IConsumerSurface::Create("layer1");
    auto producer = consumer->GetProducer();
    auto producerSurface = Surface::CreateSurfaceAsProducer(producer);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    auto surface = client->CreateRSSurface(producerSurface);
    auto rsSurface = std::static_pointer_cast<RSSurfaceOhos>(surface);
    hpaeBuffer2->rsSurface_ = rsSurface;
    hpaeBuffer2->producerSurface_ = producerSurface;
    ASSERT_EQ(hpaeBuffer2->FlushFrame(), false);
    hpaeBuffer2->surfaceHandler_ = nullptr;
    ASSERT_EQ(hpaeBuffer2->FlushFrame(), false);
}

/**
 * @tc.name: ForceDropFrameTest
 * @tc.desc: Test ForceDropFrame
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBufferTest, ForceDropFrameTest, TestSize.Level1)
{
    auto hpaeBuffer1 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer1", 222);
    ASSERT_EQ(hpaeBuffer1->ForceDropFrame(100), OHOS::GSERROR_NOT_INIT);
    auto hpaeBuffer2 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer2", 333);
    hpaeBuffer2->surfaceHandler_ = nullptr;
    ASSERT_EQ(hpaeBuffer2->ForceDropFrame(100), OHOS::GSERROR_NOT_INIT);

    auto hpaeBuffer3 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer3", 444);
    std::unique_ptr<Mock::MockIConsumerSurface> mock3 = std::make_unique<Mock::MockIConsumerSurface>();
    sptr<Mock::MockIConsumerSurface> mockSptr3 = mock3.release();
    EXPECT_CALL(*mockSptr3, AcquireBuffer(_, _, _)).WillRepeatedly(testing::Return(OHOS::SURFACE_ERROR_OK));
    EXPECT_CALL(*mockSptr3, ReleaseBuffer(_, _)).WillRepeatedly(testing::Return(OHOS::SURFACE_ERROR_OK));
    hpaeBuffer3->surfaceHandler_->SetConsumer(mockSptr3);
    ASSERT_NE(hpaeBuffer3->ForceDropFrame(100), true);

    auto hpaeBuffer4 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer4", 4444);
    std::unique_ptr<Mock::MockIConsumerSurface> mock4 = std::make_unique<Mock::MockIConsumerSurface>();
    sptr<Mock::MockIConsumerSurface> mockSptr4 = mock4.release();
    EXPECT_CALL(*mockSptr4, AcquireBuffer(_, _, _)).WillRepeatedly(testing::Return(SURFACE_ERROR_OK));
    EXPECT_CALL(*mockSptr4, ReleaseBuffer(_, _)).WillRepeatedly(testing::Return(GSERROR_NO_BUFFER));
    hpaeBuffer4->surfaceHandler_->SetConsumer(mockSptr4);
    ASSERT_NE(hpaeBuffer4->ForceDropFrame(100), GSERROR_OK);

    auto hpaeBuffer5 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer5", 5555);
    std::unique_ptr<Mock::MockIConsumerSurface> mock5 = std::make_unique<Mock::MockIConsumerSurface>();
    sptr<Mock::MockIConsumerSurface> mockSptr5 = mock5.release();
    EXPECT_CALL(*mockSptr5, AcquireBuffer(_, _, _)).WillRepeatedly(testing::Return(GSERROR_NO_BUFFER));
    EXPECT_CALL(*mockSptr5, ReleaseBuffer(_, _)).WillRepeatedly(testing::Return(GSERROR_NO_BUFFER));
    hpaeBuffer5->surfaceHandler_->SetConsumer(mockSptr5);
    ASSERT_NE(hpaeBuffer5->ForceDropFrame(100), false);
}

/**
 * @tc.name: CreateSurfaceTest
 * @tc.desc: Test CreateSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeBufferTest, CreateSurfaceTest, TestSize.Level1)
{
    auto hpaeBuffer0 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 55);
    auto consumer0 = IConsumerSurface::Create("layer000");
    hpaeBuffer0->surfaceHandler_->SetConsumer(consumer0);
    hpaeBuffer0->rsSurface_ = nullptr;
    sptr<IBufferConsumerListener> listener0 = new RSHpaeRenderListener(hpaeBuffer0->surfaceHandler_);
    ASSERT_NE(hpaeBuffer0->CreateSurface(listener0), false);

    auto hpaeBuffer1 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 66);
    sptr<IBufferConsumerListener> listener1 = new RSHpaeRenderListener(hpaeBuffer1->surfaceHandler_);
    ASSERT_NE(hpaeBuffer1->CreateSurface(listener1), false);

    auto hpaeBuffer2 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 77);
    auto consumer2 = IConsumerSurface::Create("layer002");
    auto producer = consumer2->GetProducer();
    auto producerSurface = Surface::CreateSurfaceAsProducer(producer);
    auto client = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    auto surface = client->CreateRSSurface(producerSurface);
    auto rsSurface2 = std::static_pointer_cast<RSSurfaceOhos>(surface);
    hpaeBuffer2->rsSurface_ = rsSurface2;
    sptr<IBufferConsumerListener> listener2 = new RSHpaeRenderListener(hpaeBuffer2->surfaceHandler_);
    ASSERT_EQ(hpaeBuffer2->CreateSurface(listener2), true);

    auto hpaeBuffer3 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 88);
    auto consumer3 = IConsumerSurface::Create("layer003");
    auto producer3 = consumer3->GetProducer();
    auto producerSurface3 = Surface::CreateSurfaceAsProducer(producer3);
    auto client3 = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    auto surface3 = client3->CreateRSSurface(producerSurface3);
    auto rsSurface3 = std::static_pointer_cast<RSSurfaceOhos>(surface3);
    hpaeBuffer3->surfaceHandler_->SetConsumer(consumer3);
    hpaeBuffer3->rsSurface_ = rsSurface3;
    sptr<IBufferConsumerListener> listener4 = new RSHpaeRenderListener(hpaeBuffer3->surfaceHandler_);
    ASSERT_EQ(hpaeBuffer3->CreateSurface(listener4), true);

    std::unique_ptr<Mock::MockIConsumerSurface> mock5 = std::make_unique<Mock::MockIConsumerSurface>();
    sptr<Mock::MockIConsumerSurface> mockSptr5 = mock5.release();
    EXPECT_CALL(*mockSptr5, RegisterConsumerListener(_)).WillRepeatedly(testing::Return(GSERROR_NO_BUFFER));
    EXPECT_CALL(*mockSptr5, Create(_)).WillRepeatedly(testing::Return(nullptr));

    auto hpaeBuffer5 = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 99);
    auto consumer5 = IConsumerSurface::Create("layer005");
    auto producer5 = consumer5->GetProducer();
    auto producerSurface5 = Surface::CreateSurfaceAsProducer(producer5);
    auto client5 = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    auto surface5 = client5->CreateRSSurface(producerSurface5);
    auto rsSurface5 = std::static_pointer_cast<RSSurfaceOhos>(surface5);
    hpaeBuffer5->surfaceHandler_->SetConsumer(mockSptr5);
    hpaeBuffer5->rsSurface_ = rsSurface5;
    sptr<IBufferConsumerListener> listener5 = new RSHpaeRenderListener(hpaeBuffer5->surfaceHandler_);
    ASSERT_EQ(hpaeBuffer5->CreateSurface(listener5), true);

    EXPECT_CALL(*mockSptr5, Create(_)).WillRepeatedly(testing::Return(mockSptr5));
    ASSERT_EQ(hpaeBuffer5->CreateSurface(listener5), true);
    delete mockSptr5;
}

} // namespace OHOS::Rosen