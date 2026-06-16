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

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"

#include "command_modifier/rs_depth_node_command_modifier.h"
#include "ui/rs_depth_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSDepthNodeCommandModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDepthNodeCommandModifierTest::SetUpTestCase() {}
void RSDepthNodeCommandModifierTest::TearDownTestCase() {}
void RSDepthNodeCommandModifierTest::SetUp() {}
void RSDepthNodeCommandModifierTest::TearDown() {}

/**
 * @tc.name: DepthSpaceTypeTest001
 * @tc.desc: Test GetType, SetParam same/different, GetParam, DumpParam
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthNodeCommandModifierTest, DepthSpaceTypeTest001, TestSize.Level1)
{
    auto node = RSDepthNode::Create(false, false, nullptr);
    DepthSpaceTypeCmdParam param{DepthSpaceType::INSTANCE};
    auto mod = std::make_shared<DepthSpaceTypeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::SET_DEPTH_SPACE_TYPE);
    EXPECT_EQ(mod->GetParam().depthSpaceType_, DepthSpaceType::INSTANCE);

    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);

    DepthSpaceTypeCmdParam param2{DepthSpaceType::GLOBAL};
    ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().depthSpaceType_, DepthSpaceType::GLOBAL);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("depthSpaceType"), std::string::npos);
}

/**
 * @tc.name: DepthSpaceTypeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthNodeCommandModifierTest, DepthSpaceTypeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSDepthNode::Create(false, false, nullptr); weakNode = n; }
    DepthSpaceTypeCmdParam param{DepthSpaceType::INSTANCE};
    auto mod = std::make_shared<DepthSpaceTypeCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: DepthSpaceTypeTest003
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthNodeCommandModifierTest, DepthSpaceTypeTest003, TestSize.Level1)
{
    auto node = RSDepthNode::Create(false, false, nullptr);
    DepthSpaceTypeCmdParam param{DepthSpaceType::INSTANCE};
    auto mod = std::make_shared<DepthSpaceTypeCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

} // namespace Rosen
} // namespace OHOS
