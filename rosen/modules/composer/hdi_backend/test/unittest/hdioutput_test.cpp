/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_output.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiOutputTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::shared_ptr<HdiOutput> hdiOutput_;
};

void HdiOutputTest::SetUpTestCase()
{
    uint32_t screenId = 0;
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId);
}

void HdiOutputTest::TearDownTestCase() {}

namespace {
/**
 * @tc.name: GetScreenId001
 * @tc.desc: Verify the GetScreenId of hdioutput
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiOutputTest, GetScreenId001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetScreenId(), 0u);
}

/**
 * @tc.name: Init001
 * @tc.desc: Verify the Init of hdioutput
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiOutputTest, Init001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
}

/**
 * @tc.name: GetOutputDamage001
 * @tc.desc: Verify the GetOutputDamage of hdioutput
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiOutputTest, GetOutputDamage001, Function | MediumTest| Level3)
{
    uint32_t num = 1;
    IRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiOutputTest::hdiOutput_->SetOutputDamage(num, iRect);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetOutputDamage().x, iRect.x);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetOutputDamage().y, iRect.y);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetOutputDamage().w, iRect.w);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetOutputDamage().h, iRect.h);
}

/**
 * @tc.name: GetOutputDamageNum001
 * @tc.desc: Verify the GetOutputDamageNum of hdioutput
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiOutputTest, GetOutputDamageNum001, Function | MediumTest| Level3)
{
    uint32_t num = 1;
    IRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiOutputTest::hdiOutput_->SetOutputDamage(num, iRect);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetOutputDamageNum(), 1u);
}

/**
 * @tc.name: GetProducerSurface001
 * @tc.desc: Verify the GetProducerSurface of hdioutput
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiOutputTest, GetProducerSurface001, Function | MediumTest| Level3)
{
    ASSERT_NE(HdiOutputTest::hdiOutput_->GetFrameBufferSurface(), nullptr);
}

/**
 * @tc.name: GetFramebuffer001
 * @tc.desc: Verify the GetFramebuffer of hdioutput
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiOutputTest, GetFramebuffer001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetFramebuffer(), nullptr);
}

/**
 * @tc.name: GetFramebufferFence001
 * @tc.desc: Verify the GetFramebufferFence of hdioutput
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiOutputTest, GetFramebufferFence001, Function | MediumTest| Level3)
{
    ASSERT_NE(HdiOutputTest::hdiOutput_->GetFramebufferFence(), nullptr);
}
} // namespace
} // namespace Rosen
} // namespace OHOS