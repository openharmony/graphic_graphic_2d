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

#include "gtest/gtest.h"

#include "render/rs_kawase_blur.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class KawaseBlurFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void KawaseBlurFilterTest::SetUpTestCase() {}
void KawaseBlurFilterTest::TearDownTestCase() {}
void KawaseBlurFilterTest::SetUp() {}
void KawaseBlurFilterTest::TearDown() {}

/**
 * @tc.name: testInterface
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(KawaseBlurFilterTest, testInterface, TestSize.Level1)
{
    KawaseBlurFilter *kawaseBlurFilter = KawaseBlurFilter::GetKawaseBlurFilter();

    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    KawaseParameter param = KawaseParameter(src, dst, 1);
    std::shared_ptr<Drawing::Image> image;

    kawaseBlurFilter->ApplyKawaseBlur(canvas, image, param);
}
} // namespace Rosen
} // namespace OHOS
