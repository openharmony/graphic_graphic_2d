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

#include "gtest/gtest.h"
#include "parcel.h" // Assumed definition of Parcel
#include "ge_visual_effect_container.h"
#include "effect/rs_render_shader_base.h"
using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {

class RSNGRenderShaderBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
void RSNGRenderShaderBaseTest::SetUpTestCase() {}
void RSNGRenderShaderBaseTest::TearDownTestCase() {}
void RSNGRenderShaderBaseTest::SetUp() {}
void RSNGRenderShaderBaseTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: Verify that the create method returns a non-empty instance for the registered shader type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Create001, TestSize.Level1)
{
    // Test three known filter types
    auto contour = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_NE(contour, nullptr);

    auto ripple = RSNGRenderShaderBase::Create(RSNGEffectType::WAVY_RIPPLE_LIGHT);
    EXPECT_NE(ripple, nullptr);

    auto aurora = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(aurora, nullptr);
}

/**
 * @tc.name: Create002
 * @tc.desc: Test that the create method returns a nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Create002, TestSize.Level1)
{
    // Use an invalid enum value
    RSNGEffectType invalidType = static_cast<RSNGEffectType>(-1);
    auto shader = RSNGRenderShaderBase::Create(invalidType);
    EXPECT_EQ(shader, nullptr);
}

/**
 * @tc.name: GetShaderTypeString001
 * @tc.desc: Test GetEffectTypeString by valid type and unknown type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, GetShaderTypeString001, TestSize.Level1)
{
    // Valid types
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::INVALID), "Invalid");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT),
        "ContourDiagonalFlowLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::WAVY_RIPPLE_LIGHT), "WavyRippleLight");
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::AURORA_NOISE), "AuroraNoise");
    // Unknown type
    RSNGEffectType unknownType = static_cast<RSNGEffectType>(999); // 999 is random value
    EXPECT_EQ(RSNGRenderEffectHelper::GetEffectTypeString(unknownType), "UNKNOWN");
}

/**
 * @tc.name: Dump001
 * @tc.desc: Test Dump with one effect type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Dump001, TestSize.Level1)
{
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    ASSERT_NE(shader, nullptr);

    std::string out;
    shader->Dump(out);

    // Should contain the type name followed by ": "
    std::string typeName = RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    EXPECT_NE(out.find(typeName + ": "), std::string::npos);
}

/**
 * @tc.name: Dump002
 * @tc.desc: Test Dump with two effect type
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Dump002, TestSize.Level1)
{
    auto head = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    auto next = RSNGRenderShaderBase::Create(RSNGEffectType::WAVY_RIPPLE_LIGHT);
    ASSERT_NE(head, nullptr);
    ASSERT_NE(next, nullptr);

    head->nextEffect_ = next;

    std::string out;
    head->Dump(out);

    // Chained output should contain ", " separator
    EXPECT_NE(out.find(", "), std::string::npos);
    // And include the next effect's type name
    std::string nextName = RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::WAVY_RIPPLE_LIGHT);
    EXPECT_NE(out.find(nextName), std::string::npos);
}

/**
 * @tc.name: AppendToGEContainerTest
 * @tc.desc: Test AppendToGEContainerTest
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, AppendToGEContainerTest, TestSize.Level1)
{
    std::shared_ptr<Drawing::GEVisualEffectContainer> nullContainer = nullptr;
    std::string name = "ge1";
    auto shader = std::make_shared<Drawing::GEVisualEffect>(name);
    RSNGRenderEffectHelper::AppendToGEContainer(nullContainer, shader);
    SUCCEED();

    auto container = std::make_shared<Drawing::GEVisualEffectContainer>();
    RSNGRenderEffectHelper::AppendToGEContainer(container, shader);

    EXPECT_EQ(container->filterVec_.size(), 1u);
    EXPECT_EQ(container->filterVec_[0], shader);
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Test Unmarshalling with valid parcel
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Unmarshalling001, TestSize.Level1)
{
    auto original = RSNGRenderShaderBase::Create(RSNGEffectType::WAVY_RIPPLE_LIGHT);
    EXPECT_NE(original, nullptr);
    Parcel parcel;
    EXPECT_TRUE(original->Marshalling(parcel));
    std::shared_ptr<RSNGRenderShaderBase> val;
    bool result = RSNGRenderShaderBase::Unmarshalling(parcel, val);
    EXPECT_TRUE(result);
    EXPECT_NE(val, nullptr);
    EXPECT_EQ(val->GetType(), RSNGEffectType::WAVY_RIPPLE_LIGHT);
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: Test Unmarshalling with two valid parcel
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, Unmarshalling002, TestSize.Level1)
{
    auto head = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    auto next = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(head, nullptr);
    EXPECT_NE(next, nullptr);
    head->nextEffect_ = next;
    Parcel parcel;
    EXPECT_TRUE(head->Marshalling(parcel));
    std::shared_ptr<RSNGRenderShaderBase> val;
    bool result = RSNGRenderShaderBase::Unmarshalling(parcel, val);
    EXPECT_TRUE(result);
    // Verify head and next are correctly linked
    EXPECT_NE(val, nullptr);
    EXPECT_EQ(val->GetType(), RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    auto chained = val->nextEffect_;
    EXPECT_NE(chained, nullptr);
    EXPECT_EQ(chained->GetType(), RSNGEffectType::AURORA_NOISE);
}

/**
 * @tc.name: CheckEnableEDR001
 * @tc.desc: 验证 Unmarshalling 方法在链式Parcel（两节点）时返回true并正确链接两个实例
 * @tc.type: FUNC
 */
HWTEST_F(RSNGRenderShaderBaseTest, CheckEnableEDR001, TestSize.Level1)
{
    auto head = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    auto next = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    EXPECT_NE(head, nullptr);
    EXPECT_NE(next, nullptr);
    head->nextEffect_ = next;
    Parcel parcel;
    EXPECT_TRUE(head->Marshalling(parcel));
    std::shared_ptr<RSNGRenderShaderBase> val;
    bool result = RSNGRenderShaderBase::Unmarshalling(parcel, val);
    EXPECT_TRUE(result);
    // Verify head and next are correctly linked
    EXPECT_NE(val, nullptr);
    EXPECT_EQ(val->GetType(), RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    auto chained = val->nextEffect_;
    EXPECT_NE(chained, nullptr);
    EXPECT_EQ(chained->GetType(), RSNGEffectType::AURORA_NOISE);
}

} // namespace OHOS::Rosen