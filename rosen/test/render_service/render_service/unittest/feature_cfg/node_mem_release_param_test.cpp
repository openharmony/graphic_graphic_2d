/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "node_mem_release_param.h"
 
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
 
class NodeMemReleaseParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
 
void NodeMemReleaseParamTest::SetUpTestCase() {}
void NodeMemReleaseParamTest::TearDownTestCase() {}
void NodeMemReleaseParamTest::SetUp() {}
void NodeMemReleaseParamTest::TearDown() {}
 
/**
 * @tc.name: NodeOffTreeMemReleaseEnabledTest
 * @tc.desc: Verify the SetNodeOffTreeMemReleaseEnabled IsNodeOffTreeMemReleaseEnabled function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NodeMemReleaseParamTest, NodeOffTreeMemReleaseEnabledTest, Function | SmallTest | Level1)
{
    NodeMemReleaseParam::SetNodeOffTreeMemReleaseEnabled(true);
    ASSERT_EQ(NodeMemReleaseParam::IsNodeOffTreeMemReleaseEnabled(), true);
    NodeMemReleaseParam::SetNodeOffTreeMemReleaseEnabled(false);
    ASSERT_EQ(NodeMemReleaseParam::IsNodeOffTreeMemReleaseEnabled(), false);
}
 
/**
 * @tc.name: CanvasDrawingNodeDMAMemEnabledTest
 * @tc.desc: Verify the SetCanvasDrawingNodeDMAMemEnabled IsCanvasDrawingNodeDMAMemEnabled function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NodeMemReleaseParamTest, CanvasDrawingNodeDMAMemEnabledTest, Function | SmallTest | Level1)
{
    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(true);
    ASSERT_EQ(NodeMemReleaseParam::IsCanvasDrawingNodeDMAMemEnabled(), true);
    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(false);
    ASSERT_EQ(NodeMemReleaseParam::IsCanvasDrawingNodeDMAMemEnabled(), false);
}
 
} // namespace Rosen
} // namespace OHOS