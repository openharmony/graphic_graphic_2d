/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "ui_effect/property/include/rs_ui_filter_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIFilterBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIFilterBaseTest::SetUpTestCase() {}
void RSUIFilterBaseTest::TearDownTestCase() {}
void RSUIFilterBaseTest::SetUp() {}
void RSUIFilterBaseTest::TearDown() {}

/**
 * @tc.name: CreateNGBlurFilter
 * @tc.desc: test for RSNGFilterHelper::CreateNGBlurFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGBlurFilter, TestSize.Level1)
{
    auto blurFilter = RSNGFilterHelper::CreateNGBlurFilter(10.f, 10.f);
    EXPECT_NE(blurFilter, nullptr);
}

/**
 * @tc.name: CreateNGMaterialBlurFilter
 * @tc.desc: test for RSNGFilterHelper::CreateNGMaterialBlurFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGMaterialBlurFilter, TestSize.Level1)
{
    MaterialParam materialParam;
    auto materialBlurFilter = RSNGFilterHelper::CreateNGMaterialBlurFilter(materialParam);
    EXPECT_NE(materialBlurFilter, nullptr);
}
} // namespace OHOS::Rosen
