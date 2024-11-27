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
#include "animation/rs_animation.h"
#include "core/transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr static float FLOAT_DATA_INIT = 0.5f;

class RsModifiersTest : public testing::Test {
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

void RsModifiersTest::SetUpTestCase() {}
void RsModifiersTest::TearDownTestCase() {}
void RsModifiersTest::SetUp() {}
void RsModifiersTest::TearDown() {}

/**
 * @tc.name: AddModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsModifiersTest, AddModifier01, TestSize.Level1)
{
    auto prop = std::make_shared<RSAnimatableProperty<float>>(floatData[0]);
    auto modifier = std::make_shared<RSAlphaModifier>(prop);
    auto node = RSCanvasNode::Create();

    node->AddModifier(modifier);
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
}

} // namespace OHOS::Rosen
