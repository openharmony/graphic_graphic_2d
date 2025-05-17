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
#include "render/rs_render_ripple_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderRippleMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderRippleMaskTest::SetUpTestCase() {}
void RSRenderRippleMaskTest::TearDownTestCase() {}
void RSRenderRippleMaskTest::SetUp() {}
void RSRenderRippleMaskTest::TearDown() {}

/**
 * @tc.name: CreateRenderPropert001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, CreateRenderPropert001, TestSize.Level1)
{
    auto renderPropert = RSRenderRippleMaskPara::CreateRenderPropert(RSUIFilterType::RIPPLE_MASK_RADIUS);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = RSRenderRippleMaskPara::CreateRenderPropert(RSUIFilterType::RIPPLE_MASK_WIDTH);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = RSRenderRippleMaskPara::CreateRenderPropert(RSUIFilterType::RIPPLE_MASK_CENTER);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = RSRenderRippleMaskPara::CreateRenderPropert(RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = RSRenderRippleMaskPara::CreateRenderPropert(RSUIFilterType::BLUR);
    EXPECT_EQ(renderPropert, nullptr);
}

/**
 * @tc.name: GetAnimatRenderProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, GetAnimatRenderProperty001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    auto uiFilterType = RSUIFilterType::RIPPLE_MASK;
    auto animatRenderProperty = rsRenderRippleMaskPara->GetAnimatRenderProperty<Vector2f>(uiFilterType);
    EXPECT_EQ(animatRenderProperty, nullptr);
}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    std::string out;
    rsRenderRippleMaskPara->GetDescription(out);
    EXPECT_FALSE(out.empty());
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    Parcel parcel;
    auto ret = rsRenderRippleMaskPara->WriteToParcel(parcel);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    Parcel parcel;
    auto ret = rsRenderRippleMaskPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderRippleMaskTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderRippleMaskPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());
}
} // namespace OHOS::Rosen