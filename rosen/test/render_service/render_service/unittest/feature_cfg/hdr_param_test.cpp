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

#include "hdr_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class HdrParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdrParamTest::SetUpTestCase() {}
void HdrParamTest::TearDownTestCase() {}
void HdrParamTest::SetUp() {}
void HdrParamTest::TearDown() {}

/**
 * @tc.name: SetHdrVideoEnable
 * @tc.desc: Verify the SetHdrVideoEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HdrParamTest, SetHdrVideoEnable, Function | SmallTest | Level1)
{
    HDRParam hdrParam;
    hdrParam.SetHdrVideoEnable(true);
    EXPECT_EQ(hdrParam.isHdrVideoEnable_, true);
    hdrParam.SetHdrVideoEnable(false);
    EXPECT_EQ(hdrParam.isHdrVideoEnable_, false);
}

/**
 * @tc.name: SetHdrImageEnable
 * @tc.desc: Verify the SetHdrImageEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HdrParamTest, SetHdrImageEnable, Function | SmallTest | Level1)
{
    HDRParam hdrParam;
    hdrParam.SetHdrImageEnable(true);
    EXPECT_EQ(hdrParam.isHdrImageEnable_, true);
    hdrParam.SetHdrImageEnable(false);
    EXPECT_EQ(hdrParam.isHdrImageEnable_, false);
}

/**
 * @tc.name: IsHdrVideoEnable
 * @tc.desc: Verify the result of IsHdrVideoEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HdrParamTest, IsHdrVideoEnable, Function | SmallTest | Level1)
{
    HDRParam hdrParam;
    hdrParam.isHdrVideoEnable_ = true;
    EXPECT_TRUE(hdrParam.IsHdrVideoEnable());
    hdrParam.isHdrVideoEnable_ = false;
    EXPECT_FALSE(hdrParam.IsHdrVideoEnable());
}

/**
 * @tc.name: IsHdrImageEnable
 * @tc.desc: Verify the result of IsHdrImageEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(HdrParamTest, IsHdrImageEnable, Function | SmallTest | Level1)
{
    HDRParam hdrParam;
    hdrParam.isHdrImageEnable_ = true;
    EXPECT_TRUE(hdrParam.IsHdrImageEnable());
    hdrParam.isHdrImageEnable_ = false;
    EXPECT_FALSE(hdrParam.IsHdrImageEnable());
}
} // namespace Rosen
} // namespace OHOS