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

#include <filesystem>
#include "gtest/gtest.h"
#include "common/rs_singleton.h"
#include "params/rs_rcd_render_params.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_rcd_render_listener.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node_drawable.h"
#include "pipeline/rs_display_render_node.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRcdSurfaceRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
};

void RSRcdSurfaceRenderNodeDrawableTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RSRcdSurfaceRenderNodeDrawableTest::TearDownTestCase() {}
void RSRcdSurfaceRenderNodeDrawableTest::SetUp() {}
void RSRcdSurfaceRenderNodeDrawableTest::TearDown() {}

struct XMLProperty {
    std::string name;
    std::string value;
};

void InitRcdRenderParams(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("/sys_prod/etc/display/RoundCornerDisplay/test.bin");
    rcdParams->SetBufferSize(1);
    rcdParams->SetCldWidth(1);
    rcdParams->SetCldHeight(1);
    rcdParams->SetSrcRect(RectI(0, 0, 1, 1));
    rcdParams->SetDstRect(RectI(0, 0, 1, 1));
    rcdParams->SetRcdBitmap(std::make_shared<Drawing::Bitmap>());
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

void InitRcdRenderParamsInvalid01(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("/sys_prod/etc/display/RoundCornerDisplay/test.bin");
    rcdParams->SetBufferSize(-1);  // invalid buffer size
    rcdParams->SetCldWidth(1);
    rcdParams->SetCldHeight(1);
    rcdParams->SetSrcRect(RectI(0, 0, 1, 1));
    rcdParams->SetDstRect(RectI(0, 0, 1, 1));
    rcdParams->SetRcdBitmap(std::make_shared<Drawing::Bitmap>());
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

void InitRcdRenderParamsInvalid02(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("/sys_prod/etc/display/RoundCornerDisplay/test.bin");
    rcdParams->SetBufferSize(1);
    rcdParams->SetCldWidth(-1);  // invalid cld height
    rcdParams->SetCldHeight(1);
    rcdParams->SetSrcRect(RectI(0, 0, 1, 1));
    rcdParams->SetDstRect(RectI(0, 0, 1, 1));
    rcdParams->SetRcdBitmap(std::make_shared<Drawing::Bitmap>());
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

void InitRcdRenderParamsInvalid03(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("/sys_prod/etc/display/RoundCornerDisplay/test.bin");
    rcdParams->SetBufferSize(1);
    rcdParams->SetCldWidth(1);
    rcdParams->SetCldHeight(1);
    rcdParams->SetSrcRect(RectI(0, 0, 1, 1));
    rcdParams->SetDstRect(RectI(0, 0, 1, 1));
    rcdParams->SetRcdBitmap(nullptr);  // invalid bitmap
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

void InitRcdRenderParamsInvalid04(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("");  // invalid path
    rcdParams->SetBufferSize(1);
    rcdParams->SetCldWidth(1);
    rcdParams->SetCldHeight(1);
    rcdParams->SetSrcRect(RectI(0, 0, 1, 1));
    rcdParams->SetDstRect(RectI(0, 0, 1, 1));
    rcdParams->SetRcdBitmap(std::make_shared<Drawing::Bitmap>());
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

void InitRcdRenderParamsInvalid05(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("");
    rcdParams->SetBufferSize(1);
    rcdParams->SetCldWidth(1);
    rcdParams->SetCldHeight(1);
    rcdParams->SetSrcRect(RectI(0, 0, -1, -1));  // invalid src rect
    rcdParams->SetDstRect(RectI(0, 0, -1, -1));  // invalid dst rect
    rcdParams->SetRcdBitmap(std::make_shared<Drawing::Bitmap>());
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

void InitRcdRenderParamsInvalid06(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("");
    rcdParams->SetBufferSize(1); // invalid buffer size
    rcdParams->SetCldWidth(1);   // invalid cld width
    rcdParams->SetCldHeight(1);  // invalid cld height
    rcdParams->SetSrcRect(RectI(0, 0, 1, 1));
    rcdParams->SetDstRect(RectI(0, 0, 1, 1));
    rcdParams->SetRcdBitmap(std::make_shared<Drawing::Bitmap>());
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

void InitRcdRenderParamsInvalid07(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("");  // invalid path
    rcdParams->SetBufferSize(1);
    rcdParams->SetCldWidth(1);
    rcdParams->SetCldHeight(1);
    rcdParams->SetSrcRect(RectI(0, 0, 1, 1));
    rcdParams->SetDstRect(RectI(0, 0, 1, 1));
    rcdParams->SetRcdBitmap(nullptr);  // invalid bitmap
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

void InitRcdRenderParamsInvalid08(RSRenderParams* params)
{
    if (params == nullptr) {
        return;
    }
    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetPathBin("");                   // Invalid path
    rcdParams->SetBufferSize(1);                 // invalid buffer size
    rcdParams->SetCldWidth(1);                   // invalid cld width
    rcdParams->SetCldHeight(1);                  // invalid cld height
    rcdParams->SetSrcRect(RectI(0, 0, -1, -1));  // invalid src rect
    rcdParams->SetDstRect(RectI(0, 0, -1, -1));  // invalid dst rect
    rcdParams->SetRcdBitmap(nullptr);            // invalid bitmap
    rcdParams->SetRcdEnabled(true);
    rcdParams->SetResourceChanged(false);
}

std::shared_ptr<Drawing::Bitmap> LoadBitmapFromFile(const char* path)
{
    if (path == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Bitmap> bitmap;
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    rcdInstance.LoadImg(path, image);
    if (image == nullptr) {
        std::cout << "LoadBitmapFromFile: current os no rcd source" << std::endl;
        return nullptr;
    }

    rcdInstance.DecodeBitmap(image, bitmap);
    return bitmap;
}

void SetBitmapToRcdRenderParams(RSRenderParams *params, const char* path)
{
    if (params == nullptr || path == nullptr) {
        return;
    }

    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetRcdBitmap(nullptr);

    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Bitmap> bitmap;
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    rcdInstance.LoadImg(path, image);
    if (image == nullptr) {
        std::cout << "RSRcdSurfaceRenderNodeDrawableTest: current os no rcd source" << std::endl;
        return;
    }
    rcdInstance.DecodeBitmap(image, bitmap);

    rcdParams->SetSrcRect(RectI(0, 0, bitmap->GetWidth(), bitmap->GetHeight()));
    rcdParams->SetDstRect(RectI(0, 0, bitmap->GetWidth(), bitmap->GetHeight()));
    rcdParams->SetRcdBitmap(bitmap);
}

std::shared_ptr<DrawableV2::RSRcdSurfaceRenderNodeDrawable> GetRcdTestDrawable(
    RCDSurfaceType type = RCDSurfaceType::BOTTOM)
{
    auto node = RSRcdSurfaceRenderNode::Create(0, type);
    if (node == nullptr) {
        return nullptr;
    }
    return std::static_pointer_cast<DrawableV2::RSRcdSurfaceRenderNodeDrawable>(node->renderDrawable_);
}

/*
 * @tc.name: RCDDrawablePrepareResource
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.RCDDrawablePrepareResource
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, RCDDrawablePrepareResource, TestSize.Level1)
{
    auto bottomDrawable = GetRcdTestDrawable();
    EXPECT_NE(bottomDrawable, nullptr);
    auto surfaceHandler = bottomDrawable->surfaceHandler_;
    bottomDrawable->surfaceHandler_ = nullptr;
    bool res = bottomDrawable->PrepareResourceBuffer(); // No surfaceHanlder failure path
    EXPECT_TRUE(!res && !bottomDrawable->IsSurfaceCreated() && bottomDrawable->GetRSSurface() == nullptr &&
        bottomDrawable->GetConsumerListener() == nullptr);
    bottomDrawable->surfaceHandler_ = surfaceHandler;
    res = bottomDrawable->PrepareResourceBuffer(); // CreateSurface success path
    EXPECT_TRUE(res && bottomDrawable->IsSurfaceCreated() && bottomDrawable->GetRSSurface() != nullptr &&
        bottomDrawable->GetConsumerListener() != nullptr);
    res = bottomDrawable->PrepareResourceBuffer();  // Surface already created path
    EXPECT_TRUE(res);
}

/*
 * @tc.name: RCDDrawableCreateSurface
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.RCDDrawableCreateSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, RCDDrawableCreateSurface, TestSize.Level1)
{
    auto topDrawable = GetRcdTestDrawable(RCDSurfaceType::TOP);
    EXPECT_NE(topDrawable, nullptr);
    auto bottomDrawable = GetRcdTestDrawable(RCDSurfaceType::BOTTOM);
    EXPECT_NE(bottomDrawable, nullptr);
    sptr<IBufferConsumerListener> topListener = new RSRcdRenderListener(topDrawable->surfaceHandler_);
    sptr<IBufferConsumerListener> bottomListener = new RSRcdRenderListener(bottomDrawable->surfaceHandler_);
    EXPECT_TRUE(topDrawable->CreateSurface(topListener));
    EXPECT_TRUE(bottomDrawable->CreateSurface(bottomListener));
}

/*
 * @tc.name: RCDDrawableSetterGetter
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.RCDDrawableSetterGetter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, RCDDrawableSetterGetter, TestSize.Level1)
{
    auto bottomDrawable = GetRcdTestDrawable();
    EXPECT_NE(bottomDrawable, nullptr);
    InitRcdRenderParams(bottomDrawable->renderParams_.get());
    EXPECT_EQ(bottomDrawable->GetRenderTargetId(), 0);
    EXPECT_NE(bottomDrawable->GetRcdBitmap(), nullptr);
    EXPECT_EQ(bottomDrawable->GetRcdBufferWidth(), 0);
    EXPECT_EQ(bottomDrawable->GetRcdBufferHeight(), 0);
    EXPECT_EQ(bottomDrawable->GetRcdBufferSize(), 1);
    EXPECT_EQ(bottomDrawable->IsRcdEnabled(), true);
    EXPECT_EQ(bottomDrawable->IsResourceChanged(), false);
    auto config = bottomDrawable->GetHardenBufferRequestConfig();
    EXPECT_NE(config.timeout, 0);

    auto params = std::move(bottomDrawable->renderParams_);
    EXPECT_EQ(bottomDrawable->GetRcdBitmap(), nullptr);
    EXPECT_EQ(bottomDrawable->GetRcdBufferSize(), 0);
    EXPECT_EQ(bottomDrawable->IsRcdEnabled(), false);
    EXPECT_EQ(bottomDrawable->IsResourceChanged(), false);
    bottomDrawable->renderParams_ = std::move(params);
    config = bottomDrawable->GetHardenBufferRequestConfig();
    EXPECT_NE(config.timeout, 0);
}

/*
 * @tc.name: RCDDrawableInvalidParameters
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.RCDDrawableInvalidParameters
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, RCDDrawableInvalidParameters, TestSize.Level1)
{
    auto rcdDrawable = GetRcdTestDrawable();

    auto surfaceBuffer = new SurfaceBufferImpl();
    rcdDrawable->surfaceHandler_->buffer_.buffer = surfaceBuffer;

    InitRcdRenderParamsInvalid01(rcdDrawable->renderParams_.get());
    EXPECT_TRUE(rcdDrawable->FillHardwareResource(2, 2) == false);

    InitRcdRenderParamsInvalid02(rcdDrawable->renderParams_.get());
    EXPECT_TRUE(rcdDrawable->FillHardwareResource(2, 2) == false);

    InitRcdRenderParamsInvalid03(rcdDrawable->renderParams_.get());
    EXPECT_TRUE(rcdDrawable->FillHardwareResource(2, 2) == false);

    InitRcdRenderParamsInvalid04(rcdDrawable->renderParams_.get());
    EXPECT_TRUE(rcdDrawable->FillHardwareResource(2, 2) == false);

    InitRcdRenderParamsInvalid05(rcdDrawable->renderParams_.get());
    EXPECT_TRUE(rcdDrawable->FillHardwareResource(2, 2) == false);

    InitRcdRenderParamsInvalid06(rcdDrawable->renderParams_.get());
    EXPECT_TRUE(rcdDrawable->FillHardwareResource(2, 2) == false);

    InitRcdRenderParamsInvalid07(rcdDrawable->renderParams_.get());
    EXPECT_TRUE(rcdDrawable->FillHardwareResource(2, 2) == false);

    InitRcdRenderParamsInvalid08(rcdDrawable->renderParams_.get());
    EXPECT_TRUE(rcdDrawable->FillHardwareResource(2, 2) == false);

    delete surfaceBuffer;
}

/*
 * @tc.name: ProcessRcdSurfaceRenderNode1
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.ProcessRcdSurfaceRenderNode1
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, ProcessRcdSurfaceRenderNode1, TestSize.Level1)
{
    auto bottomDrawable = GetRcdTestDrawable();
    auto rcdParams = static_cast<RSRcdRenderParams*>(bottomDrawable->renderParams_.get());
    EXPECT_TRUE(bottomDrawable != nullptr && rcdParams != nullptr);

    const char* path = "port_down.png";
    SetBitmapToRcdRenderParams(bottomDrawable->renderParams_.get(), path);
    rcdParams->SetResourceChanged(true);

    std::shared_ptr<RSProcessor> processor = nullptr;
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));

    processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));

    bottomDrawable->SetSurfaceType(static_cast<uint32_t>(RCDSurfaceType::INVALID));
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));
    bottomDrawable->SetSurfaceType(static_cast<uint32_t>(RCDSurfaceType::BOTTOM));

    RSUniRenderThread::Instance().InitGrContext();
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));

    rcdParams->SetResourceChanged(false);
    auto renderParams = std::move(bottomDrawable->renderParams_);
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));
    bottomDrawable->renderParams_ = std::move(renderParams);

    bottomDrawable->PrepareResourceBuffer();
    bottomDrawable->ProcessRcdSurfaceRenderNode(processor);
    bottomDrawable->surfaceCreated_ = true;
    EXPECT_TRUE(bottomDrawable->ProcessRcdSurfaceRenderNode(processor));
}

/*
 * @tc.name: ConsumeAndUpdateBufferTest1
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.ConsumeAndUpdateBufferTest1
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, ConsumeAndUpdateBufferTest1, TestSize.Level1)
{
    auto bottomDrawable = GetRcdTestDrawable();
    EXPECT_NE(bottomDrawable, nullptr);

    std::shared_ptr<RSProcessor> processor = nullptr;
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));

    processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));

    auto renderParams = std::move(bottomDrawable->renderParams_);
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));
    bottomDrawable->renderParams_ = std::move(renderParams);

    bottomDrawable->PrepareResourceBuffer();
    bottomDrawable->ProcessRcdSurfaceRenderNode(processor);
}

/*
 * @tc.name: ConsumeAndUpdateBufferTest2
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.ConsumeAndUpdateBufferTest2
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, ConsumeAndUpdateBufferTest2, TestSize.Level1)
{
    auto bottomDrawable = GetRcdTestDrawable();
    EXPECT_NE(bottomDrawable, nullptr);

    auto renderParams = std::move(bottomDrawable->renderParams_);
    EXPECT_TRUE(!bottomDrawable->ConsumeAndUpdateBuffer());
    bottomDrawable->renderParams_ = std::move(renderParams);

    auto surfaceHandler = bottomDrawable->surfaceHandler_;
    bottomDrawable->surfaceHandler_ = nullptr;
    EXPECT_TRUE(!bottomDrawable->ConsumeAndUpdateBuffer());
    bottomDrawable->surfaceHandler_ = surfaceHandler;

    auto bufferCount = surfaceHandler->bufferAvailableCount_.load();
    surfaceHandler->bufferAvailableCount_.store(0);
    EXPECT_TRUE(bottomDrawable->ConsumeAndUpdateBuffer());
    surfaceHandler->bufferAvailableCount_.store(bufferCount);

    auto consumer = surfaceHandler->consumer_;
    surfaceHandler->consumer_ = nullptr;
    EXPECT_TRUE(bottomDrawable->ConsumeAndUpdateBuffer());
    surfaceHandler->consumer_ = consumer;

    bottomDrawable->PrepareResourceBuffer();
    bottomDrawable->ConsumeAndUpdateBuffer();
}


/*
 * @tc.name: RSRcdSurfaceRenderNodeDrawable
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.RSRcdSurfaceRenderNodeDrawable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, RSRcdSurfaceRenderNodeDrawable01, TestSize.Level1)
{
    auto rcdDrawable = GetRcdTestDrawable();
    rcdDrawable->ClearBufferCache();

    rcdDrawable->IsSurfaceCreated();
    rcdDrawable->IsBottomSurface();
    rcdDrawable->IsTopSurface();
    rcdDrawable->IsInvalidSurface();
    rcdDrawable->GetRSSurface();
    rcdDrawable->GetHardenBufferRequestConfig();
    auto comsumer = rcdDrawable->GetConsumerListener();
    rcdDrawable->CreateSurface(comsumer);
    rcdDrawable->SetHardwareResourceToBuffer();

    InitRcdRenderParams(rcdDrawable->renderParams_.get());
    auto bufferSize = rcdDrawable->GetRcdBufferSize();
    EXPECT_EQ(bufferSize, 1);
    auto bufferHeight = rcdDrawable->GetRcdBufferHeight();
    EXPECT_EQ(bufferHeight, 0);
    auto bufferWidth = rcdDrawable->GetRcdBufferWidth();
    EXPECT_EQ(bufferWidth, 0);
    rcdDrawable->GetHardenBufferRequestConfig();
    rcdDrawable->SetHardwareResourceToBuffer();

    auto rcdParams = static_cast<RSRcdRenderParams*>(rcdDrawable->renderParams_.get());
    const char* path = "port_down.png";
    SetBitmapToRcdRenderParams(rcdDrawable->renderParams_.get(), path);
    if (rcdDrawable->GetRcdBitmap() == nullptr) {
        return;
    }
    rcdParams->SetResourceChanged(true);
    rcdDrawable->GetHardenBufferRequestConfig();
    EXPECT_TRUE(rcdDrawable->GetRcdBufferWidth() > 0);

    rcdDrawable->ClearBufferCache();
}

/*
 * @tc.name: RSRcdSurfaceRenderNodeDrawableResource
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.RSRcdSurfaceRenderNodeDrawableResource
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, RSRcdSurfaceRenderNodeDrawableResource, TestSize.Level1)
{
    auto surfaceNodeDrawablePtr = GetRcdTestDrawable();
    auto cldInfo = surfaceNodeDrawablePtr->GetCldInfo();
    EXPECT_TRUE(cldInfo.cldWidth >= 0);

    sptr<IBufferConsumerListener> listener = new RSRcdRenderListener(surfaceNodeDrawablePtr->surfaceHandler_);
    surfaceNodeDrawablePtr->CreateSurface(listener);
    surfaceNodeDrawablePtr->ClearBufferCache();
    EXPECT_TRUE(surfaceNodeDrawablePtr->surface_ != nullptr);
}

/*
 * @tc.name: ProcessFillHardwareResource01
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.ProcessFillHardwareResource01
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, ProcessFillHardwareResource01, TestSize.Level1)
{
    // prepare test
    auto rcdDrawable = GetRcdTestDrawable();
    const char* path = "port_down.png";
    auto bitmap = LoadBitmapFromFile(path);
    if (bitmap == nullptr) {
        std::cout << "ProcessFillHardwareResource01: current os no rcd source" << std::endl;
        return;
    }

    auto surfaceBuffer = new SurfaceBufferImpl();
    rcdDrawable->surfaceHandler_->buffer_.buffer = nullptr;
    bool flag1 = rcdDrawable->FillHardwareResource(2, 2);
    rcdDrawable->surfaceHandler_->buffer_.buffer = surfaceBuffer;
    EXPECT_TRUE(flag1 == false);

    BufferHandle* bufferHandle = AllocateBufferHandle(64, 128);
    EXPECT_TRUE(bufferHandle != nullptr);
    surfaceBuffer->SetBufferHandle(bufferHandle);
    surfaceBuffer->handle_->stride = 1;
    surfaceBuffer->handle_->size = 64 + 128 + sizeof(BufferHandle);

    auto rcdParams = static_cast<RSRcdRenderParams*>(rcdDrawable->renderParams_.get());
    bool flag2 = true;
    rcdParams->SetBufferSize(-1);
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(2, 2);
    rcdParams->SetBufferSize(10000);
    rcdParams->SetCldWidth(-1);
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(2, 2);
    rcdParams->SetCldWidth(2);
    rcdParams->SetCldHeight(-1);
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(2, 2);
    rcdParams->SetCldHeight(2);
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(-1, 2);
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(2, -1);
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(2, -1);
    rcdParams->SetPathBin("");
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(2, 2);
    rcdParams->SetPathBin("/sys_prod/etc/display/RoundCornerDisplay/test.bin");
    EXPECT_TRUE(flag2);

    bool flag3 = true;
    flag3 = flag3 && !rcdDrawable->FillHardwareResource(2, 2);
    std::shared_ptr<uint8_t> buffer = std::make_shared<uint8_t>(10000);
    surfaceBuffer->handle_->virAddr = static_cast<void*>(buffer.get());
    surfaceBuffer->handle_->stride = -1;
    flag3 = flag3 && !rcdDrawable->FillHardwareResource(2, 2);
    surfaceBuffer->handle_->stride = 1;
    surfaceBuffer->handle_->size = 0;
    flag3 = flag3 && !rcdDrawable->FillHardwareResource(2, 2);
    EXPECT_TRUE(flag3);

    surfaceBuffer->handle_= nullptr;
    FreeBufferHandle(bufferHandle);

    delete surfaceBuffer;
}

/*
 * @tc.name: ProcessFillHardwareResource02
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.ProcessFillHardwareResource02
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, ProcessFillHardwareResource02, TestSize.Level1)
{
    // prepare test
    auto rcdDrawable = GetRcdTestDrawable();
    const char* path = "port_down.png";
    auto bitmap = LoadBitmapFromFile(path);
    if (bitmap == nullptr) {
        std::cout << "ProcessFillHardwareResource01: current os no rcd source" << std::endl;
        return;
    }

    auto surfaceHandler = rcdDrawable->surfaceHandler_;
    rcdDrawable->surfaceHandler_ = nullptr;
    bool flag = rcdDrawable->FillHardwareResource(2, 2);
    EXPECT_TRUE(flag == false);
    rcdDrawable->surfaceHandler_ = surfaceHandler;

    auto params = std::move(rcdDrawable->renderParams_);
    flag = rcdDrawable->FillHardwareResource(2, 2);
    EXPECT_TRUE(flag == false);
    rcdDrawable->renderParams_ = std::move(params);

    auto surfaceBuffer = new SurfaceBufferImpl();
    rcdDrawable->surfaceHandler_->buffer_.buffer = surfaceBuffer;
    BufferHandle* bufferHandle = AllocateBufferHandle(64, 128);
    surfaceBuffer->SetBufferHandle(bufferHandle);
    surfaceBuffer->handle_->stride = 1;
    surfaceBuffer->handle_->size = 64 + 128 + sizeof(BufferHandle);

    auto rcdParams = static_cast<RSRcdRenderParams*>(rcdDrawable->renderParams_.get());
    rcdParams->SetBufferSize(10000);
    rcdParams->SetCldWidth(2);
    rcdParams->SetCldHeight(2);
    std::shared_ptr<uint8_t> buffer = std::make_shared<uint8_t>(10000);
    surfaceBuffer->handle_->virAddr = static_cast<void *>(buffer.get());

    rcdParams->SetPathBin("/sys_prod/etc/display/RoundCornerDisplay/not_exist.bin");
    flag = rcdDrawable->FillHardwareResource(2, 2);
    EXPECT_TRUE(flag == false);

    rcdParams->SetPathBin("/sys_prod/etc/display/RoundCornerDisplay/port_down.bin");
    rcdDrawable->FillHardwareResource(2, 2);

    surfaceBuffer->handle_= nullptr;
    FreeBufferHandle(bufferHandle);
    delete surfaceBuffer;
}
} // OHOS::Rosen