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
#include "render/rs_displacement_distort_filter.h"
#include "common/rs_vector2.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplacementDistortFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplacementDistortFilterTest::SetUpTestCase() {}
void RSDisplacementDistortFilterTest::TearDownTestCase() {}
void RSDisplacementDistortFilterTest::SetUp() {}
void RSDisplacementDistortFilterTest::TearDown() {}

/**
 * @tc.name: GenerateGEVisualEffect001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplacementDistortFilterTest, GenerateGEVisualEffect001, TestSize.Level1)
{
    auto mask = std::make_shared<RSShaderMask>();
    auto rsDisplacementDistortFilter = std::make_shared<RSDisplacementDistortFilter>(mask);

    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsDisplacementDistortFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: GetFactor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplacementDistortFilterTest, GetFactor001, TestSize.Level1)
{
    auto mask = std::make_shared<RSShaderMask>();
    auto rsDisplacementDistortFilter = std::make_shared<RSDisplacementDistortFilter>(mask);

    auto factor = rsDisplacementDistortFilter->GetFactor();
    // 1.f, 1.f is default factor
    EXPECT_EQ(factor, Vector2f(1.f, 1.f));
}


/**
 * @tc.name: GetMask001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDisplacementDistortFilterTest, GetMask001, TestSize.Level1)
{
    auto mask = std::make_shared<RSShaderMask>();
    auto rsDisplacementDistortFilter = std::make_shared<RSDisplacementDistortFilter>(mask);

    auto tempMask = rsDisplacementDistortFilter->GetMask();
    EXPECT_EQ(tempMask, mask);
}
} // namespace OHOS::Rosen