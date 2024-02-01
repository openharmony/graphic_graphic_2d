/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class EffectNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void EffectNodeCommandTest::SetUpTestCase() {}
void EffectNodeCommandTest::TearDownTestCase() {}
void EffectNodeCommandTest::SetUp() {}
void EffectNodeCommandTest::TearDown() {}

/**
 * @tc.name: UpdateRange
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EffectNodeCommandTest, Create, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(0);
    EffectNodeCommandHelper::Create(context, nodeId);
}

} // namespace Rosen
} // namespace OHOS