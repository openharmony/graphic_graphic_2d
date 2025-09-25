
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

#include "effet/rs_render_mask_base.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderMaskrBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderMaskBaseTest::SetUpTestCase() {}
void RSRenderMaskBaseTest::TearDownTestCase() {}
void RSRenderMaskBaseTest::SetUp() {}
void RSRenderMaskBaseTest::TearDown() {}

/**
 * @tc.name: RSNGRenderMaskBaseUnmarshallingTest010
 * @tc.desc: Test marshalling and unmarshalling with SDF_UNION_OP_MASK, SDF_SMOOTH_UNION_OP_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMaskBaseTest, RSNGRenderMaskBaseUnmarshallingTest010, TestSize.Level1)
{
    auto head = RSNGRenderMaskBase::Create(RSNGEffectType::SDF_UNION_OP_MASK);
    auto next = RSNGRenderMaskBase::Create(RSNGEffectType::SDF_SMOOTH_UNION_OP_MASK);

    EXPECT_NE(head, nullptr);
    EXPECT_NE(next, nullptr);
    head->nextEffect_ = next;

    Parcel parcel;
    head->Marshalling(parcel);

    RSNGEffectTypeUnderlying sdfUnionOpMask = static_cast<RSNGEffectTypeUnderlying>(RSNGEffectType::SDF_UNION_OP_MASK);
    parcel.WriteUint32(sdfUnionOpMask);

    std::shared_ptr<RSNGRenderMaskBase> result;
    EXPECT_TRUE(RSNGRenderMaskBase::Unmarshalling(parcel, result));
}

/**
 * @tc.name: RSNGRenderMaskBaseUnmarshallingTest011
 * @tc.desc: Test marshalling and unmarshalling with SDF_UNION_OP_MASK, SDF_RRECT_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderMaskBaseTest, RSNGRenderMaskBaseUnmarshallingTest010, TestSize.Level1)
{
    auto head = RSNGRenderMaskBase::Create(RSNGEffectType::SDF_UNION_OP_MASK);
    auto next = RSNGRenderMaskBase::Create(RSNGEffectType::SDF_RRECT_MASK);

    EXPECT_NE(head, nullptr);
    EXPECT_NE(next, nullptr);
    head->nextEffect_ = next;

    Parcel parcel;
    head->Marshalling(parcel);

    RSNGEffectTypeUnderlying sdfUnionOpMask = static_cast<RSNGEffectTypeUnderlying>(RSNGEffectType::SDF_UNION_OP_MASK);
    parcel.WriteUint32(sdfUnionOpMask);

    std::shared_ptr<RSNGRenderMaskBase> result;
    EXPECT_TRUE(RSNGRenderMaskBase::Unmarshalling(parcel, result));
}

} // namespace OHOS::Rosen