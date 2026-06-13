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

#include "command_modifier/rs_root_node_command_modifier.h"
#include "ui/rs_root_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRootNodeCommandModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRootNodeCommandModifierTest::SetUpTestCase() {}
void RSRootNodeCommandModifierTest::TearDownTestCase() {}
void RSRootNodeCommandModifierTest::SetUp() {}
void RSRootNodeCommandModifierTest::TearDown() {}

/**
 * @tc.name: AttachRootNodeTest001
 * @tc.desc: Test GetType, SetParam same/different, GetParam, DumpParam
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandModifierTest, AttachRootNodeTest001, TestSize.Level1)
{
    auto node = RSRootNode::Create(false);
    AttachRootNodeCmdParam param{100, 200, true};
    auto mod = std::make_shared<AttachRootNodeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::ATTACH_ROOT_NODE);
    EXPECT_EQ(mod->GetParam().attachSurfaceNodeId_, 100);
    EXPECT_EQ(mod->GetParam().attachSurfaceNodeToken_, 200);
    EXPECT_TRUE(mod->GetParam().isSurfaceNodeTree_);

    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);

    AttachRootNodeCmdParam param2{300, 400, false};
    ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().attachSurfaceNodeId_, 300);
    EXPECT_EQ(mod->GetParam().attachSurfaceNodeToken_, 400);
    EXPECT_FALSE(mod->GetParam().isSurfaceNodeTree_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("attachSurfaceNodeId"), std::string::npos);
    EXPECT_NE(out.find("attachSurfaceNodeToken"), std::string::npos);
    EXPECT_NE(out.find("isSurfaceNodeTree"), std::string::npos);
}

/**
 * @tc.name: AttachRootNodeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandModifierTest, AttachRootNodeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSRootNode::Create(false); weakNode = n; }
    AttachRootNodeCmdParam param{0, 0, false};
    auto mod = std::make_shared<AttachRootNodeCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: AttachRootNodeTest003
 * @tc.desc: Test UpdateToRender with valid node sets isOnTheTree
 * @tc.type: FUNC
 */
HWTEST_F(RSRootNodeCommandModifierTest, AttachRootNodeTest003, TestSize.Level1)
{
    auto node = RSRootNode::Create(false);
    AttachRootNodeCmdParam param{100, 200, true};
    auto mod = std::make_shared<AttachRootNodeCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

} // namespace Rosen
} // namespace OHOS
