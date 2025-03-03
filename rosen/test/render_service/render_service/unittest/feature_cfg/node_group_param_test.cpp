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

#include "node_group_param.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class NodeGroupParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NodeGroupParamTest::SetUpTestCase() {}
void NodeGroupParamTest::TearDownTestCase() {}
void NodeGroupParamTest::SetUp() {}
void NodeGroupParamTest::TearDown() {}

/*
 * @tc.name: SetGroupByUIEnabled
 * @tc.desc: Test SetGroupByUIEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NodeGroupParamTest, SetGroupByUIEnabled, Function | SmallTest | Level1)
{
    std::shared_ptr<NodeGroupParam> nodeGroupParam = std::make_shared<NodeGroupParam>();
    nodeGroupParam->SetGroupByUIEnabled(true);
    EXPECT_EQ(nodeGroupParam->groupByUIEnabled_, true);
    nodeGroupParam->SetGroupByUIEnabled(false);
    EXPECT_EQ(nodeGroupParam->groupByUIEnabled_, false);
}

/*
 * @tc.name: IsGroupByUIEnabled
 * @tc.desc: Test IsGroupByUIEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NodeGroupParamTest, IsGroupByUIEnabled, Function | SmallTest | Level1)
{
    std::shared_ptr<NodeGroupParam> nodeGroupParam = std::make_shared<NodeGroupParam>();
    EXPECT_EQ(nodeGroupParam->IsGroupByUIEnabled(), true);
    nodeGroupParam->groupByUIEnabled_ = false;
    EXPECT_EQ(nodeGroupParam->IsGroupByUIEnabled(), false);
}
} // namespace OHOS::Rosen