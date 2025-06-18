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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/appearance/rs_foreground_filter_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSForegroundFilterModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSBorderModifierTest
 * @tc.desc: Test Set/Get functions of RSBorderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSForegroundFilterModifierNGTypeTest, RSForegroundFilterModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSForegroundFilterModifier> modifier =
        std::make_shared<ModifierNG::RSForegroundFilterModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::FOREGROUND_FILTER);

    modifier->SetSpherize(2.5f);
    EXPECT_FLOAT_EQ(modifier->GetSpherize(), 2.5f);


    modifier->SetForegroundEffectRadius(3.5f);
    EXPECT_FLOAT_EQ(modifier->GetForegroundEffectRadius(), 3.5f);

    modifier->SetMotionBlurParam(nullptr);
    EXPECT_EQ(modifier->GetMotionBlurParam(), nullptr);
    float radius = 1.0f;
    Vector2f scaleAnchor = {2.0f, 3.0f};
    std::shared_ptr<MotionBlurParam> params = std::make_shared<MotionBlurParam>(radius, scaleAnchor);
    modifier->SetMotionBlurParam(params);
    EXPECT_EQ(modifier->GetMotionBlurParam(), params);

    RSFlyOutPara flyParams {};
    modifier->SetFlyOutParams(flyParams);
    EXPECT_EQ(modifier->GetFlyOutParams(), flyParams);

    modifier->SetFlyOutDegree(4.2f);
    EXPECT_EQ(modifier->GetFlyOutDegree(), 4.2f);

    modifier->SetDistortionK(0.8f);
    EXPECT_EQ(modifier->GetDistortionK(), std::optional<float>(0.8f));

    modifier->SetAttractionFraction(0.33f);
    EXPECT_EQ(modifier->GetAttractionFraction(), 0.33f);

    Vector2f dst { 7.7f, 8.8f };
    modifier->SetAttractionDstPoint(dst);
    EXPECT_EQ(modifier->GetAttractionDstPoint(), dst);

    modifier->MarkNodeDirty();
}

HWTEST_F(RSForegroundFilterModifierNGTypeTest, SetUIFilterTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSForegroundFilterModifier> modifier =
        std::make_shared<ModifierNG::RSForegroundFilterModifier>();
    modifier->SetUIFilter(nullptr);

    auto rsUIFilter = std::make_shared<RSUIFilter>();
    EXPECT_NE(rsUIFilter, nullptr);

    std::shared_ptr<ModifierNG::RSForegroundFilterModifier> modifier01 =
    std::make_shared<ModifierNG::RSForegroundFilterModifier>();

    rsUIFilter->Insert(nullptr);
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);
    rsUIFilter->Insert(rsUIFilterParaBase);

    modifier01->SetUIFilter(rsUIFilter);
    auto node = RSCanvasNode::Create();
    modifier01->node_ = node->weak_from_this();
    modifier01->SetUIFilter(rsUIFilter);
    modifier01->SetUIFilter(rsUIFilter);
    auto rsUIFilter01 = std::make_shared<RSUIFilter>();
    modifier01->SetUIFilter(rsUIFilter01);
}

HWTEST_F(RSForegroundFilterModifierNGTypeTest, AttachUIFilterPropertyTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSForegroundFilterModifier> modifier =
        std::make_shared<ModifierNG::RSForegroundFilterModifier>();
    modifier->AttachUIFilterProperty(nullptr);

    auto node = RSCanvasNode::Create();
    modifier->node_ = node->weak_from_this();

    auto rsUIFilter = std::make_shared<RSUIFilter>();

    modifier->AttachUIFilterProperty(rsUIFilter);

    EXPECT_NE(rsUIFilter, nullptr);
    rsUIFilter->Insert(nullptr);
    float radius = 1.0f;
    auto filterBlurPara = std::make_shared<FilterBlurPara>();
    filterBlurPara->SetRadius(radius);
    auto rsUIBlurFilterPara = std::make_shared<RSUIBlurFilterPara>();
    rsUIBlurFilterPara->SetBlurPara(filterBlurPara);
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUIBlurFilterPara);
    rsUIFilter->Insert(rsUIFilterParaBase);
    modifier->AttachUIFilterProperty(rsUIFilter);
}
} // namespace OHOS::Rosen