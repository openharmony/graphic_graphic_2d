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
#include "skia_adapter/skia_shader_effect.h"
#include "effect/shader_effect.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaShaderEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaShaderEffectTest::SetUpTestCase() {}
void SkiaShaderEffectTest::TearDownTestCase() {}
void SkiaShaderEffectTest::SetUp() {}
void SkiaShaderEffectTest::TearDown() {}

/**
 * @tc.name: InitWithImage001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithImage001, TestSize.Level1)
{
    Image image;
    Matrix matrix;
    SamplingOptions samplingOptions;
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithImage(image, TileMode::MIRROR, TileMode::REPEAT, samplingOptions, matrix);
}

/**
 * @tc.name: InitWithPicture001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaShaderEffectTest, InitWithPicture001, TestSize.Level1)
{
    Picture picture;
    Matrix matrix;
    Rect rect;
    SkiaShaderEffect skiaShaderEffect;
    skiaShaderEffect.InitWithPicture(picture, TileMode::MIRROR, TileMode::CLAMP, FilterMode::LINEAR, matrix, rect);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS