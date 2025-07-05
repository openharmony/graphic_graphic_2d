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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"

#include "modifier_ng/geometry/rs_transform_render_modifier.h"
#include "property/rs_properties.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSTransformRenderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSTransformRenderModifierTest
 * @tc.desc: ResetProperties & GetType
 * @tc.type: FUNC
 */
HWTEST_F(RSTransformRenderModifierNGTypeTest, RSTransformRenderModifierTest, TestSize.Level1)
{
    RSTransformRenderModifier modifier;
    RSProperties properties;
    modifier.ResetProperties(properties);

    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::TRANSFORM);

    EXPECT_EQ(properties.GetSandBox(), std::nullopt);
    EXPECT_EQ(properties.GetPositionZ(), 0.f);
    EXPECT_EQ(properties.GetPositionZApplicableCamera3D(), false);
    EXPECT_EQ(properties.GetPivot(), Vector2f(0.5f, 0.5f));
    EXPECT_EQ(properties.GetPivotZ(), 0.f);
    EXPECT_EQ(properties.GetQuaternion(), Quaternion());
    EXPECT_EQ(properties.GetRotation(), 0.f);
    EXPECT_EQ(properties.GetRotationX(), 0.f);
    EXPECT_EQ(properties.GetRotationY(), 0.f);
    EXPECT_EQ(properties.GetCameraDistance(), 0.f);
    EXPECT_EQ(properties.GetTranslate(), Vector2f());
    EXPECT_EQ(properties.GetTranslateZ(), 0.f);
    EXPECT_EQ(properties.GetScale(), Vector2f(1.0f, 1.0f));
    EXPECT_EQ(properties.GetScaleZ(), 1.f);
    EXPECT_EQ(properties.GetSkew(), Vector3f());
    EXPECT_EQ(properties.GetPersp(), Vector4f(0.f, 0.f, 0.f, 1.f));
}

/**
 * @tc.name: OnSetDirtyTest
 * @tc.desc: Test the function OnSetDirty
 * @tc.type: FUNC
 */
HWTEST_F(RSTransformRenderModifierNGTypeTest, OnSetDirtyTest, TestSize.Level1)
{
    uint64_t id = 0;
    float value = 0.0f;
    auto property = std::make_shared<RSRenderProperty<float>>(value, id);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::TRANSFORM, property, id, ModifierNG::RSPropertyType::POSITION_Z);
    modifier->OnSetDirty();
    EXPECT_TRUE(modifier->HasProperty(ModifierNG::RSPropertyType::POSITION_Z));

    NodeId nodeId = 1;
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSCanvasRenderNode>(nodeId);
    std::weak_ptr<RSRenderNode> weakPtr = nodePtr;
    modifier->target_ = weakPtr;
    modifier->OnSetDirty();
    EXPECT_NE(modifier->target_.lock(), nullptr);
}
} // namespace OHOS::Rosen