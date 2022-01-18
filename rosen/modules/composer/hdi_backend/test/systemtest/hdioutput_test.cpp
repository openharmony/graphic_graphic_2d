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
class HdiOutputSysTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::shared_ptr<HdiOutput> hdiOutput_;
};

void HdiOutputSysTest::SetUpTestCase()
{
    uint32_t screenId = 0;
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId);
}

void HdiOutputSysTest::TearDownTestCase() {}

namespace {
/**
 * @tc.name: TestHdiOutput001
 * @tc.desc: Verify the hdioutput
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiOutputSysTest, TestHdiOutput001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetScreenId(), 0u);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
    uint32_t num = 1;
    IRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiOutputSysTest::hdiOutput_->SetOutputDamage(num, iRect);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetOutputDamage().x, iRect.x);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetOutputDamage().y, iRect.y);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetOutputDamage().w, iRect.w);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetOutputDamage().h, iRect.h);

    IRect iRect1 = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiOutputSysTest::hdiOutput_->SetOutputDamage(num, iRect1);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetOutputDamageNum(), 1u);

    IRect iRect2 = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiOutputSysTest::hdiOutput_->SetOutputDamage(num, iRect2);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetOutputDamageNum(), 1u);

    ASSERT_NE(HdiOutputSysTest::hdiOutput_->GetProducerSurface(), nullptr);
    ASSERT_EQ(HdiOutputSysTest::hdiOutput_->GetFramebuffer(), nullptr);
    ASSERT_NE(HdiOutputSysTest::hdiOutput_->GetFramebufferFence(), nullptr);
}
} // namespace
} // namespace Rosen
} // namespace OHOS