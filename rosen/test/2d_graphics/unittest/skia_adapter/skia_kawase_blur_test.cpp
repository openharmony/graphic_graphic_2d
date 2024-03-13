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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_kawase_blur.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaKawaseBlurTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaKawaseBlurTest::SetUpTestCase() {}
void SkiaKawaseBlurTest::TearDownTestCase() {}
void SkiaKawaseBlurTest::SetUp() {}
void SkiaKawaseBlurTest::TearDown() {}

/**
 * @tc.name: DrawWithKawaseBlurTest
 * @tc.desc: Test DrawWithKawaseBlur
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaKawaseBlurTest, DrawWithKawaseBlurTest, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Brush brush;
    Drawing::Rect src;
    Drawing::Rect dst;
    float radius = 1.f;
    auto param = Drawing::KawaseParameters{src, dst, radius, nullptr, 1.f};
    std::shared_ptr<Drawing::Image> image;
    Drawing::KawaseProperties properties;

    std::shared_ptr<SkiaKawaseBlur> kawaseFunc = std::make_shared<SkiaKawaseBlur>();
    kawaseFunc->DrawWithKawaseBlur(canvas, brush, image, param, properties);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS