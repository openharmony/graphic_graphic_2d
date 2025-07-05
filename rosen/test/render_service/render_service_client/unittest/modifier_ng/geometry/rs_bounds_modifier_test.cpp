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
#include "modifier_ng/geometry/rs_bounds_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSBoundsModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSBoundsModifierTest
 * @tc.desc: Test Set/Get functions of RSBoundsModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSBoundsModifierNGTypeTest, RSBoundsModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSBoundsModifier> modifier = std::make_shared<ModifierNG::RSBoundsModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::BOUNDS);

    Vector4f bounds = { 1.0f, 2.0f, 3.0f, 4.0f };
    modifier->SetBounds(bounds);
    EXPECT_EQ(modifier->GetBounds(), bounds);

    Vector2f size = { 3.0f, 4.0f };
    modifier->SetBoundsSize(size);
    EXPECT_EQ(modifier->GetBoundsSize(), size);

    float width = 3.0f;
    modifier->SetBoundsWidth(width);
    EXPECT_EQ(modifier->GetBoundsWidth(), width);

    float height = 4.0f;
    modifier->SetBoundsHeight(height);
    EXPECT_EQ(modifier->GetBoundsHeight(), height);

    Vector2f position = { 1.0f, 2.0f };
    modifier->SetBoundsPosition(position);
    EXPECT_EQ(modifier->GetBoundsPosition(), position);

    float positionX = 1.0f;
    modifier->SetBoundsPositionX(positionX);
    EXPECT_EQ(modifier->GetBoundsPositionX(), positionX);

    float positionY = 2.0f;
    modifier->SetBoundsPositionY(positionY);
    EXPECT_EQ(modifier->GetBoundsPositionY(), positionY);
}

/**
 * @tc.name: ApplyGeometryTest
 * @tc.desc: Test the function ApplyGeometry
 * @tc.type: FUNC
 */
HWTEST_F(RSBoundsModifierNGTypeTest, ApplyGeometryTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    // geometry is null
    std::shared_ptr<RSObjAbsGeometry> geometry = nullptr;
    modifier->ApplyGeometry(geometry);
    EXPECT_EQ(geometry, nullptr);
    // geometry and property is not null
    Vector4f bounds(100.0f);
    modifier->SetBounds(bounds);
    geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);
    EXPECT_NE(geometry, nullptr);
    // property is null, geometry is not null
    auto modifier2 = std::make_shared<ModifierNG::RSBoundsModifier>();
    auto geometry2 = std::make_shared<RSObjAbsGeometry>();
    modifier2->ApplyGeometry(geometry2);
    EXPECT_NE(geometry2, nullptr);
}
} // namespace OHOS::Rosen