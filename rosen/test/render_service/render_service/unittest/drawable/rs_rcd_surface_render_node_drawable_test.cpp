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

void RSRcdSurfaceRenderNodeDrawableTest::SetUpTestCase() {}
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

void SetBitmapToRcdRenderParams(RSRenderParams *params, const char* path)
{
    if (params == nullptr || path == nullptr) {
        return;
    }

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

    auto rcdParams = static_cast<RSRcdRenderParams*>(params);
    rcdParams->SetSrcRect(RectI(0, 0, bitmap->GetWidth(), bitmap->GetHeight()));
    rcdParams->SetDstRect(RectI(0, 0, bitmap->GetWidth(), bitmap->GetHeight()));
    rcdParams->SetRcdBitmap(bitmap);
}

std::shared_ptr<DrawableV2::RSRcdSurfaceRenderNodeDrawable> GetRcdTestDrawable()
{
    auto rcdDrawable = RSRcdSurfaceRenderNode::Create(0, RCDSurfaceType::BOTTOM);
    if (rcdDrawable == nullptr) {
        return nullptr;
    }
    return std::static_pointer_cast<DrawableV2::RSRcdSurfaceRenderNodeDrawable>(rcdDrawable->renderDrawable_);
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
    res = bottomDrawable->CreateSurface(bottomDrawable->consumerListener_);  // Surface already created path
    EXPECT_TRUE(res);
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

    auto renderParams = std::move(bottomDrawable->renderParams_);
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));
    bottomDrawable->renderParams_ = std::move(renderParams);

    bottomDrawable->PrepareResourceBuffer();
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));
}

/*
 * @tc.name: ConsumeAndUpdateBufferTest
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.ConsumeAndUpdateBufferTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, ConsumeAndUpdateBufferTest, TestSize.Level1)
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
    EXPECT_TRUE(!bottomDrawable->ProcessRcdSurfaceRenderNode(processor));
}

/*
 * @tc.name: RSRcdSurfaceRenderNodeDrawable
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.RSRcdSurfaceRenderNodeDrawable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, RSRcdSurfaceRenderNodeDrawable, TestSize.Level1)
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
    rcdParams->SetResourceChanged(true);
    rcdDrawable->GetHardenBufferRequestConfig();
    EXPECT_EQ(rcdDrawable->GetRcdBufferWidth(), 1260);

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
 * @tc.name: ProcessFillHardwareResource
 * @tc.desc: Test RSRcdSurfaceRenderNodeDrawableTest.ProcessFillHardwareResource
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRcdSurfaceRenderNodeDrawableTest, ProcessFillHardwareResource, TestSize.Level1)
{
    // prepare test
    auto rcdDrawable = GetRcdTestDrawable();

    HardwareLayerInfo info{};
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

    bool flag2 = true;
    info.bufferSize = -1;
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(2, 2);
    info.bufferSize = 10000;
    info.cldWidth = -1;
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(2, 2);
    info.cldWidth = 2;
    info.cldHeight = -1;
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(2, 2);
    info.cldHeight = 2;
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(-1, 2);
    flag2 = flag2 && !rcdDrawable->FillHardwareResource(2, -1);
    info.pathBin = "";
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
}
} // OHOS::Rosen