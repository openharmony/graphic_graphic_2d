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
#include "modifier_ng/geometry/rs_frame_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSFrameModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSFrameModifierTest
 * @tc.desc: Test Set/Get functions of RSFrameModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSFrameModifierNGTypeTest, RSFrameModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSFrameModifier> modifier = std::make_shared<ModifierNG::RSFrameModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::FRAME);

    Vector4f frame = { 1.0f, 2.0f, 3.0f, 4.0f };
    modifier->SetFrame(frame);
    EXPECT_EQ(modifier->GetFrame(), frame);

    Vector2f frameSize = { 5.0f, 6.0f };
    modifier->SetFrameSize(frameSize);
    EXPECT_EQ(modifier->GetFrameSize(), frameSize);

    float frameWidth = 7.0f;
    modifier->SetFrameWidth(frameWidth);
    EXPECT_EQ(modifier->GetFrameWidth(), frameWidth);

    float frameHeight = 8.0f;
    modifier->SetFrameHeight(frameHeight);
    EXPECT_EQ(modifier->GetFrameHeight(), frameHeight);

    Vector2f framePosition = { 9.0f, 10.0f };
    modifier->SetFramePosition(framePosition);
    EXPECT_EQ(modifier->GetFramePosition(), framePosition);

    float framePositionX = 11.0f;
    modifier->SetFramePositionX(framePositionX);
    EXPECT_EQ(modifier->GetFramePositionX(), framePositionX);

    float framePositionY = 12.0f;
    modifier->SetFramePositionY(framePositionY);
    EXPECT_EQ(modifier->GetFramePositionY(), framePositionY);
}
} // namespace OHOS::Rosen