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

#include <memory>

#include "gtest/gtest.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "property/rs_properties.h"
#include "modifier_ng/appearance/rs_depth_space_render_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSDepthSpaceRenderModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override {}
    void TearDown() override {}
};

void RSDepthSpaceRenderModifierTest::SetUpTestCase() {}
void RSDepthSpaceRenderModifierTest::TearDownTestCase() {}

/**
 * @tc.name: GetType001
 * @tc.desc: Test GetType of RSDepthSpaceRenderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceRenderModifierTest, GetType001, TestSize.Level1)
{
    ModifierNG::RSDepthSpaceRenderModifier modifier;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::DEPTH_SPACE);
}

/**
 * @tc.name: ResetProperties001
 * @tc.desc: Test ResetProperties resets all depth space properties to default
 * @tc.type: FUNC
 */
HWTEST_F(RSDepthSpaceRenderModifierTest, ResetProperties001, TestSize.Level1)
{
    RSProperties properties;
    ModifierNG::RSDepthSpaceRenderModifier modifier;

    // Reset properties to default values
    modifier.ResetProperties(properties);

    // Verify depth image is reset to nullptr
    EXPECT_EQ(properties.GetDepthImage(), nullptr);
}
} // namespace OHOS::Rosen