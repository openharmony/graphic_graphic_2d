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
#include "delegate/rs_functional_delegate.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSFunctionalDelegateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSFunctionalDelegate> rsfunctionaldelegate_;
};

void RSFunctionalDelegateTest::SetUpTestCase() {}
void RSFunctionalDelegateTest::TearDownTestCase() {}
void RSFunctionalDelegateTest::SetUp()
{
    rsfunctionaldelegate_ = std::make_shared<RSFunctionalDelegate>();
}
void RSFunctionalDelegateTest::TearDown() {}

/**
 * @tc.name: Repaint001
 * @tc.desc: test results of Repaint
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSFunctionalDelegateTest, Repaint001, TestSize.Level1)
{
    int callback = 1;
    rsfunctionaldelegate_->SetRepaintCallback([&](){callback = 2;});
    rsfunctionaldelegate_->Repaint();
    ASSERT_EQ(callback, 2);
}

/**
 * @tc.name: Repaint02
 * @tc.desc: test results of Repaint
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSFunctionalDelegateTest, Repaint002, TestSize.Level1)
{
    rsfunctionaldelegate_->SetRepaintCallback(nullptr);
    rsfunctionaldelegate_->Repaint();
}
} // namespace OHOS::Rosen