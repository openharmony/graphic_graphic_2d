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
#include "modifier_ng/foreground/rs_env_foreground_color_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSEnvForegroundColorModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSEnvForegroundColorModifierTest
 * @tc.desc: Test Set/Get functions of RSEnvForegroundColorModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSEnvForegroundColorModifierNGTypeTest, RSEnvForegroundColorModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSEnvForegroundColorModifier> modifier = std::make_shared<ModifierNG::RSEnvForegroundColorModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::ENV_FOREGROUND_COLOR);

    EXPECT_EQ(modifier->GetEnvForegroundColor(), Color());
    EXPECT_EQ(modifier->GetEnvForegroundColorStrategy(), ForegroundColorStrategyType::INVALID);

    Color colorTest = { 15, 14, 13, 12 };
    modifier->SetEnvForegroundColor(colorTest);
    EXPECT_EQ(modifier->GetEnvForegroundColor(), colorTest);

    modifier->SetEnvForegroundColorStrategy(ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR);
    EXPECT_EQ(modifier->GetEnvForegroundColorStrategy(), ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR);
}
} // namespace OHOS::Rosen