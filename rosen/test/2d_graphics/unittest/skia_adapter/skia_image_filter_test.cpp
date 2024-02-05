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
#include "include/effects/SkImageFilters.h"
#include "skia_adapter/skia_image_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImageFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaImageFilterTest::SetUpTestCase() {}
void SkiaImageFilterTest::TearDownTestCase() {}
void SkiaImageFilterTest::SetUp() {}
void SkiaImageFilterTest::TearDown() {}

/**
 * @tc.name: InitWithBlur001
 * @tc.desc: Test InitWithBlur
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageFilterTest, InitWithBlur001, TestSize.Level1)
{
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();
    skiaImageFilter->InitWithBlur(
        5.0f, 5.0f, TileMode::REPEAT, nullptr, ImageBlurType::GAUSS); // 5.0f: sigmaX and sigmaY
}

/**
 * @tc.name: InitWithBlur002
 * @tc.desc: Test InitWithBlur
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageFilterTest, InitWithBlur002, TestSize.Level1)
{
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();

    skiaImageFilter->InitWithBlur(
        5.0f, 5.0f, TileMode::MIRROR, nullptr, ImageBlurType::GAUSS); // 5.0f: sigmaX and sigmaY
}

/**
 * @tc.name: InitWithBlur003
 * @tc.desc: Test InitWithBlur
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageFilterTest, InitWithBlur003, TestSize.Level1)
{
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();
    skiaImageFilter->InitWithBlur(
        5.0f, 5.0f, TileMode::DECAL, nullptr, ImageBlurType::GAUSS); // 5.0f: sigmaX and sigmaY
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageFilterTest, Serialize001, TestSize.Level1)
{
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();
    skiaImageFilter->Serialize();
    sk_sp<SkImageFilter> blur(SkImageFilters::Blur(20.0f, 20.0f, nullptr)); // 20.0f: sigmaX and sigmaY
    skiaImageFilter->SetSkImageFilter(blur);
    skiaImageFilter->Serialize();
}

/**
 * @tc.name: Deserialize001
 * @tc.desc: Test Deserialize
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageFilterTest, Deserialize001, TestSize.Level1)
{
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();
    skiaImageFilter->Deserialize(nullptr);
    std::shared_ptr<Data> data = std::make_shared<Data>();
    skiaImageFilter->Deserialize(data);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS