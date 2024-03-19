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
#include "skia_adapter/skia_kawase_blur_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaKawaseBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaKawaseBlurFilterTest::SetUpTestCase() {}
void SkiaKawaseBlurFilterTest::TearDownTestCase() {}
void SkiaKawaseBlurFilterTest::SetUp() {}
void SkiaKawaseBlurFilterTest::TearDown() {}

/**
 * @tc.name: GetKawaseBlurFilterTest
 * @tc.desc: Test GetKawaseBlurFilter
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaKawaseBlurFilterTest, GetKawaseBlurFilterTest, TestSize.Level1)
{
    SkiaKawaseBlurFilter* filter = SkiaKawaseBlurFilter::GetKawaseBlurFilter();
    EXPECT_NE(filter, nullptr);
}

/**
 * @tc.name: ApplyKawaseBlurTest
 * @tc.desc: Test ApplyKawaseBlur
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaKawaseBlurFilterTest, ApplyKawaseBlurTest, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    float radius = 1.f;
    auto param = Drawing::KawaseParameters{src, dst, radius, nullptr, 1.f};
    std::shared_ptr<Drawing::Image> image;
    Drawing::KawaseProperties properties;

    SkiaKawaseBlurFilter::GetKawaseBlurFilter()->ApplyKawaseBlur(canvas, image, param, properties);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS