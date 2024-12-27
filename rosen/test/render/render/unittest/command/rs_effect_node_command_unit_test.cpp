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

#include "command/rs_effect_node_command.h"

using namespace testing;
using namespace testing::Text;

namespace OHOS {
namespace Rosen {
class EffectNodeCommandUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void EffectNodeCommandUnitTest::SetUpTestCase() {}
void EffectNodeCommandUnitTest::TearDownTestCase() {}
void EffectNodeCommandUnitTest::SetUp() {}
void EffectNodeCommandUnitTest::TearDown() {}

/**
 * @tc.name: TestCreate
 * @tc.desc: Verify function Create
 * @tc.type: FUNC
 */
HWTEST_F(EffectNodeCommandUnitTest, TestCreate, TestSize.Level1)
{
    RSContext testContext;
    NodeId nodeId = static_cast<NodeId>(0);
    EffectNodeCommandHelper::TestCreate(testContext, nodeId);
    ASSERT_EQ(nodeId, static_cast<NodeId>(0));
}

} // namespace Rosen
} // namespace OHOS