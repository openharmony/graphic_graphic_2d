
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
#include "ge_visual_effect_container.h"
#include "parcel.h" // Assumed definition of Parcel

#include "effect/rs_render_shape_base.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderShapeBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderShapeBaseTest::SetUpTestCase() {}
void RSRenderShapeBaseTest::TearDownTestCase() {}
void RSRenderShapeBaseTest::SetUp() {}
void RSRenderShapeBaseTest::TearDown() {}

/**
 * @tc.name: RSNGRenderShapeBaseUnmarshallingTest010
 * @tc.desc: Test marshalling and unmarshalling with SDF_UNION_OP_SHAPE, SDF_SMOOTH_UNION_OP_SHAPE
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderShapeBaseTest, RSNGRenderShapeBaseUnmarshallingTest010, TestSize.Level1)
{
    auto head = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    auto next = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE);

    EXPECT_NE(head, nullptr);
    EXPECT_NE(next, nullptr);
    head->nextEffect_ = next;

    Parcel parcel;
    head->Marshalling(parcel);

    RSNGEffectTypeUnderlying sdfUnionOpShape = static_cast<RSNGEffectTypeUnderlying>(RSNGEffectType::SDF_UNION_OP_SHAPE);
    parcel.WriteUint32(sdfUnionOpShape);

    std::shared_ptr<RSNGRenderShapeBase> result;
    EXPECT_TRUE(RSNGRenderShapeBase::Unmarshalling(parcel, result));
}

/**
 * @tc.name: RSNGRenderShapeBaseUnmarshallingTest011
 * @tc.desc: Test marshalling and unmarshalling with SDF_UNION_OP_SHAPE, SDF_RRECT_SHAPE
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderShapeBaseTest, RSNGRenderShapeBaseUnmarshallingTest011, TestSize.Level1)
{
    auto head = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    auto next = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);

    EXPECT_NE(head, nullptr);
    EXPECT_NE(next, nullptr);
    head->nextEffect_ = next;

    Parcel parcel;
    head->Marshalling(parcel);

    RSNGEffectTypeUnderlying sdfUnionOpShape = static_cast<RSNGEffectTypeUnderlying>(RSNGEffectType::SDF_UNION_OP_SHAPE);
    parcel.WriteUint32(sdfUnionOpShape);

    std::shared_ptr<RSNGRenderShapeBase> result;
    EXPECT_TRUE(RSNGRenderShapeBase::Unmarshalling(parcel, result));
}

} // namespace OHOS::Rosen