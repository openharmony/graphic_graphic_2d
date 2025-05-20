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
#include "render/rs_render_blur_filter.h"
#include "render/rs_render_displacement_distort_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderFilterTest::SetUpTestCase() {}
void RSRenderFilterTest::TearDownTestCase() {}
void RSRenderFilterTest::SetUp() {}
void RSRenderFilterTest::TearDown() {}

/**
 * @tc.name: Insert001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterTest, Insert001, TestSize.Level1)
{
    auto rsRenderFilter = std::make_shared<RSRenderFilter>();
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    rsRenderFilter->Insert(RSUIFilterType::BLUR, rsRenderFilterParaBase);
    EXPECT_NE(rsRenderFilter->properties_.size(), 0);
}

/**
 * @tc.name: GetRenderFilterPara001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterTest, GetRenderFilterPara001, TestSize.Level1)
{
    auto rsRenderFilter = std::make_shared<RSRenderFilter>();
    auto rsRenderFilterParaBase = std::make_shared<RSRenderFilterParaBase>(RSUIFilterType::BLUR);
    rsRenderFilter->Insert(RSUIFilterType::BLUR, rsRenderFilterParaBase);
    auto renderFilterPara = rsRenderFilter->GetRenderFilterPara(RSUIFilterType::BLUR);
    EXPECT_NE(renderFilterPara, nullptr);
}

/**
 * @tc.name: CreateRenderFilterPara001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterTest, CreateRenderFilterPara001, TestSize.Level1)
{
    auto blurPara = RSRenderFilter::CreateRenderFilterPara(RSUIFilterType::BLUR);
    EXPECT_NE(blurPara, nullptr);

    auto displacementDistortPara = RSRenderFilter::CreateRenderFilterPara(RSUIFilterType::DISPLACEMENT_DISTORT);
    EXPECT_NE(displacementDistortPara, nullptr);

    auto ripplePara = RSRenderFilter::CreateRenderFilterPara(RSUIFilterType::RIPPLE_MASK);
    EXPECT_EQ(ripplePara, nullptr);

    auto soundWavePara = RSRenderFilter::CreateRenderFilterPara(RSUIFilterType::SOUND_WAVE);
    EXPECT_NE(soundWavePara, nullptr);

    auto edgeLightPara = RSRenderFilter::CreateRenderFilterPara(RSUIFilterType::EDGE_LIGHT);
    EXPECT_NE(edgeLightPara, nullptr);
}

/**
 * @tc.name: MarshallingPropertyType001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterTest, MarshallingPropertyType001, TestSize.Level1)
{
    Parcel parcel;
    std::vector<RSUIFilterType> rsUIFilterVec;
    auto ret = RSRenderFilter::MarshallingPropertyType(parcel, rsUIFilterVec);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: UnmarshallingPropertyType001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterTest, UnmarshallingPropertyType001, TestSize.Level1)
{
    Parcel parcel;
    std::vector<RSUIFilterType> rsUIFilterVec;
    int maxSize = 100;
    auto ret = RSRenderFilter::UnmarshallingPropertyType(parcel, rsUIFilterVec, maxSize);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Dump001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterTest, Dump001, TestSize.Level1)
{
    auto rsRenderFilter = std::make_shared<RSRenderFilter>();
    std::string out;
    rsRenderFilter->Dump(out);
    EXPECT_EQ(out.length(), 0);
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderFilter = std::make_shared<RSRenderFilter>();
    Parcel parcel;
    auto ret = rsRenderFilter->WriteToParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderFilter = std::make_shared<RSRenderFilter>();
    Parcel parcel;
    auto ret = rsRenderFilter->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetUIFilterTypes001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderFilterTest, GetUIFilterTypes001, TestSize.Level1)
{
    auto rsRenderFilter = std::make_shared<RSRenderFilter>();
    auto uiFilterTypes = rsRenderFilter->GetUIFilterTypes();
    EXPECT_TRUE(uiFilterTypes.empty());
}

} // namespace OHOS::Rosen