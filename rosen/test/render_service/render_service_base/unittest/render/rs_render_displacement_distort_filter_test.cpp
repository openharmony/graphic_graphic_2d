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
#include "render/rs_render_displacement_distort_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderDisplacementDistortFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderDisplacementDistortFilterTest::SetUpTestCase() {}
void RSRenderDisplacementDistortFilterTest::TearDownTestCase() {}
void RSRenderDisplacementDistortFilterTest::SetUp() {}
void RSRenderDisplacementDistortFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);
    std::string out;
    rsRenderDispDistortFilterPara->GetDescription(out);
    std::cout << out << std::endl;
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderDispDistortFilterPara->WriteToParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderDispDistortFilterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CreateRenderPropert001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, CreateRenderPropert001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);

    auto renderPropert = rsRenderDispDistortFilterPara->CreateRenderPropert(RSUIFilterType::RIPPLE_MASK);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = rsRenderDispDistortFilterPara->CreateRenderPropert(RSUIFilterType::RADIAL_GRADIENT_MASK);
    EXPECT_EQ(renderPropert, nullptr);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderDisplacementDistortFilterTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderDispDistortFilterPara = std::make_shared<RSRenderDispDistortFilterPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderDispDistortFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());
}

} // namespace OHOS::Rosen