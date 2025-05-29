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
#include "ui_effect/property/include/rs_ui_filter.h"
#include "ui_effect/filter/include/filter_blur_para.h"
#include "ui_effect/property/include/rs_ui_blur_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIFilterTest::SetUpTestCase() {}
void RSUIFilterTest::TearDownTestCase() {}
void RSUIFilterTest::SetUp() {}
void RSUIFilterTest::TearDown() {}

/**
 * @tc.name: GetRSRenderFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterTest, GetRSRenderFilter001, TestSize.Level1)
{
    auto rsUIFilter = std::make_shared<RSUIFilter>();
    auto rsRenderFilter = rsUIFilter->GetRSRenderFilter();
    EXPECT_EQ(rsRenderFilter, nullptr);
}

/**
 * @tc.name: Dump001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterTest, Dump001, TestSize.Level1)
{
    auto rsUIFilter = std::make_shared<RSUIFilter>();
    std::string out;
    rsUIFilter->Dump(out);
    EXPECT_EQ(out.length(), 0);
}

/**
 * @tc.name: CreateRenderProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterTest, CreateRenderProperty001, TestSize.Level1)
{
    auto rsUIFilter = std::make_shared<RSUIFilter>();
    PropertyId id = 1;
    auto rsRenderPropertyBase = rsUIFilter->CreateRenderProperty(id);
    EXPECT_EQ(rsRenderPropertyBase, nullptr);
}

/**
 * @tc.name: Insert001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterTest, Insert001, TestSize.Level1)
{
    auto rsUIFilter = std::make_shared<RSUIFilter>();
    rsUIFilter->Insert(nullptr);
    EXPECT_TRUE(rsUIFilter->propertyTypes_.empty());

    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);

    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);

    rsUIFilter->Insert(rsUIFilterParaBase);
    EXPECT_EQ(rsUIFilter->paras_[RSUIFilterType::BLUR], rsUIFilterParaBase);
    rsUIFilter->Insert(rsUIFilterParaBase);
    EXPECT_EQ(rsUIFilter->propertyTypes_.size(), 1);
}

/**
 * @tc.name: IsStructureSame001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterTest, IsStructureSame001, TestSize.Level1)
{
    auto rsUIFilter1 = std::make_shared<RSUIFilter>();
    auto rsUIFilter2 = std::make_shared<RSUIFilter>();
    EXPECT_TRUE(rsUIFilter1->IsStructureSame(rsUIFilter2));

    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    float radius = 1.0f;
    filterBlurPara->SetRadius(radius);
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);
    rsUIFilter1->Insert(rsUIBlurFilterPara);
    EXPECT_FALSE(rsUIFilter1->IsStructureSame(rsUIFilter2));

    rsUIFilter2->Insert(rsUIBlurFilterPara);
    EXPECT_TRUE(rsUIFilter1->IsStructureSame(rsUIFilter2));

    rsUIFilter2->paras_[RSUIFilterType::BLUR] = nullptr;
    EXPECT_FALSE(rsUIFilter1->IsStructureSame(rsUIFilter2));

    rsUIFilter2->propertyTypes_.clear();
    rsUIFilter2->propertyTypes_.push_back(RSUIFilterType::DISPLACEMENT_DISTORT);
    EXPECT_FALSE(rsUIFilter1->IsStructureSame(rsUIFilter2));
}

/**
 * @tc.name: SetValue001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterTest, SetValue001, TestSize.Level1)
{
    float radius1 = 1.0f;
    auto filterBlurPara1 = std::make_shared<FilterBlurPara>();
    filterBlurPara1->SetRadius(radius1);
    auto rsUIBlurFilterPara1 = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara1->SetBlurPara(filterBlurPara1);
    auto rsUIFilterParaBase1 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara1);

    auto rsUIFilter1 = std::make_shared<RSUIFilter>();
    rsUIFilter1->Insert(rsUIFilterParaBase1);

    float radius2 = 0.5f;
    auto filterBlurPara2 = std::make_shared<FilterBlurPara>();
    filterBlurPara2->SetRadius(radius2);
    auto rsUIBlurFilterPara2 = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara2->SetBlurPara(filterBlurPara2);
    auto rsUIFilterParaBase2 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara2);

    auto rsUIFilter2 = std::make_shared<RSUIFilter>();
    rsUIFilter2->Insert(rsUIFilterParaBase2);

    rsUIFilter1->SetValue(rsUIFilter2);
    auto rsUIFilterParaBase3 = rsUIFilter1->GetUIFilterPara(RSUIFilterType::BLUR);
    EXPECT_TRUE(rsUIBlurFilterPara2->Equals(rsUIFilterParaBase3));
}

/**
 * @tc.name: GetAllTypes001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterTest, GetAllTypes001, TestSize.Level1)
{
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);

    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);

    auto rsUIFilter = std::make_shared<RSUIFilter>();
    rsUIFilter->Insert(rsUIFilterParaBase);

    auto rsUIFilterTypeVector = rsUIFilter->GetAllTypes();
    EXPECT_NE(rsUIFilterTypeVector.size(), 0);
}

/**
 * @tc.name: GetUIFilterPara001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterTest, GetUIFilterPara001, TestSize.Level1)
{
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);

    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);

    auto rsUIFilter = std::make_shared<RSUIFilter>();
    rsUIFilter->Insert(rsUIFilterParaBase);

    auto rsUIFilterParaBase1 = rsUIFilter->GetUIFilterPara(RSUIFilterType::BLUR);
    EXPECT_NE(rsUIFilterParaBase1, nullptr);
}

/**
 * @tc.name: GetUIFilterTypes001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterTest, GetUIFilterTypes001, TestSize.Level1)
{
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);

    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);

    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);

    auto rsUIFilter = std::make_shared<RSUIFilter>();
    rsUIFilter->Insert(rsUIFilterParaBase);

    auto rsUIFilterTypeVector = rsUIFilter->GetUIFilterTypes();
    EXPECT_NE(rsUIFilterTypeVector.size(), 0);
}
} // namespace OHOS::Rosen