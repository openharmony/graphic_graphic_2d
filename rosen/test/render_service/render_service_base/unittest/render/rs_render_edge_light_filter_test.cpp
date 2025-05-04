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
#include "render/rs_render_edge_light_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderEdgeLightFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderEdgeLightFilterTest::SetUpTestCase() {}
void RSRenderEdgeLightFilterTest::TearDownTestCase() {}
void RSRenderEdgeLightFilterTest::SetUp() {}
void RSRenderEdgeLightFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderEdgeLightFilterPara= std::make_shared<RSRenderEdgeLightFilterPara>(0);
    std::string out;
    rsRenderEdgeLightFilterPara->GetDescription(out);
    EXPECT_NE(out.find("RSRenderEdgeLightFilterPara"), string::npos);
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderEdgeLightFilterPara= std::make_shared<RSRenderEdgeLightFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderEdgeLightFilterPara->WriteToParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderEdgeLightFilterPara= std::make_shared<RSRenderEdgeLightFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderEdgeLightFilterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderEdgeLightFilterTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderEdgeLightFilterPara= std::make_shared<RSRenderEdgeLightFilterPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderEdgeLightFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());
}

} // namespace OHOS::Rosen