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
    FilterParam filterParam;
    filterParam.SetFilterCacheEnable(true);
    EXPECT_EQ(filterParam.isFilterCacheEnable_, true);
    filterParam.SetFilterCacheEnable(false);
    EXPECT_EQ(filterParam.isFilterCacheEnable_, false);
}

/**
 * @tc.name: SetEffectMergeEnable
 * @tc.desc: Verify the SetEffectMergeEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, SetEffectMergeEnable, Function | SmallTest | Level1)
{
    FilterParam filterParam;
    filterParam.SetEffectMergeEnable(true);
    EXPECT_EQ(filterParam.isEffectMergeEnable_, true);
    filterParam.SetEffectMergeEnable(false);
    EXPECT_EQ(filterParam.isEffectMergeEnable_, false);
}

/**
 * @tc.name: IsFilterCacheEnable
 * @tc.desc: Verify the result of IsFilterCacheEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, IsFilterCacheEnable, Function | SmallTest | Level1)
{
    FilterParam filterParam;
    filterParam.isFilterCacheEnable_ = true;
    EXPECT_TRUE(filterParam.IsFilterCacheEnable());
    filterParam.isFilterCacheEnable_ = false;
    EXPECT_FALSE(filterParam.IsFilterCacheEnable());
}

/**
 * @tc.name: IsEffectMergeEnable
 * @tc.desc: Verify the result of IsEffectMergeEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, IsEffectMergeEnable, Function | SmallTest | Level1)
{
    FilterParam filterParam;
    filterParam.isEffectMergeEnable_ = true;
    EXPECT_TRUE(filterParam.IsEffectMergeEnable());
    filterParam.isEffectMergeEnable_ = false;
    EXPECT_FALSE(filterParam.IsEffectMergeEnable());
}
} // namespace Rosen
} // namespace OHOS