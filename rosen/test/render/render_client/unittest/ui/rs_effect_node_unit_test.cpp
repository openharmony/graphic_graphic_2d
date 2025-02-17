/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ui/rs_effect_node.h"
#include "transaction/rs_transaction_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RsEffectNodesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsEffectNodesTest::SetUpTestCase() {}
void RsEffectNodesTest::TearDownTestCase() {}
void RsEffectNodesTest::SetUp() {}
void RsEffectNodesTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsEffectNodesTest, Create001, TestSize.Level1)
{
    RSEffectNode::SharedPtr effectNode = RSEffectNode::Create(true);
    ASSERT_TRUE(effectNode != nullptr);
}

/**
 * @tc.name: SetFreeze
 * @tc.desc: test results of SetFreeze
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsEffectNodesTest, SetFreeze, TestSize.Level1)
{
    RSEffectNode::SharedPtr effectNode = RSEffectNode::Create(true);
    effectNode->SetFreeze(true);
    ASSERT_TRUE(effectNode != nullptr);

    RSSystemProperties::GetUniRenderEnabled();
    RSSystemProperties::isUniRenderEnabled_ = true;
    effectNode->SetFreeze(true);
    ASSERT_TRUE(RSSystemProperties::isUniRenderEnabled_);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    effectNode->SetFreeze(true);
    ASSERT_TRUE(RSSystemProperties::isUniRenderEnabled_);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: Create002
 * @tc.desc: test results of Create
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsEffectNodesTest, Create002, TestSize.Level1)
{
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSEffectNode::SharedPtr effectNode = RSEffectNode::Create(true);
    EXPECT_TRUE(effectNode != nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

} // namespace Rosen
} // namespace OHOS