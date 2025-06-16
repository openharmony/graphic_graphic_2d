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
#include <string.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/appearance/rs_point_light_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSPointLightModifierNGTypeTest : public testing::Test {
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
HWTEST_F(RSPointLightModifierNGTypeTest, RSPointLightModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSPointLightModifier> modifier = std::make_shared<ModifierNG::RSPointLightModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::POINT_LIGHT);

    modifier->SetLightIntensity(1.0f);
    EXPECT_EQ(modifier->GetLightIntensity(), 1.0f);

    Color lightColor(15, 11, 15, 16);
    modifier->SetLightColor(lightColor);
    EXPECT_EQ(modifier->GetLightColor(), lightColor);

    Vector4f lightPosition(1.0f, 2.0f, 3.0f, 4.0f);
    modifier->SetLightPosition(lightPosition);
    EXPECT_EQ(modifier->GetLightPosition(), lightPosition);

    modifier->SetIlluminatedBorderWidth(1.0f);
    EXPECT_EQ(modifier->GetIlluminatedBorderWidth(), 1.0f);

    modifier->SetIlluminatedType(1);
    EXPECT_EQ(modifier->GetIlluminatedType(), 1);

    modifier->SetBloom(1.0f);
    EXPECT_EQ(modifier->GetBloom(), 1.0f);
}
} // namespace OHOS::Rosen