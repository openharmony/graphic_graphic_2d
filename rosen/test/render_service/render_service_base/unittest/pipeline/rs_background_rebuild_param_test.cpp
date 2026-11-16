/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "pipeline/rs_background_rebuild_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSBackgroundRebuildParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RSBackgroundRebuildParamTest::SetUpTestCase() {}
void RSBackgroundRebuildParamTest::TearDownTestCase() {}
void RSBackgroundRebuildParamTest::SetUp() {}
void RSBackgroundRebuildParamTest::TearDown() {}

/**
 * @tc.name: InstanceTest
 * @tc.desc: Verify Instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundRebuildParamTest, InstanceTest, Function | SmallTest | Level1)
{
    RSBackgroundRebuildParam& instance = RSBackgroundRebuildParam::Instance();
    EXPECT_EQ(&instance, &RSBackgroundRebuildParam::Instance());
}

/**
 * @tc.name: SetBackgroundRebuildEnabledTrue
 * @tc.desc: Verify SetBackgroundRebuildEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundRebuildParamTest, SetBackgroundRebuildEnabledTrue, Function | SmallTest | Level1)
{
    RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(true);
    EXPECT_EQ(RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled(), true);
}

/**
 * @tc.name: SetBackgroundRebuildEnabledFalse
 * @tc.desc: Verify SetBackgroundRebuildEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundRebuildParamTest, SetBackgroundRebuildEnabledFalse, Function | SmallTest | Level1)
{
    RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(false);
    EXPECT_EQ(RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled(), false);
}

/**
 * @tc.name: IsBackgroundRebuildEnabledDefault
 * @tc.desc: Verify IsBackgroundRebuildEnabledDefault
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundRebuildParamTest, IsBackgroundRebuildEnabledDefault, Function | SmallTest | Level1)
{
    RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(false);
    EXPECT_EQ(RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled(), false);
}
} // namespace Rosen
} // namespace OHOS
