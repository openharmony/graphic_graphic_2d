/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "ui/rs_display_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplayNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplayNodeTest::SetUpTestCase() {}
void RSDisplayNodeTest::TearDownTestCase() {}
void RSDisplayNodeTest::SetUp() {}
void RSDisplayNodeTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSDisplayNodeTest, Create001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create RSDisplayNode
    */
    RSDisplayNodeConfig c;
    RSDisplayNode::SharedPtr rootNode = RSDisplayNode::Create(c);
    ASSERT_TRUE(rootNode != nullptr);
}
} // namespace OHOS::Rosen