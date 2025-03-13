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
#include "include/command/rs_proxy_node_command.h"

using namespace testing;
using namespace testing::Text;

namespace OHOS::Rosen {
class RSProxyNodeCommandUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSProxyNodeCommandUnitTest::SetUpTestCase() {}
void RSProxyNodeCommandUnitTest::TearDownTestCase() {}
void RSProxyNodeCommandUnitTest::SetUp() {}
void RSProxyNodeCommandUnitTest::TearDown() {}

/**
 * @tc.name: TestRSProxyNodeCommand01
 * @tc.desc: Verify function ResetContextVariableCache.
 * @tc.type: FUNC
 */
HWTEST_F(RSProxyNodeCommandUnitTest, TestRSProxyNodeCommand01, TestSize.Level1)
{
    RSContext testContext;
    NodeId id = static_cast<NodeId>(1);
    ProxyNodeCommandHelper::ResetContextVariableCache(testContext, id);
    ASSERT_EQ(id, static_cast<NodeId>(1));

    NodeId targetId = static_cast<NodeId>(2);
    ProxyNodeCommandHelper::Create(testContext, id, targetId);
    ProxyNodeCommandHelper::ResetContextVariableCache(testContext, id);
    ASSERT_EQ(targetId, static_cast<NodeId>(2));
}
} // namespace OHOS::Rosen
