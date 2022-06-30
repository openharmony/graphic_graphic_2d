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

#include "effect/filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FilterTest::SetUpTestCase() {}
void FilterTest::TearDownTestCase() {}
void FilterTest::SetUp() {}
void FilterTest::TearDown() {}

/*
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, CreateAndDestroy001, TestSize.Level1)
{
    auto filter = std::make_unique<Filter>();
    EXPECT_TRUE(nullptr != filter);
}

/*
 * @tc.name: GetterAndSetterColorFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, GetterAndSetterColorFilter001, TestSize.Level1)
{
    auto filter = std::make_unique<Filter>();
    ASSERT_TRUE(filter != nullptr);
    auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::BLEND_MODE);
    filter->SetColorFilter(colorFilter);
    auto result = filter->GetColorFilter();
    EXPECT_FALSE(result != colorFilter);
}

/*
 * @tc.name: GetterAndSetterColorFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, GetterAndSetterColorFilter002, TestSize.Level1)
{
    auto filter = std::make_unique<Filter>();
    ASSERT_TRUE(filter != nullptr);
    auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    filter->SetColorFilter(colorFilter);
    auto result = filter->GetColorFilter();
    EXPECT_FALSE(result != colorFilter);
}

/*
 * @tc.name: GetterAndSetterMaskFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, GetterAndSetterMaskFilter001, TestSize.Level1)
{
    auto filter = std::make_unique<Filter>();
    ASSERT_TRUE(filter != nullptr);
    auto maskFilter = std::make_shared<MaskFilter>(MaskFilter::FilterType::BLUR, BlurType::SOLID, 0.5f);
    filter->SetMaskFilter(maskFilter);
    auto result = filter->GetMaskFilter();
    EXPECT_FALSE(result != maskFilter);
}

/*
 * @tc.name: GetterAndSetterMaskFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, GetterAndSetterMaskFilter002, TestSize.Level1)
{
    auto filter = std::make_unique<Filter>();
    ASSERT_TRUE(filter != nullptr);
    auto maskFilter = std::make_shared<MaskFilter>(MaskFilter::FilterType::NO_TYPE, BlurType::SOLID, 0.2f);
    filter->SetMaskFilter(maskFilter);
    auto result = filter->GetMaskFilter();
    EXPECT_FALSE(result != maskFilter);
}

/*
 * @tc.name: GetterAndSetterFilterQuality001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, GetterAndSetterFilterQuality001, TestSize.Level1)
{
    auto filter = std::make_unique<Filter>();
    ASSERT_TRUE(filter != nullptr);
    Filter::FilterQuality filterQuality = Filter::FilterQuality::LOW;
    filter->SetFilterQuality(filterQuality);
    auto result = filter->GetFilterQuality();
    EXPECT_FALSE(result != filterQuality);
}

/*
 * @tc.name: GetterAndSetterFilterQuality002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, GetterAndSetterFilterQuality002, TestSize.Level1)
{
    auto filter = std::make_unique<Filter>();
    ASSERT_TRUE(filter != nullptr);
    Filter::FilterQuality filterQuality = Filter::FilterQuality::MEDIUM;
    filter->SetFilterQuality(filterQuality);
    auto result = filter->GetFilterQuality();
    EXPECT_FALSE(result != filterQuality);
}

/*
 * @tc.name: OverrideEquals001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, OverrideEquals001, TestSize.Level1)
{
    Filter filter1;
    auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::BLEND_MODE);
    filter1.SetColorFilter(colorFilter);
    Filter filter2;
    auto colorFilter2 = std::make_shared<ColorFilter>(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    filter2.SetColorFilter(colorFilter2);
    EXPECT_FALSE(filter1 == filter2);
}

/*
 * @tc.name: OverrideEquals002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, OverrideEquals002, TestSize.Level1)
{
    Filter filter1;
    Filter filter2 = filter1;
    EXPECT_TRUE(filter1 == filter2);
}

/*
 * @tc.name: OverrideNotEquals001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, OverrideNotEquals001, TestSize.Level1)
{
    Filter filter1;
    auto colorFilter = std::make_shared<ColorFilter>(ColorFilter::FilterType::BLEND_MODE);
    filter1.SetColorFilter(colorFilter);
    Filter filter2;
    auto colorFilter2 = std::make_shared<ColorFilter>(ColorFilter::FilterType::LINEAR_TO_SRGB_GAMMA);
    filter2.SetColorFilter(colorFilter2);
    EXPECT_TRUE(filter1 != filter2);
}

/*
 * @tc.name: OverrideNotEquals002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(FilterTest, OverrideNotEquals002, TestSize.Level1)
{
    Filter filter1;
    Filter filter2;
    EXPECT_FALSE(filter1 != filter2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS