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
#include "include/render/rs_shadow.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSShadowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShadowTest::SetUpTestCase() {}
void RSShadowTest::TearDownTestCase() {}
void RSShadowTest::SetUp() {}
void RSShadowTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSShadowTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    std::shared_ptr<RSShadow>rsShadow = std::make_shared<RSShadow>();
    ASSERT_FALSE(rsShadow->IsValid());
    rsShadow->SetElevation(5.f);
    ASSERT_FALSE(rsShadow->IsValid());
}

/**
 * @tc.name: TestRSShadow001
 * @tc.desc: IsValid test.
 * @tc.type: FUNC
 */
HWTEST_F(RSShadowTest, TestRSShadow001, TestSize.Level1)
{
    RSShadow shadow;
    shadow.SetRadius(0.1);
    shadow.IsValid();
}

/**
 * @tc.name: TestRSShadow002
 * @tc.desc: IsValid test.
 * @tc.type: FUNC
 */
HWTEST_F(RSShadowTest, TestRSShadow002, TestSize.Level1)
{
    RSShadow shadow;
    shadow.SetElevation(0.1);
    shadow.IsValid();
}
} // namespace OHOS::Rosen
