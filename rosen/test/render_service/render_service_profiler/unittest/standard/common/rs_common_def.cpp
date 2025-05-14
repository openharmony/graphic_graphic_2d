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

#include "common/rs_common_def.h"

#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class UIFirstTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: FixUiFirstCcmTest
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIFirstTest, FixUiFirstCcmTest, Level1 | Standard)
{
    auto value = 1;
    EXPECT_EQ(static_cast<uint8_t>(UiFirstCcmType::SINGLE), value++);
    EXPECT_EQ(static_cast<uint8_t>(UiFirstCcmType::MULTI), value++);
    EXPECT_EQ(static_cast<uint8_t>(UiFirstCcmType::HYBRID), value++);
}

/*
 * @tc.name: FixRSUIFirstSwitch
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UIFirstTest, FixRSUIFirstSwitch, Level1 | Standard)
{
    auto value = 0;
    EXPECT_EQ(static_cast<uint16_t>(RSUIFirstSwitch::NONE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSUIFirstSwitch::MODAL_WINDOW_CLOSE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSUIFirstSwitch::FORCE_DISABLE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSUIFirstSwitch::FORCE_ENABLE), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSUIFirstSwitch::FORCE_ENABLE_LIMIT), value++);
    EXPECT_EQ(static_cast<uint16_t>(RSUIFirstSwitch::FORCE_DISABLE_NONFOCUS), value++);
}
} // namespace OHOS::Rosen