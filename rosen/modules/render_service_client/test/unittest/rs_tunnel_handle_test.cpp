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

#include <gtest/gtest.h>
#include <hilog/log.h>
#include <memory>
#include <unistd.h>

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
static const int nodeId1 = 1;

class RSTunnelHandleTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RSTunnelHandleTest" };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSContext> context = nullptr;
    static inline std::shared_ptr<RSBaseRenderNode> node1 = nullptr;
};

void RSTunnelHandleTest::SetUpTestCase()
{
    context = std::make_shared<RSContext>();
    node1 = std::make_shared<RSBaseRenderNode>(nodeId1, context->weak_from_this());
}

void RSTunnelHandleTest::TearDownTestCase()
{
    node1 = nullptr;
    context = nullptr;
}

void RSTunnelHandleTest::SetUp() {}
void RSTunnelHandleTest::TearDown() {}

/**
 * @tc.name: GetDefaultTunnelHandleChange
 * @tc.desc: Verify the main function of RSTunnelHandle
 * @tc.type: FUNC
 * @tc.require: issueI5HSJS
 */
HWTEST_F(RSTunnelHandleTest, GetDefaultTunnelHandleChange, Function | SmallTest | Level2)
{
    // check TunnelHandleChange
    EXPECT_EQ(node1->GetTunnelHandleChange(), false);
}

/**
 * @tc.name: SetTunnelHandleChange
 * @tc.desc: Verify the main function of RSTunnelHandle
 * @tc.type: FUNC
 * @tc.require: issueI5HSJS
 */
HWTEST_F(RSTunnelHandleTest, SetTunnelHandleChange, Function | SmallTest | Level2)
{
    // call SetTunnelHandleChange
    node1->SetTunnelHandleChange(true);
    // check TunnelHandleChange
    EXPECT_EQ(node1->GetTunnelHandleChange(), true);
}
} // namespace OHOS::Rosen
