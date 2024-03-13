/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "utils/kawase_blur_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class KawaseBlurUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void KawaseBlurUtilsTest::SetUpTestCase() {}
void KawaseBlurUtilsTest::TearDownTestCase() {}
void KawaseBlurUtilsTest::SetUp() {}
void KawaseBlurUtilsTest::TearDown() {}

/**
 * @tc.name: testInterface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurUtilsTest, testInterface, TestSize.Level1)
{
    std::unique_ptr<KawaseBlurUtils> kawaseFunc = std::make_unique<KawaseBlurUtils>();
    Drawing::Canvas canvas;
    Drawing::Brush brush;
    Drawing::Rect src;
    Drawing::Rect dst;
    float radius = 1.f;
    auto param = Drawing::KawaseParameters{src, dst, radius, nullptr, 1.f};
    std::shared_ptr<Drawing::Image> image;
    Drawing::KawaseProperties properties;
    kawaseFunc->ApplyKawaseBlur(canvas, brush, image, param, properties);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS