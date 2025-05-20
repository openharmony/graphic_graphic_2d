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

#include "render/rs_shader_mask.h"
#include "ge_ripple_shader_mask.h"
#include "render/rs_render_ripple_mask.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSShaderMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShaderMaskTest::SetUpTestCase() {}
void RSShaderMaskTest::TearDownTestCase() {}
void RSShaderMaskTest::SetUp() {}
void RSShaderMaskTest::TearDown() {}

/**
* @tc.name: CalHashTest001
* @tc.desc: Verify function CalHash
* @tc.type: FUNC
*/
HWTEST_F(RSShaderMaskTest, CalHashTest001, TestSize.Level1)
{
    auto rsRenderMaskPara = std::make_shared<RSRenderMaskPara>(RSUIFilterType::RIPPLE_MASK);
    auto rsShaderMask = std::make_shared<RSShaderMask>(rsRenderMaskPara);
    rsShaderMask->CalHash();
    EXPECT_EQ(rsShaderMask->hash_, 0);
}


} // namespace OHOS::Rosen