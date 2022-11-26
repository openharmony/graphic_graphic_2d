/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "limit_number.h"

#include "ui/rs_proxy_node.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProxyNodeTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSProxyNodeTest::SetUpTestCase() {}
void RSProxyNodeTest::TearDownTestCase() {}
void RSProxyNodeTest::SetUp() {}
void RSProxyNodeTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSProxyNodeTest, LifeCycle001, TestSize.Level1)
{
	// return shared_ptr
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = RSProxyNode::Create(nodeId);
    ASSERT_TRUE(node != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSProxyNode::SharedPtr child2 = RSProxyNode::Create(child1->GetId());
    ASSERT_FALSE(child2 != nullptr);

    node->ResetContextVariableCache();
}
}