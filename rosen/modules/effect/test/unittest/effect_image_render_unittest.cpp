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

#include <gtest/gtest.h>
#include "effect_image_render.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class EffectImageFilterUnittest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

class EffectImageRenderUnittest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: FilterTest001
 * @tc.desc: create a filter
 */
HWTEST_F(EffectImageFilterUnittest, FilterTest001, TestSize.Level1)
{
    auto filter = EffectImageFilter::Blur(0.5);
    EXPECT_TRUE(filter != nullptr);

    filter = EffectImageFilter::Blur(-1);
    EXPECT_TRUE(filter == nullptr);

    filter = EffectImageFilter::Brightness(0.5);
    EXPECT_TRUE(filter != nullptr);

    filter = EffectImageFilter::Brightness(-1);
    EXPECT_TRUE(filter == nullptr);

    filter = EffectImageFilter::Brightness(10);
    EXPECT_TRUE(filter == nullptr);

    filter = EffectImageFilter::Grayscale();
    EXPECT_TRUE(filter != nullptr);

    filter = EffectImageFilter::Invert();
    EXPECT_TRUE(filter != nullptr);

    Drawing::ColorMatrix matrix;
    filter = EffectImageFilter::ApplyColorMatrix(matrix);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ApplyTest001
 * @tc.desc: apply drawing
 */
HWTEST_F(EffectImageFilterUnittest, ApplyTest001, TestSize.Level1)
{
    auto filter = EffectImageFilter::Brightness(0.5);
    EXPECT_TRUE(filter != nullptr);
    auto ret = filter->Apply(nullptr);
    EXPECT_TRUE(ret != DrawingError::ERR_OK);

    auto image = std::make_shared<EffectImageChain>();
    EXPECT_TRUE(image != nullptr);
    ret = filter->Apply(image);
    EXPECT_TRUE(ret == DrawingError::ERR_OK);

    auto filterBlur = EffectImageFilter::Blur(0.5);
    EXPECT_TRUE(filterBlur != nullptr);
    ret = filterBlur->Apply(nullptr);
    EXPECT_TRUE(ret != DrawingError::ERR_OK);
}

/**
 * @tc.name: RenderTest001
 * @tc.desc: Test Render
 */
HWTEST_F(EffectImageRenderUnittest, RenderTest001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size ={ 1, 1 };
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));

    std::vector<std::shared_ptr<EffectImageFilter>> imageFilter;
    imageFilter.emplace_back(nullptr);
    auto filterBlur = EffectImageFilter::Blur(0.5);
    EXPECT_TRUE(filterBlur != nullptr);
    imageFilter.emplace_back(filterBlur);

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    EffectImageRender imageRender;
    auto ret = imageRender.Render(srcPixelMap, imageFilter, true, dstPixelMap);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    ret = imageRender.Render(nullptr, imageFilter, true, dstPixelMap);
    ASSERT_NE(ret, DrawingError::ERR_OK);
}
} // namespace Rosen
} // namespace OHOS