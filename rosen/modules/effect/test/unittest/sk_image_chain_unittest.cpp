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

#include "sk_image_chain_unittest.h"
#include "sk_image_chain.h"
#include "sk_image_filter_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

/**
 * @tc.name: Render_CPU_Normal_Process
 * @tc.desc: render normaly.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, Render_CPU_Normal_Process, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 2048;
    opts.size.height = 2048;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));

    auto filterBlur = Rosen::SKImageFilterFactory::Blur(0.5);
    std::vector<sk_sp<SkImageFilter>> imageFilter;
    imageFilter.emplace_back(filterBlur);

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    Rosen::SKImageChain skImage(srcPixelMap);
    DrawError ret = skImage.Render(imageFilter, true, dstPixelMap);
    ASSERT_EQ(ret, DrawError::ERR_OK);
}

/**
 * @tc.name: Render_CPU_Filter_10_Times
 * @tc.desc: render filter 10 times.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, Render_CPU_Filter_10_Times, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 2048;
    opts.size.height = 2048;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));

    std::vector<sk_sp<SkImageFilter>> imageFilter;
    for (int i = 0; i < 10; ++i) {
        auto filterBlur = Rosen::SKImageFilterFactory::Blur(i);
        imageFilter.emplace_back(filterBlur);
    }

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    Rosen::SKImageChain skImage(srcPixelMap);
    DrawError ret = skImage.Render(imageFilter, true, dstPixelMap);
    ASSERT_EQ(ret, DrawError::ERR_OK);
}

/**
 * @tc.name: Render_Options_Size_Is_Negative
 * @tc.desc: render options Size is negative.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, Render_Options_Size_Is_Negative, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = -1;
    opts.size.height = -1;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    ASSERT_EQ(uniPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    ASSERT_EQ(srcPixelMap, nullptr);

    auto filterBlur = Rosen::SKImageFilterFactory::Blur(0.5);
    std::vector<sk_sp<SkImageFilter>> imageFilter;
    imageFilter.emplace_back(filterBlur);

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    Rosen::SKImageChain skImage(srcPixelMap);
    DrawError ret = skImage.Render(imageFilter, true, dstPixelMap);
    ASSERT_NE(ret, DrawError::ERR_OK);
}


/**
 * @tc.name: Render_Blur_Radius_Is_Negative
 * @tc.desc: render blur radius is negative.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, Render_Blur_Radius_Is_Negative, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 2048;
    opts.size.height = 2048;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));

    auto filterBlur = Rosen::SKImageFilterFactory::Blur(-10);
    ASSERT_EQ(filterBlur, nullptr);
    std::vector<sk_sp<SkImageFilter>> imageFilter;
    imageFilter.emplace_back(filterBlur);

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    Rosen::SKImageChain skImage(srcPixelMap);
    DrawError ret = skImage.Render(imageFilter, true, dstPixelMap);
    ASSERT_EQ(ret, DrawError::ERR_OK);
}

/**
 * @tc.name: Render_SKImageChain_srcPixelMap_Is_Null
 * @tc.desc: render pixelMap is null.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, Render_SKImageChain_srcPixelMap_Is_Null, TestSize.Level1)
{
    auto filterBlur = Rosen::SKImageFilterFactory::Blur(10);
    std::vector<sk_sp<SkImageFilter>> imageFilter;
    imageFilter.emplace_back(filterBlur);

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    std::shared_ptr<Media::PixelMap> srcPixelMap = nullptr;
    Rosen::SKImageChain skImage(srcPixelMap);
    DrawError ret = skImage.Render(imageFilter, true, dstPixelMap);
    ASSERT_NE(ret, DrawError::ERR_OK);
}

/**
 * @tc.name: DrawTest001
 * @tc.desc: draw nothing
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest001 start";

    // create from PixelMap
    auto skImageChain = std::make_shared<SKImageChain>(nullptr, nullptr);
    ASSERT_NE(skImageChain, nullptr);

    skImageChain->Draw();
    skImageChain->ForceCPU(false);
    skImageChain->Draw();
}

/**
 * @tc.name: DrawTest002
 * @tc.desc: create a SKImageChain with PixelMap and draw
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest002 start";

    // create from PixelMap
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(uniPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    auto skImageChain = std::make_shared<SKImageChain>(srcPixelMap);
    ASSERT_NE(skImageChain, nullptr);

    skImageChain->Draw();
    skImageChain->ForceCPU(false);
    skImageChain->Draw();
    skImageChain->ForceCPU(true);
}

/**
 * @tc.name: DrawTest003
 * @tc.desc: create a SKImageChain with non-nullptr canvas and draw
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DrawTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest DrawTest003 start";

    // create from PixelMap
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(uniPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    SkImageInfo imageInfo = SkImageInfo::Make(srcPixelMap->GetWidth(), srcPixelMap->GetHeight(),
        SkColorType::kRGBA_8888_SkColorType,
        static_cast<SkAlphaType>(srcPixelMap->GetAlphaType()));
    std::shared_ptr<SkPixmap> dstPixmap = std::make_shared<SkPixmap>(imageInfo, srcPixelMap->GetPixels(),
        srcPixelMap->GetRowStride());
#ifdef USE_M133_SKIA
    sk_sp<SkSurface> cpuSurface = SkSurfaces::WritePixels(imageInfo, const_cast<void*>(dstPixmap->addr()),
        srcPixelMap->GetRowStride());
#else
    sk_sp<SkSurface> cpuSurface = SkSurface::MakeRasterDirect(imageInfo, const_cast<void*>(dstPixmap->addr()),
        srcPixelMap->GetRowStride());
#endif
    ASSERT_NE(cpuSurface, nullptr);

    auto canvas = cpuSurface->getCanvas();
    ASSERT_NE(canvas, nullptr);
    auto skImageChain = std::make_shared<SKImageChain>(canvas, nullptr);
    ASSERT_NE(skImageChain, nullptr);

    skImageChain->Draw();
    skImageChain->ForceCPU(false);
    skImageChain->Draw();
    skImageChain->ForceCPU(true);
}

HWTEST_F(SKImageChainUnittest, CreateEffect_Blur_GetPixelMap_CPU, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest CreateEffect_Blur_GetPixelMap_CPU start";

    // create from PixelMap
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(uniPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    SkImageInfo imageInfo = SkImageInfo::Make(srcPixelMap->GetWidth(), srcPixelMap->GetHeight(),
        SkColorType::kRGBA_8888_SkColorType,
        static_cast<SkAlphaType>(srcPixelMap->GetAlphaType()));

    auto skImageChain = std::make_shared<SKImageChain>(srcPixelMap);
    ASSERT_NE(skImageChain, nullptr);

    auto filterBlur = Rosen::SKImageFilterFactory::Blur(0.5);
    ASSERT_NE(filterBlur, nullptr);
    skImageChain->SetFilters(filterBlur);

    skImageChain->ForceCPU(true);
    skImageChain->Draw();

    ASSERT_NE(skImageChain->GetPixelMap(), nullptr);
}

HWTEST_F(SKImageChainUnittest, CreateEffect_Blur_GetPixelMap_CPU_10000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest CreateEffect_Blur_GetPixelMap_CPU_10000 start";

    for (int i = 0;i < 10000; i++) {
        // create from PixelMap
        Media::InitializationOptions opts;
        opts.size.width = 1;
        opts.size.height = 1;
        opts.editable = true;
        auto uniPixelMap = Media::PixelMap::Create(opts);
        ASSERT_NE(uniPixelMap, nullptr);

        std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
        SkImageInfo imageInfo = SkImageInfo::Make(srcPixelMap->GetWidth(), srcPixelMap->GetHeight(),
            SkColorType::kRGBA_8888_SkColorType,
            static_cast<SkAlphaType>(srcPixelMap->GetAlphaType()));

        auto skImageChain = std::make_shared<SKImageChain>(srcPixelMap);
        ASSERT_NE(skImageChain, nullptr);

        auto filterBlur = Rosen::SKImageFilterFactory::Blur(0.5);
        ASSERT_NE(filterBlur, nullptr);
        skImageChain->SetFilters(filterBlur);

        skImageChain->ForceCPU(true);
        skImageChain->Draw();

        ASSERT_NE(skImageChain->GetPixelMap(), nullptr);
    }
}

HWTEST_F(SKImageChainUnittest, CreateEffect_Blur_GetPixelMap_GPU, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest CreateEffect_Blur_GetPixelMap_GPU start";

    // create from PixelMap
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(uniPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    SkImageInfo imageInfo = SkImageInfo::Make(srcPixelMap->GetWidth(), srcPixelMap->GetHeight(),
        SkColorType::kRGBA_8888_SkColorType,
        static_cast<SkAlphaType>(srcPixelMap->GetAlphaType()));

    auto skImageChain = std::make_shared<SKImageChain>(srcPixelMap);
    ASSERT_NE(skImageChain, nullptr);

    auto filterBlur = Rosen::SKImageFilterFactory::Blur(0.5);
    ASSERT_NE(filterBlur, nullptr);
    skImageChain->SetFilters(filterBlur);

    skImageChain->ForceCPU(false);
    skImageChain->Draw();

    ASSERT_NE(skImageChain->GetPixelMap(), nullptr);
}

HWTEST_F(SKImageChainUnittest, CreateEffect_Blur_GetPixelMap_GPU_100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest CreateEffect_Blur_GetPixelMap_GPU_100 start";

    for (int i = 0;i < 100; i++) {
        // create from PixelMap
        Media::InitializationOptions opts;
        opts.size.width = 1;
        opts.size.height = 1;
        opts.editable = true;
        auto uniPixelMap = Media::PixelMap::Create(opts);
        ASSERT_NE(uniPixelMap, nullptr);

        std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
        SkImageInfo imageInfo = SkImageInfo::Make(srcPixelMap->GetWidth(), srcPixelMap->GetHeight(),
            SkColorType::kRGBA_8888_SkColorType,
            static_cast<SkAlphaType>(srcPixelMap->GetAlphaType()));

        auto skImageChain = std::make_shared<SKImageChain>(srcPixelMap);
        ASSERT_NE(skImageChain, nullptr);

        auto filterBlur = Rosen::SKImageFilterFactory::Blur(0.5);
        ASSERT_NE(filterBlur, nullptr);
        skImageChain->SetFilters(filterBlur);

        skImageChain->ForceCPU(false);
        skImageChain->Draw();

        ASSERT_NE(skImageChain->GetPixelMap(), nullptr);
    }
}

/**
 * @tc.name: GetPixelMapTest001
 * @tc.desc: test GetPixelMap
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, GetPixelMapTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest GetPixelMapTest001 start";

    auto skImageChain = std::make_shared<SKImageChain>(nullptr, nullptr);
    ASSERT_NE(skImageChain, nullptr);

    ASSERT_EQ(skImageChain->GetPixelMap(), nullptr);
}

/**
 * @tc.name: SetFiltersTest001
 * @tc.desc: test SetFilters
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, SetTest001, TestSize.Level1)
{
    auto skImageChain = std::make_shared<SKImageChain>(nullptr, nullptr);
    ASSERT_NE(skImageChain, nullptr);

    auto filterBlur = Rosen::SKImageFilterFactory::Blur(0.5);
    ASSERT_NE(filterBlur, nullptr);
    skImageChain->SetFilters(filterBlur);
    
    auto filterBrightness = Rosen::SKImageFilterFactory::Brightness(0.5);
    ASSERT_NE(filterBrightness, nullptr);
    skImageChain->SetFilters(filterBrightness);
}

/**
 * @tc.name: SetClipTest001
 * @tc.desc: test Set Clip Functions
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, SetClipTest001, TestSize.Level1)
{
    // create from PixelMap
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(uniPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    auto skImageChain = std::make_shared<SKImageChain>(srcPixelMap);
    ASSERT_NE(skImageChain, nullptr);

    SkRect rect = SkRect::MakeEmpty();
    SkPath *path = new SkPath();
    SkRRect *rRect = new SkRRect();
    skImageChain->SetClipRect(&rect);
    skImageChain->Draw();
    skImageChain->SetClipRect(nullptr);

    skImageChain->SetClipPath(path);
    skImageChain->Draw();
    skImageChain->SetClipPath(nullptr);

    skImageChain->SetClipRRect(rRect);
    skImageChain->Draw();

    delete path;
    delete rRect;
}

/**
 * @tc.name: PixelFormatConvert
 * @tc.desc: test for pixel convert
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, PixelFormatConvert, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest PixelFormatConvert start";

    auto skImageChain = std::make_shared<SKImageChain>(nullptr, nullptr);
    ASSERT_NE(skImageChain, nullptr);

    EXPECT_EQ(skImageChain->PixelFormatConvert(Media::PixelFormat::RGB_565),
        SkColorType::kRGB_565_SkColorType);

    EXPECT_EQ(skImageChain->PixelFormatConvert(Media::PixelFormat::ALPHA_8),
        SkColorType::kAlpha_8_SkColorType);

    EXPECT_EQ(skImageChain->PixelFormatConvert(Media::PixelFormat::UNKNOWN),
        SkColorType::kUnknown_SkColorType);

    GTEST_LOG_(INFO) << "SKImageChainUnittest PixelFormatConvert end";
}

/**
 * @tc.name: ForceCPU
 * @tc.desc: test for pixel convert
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, ForceCPU, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageChainUnittest ForceCPU start";

    // create from PixelMap
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(uniPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    auto skImageChain = std::make_shared<SKImageChain>(srcPixelMap);
    ASSERT_NE(skImageChain, nullptr);

    skImageChain->InitWithoutCanvas();
    skImageChain->ForceCPU(true);
    ASSERT_TRUE(skImageChain->CreateGPUCanvas());
    skImageChain->ForceCPU(false);

    skImageChain->ForceCPU(true);
    ASSERT_TRUE(skImageChain->CreateCPUCanvas());

    GTEST_LOG_(INFO) << "SKImageChainUnittest ForceCPU end";
}

/**
 * @tc.name: DestroyGPUCanvasTest001
 * @tc.desc: test DestroyGPUCanvas
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageChainUnittest, DestroyGPUCanvasTest001, TestSize.Level1)
{
    // create from PixelMap
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.editable = true;
    auto uniPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(uniPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    auto skImageChain = std::make_shared<SKImageChain>(srcPixelMap);
    ASSERT_NE(skImageChain, nullptr);

    SkRect rect = SkRect::MakeEmpty();
    SkPath *path = new SkPath();
    SkRRect *rRect = new SkRRect();
    skImageChain->SetClipRect(&rect);
    skImageChain->ForceCPU(false);
    EXPECT_EQ(skImageChain->Draw(), DrawError::ERR_OK);

    delete path;
    delete rRect;
}
} // namespace Rosen
} // namespace OHOS