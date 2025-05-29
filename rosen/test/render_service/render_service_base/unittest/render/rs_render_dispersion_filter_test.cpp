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
#include "render/rs_render_dispersion_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderDispersionFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderDispersionFilterTest::SetUpTestCase() {}
void RSRenderDispersionFilterTest::TearDownTestCase() {}
void RSRenderDispersionFilterTest::SetUp() {}
void RSRenderDispersionFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);
    std::string out;
    rsRenderDispersionFilterPara->GetDescription(out);
    EXPECT_FALSE(out.empty());
}

/**
 * @tc.name: CreateRenderProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, CreateRenderProperty001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);

    auto renderPropert = rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::PIXEL_MAP_MASK);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::RADIAL_GRADIENT_MASK);
    EXPECT_EQ(renderPropert, nullptr);

    renderPropert = rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::DISPERSION_RED_OFFSET);
    EXPECT_NE(renderPropert, nullptr);
    renderPropert = rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::DISPERSION_GREEN_OFFSET);
    EXPECT_NE(renderPropert, nullptr);
    renderPropert = rsRenderDispersionFilterPara->CreateRenderProperty(RSUIFilterType::DISPERSION_BLUE_OFFSET);
    EXPECT_NE(renderPropert, nullptr);
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderDispersionFilterPara->WriteToParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderDispersionFilterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderDispersionFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());
}

/**
 * @tc.name: GetRenderMask001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDispersionFilterTest, GetRenderMask001, TestSize.Level1)
{
    auto rsRenderDispersionFilterPara = std::make_shared<RSRenderDispersionFilterPara>(0);
    auto renderMask = rsRenderDispersionFilterPara->GetRenderMask();
    EXPECT_NE(renderMask, nullptr);
}

} // namespace OHOS::Rosen