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
    EXPECT_EQ(FilterParam::IsFilterCacheEnable(), true);
    FilterParam::SetFilterCacheEnable(false);
    EXPECT_EQ(FilterParam::IsFilterCacheEnable(), false);
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
    EXPECT_EQ(FilterParam::IsEffectMergeEnable(), true);
    FilterParam::SetEffectMergeEnable(false);
    EXPECT_EQ(FilterParam::IsEffectMergeEnable(), false);
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
    EXPECT_EQ(FilterParam::IsBlurAdaptiveAdjust(), true);
    FilterParam::SetBlurAdaptiveAdjust(false);
    EXPECT_EQ(FilterParam::IsBlurAdaptiveAdjust(), false);
}

/**
 * @tc.name: MesablurAllEnableTest
 * @tc.desc: Verify the SetMesablurAllEnable and IsMesablurAllEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, MesablurAllEnableTest, Function | SmallTest | Level1)
{
    FilterParam::SetMesablurAllEnable(true);
    EXPECT_EQ(FilterParam::IsMesablurAllEnable(), true);
    FilterParam::SetMesablurAllEnable(false);
    EXPECT_EQ(FilterParam::IsMesablurAllEnable(), false);
}

/**
 * @tc.name: SimplifiedMesaModeTest
 * @tc.desc: Verify the SetSimplifiedMesaMode and GetSimplifiedMesaMode function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(FilterParamTest, SimplifiedMesaModeTest, Function | SmallTest | Level1)
{
    for (int i = 0; i < 3; i++) {
        FilterParam::SetSimplifiedMesaMode(i);
        EXPECT_EQ(FilterParam::GetSimplifiedMesaMode(), i);
    }
}
} // namespace Rosen
} // namespace OHOS