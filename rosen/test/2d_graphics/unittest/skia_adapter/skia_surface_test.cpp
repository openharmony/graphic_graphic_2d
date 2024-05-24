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

#include <memory>
#include "gtest/gtest.h"
#include "effect/color_space.h"
#include "image/gpu_context.h"
#include "skia_adapter/skia_canvas.h"
#include "skia_adapter/skia_gpu_context.h"
#include "skia_adapter/skia_surface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaSurfaceTest::SetUpTestCase() {}
void SkiaSurfaceTest::TearDownTestCase() {}
void SkiaSurfaceTest::SetUp() {}
void SkiaSurfaceTest::TearDown() {}

#ifdef ACE_ENABLE_GPU
/**
 * @tc.name: Bind001
 * @tc.desc: Test Bind
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, Bind001, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    ASSERT_TRUE(surface != nullptr);
    surface->FlushAndSubmit(true);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(100, 100, bitmapFormat); // 100: width, height
    ASSERT_TRUE(surface->Bind(bitmap));

    auto image = surface->GetImageSnapshot();
    GPUContext context;
#ifdef NEW_SKIA
    GrMockOptions options;
    context.GetImpl<SkiaGPUContext>()->SetGrContext(GrDirectContext::MakeMock(&options));
#endif
    Bitmap bitmap2;
    bitmap2.Build(10, 10, bitmapFormat); // 10: width, height
    image->BuildFromBitmap(context, bitmap2);
    auto surface2 = std::make_unique<SkiaSurface>();
    ASSERT_TRUE(surface2 != nullptr);
    ASSERT_TRUE(!surface2->Bind(*image));
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: MakeFromBackendRenderTarget001
 * @tc.desc: Test MakeFromBackendRenderTarget
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, MakeFromBackendRenderTarget001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    skiaCanvas->ImportSkCanvas(nullptr);
    auto gpuContext = skiaCanvas->GetGPUContext();
    TextureInfo info;
    info.SetWidth(10);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::NO_TYPE);
    auto surface1 = SkiaSurface::MakeFromBackendRenderTarget(gpuContext.get(), info, TextureOrigin::TOP_LEFT,
        ColorType::COLORTYPE_RGBA_8888, colorSpace, nullptr, nullptr);
    auto surface2 = SkiaSurface::MakeFromBackendRenderTarget(gpuContext.get(), info, TextureOrigin::TOP_LEFT,
        ColorType::COLORTYPE_RGBA_8888, nullptr, nullptr, nullptr);
    ASSERT_TRUE(surface1 == nullptr);
    ASSERT_TRUE(surface2 == nullptr);
}

/**
 * @tc.name: MakeFromBackendTexture001
 * @tc.desc: Test MakeFromBackendTexture
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, MakeFromBackendTexture001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    skiaCanvas->ImportSkCanvas(nullptr);
    auto gpuContext = skiaCanvas->GetGPUContext();
    TextureInfo info;
    info.SetWidth(10);
    auto surface = SkiaSurface::MakeFromBackendTexture(nullptr, info, TextureOrigin::TOP_LEFT, 1,
        ColorType::COLORTYPE_RGBA_8888, nullptr, nullptr, nullptr);
    ASSERT_TRUE(surface == nullptr);
    auto surface = SkiaSurface::MakeFromBackendTexture(gpuContext.get(), info, TextureOrigin::TOP_LEFT, 1,
        ColorType::COLORTYPE_RGBA_8888, nullptr, nullptr, nullptr);
    ASSERT_TRUE(surface == nullptr);
}

/**
 * @tc.name: Wait001
 * @tc.desc: Test Wait
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, Wait001, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    ASSERT_TRUE(surface != nullptr);
    VkSemaphore semaphore;
    surface->Wait(5, semaphore); // 5: time
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat); // 10: width, height
    ASSERT_TRUE(surface->Bind(bitmap));
    surface->Wait(1000, semaphore);
}

/**
 * @tc.name: SetDrawingArea001
 * @tc.desc: Test SetDrawingArea
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, SetDrawingArea001, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    ASSERT_TRUE(surface != nullptr);
    const std::vector<RectI>& rects;
    surface->SetDrawingArea(rects);
}

/**
 * @tc.name: ClearDrawingArea001
 * @tc.desc: Test ClearDrawingArea
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, ClearDrawingArea001, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    ASSERT_TRUE(surface != nullptr);
    surface->ClearDrawingArea();
}
#endif
#endif

/**
 * @tc.name: MakeRenderTarget001
 * @tc.desc: Test MakeRenderTarget
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, MakeRenderTarget001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    skiaCanvas->ImportSkCanvas(nullptr);
    auto gpuContext = skiaCanvas->GetGPUContext();
    ImageInfo imageInfo;
    auto surface1 = SkiaSurface::MakeRenderTarget(gpuContext.get(), true, imageInfo);
    auto surface2 = SkiaSurface::MakeRenderTarget(nullptr, true, imageInfo);
    ASSERT_TRUE(surface1 == nullptr);
    ASSERT_TRUE(surface2 == nullptr);
}

/**
 * @tc.name: MakeRaster001
 * @tc.desc: Test MakeRaster
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, MakeRaster001, TestSize.Level1)
{
    ImageInfo info{-1, -1, ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE};
    auto surface = SkiaSurface::MakeRaster(info);
    ASSERT_TRUE(surface == nullptr);
}

/**
 * @tc.name: MakeRasterN32Premul001
 * @tc.desc: Test MakeRasterN32Premul
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, MakeRasterN32Premul001, TestSize.Level1)
{
    auto surface = SkiaSurface::MakeRasterN32Premul(800, 800); // 800: width, height
    ASSERT_TRUE(surface != nullptr);
    auto surface2 = SkiaSurface::MakeRasterN32Premul(-1, -1); // -1: width, height
    ASSERT_TRUE(surface2 == nullptr);
}

/**
 * @tc.name: MakeSurface001
 * @tc.desc: Test MakeSurface
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, MakeSurface001, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    ASSERT_TRUE(surface != nullptr);
    auto surface2 = surface->MakeSurface(800, 800); // 800: width, height
    ASSERT_TRUE(surface2 == nullptr);
}

/**
 * @tc.name: GetSkSurface001
 * @tc.desc: Test GetSkSurface
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, GetSkSurface001, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    ASSERT_TRUE(surface->GetSkSurface() == nullptr);
}

/**
 * @tc.name: FlushAndSubmit001
 * @tc.desc: Test FlushAndSubmit
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, FlushAndSubmit001, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    ASSERT_TRUE(surface != nullptr);
    surface->FlushAndSubmit(false);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat); // 10: width, height
    ASSERT_TRUE(surface->Bind(bitmap));
    surface->FlushAndSubmit(true);
}

/**
 * @tc.name: Flush001
 * @tc.desc: Test Flush
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, Flush001, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    ASSERT_TRUE(surface != nullptr);
    surface->Flush(nullptr);
}

/**
 * @tc.name: Width001
 * @tc.desc: Test Width
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, Width001, TestSize.Level1)
{
    auto sf = SkiaSurface::MakeRasterN32Premul(800, 800); // 800: width, height
    ASSERT_TRUE(sf != nullptr);
    ASSERT_TRUE(sf->Width() == 800);
}

/**
 * @tc.name: Height001
 * @tc.desc: Test Height
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaSurfaceTest, Height001, TestSize.Level1)
{
    auto sf = SkiaSurface::MakeRasterN32Premul(800, 800); // 800: width, height
    ASSERT_TRUE(sf != nullptr);
    ASSERT_TRUE(sf->Height() == 800);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
