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

#include "render/rs_render_bezier_warp_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderBezierWarpFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderBezierWarpFilterTest::SetUpTestCase() {}
void RSRenderBezierWarpFilterTest::TearDownTestCase() {}
void RSRenderBezierWarpFilterTest::SetUp() {}
void RSRenderBezierWarpFilterTest::TearDown() {}

/**
 * @tc.name: GetDescriptionTest001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, GetDescriptionTest001, TestSize.Level1)
{
    std::string out;
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    rsRenderBezierWarpFilterPara->GetDescription(out);
    EXPECT_EQ(out, "RSRenderBezierWarpFilterPara");
}

/**
 * @tc.name: WriteToParcelTest001
 * @tc.desc: Verify function WriteToParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, WriteToParcelTest001, TestSize.Level1)
{
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderBezierWarpFilterPara->WriteToParcel(parcel);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ReadFromParcelTest001
 * @tc.desc: Verify function ReadFromParcel
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, ReadFromParcelTest001, TestSize.Level1)
{
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderBezierWarpFilterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CreateRenderPropertyTest001
 * @tc.desc: Verify function CreateRenderProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, CreateRenderPropertyTest001, TestSize.Level1)
{
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    int point0  = static_cast<int>(RSUIFilterType::BEZIER_CONTROL_POINT0);
    int point11 = static_cast<int>(RSUIFilterType::BEZIER_CONTROL_POINT11);

    auto rsRenderPropertyBase1 = rsRenderBezierWarpFilterPara->CreateRenderProperty(RSUIFilterType::NONE);
    EXPECT_EQ(rsRenderPropertyBase1, nullptr);

    for (int i = point0; i <= point11; i++) {
        auto tempType = RSUIFilterType(i);
        auto rsRenderPropertyBase = rsRenderBezierWarpFilterPara->CreateRenderProperty(tempType);
        EXPECT_NE(rsRenderPropertyBase, nullptr);
    }
}

/**
 * @tc.name: GetLeafRenderPropertiesTest001
 * @tc.desc: Verify function CreateRenderProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderBezierWarpFilterTest, GetLeafRenderPropertiesTest001, TestSize.Level1)
{
    auto rsRenderBezierWarpFilterPara = std::make_shared<RSRenderBezierWarpFilterPara>(0);
    auto out = rsRenderBezierWarpFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(out.empty());
}
} // namespace OHOS::Rosen