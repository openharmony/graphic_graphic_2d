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

#include "drm_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class DrmParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DrmParamTest::SetUpTestCase() {}
void DrmParamTest::TearDownTestCase() {}
void DrmParamTest::SetUp() {}
void DrmParamTest::TearDown() {}

/**
 * @tc.name: SetDrmEnable
 * @tc.desc: Verify the SetDrmEnable function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DrmParamTest, SetDrmEnable, Function | SmallTest | Level1)
{
    DRMParam::SetDrmEnable(false);
    EXPECT_EQ(DRMParam::IsDrmEnable(), false);
    DRMParam::SetDrmEnable(true);
    EXPECT_EQ(DRMParam::IsDrmEnable(), true);
}
} // namespace Rosen
} // namespace OHOS