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
    sk_sp<SkSurface> cpuSurface = SkSurface::MakeRasterDirect(imageInfo, const_cast<void*>(dstPixmap->addr()),
        srcPixelMap->GetRowStride());
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
} // namespace Rosen
} // namespace OHOS