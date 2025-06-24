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
#include "gtest/gtest.h"

#include <parameter.h>
#include <parameters.h>
#include <unistd.h>
#include "param/sys_param.h" 
#include "hpae_base/rs_hpae_filter_cache_manager.h"
#include "hpae_base/rs_hpae_base_types.h"
#include "hpae_base/rs_hpae_fusion_operator.h"
#include "hpae_base/rs_hpae_hianimation.h"
#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"
#include "hpae_base/rs_hpae_base_data.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_render_maskcolor_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHpaeFilterCacheManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::string hpaeSwitch;
    static inline std::string hpaeAaeSwitch;
};

void RSHpaeFilterCacheManagerTest::SetUpTestCase()
{
    hpaeSwitch = OHOS::system::GetParameter("debug.graphic.hpae.blur.enabled", "0");
    hpaeAaeSwitch = OHOS::system::GetParameter("rosen.graphic.hpae.blur.aae.enabled", "0");
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", "1");
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", "1");
}
void RSHpaeFilterCacheManagerTest::TearDownTestCase()
{
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled",hpaeSwitch);
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled",hpaeAaeSwitch);
}
void RSHpaeFilterCacheManagerTest::SetUp() {}
void RSHpaeFilterCacheManagerTest::TearDown() {}

/**
 * @tc.name: IsCacheVaildTest
 * @tc.desc: Verify function IsCacheVaild
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, IsCacheValidTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    bool valid = hpaeCacheManager.IsCacheValid();
    EXPECT_FALSE(valid);
}

/**
 * @tc.name: DrawFilterTest
 * @tc.desc: Verify function DrawFilter
 * @tc.type:FUNC
 * @tc.require:
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, DrawFilterTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    Drawing::Canvas canvas(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::Canvas canvasIn(100, 100);
    Drawing::Canvas canvasOut(100, 100);
    std::shared_ptr<RSFilter> rsFilter = nullptr;
    HpaeBufferInfo inputBuffer;
    HpaeBufferInfo outputBuffer;
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(&canvasIn);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(&canvasOut);
    RSHpaeBaseData::GetInstance().SetHpaeInputBuffer(inputBuffer);
    RSHpaeBaseData::GetInstance().SetHpaeOutputBuffer(outputBuffer);
    int ret = hpaeCacheManager.DrawFilter(paintFilterCanvas, rsFilter, false);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: DrawFilterTest
 * @tc.desc: Verify function DrawFilter
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, DrawFilterTest01, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    Drawing::Canvas canvas(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::Canvas canvasIn(100, 100);
    Drawing::Canvas canvasOut(100, 100);
    auto rsFilter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    HpaeBufferInfo inputBuffer;
    HpaeBufferInfo outputBuffer;

    RSHpaeBaseData::GetInstance().SetHpaeInputBuffer(inputBuffer);
    RSHpaeBaseData::GetInstance().SetHpaeOutputBuffer(outputBuffer);
    int ret = hpaeCacheManager.DrawFilter(paintFilterCanvas, rsFilter, false);
    EXPECT_EQ(ret, -1);

    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(&canvasIn);
    outputBuffer.canvas = nullptr;
    RSHpaeBaseData::GetInstance().SetHpaeInputBuffer(inputBuffer);
    RSHpaeBaseData::GetInstance().SetHpaeOutputBuffer(outputBuffer);
    ret = hpaeCacheManager.DrawFilter(paintFilterCanvas, rsFilter, false);
    EXPECT_EQ(ret, -1);

    inputBuffer.canvas = nullptr;
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(&canvasOut);
    RSHpaeBaseData::GetInstance().SetHpaeInputBuffer(inputBuffer);
    RSHpaeBaseData::GetInstance().SetHpaeOutputBuffer(outputBuffer);
    ret = hpaeCacheManager.DrawFilter(paintFilterCanvas, rsFilter, false);
    EXPECT_EQ(ret, -1);

    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(&canvasIn);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(&canvasOut);
    RSHpaeBaseData::GetInstance().SetHpaeInputBuffer(inputBuffer);
    RSHpaeBaseData::GetInstance().SetHpaeOutputBuffer(outputBuffer);
    RSHpaeBaseData::GetInstance().needReset_ = true;
    hpaeCacheManager.cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    hpaeCacheManager.cachedSnapshot_->cachedImage_ = std::make_shared<Drawing::Image>();
    ret = hpaeCacheManager.DrawFilter(paintFilterCanvas, rsFilter, false);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: DrawFilterImplTest
 * @tc.desc: Verify function DrawFilterImplTest
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, DrawFilterImplTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto rsFilter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    Drawing::RectI clipBounds(0, 0, 50, 50);
    Drawing::RectI src(0, 0, 50, 50);
    int ret = hpaeCacheManager.DrawFilterImpl(rsFilter, clipBounds, src, false);
    EXPECT_EQ(ret, -1);
    hpaeCacheManager.cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    hpaeCacheManager.cachedFilteredSnapshot_->cachedImage_ = std::make_shared<Drawing::Image>();
    ret = hpaeCacheManager.DrawFilterImpl(rsFilter, clipBounds, src, false);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: DrawBackgroundTocanvasTest
 * @tc.desc: Verify function DrawBackgroundTocanvas
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, DrawBackgroundToCanvasTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    Drawing::Canvas canvas(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    HpaeBackgroundCacheItem hpaeOutputItem;
    hpaeOutputItem.blurImage_ = std::make_shared<Drawing::Image>();
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(hpaeOutputItem);

    int ret = hpaeCacheManager.DrawBackgroundToCanvas(paintFilterCanvas);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: DrawBackgroundTocanvasTest01
 * @tc.desc: Verify function DrawBackgroundTocanvas
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, DrawBackgroundToCanvasTest01, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    Drawing::Canvas canvas(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    int ret = hpaeCacheManager.DrawBackgroundToCanvas(paintFilterCanvas);
    EXPECT_EQ(ret, -1);

    HpaeBackgroundCacheItem hpaeOutputItem;
    hpaeOutputItem.blurImage_ = std::make_shared<Drawing::Image>();
    hpaeCacheManager.cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    hpaeCacheManager.curRadius_ = 10.0f;
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(hpaeOutputItem);
    paintFilterCanvas.SetAlpha(0.5);
    ret = hpaeCacheManager.DrawBackgroundToCanvas(paintFilterCanvas);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: TaskSnapshotTest
 * @tc.desc: Verify function TaskSnapshot
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, TakeSnapshotTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    Drawing::Canvas canvas(100, 100);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::Surface surface;
    paintFilterCanvas.surface_ = &surface;
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    Drawing::RectI srcRect(0, 0, 50, 50);

    hpaeCacheManager.TakeSnapshot(paintFilterCanvas, filter, srcRect);
    EXPECT_NE(hpaeCacheManager.cachedSnapshot_, nullptr);
}

/**
 * @tc.name: ProcessGpuBlurTest
 * @tc.desc: Verify function ProcessGpuBlur
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, ProcessGpuBlurTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    filter->type_ = RSFilter::MATERIAL;
    HpaeBufferInfo inputBuffer;
    auto drawingCanvasIn = std::make_unique<Drawing::Canvas>(100, 100);
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    Drawing::Surface surfaceIn;
    inputBuffer.canvas->surface_ = &surfaceIn;

    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(surfaceIn.Bind(bitmap));

    HpaeBufferInfo outputBuffer;
    auto drawingCanvasOut = std::make_unique<Drawing::Canvas>(100, 100);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());
    Drawing::Surface surfaceOut;
    outputBuffer.canvas->surface_ = &surfaceOut;

    int ret = hpaeCacheManager.ProcessGpuBlur(inputBuffer, outputBuffer, filter, 10.0f);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: ProcessGpuBlurTest01
 * @tc.desc: Verify function ProcessGpuBlur
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, ProcessGpuBlurTest01, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    HpaeBufferInfo inputBuffer;
    auto drawingCanvasIn = std::make_unique<Drawing::Canvas>(100, 100);
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    Drawing::Surface surfaceIn;
    inputBuffer.canvas->surface_ = &surfaceIn;

    HpaeBufferInfo outputBuffer;
    auto drawingCanvasOut = std::make_unique<Drawing::Canvas>(100, 100);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());
    Drawing::Surface surfaceOut;
    outputBuffer.canvas->surface_ = &surfaceOut;

    HpaeBufferInfo inputBufferNul;
    HpaeBufferInfo outputBufferNul;
    int ret = hpaeCacheManager.ProcessGpuBlur(inputBufferNul, outputBuffer, filter, 10.0f);
    EXPECT_EQ(ret, -1);
    ret = hpaeCacheManager.ProcessGpuBlur(inputBuffer, outputBufferNul, filter, 10.0f);
    EXPECT_EQ(ret, -1);
    ret = hpaeCacheManager.ProcessGpuBlur(inputBufferNul, outputBufferNul, filter, 10.0f);
    EXPECT_EQ(ret, -1);
    ret = hpaeCacheManager.ProcessGpuBlur(inputBuffer, outputBuffer, filter, 10.0f);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: GenerateHianimationTaskTest
 * @tc.desc: Verify function GenerateHianimationTask
 * @tc.type:FUNC
 * @tc.require:
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, GenerateHianimationTaskTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    HpaeBufferInfo inputBuffer;
    auto drawingCanvasIn = std::make_unique<Drawing::Canvas>(100, 100);
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    Drawing::Surface surfaceIn;
    inputBuffer.canvas->surface_ = &surfaceIn;
    BufferHandle bufferHandleIn;
    inputBuffer.bufferHandle = &bufferHandleIn;
    
    HpaeBufferInfo outputBuffer;
    auto drawingCanvasOut = std::make_unique<Drawing::Canvas>(100, 100);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());
    Drawing::Surface surfaceOut;
    outputBuffer.canvas->surface_ = &surfaceOut;
    BufferHandle bufferHandleOut;
    outputBuffer.bufferHandle = &bufferHandleOut;

    auto resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, reinterpret_cast<void*>(0x1234));
}

/**
 * @tc.name: GenerateHianimationTaskTest01
 * @tc.desc: Verify function GenerateHianimationTask
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, GenerateHianimationTaskTest01, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    HpaeBufferInfo inputBuffer;
    auto drawingCanvasIn = std::make_unique<Drawing::Canvas>(100, 100);
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    BufferHandle bufferHandleIn;
    inputBuffer.bufferHandle = &bufferHandleIn;
    
    HpaeBufferInfo outputBuffer;
    auto drawingCanvasOut = std::make_unique<Drawing::Canvas>(100, 100);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());
    BufferHandle bufferHandleOut;
    outputBuffer.bufferHandle = &bufferHandleOut;

    inputBuffer.canvas.reset();
    auto resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);
    outputBuffer.canvas.reset();
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);

    inputBuffer.bufferHandle =nullptr;
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());

    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);
    inputBuffer.canvas.reset();
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);
    outputBuffer.canvas.reset();
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);

    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);

    outputBuffer.bufferHandle = nullptr;
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());

    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr); 

    inputBuffer.canvas.reset();
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);
    outputBuffer.canvas.reset();
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);

    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr); 
    
    inputBuffer.bufferHandle = &bufferHandleIn;
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);

    inputBuffer.canvas.reset();
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);
    outputBuffer.canvas.reset();
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);

    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    resultTask = hpaeCacheManager.GenerateHianimationTask(inputBuffer, outputBuffer, 10.0f, filter);
    EXPECT_EQ(resultTask.taskPtr, nullptr);
}

/**
 * @tc.name: ProcessHianimationBlurTest
 * @tc.desc: Verify function ProcessHianimationBlur
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, ProcessHianimationBlurTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    HpaeBufferInfo inputBuffer;
    auto drawingCanvasIn = std::make_unique<Drawing::Canvas>(100, 100);
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    Drawing::Surface surfaceIn;
    inputBuffer.canvas->surface_ = &surfaceIn;
    BufferHandle bufferHandleIn;
    inputBuffer.bufferHandle = &bufferHandleIn;
    hpaeCacheManager.inputBufferInfo_ = inputBuffer;
    
    HpaeBufferInfo outputBuffer;
    auto drawingCanvasOut = std::make_unique<Drawing::Canvas>(100, 100);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());
    Drawing::Surface surfaceOut;
    outputBuffer.canvas->surface_ = &surfaceOut;
    BufferHandle bufferHandleOut;
    outputBuffer.bufferHandle = &bufferHandleOut;
    hpaeCacheManager.outputBufferInfo_ = outputBuffer;

    int ret = hpaeCacheManager.ProcessHianimationBlur(filter, 10.0f);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: ProcessHianimationBlurTest
 * @tc.desc: Verify function ProcessHianimationBlur01
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, ProcessHianimationBlurTest01, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    HpaeBufferInfo inputBuffer;
    auto drawingCanvasIn = std::make_unique<Drawing::Canvas>(100, 100);
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    Drawing::Surface surfaceIn;
    inputBuffer.canvas->surface_ = &surfaceIn;
    BufferHandle bufferHandleIn;
    inputBuffer.bufferHandle = &bufferHandleIn;
   
    HpaeBufferInfo outputBuffer;
    auto drawingCanvasOut = std::make_unique<Drawing::Canvas>(100, 100);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());
    Drawing::Surface surfaceOut;
    outputBuffer.canvas->surface_ = &surfaceOut;
    BufferHandle bufferHandleOut;
    outputBuffer.bufferHandle = &bufferHandleOut;
    int ret = hpaeCacheManager.ProcessHianimationBlur(filter, 10.0f);
    EXPECT_EQ(ret, -1);

    hpaeCacheManager.inputBufferInfo_ = inputBuffer;
    hpaeCacheManager.outputBufferInfo_ = outputBuffer;
    auto instance = RSHpaeFfrtPatternManager::Instance().g_instance;
    RSHpaeFfrtPatternManager::Instance().g_instance = nullptr;
    ret = hpaeCacheManager.ProcessHianimationBlur(filter, 10.0f);
    EXPECT_EQ(ret, -1);
    RSHpaeFfrtPatternManager::Instance().g_instance = instance;
}

/**
 * @tc.name: ProcessHpaeBlurTest
 * @tc.desc: Verify function ProcessHpaeBlur
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, ProcessHpaeBlurTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto image = std::make_shared<Drawing::Image>();
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    filter->type_ = RSFilter::MATERIAL;
    Drawing::RectI clipBounds(0, 0, 50, 50);
    Drawing::RectI src(0, 0, 50, 50);
    hpaeCacheManager.curRadius_ = 10.0f;
    HpaeBufferInfo inputBuffer;
    auto drawingCanvasIn = std::make_unique<Drawing::Canvas>(100, 100);
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    Drawing::Surface surfaceIn;
    inputBuffer.canvas->surface_ = &surfaceIn;
    BufferHandle bufferHandleIn;
    inputBuffer.bufferHandle = &bufferHandleIn;
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(surfaceIn.Bind(bitmap));
    hpaeCacheManager.inputBufferInfo_ = inputBuffer;

    HpaeBufferInfo outputBuffer;
    auto drawingCanvasOut = std::make_unique<Drawing::Canvas>(100, 100);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());
    Drawing::Surface surfaceOut;
    outputBuffer.canvas->surface_ = &surfaceOut;
    BufferHandle bufferHandleOut;
    outputBuffer.bufferHandle = &bufferHandleOut;
    hpaeCacheManager.outputBufferInfo_ = outputBuffer;

    hpaeCacheManager.drawUsingGpu_ = false;
    int ret = hpaeCacheManager.ProcessHpaeBlur(clipBounds, image, filter, src);
    EXPECT_EQ(ret, 0);

    hpaeCacheManager.drawUsingGpu_ = true;
    ret = hpaeCacheManager.ProcessHpaeBlur(clipBounds, image, filter, src);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: ProcessHpaeBlurTest
 * @tc.desc: Verify function ProcessHpaeBlur01
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, ProcessHpaeBlurTest01, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto image = std::make_shared<Drawing::Image>();
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    filter->type_ = RSFilter::MATERIAL;
    Drawing::RectI clipBounds(0, 0, 50, 50);
    Drawing::RectI src(0, 0, 50, 50);
    hpaeCacheManager.curRadius_ = 0;
    HpaeBufferInfo inputBuffer;
    auto drawingCanvasIn = std::make_unique<Drawing::Canvas>(100, 100);
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    Drawing::Surface surfaceIn;
    inputBuffer.canvas->surface_ = &surfaceIn;
    BufferHandle bufferHandleIn;
    inputBuffer.bufferHandle = &bufferHandleIn;
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(surfaceIn.Bind(bitmap));
    hpaeCacheManager.inputBufferInfo_ = inputBuffer;

    HpaeBufferInfo outputBuffer;
    auto drawingCanvasOut = std::make_unique<Drawing::Canvas>(100, 100);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());
    Drawing::Surface surfaceOut;
    outputBuffer.canvas->surface_ = &surfaceOut;
    BufferHandle bufferHandleOut;
    outputBuffer.bufferHandle = &bufferHandleOut;

    hpaeCacheManager.drawUsingGpu_ = false;
    int ret = hpaeCacheManager.ProcessHpaeBlur(clipBounds, image, filter, src);
    EXPECT_EQ(ret, 0);
    hpaeCacheManager.curRadius_ = 10.0f;
    hpaeCacheManager.drawUsingGpu_ = true;
    ret = hpaeCacheManager.ProcessHpaeBlur(clipBounds, image, filter, src);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: GetBlurOutputTest
 * @tc.desc: Verify function GetBlurOutput
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, GetBlurOutputTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto item = hpaeCacheManager.GetBlurOutput();
    EXPECT_EQ(item.blurImage_, nullptr);
    HpaeBackgroundCacheItem item1;
    HpaeBackgroundCacheItem item2;
    HpaeBackgroundCacheItem item3;
    HpaeBackgroundCacheItem item4;
    item1.blurImage_ = std::make_shared<Drawing::Image>();
    item1.radius_ = 10.0f;
    item2.useCache_ = true;
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(item1);
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(item2);
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(item3);
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(item4);

    item = hpaeCacheManager.GetBlurOutput();
    EXPECT_EQ(item.radius_, 10.0f);
}

/**
 * @tc.name: GetBlurOutputTest01
 * @tc.desc: Verify function GetBlurOutput01
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, GetBlurOutputTest01, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    HpaeBackgroundCacheItem itemIn;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    auto surface = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(surface->Bind(bitmap));

    hpaeCacheManager.hpaeBlurOutputQueue_.clear();
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(itemIn);
    auto item = hpaeCacheManager.GetBlurOutput();
    EXPECT_EQ(item.blurImage_, nullptr);

    itemIn.surface_ = surface;
    hpaeCacheManager.hpaeBlurOutputQueue_.clear();
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(itemIn);
    item = hpaeCacheManager.GetBlurOutput();
    EXPECT_NE(item.blurImage_, nullptr);
    
    itemIn.blurImage_ = image;
    itemIn.surface_ = nullptr;
    hpaeCacheManager.hpaeBlurOutputQueue_.clear();
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(itemIn);
    item = hpaeCacheManager.GetBlurOutput();
    EXPECT_NE(item.blurImage_, nullptr);

    itemIn.blurImage_ = image;
    itemIn.surface_ = surface;
    hpaeCacheManager.hpaeBlurOutputQueue_.clear();
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(itemIn);
    item = hpaeCacheManager.GetBlurOutput();
    EXPECT_NE(item.blurImage_, nullptr);
}

/**
 * @tc.name: resetfiltercachetest
 * @tc.desc: Verify function GetBlurOutput
 * @tc.type:FUNC
 * @tc.require:  
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, ResetFilterCacheTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    RectI srcRect(0, 0, 50, 50);
    
    auto cachedSnapshot = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    auto cachedFilteredSnapshot = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    hpaeCacheManager.ResetFilterCache(cachedSnapshot,cachedFilteredSnapshot, srcRect);
    EXPECT_NE(hpaeCacheManager.cachedSnapshot_, nullptr);
    EXPECT_NE(hpaeCacheManager.cachedFilteredSnapshot_, nullptr);
}

/**
 * @tc.name: InvalidateFilterCacheTest
 * @tc.desc: Verify function GetBlurOutput
 * @tc.type:FUNC
 * @tc.require:
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, InvalidateFilterCacheTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto cachedSnapshot = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    auto cachedFilteredSnapshot = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    hpaeCacheManager.cachedSnapshot_ = cachedSnapshot;
    hpaeCacheManager.cachedFilteredSnapshot_ = cachedFilteredSnapshot;
    hpaeCacheManager.InvalidateFilterCache(FilterCacheType::BOTH);
    EXPECT_EQ(hpaeCacheManager.cachedSnapshot_, nullptr);
    EXPECT_EQ(hpaeCacheManager.cachedFilteredSnapshot_, nullptr);

    hpaeCacheManager.cachedSnapshot_ = cachedSnapshot;
    hpaeCacheManager.cachedFilteredSnapshot_ = cachedFilteredSnapshot;
    hpaeCacheManager.InvalidateFilterCache(FilterCacheType::SNAPSHOT);
    EXPECT_EQ(hpaeCacheManager.cachedSnapshot_, nullptr);
    EXPECT_NE(hpaeCacheManager.cachedFilteredSnapshot_, nullptr);

    hpaeCacheManager.cachedSnapshot_ = cachedSnapshot;
    hpaeCacheManager.cachedFilteredSnapshot_ = cachedFilteredSnapshot;
    hpaeCacheManager.InvalidateFilterCache(FilterCacheType::FILTERED_SNAPSHOT);
    EXPECT_NE(hpaeCacheManager.cachedSnapshot_, nullptr);
    EXPECT_EQ(hpaeCacheManager.cachedFilteredSnapshot_, nullptr);

    hpaeCacheManager.cachedSnapshot_ = cachedSnapshot;
    hpaeCacheManager.cachedFilteredSnapshot_ = cachedFilteredSnapshot;
    hpaeCacheManager.InvalidateFilterCache(FilterCacheType::NONE);
    EXPECT_NE(hpaeCacheManager.cachedSnapshot_, nullptr);
    EXPECT_NE(hpaeCacheManager.cachedFilteredSnapshot_, nullptr);
}

/**
 * @tc.name: CheckIfUsingGpuTest
 * @tc.desc: Verify function GetBlurOutput
 * @tc.type:FUNC
 * @tc.require:
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, CheckIfUsingGpuTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    HpaeBackgroundCacheItem hpaeOutputItem;
    hpaeCacheManager.hpaeBlurOutputQueue_.clear();
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(hpaeOutputItem);
    HianimationManager::GetInstance().taskIdMap_.clear();
    bool ret = hpaeCacheManager.CheckIfUsingGpu();
    EXPECT_EQ(ret, false);
    HianimationManager::GetInstance().taskIdMap_.insert(0);
    HianimationManager::GetInstance().taskIdMap_.insert(1);

    hpaeCacheManager.hpaeBlurOutputQueue_.clear();
    ret = hpaeCacheManager.CheckIfUsingGpu();
    EXPECT_EQ(ret, true);

    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(hpaeOutputItem);
    ret = hpaeCacheManager.CheckIfUsingGpu();
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: SetBlurOutputTest
 * @tc.desc: Verify function SetBlurOutputTest
 * @tc.type:FUNC
 * @tc.require:
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, SetBlurOutputTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    HpaeBackgroundCacheItem hpaeOutputItem;
    auto surface = std::make_shared<Drawing::Surface>();
    hpaeOutputItem.surface_ = surface;
    hpaeCacheManager.SetBlurOutput(hpaeOutputItem);
    EXPECT_EQ(hpaeCacheManager.hpaeBlurOutputQueue_.size(), 1);
    hpaeCacheManager.cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    hpaeCacheManager.SetBlurOutput(hpaeOutputItem);
    EXPECT_EQ(hpaeCacheManager.hpaeBlurOutputQueue_.size(), 2);
}

/**
 * @tc.name: BlurWithoutSnapshotTest
 * @tc.desc: Verify function BlurWithoutSnapshotTest
 * @tc.type:FUNC
 * @tc.require:
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, BlurWithoutSnapshotTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    auto image = std::make_shared<Drawing::Image>();
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    filter->type_ = RSFilter::MATERIAL;
    Drawing::RectI clipBounds(0, 0, 50, 50);
    Drawing::RectI src(0, 0, 50, 50);
    hpaeCacheManager.curRadius_ = 10.0f;
    HpaeBufferInfo inputBuffer;
    auto drawingCanvasIn = std::make_unique<Drawing::Canvas>(100, 100);
    inputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasIn.get());
    Drawing::Surface surfaceIn;
    inputBuffer.canvas->surface_ = &surfaceIn;

    BufferHandle bufferHandleIn;
    inputBuffer.bufferHandle = &bufferHandleIn;
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(surfaceIn.Bind(bitmap));
    hpaeCacheManager.inputBufferInfo_ = inputBuffer;

    HpaeBufferInfo outputBuffer;
    auto drawingCanvasOut = std::make_unique<Drawing::Canvas>(100, 100);
    outputBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvasOut.get());
    Drawing::Surface surfaceOut;
    outputBuffer.canvas->surface_ = &surfaceOut;
    BufferHandle bufferHandleOut;
    outputBuffer.bufferHandle = &bufferHandleOut;
    hpaeCacheManager.outputBufferInfo_ = outputBuffer;

    hpaeCacheManager.drawUsingGpu_ = false;
    int ret = hpaeCacheManager.BlurWithoutSnapshot(clipBounds, image, filter, src, false);
    EXPECT_EQ(ret, 0);
    ret = hpaeCacheManager.BlurWithoutSnapshot(clipBounds, nullptr, filter, src, true);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: BlurUsingFilteredSnapshotTest
 * @tc.desc: Verify function BlurUsingSnapshotTest
 * @tc.type:FUNC
 * @tc.require:
*/
HWTEST_F(RSHpaeFilterCacheManagerTest, BlurUsingFilteredSnapshotTest, TestSize.Level1)
{
    RSHpaeFilterCacheManager hpaeCacheManager;
    hpaeCacheManager.cachedFilteredSnapshot_ = nullptr;
    int ret  = hpaeCacheManager.BlurUsingFilteredSnapshot();
    EXPECT_EQ(ret, -1);

    hpaeCacheManager.cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    hpaeCacheManager.hpaeBlurOutputQueue_.clear();
    ret  = hpaeCacheManager.BlurUsingFilteredSnapshot();
    EXPECT_EQ(ret, -1);

    HpaeBackgroundCacheItem hpaeOutputItem;
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(hpaeOutputItem);
    ret  = hpaeCacheManager.BlurUsingFilteredSnapshot();
    EXPECT_EQ(ret, 0); 

    hpaeCacheManager.cachedFilteredSnapshot_->cachedImage_ = std::make_shared<Drawing::Image>();
    hpaeCacheManager.hpaeBlurOutputQueue_.clear();
    ret  = hpaeCacheManager.BlurUsingFilteredSnapshot();
    EXPECT_EQ(ret, 0);
   
    hpaeCacheManager.hpaeBlurOutputQueue_.push_back(hpaeOutputItem);
    ret  = hpaeCacheManager.BlurUsingFilteredSnapshot();
    EXPECT_EQ(ret, 0);
}

} // namespace Rosen
} // namespace OHOS