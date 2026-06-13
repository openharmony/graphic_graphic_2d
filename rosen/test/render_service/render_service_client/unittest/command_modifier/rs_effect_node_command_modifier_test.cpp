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

#include "command_modifier/rs_effect_node_command_modifier.h"
#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSEffectNodeCommandModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSEffectNodeCommandModifierTest::SetUpTestCase() {}
void RSEffectNodeCommandModifierTest::TearDownTestCase() {}
void RSEffectNodeCommandModifierTest::SetUp() {}
void RSEffectNodeCommandModifierTest::TearDown() {}

/**
 * @tc.name: PreFreezeTest001
 * @tc.desc: Test GetType, SetParam same/different, GetParam, DumpParam
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectNodeCommandModifierTest, PreFreezeTest001, TestSize.Level1)
{
    auto node = RSEffectNode::Create();
    PreFreezeCmdParam param{true, false};
    auto mod = std::make_shared<PreFreezeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::PRE_FREEZE);
    EXPECT_TRUE(mod->GetParam().preFreeze_);
    EXPECT_FALSE(mod->GetParam().preMarkedByUI_);

    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);

    PreFreezeCmdParam param2{false, true};
    ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().preFreeze_);
    EXPECT_TRUE(mod->GetParam().preMarkedByUI_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("preFreeze"), std::string::npos);
    EXPECT_NE(out.find("preMarkedByUI"), std::string::npos);
}

/**
 * @tc.name: PreFreezeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectNodeCommandModifierTest, PreFreezeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSEffectNode::Create(); weakNode = n; }
    PreFreezeCmdParam param{true, true};
    auto mod = std::make_shared<PreFreezeCmdModifier>(weakNode, param);
    mod->UpdateToRender();
}

/**
 * @tc.name: PreFreezeTest003
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectNodeCommandModifierTest, PreFreezeTest003, TestSize.Level1)
{
    auto node = RSEffectNode::Create();
    PreFreezeCmdParam param{true, false};
    auto mod = std::make_shared<PreFreezeCmdModifier>(node, param);
    mod->UpdateToRender();
}

} // namespace Rosen
} // namespace OHOS
