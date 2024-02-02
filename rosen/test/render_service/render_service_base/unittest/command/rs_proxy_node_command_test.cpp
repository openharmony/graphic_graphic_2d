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
#include "include/command/rs_proxy_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProxyNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSProxyNodeCommandTest::SetUpTestCase() {}
void RSProxyNodeCommandTest::TearDownTestCase() {}
void RSProxyNodeCommandTest::SetUp() {}
void RSProxyNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestRSProxyNodeCommand001
 * @tc.desc: ResetContextVariableCache test.
 * @tc.type: FUNC
 */
HWTEST_F(RSProxyNodeCommandTest, TestRSProxyNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    ProxyNodeCommandHelper::ResetContextVariableCache(context, id);

    NodeId targetId = static_cast<NodeId>(2);
    ProxyNodeCommandHelper::Create(context, id, targetId);
    ProxyNodeCommandHelper::ResetContextVariableCache(context, id);
}
} // namespace OHOS::Rosen
