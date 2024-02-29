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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_color_filter.h"
#include "effect/color_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaColorFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaColorFilterTest::SetUpTestCase() {}
void SkiaColorFilterTest::TearDownTestCase() {}
void SkiaColorFilterTest::SetUp() {}
void SkiaColorFilterTest::TearDown() {}

/**
 * @tc.name: InitWithCompose001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaColorFilterTest, InitWithCompose001, TestSize.Level1)
{
    auto colorFilter1 = ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA;
    auto colorFilter2 = ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithCompose(ColorFilter(colorFilter1), ColorFilter(colorFilter2));
    EXPECT_TRUE(skiaColorFilter.GetColorFilter() != nullptr);
}

/**
 * @tc.name: Compose001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaColorFilterTest, Compose001, TestSize.Level1)
{
    auto colorFilter = ColorFilter::FilterType::SRGB_GAMMA_TO_LINEAR;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLinearToSrgbGamma();
    skiaColorFilter.Compose(ColorFilter(colorFilter));
    EXPECT_TRUE(skiaColorFilter.GetColorFilter() != nullptr);
}

/**
 * @tc.name: InitWithBlendMode001
 * @tc.desc: Test InitWithBlendMode
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, InitWithBlendMode001, TestSize.Level1)
{
    ColorQuad c = 0xFF000000;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithBlendMode(c, BlendMode::CLEAR);
}

/**
 * @tc.name: InitWithColorMatrix001
 * @tc.desc: Test InitWithColorMatrix
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorMatrix001, TestSize.Level1)
{
    ColorMatrix cm;
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorMatrix(cm);
}

/**
 * @tc.name: InitWithColorFloat001
 * @tc.desc: Test InitWithColorFloat
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, InitWithColorFloat001, TestSize.Level1)
{
    static const float colorMatrixArray[20] = {
        0.402,  -1.174, -0.228, 1.0, 0.0,
        -0.598, -0.174, -0.228, 1.0, 0.0,
        -0.599, -1.175, 0.772,  1.0, 0.0,
        0.0,    0.0,    0.0,    1.0, 0.0
    };
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithColorFloat(colorMatrixArray);
}

/**
 * @tc.name: InitWithSrgbGammaToLinear001
 * @tc.desc: Test InitWithSrgbGammaToLinear
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, InitWithSrgbGammaToLinear001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithSrgbGammaToLinear();
}

/**
 * @tc.name: InitWithLuma001
 * @tc.desc: Test InitWithLuma
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, InitWithLuma001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.InitWithLuma();
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, Serialize001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    skiaColorFilter.Serialize();
    skiaColorFilter.Deserialize(nullptr);
}

/**
 * @tc.name: AsAColorMatrix001
 * @tc.desc: Test AsAColorMatrix
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaColorFilterTest, AsAColorMatrix001, TestSize.Level1)
{
    SkiaColorFilter skiaColorFilter;
    scalar colorMatrixArray[20] = {
        0.402,  -1.174, -0.228, 1.0, 0.0,
        -0.598, -0.174, -0.228, 1.0, 0.0,
        -0.599, -1.175, 0.772,  1.0, 0.0,
        0.0,    0.0,    0.0,    1.0, 0.0
    };
    skiaColorFilter.AsAColorMatrix(colorMatrixArray);
    skiaColorFilter.InitWithLuma();
    skiaColorFilter.AsAColorMatrix(colorMatrixArray);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS