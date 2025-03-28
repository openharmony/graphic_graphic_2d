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
#include <test_header.h>

#include "filter_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class FilterParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FilterParamTest::SetUpTestCase() {}
void FilterParamTest::TearDownTestCase() {}
void FilterParamTest::SetUp() {}
void FilterParamTest::TearDown() {}

/**
 * @tc.name: SetFilterCacheEnable
 * @tc.desc: Verify the SetFilterCacheEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, SetFilterCacheEnable, Function | SmallTest | Level1)
{
    FilterParam::SetFilterCacheEnable(true);
    EXPECT_EQ(FilterParam::isFilterCacheEnable_, true);
    FilterParam::SetFilterCacheEnable(false);
    EXPECT_EQ(FilterParam::isFilterCacheEnable_, false);
}

/**
 * @tc.name: SetEffectMergeEnable
 * @tc.desc: Verify the SetEffectMergeEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, SetEffectMergeEnable, Function | SmallTest | Level1)
{
    FilterParam::SetEffectMergeEnable(true);
    EXPECT_EQ(FilterParam::isEffectMergeEnable_, true);
    FilterParam::SetEffectMergeEnable(false);
    EXPECT_EQ(FilterParam::isEffectMergeEnable_, false);
}

/**
 * @tc.name: SetBlurAdaptiveAdjust
 * @tc.desc: Verify the SetBlurAdaptiveAdjust function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, SetBlurAdaptiveAdjust, Function | SmallTest | Level1)
{
    FilterParam::SetBlurAdaptiveAdjust(true);
    EXPECT_EQ(FilterParam::isBlurAdaptiveAdjust, true);
    FilterParam::SetBlurAdaptiveAdjust(false);
    EXPECT_EQ(FilterParam::isBlurAdaptiveAdjust, false);
}

/**
 * @tc.name: IsFilterCacheEnable
 * @tc.desc: Verify the result of IsFilterCacheEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, IsFilterCacheEnable, Function | SmallTest | Level1)
{
    FilterParam::isFilterCacheEnable_ = true;
    EXPECT_TRUE(FilterParam::IsFilterCacheEnable());
    FilterParam::isFilterCacheEnable_ = false;
    EXPECT_FALSE(FilterParam::IsFilterCacheEnable());
}

/**
 * @tc.name: IsEffectMergeEnable
 * @tc.desc: Verify the result of IsEffectMergeEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, IsEffectMergeEnable, Function | SmallTest | Level1)
{
    FilterParam::isEffectMergeEnable_ = true;
    EXPECT_TRUE(FilterParam::IsEffectMergeEnable());
    FilterParam::isEffectMergeEnable_ = false;
    EXPECT_FALSE(FilterParam::IsEffectMergeEnable());
}

/**
 * @tc.name: IsBlurAdaptiveAdjust
 * @tc.desc: Verify the result of IsBlurAdaptiveAdjust function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, IsBlurAdaptiveAdjust, Function | SmallTest | Level1)
{
    FilterParam::isBlurAdaptiveAdjust = true;
    EXPECT_TRUE(FilterParam::IsBlurAdaptiveAdjust());
    FilterParam::isBlurAdaptiveAdjust = false;
    EXPECT_FALSE(FilterParam::IsBlurAdaptiveAdjust());
}
} // namespace Rosen
} // namespace OHOS