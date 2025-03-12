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

#include "speciallayer_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class SpecialLayerParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SpecialLayerParamTest::SetUpTestCase() {}
void SpecialLayerParamTest::TearDownTestCase() {}
void SpecialLayerParamTest::SetUp() {}
void SpecialLayerParamTest::TearDown() {}

/**
 * @tc.name: SetSpecialLayerEnable
 * @tc.desc: Verify the SetSpecialLayerEnable function
 * @tc.type: FUNC
 * @tc.require: #
 */
HWTEST_F(SpecialLayerParamTest, SetSpecialLayerEnable, Function | SmallTest | Level1)
{
    SpecialLayerParam specialLayerParam;
    specialLayerParam.SetSpecialLayerEnable(true);
    ASSERT_EQ(specialLayerParam.isSpecialLayerEnable_, true);
    specialLayerParam.SetSpecialLayerEnable(false);
    ASSERT_EQ(specialLayerParam.isSpecialLayerEnable_, false);
}

/**
 * @tc.name: IsSpecialLayerEnable
 * @tc.desc: Verify the result of IsSpecialLayerEnable function
 * @tc.type: FUNC
 * @tc.require: #
 */
HWTEST_F(SpecialLayerParamTest, IsSpecialLayerEnable, Function | SmallTest | Level1)
{
    SpecialLayerParam specialLayerParam;
    specialLayerParam.isSpecialLayerEnable_ = true;
    ASSERT_TRUE(specialLayerParam.IsSpecialLayerEnable());
    specialLayerParam.isSpecialLayerEnable_ = false;
    ASSERT_FALSE(specialLayerParam.IsSpecialLayerEnable());
}
} // namespace Rosen
} // namespace OHOS