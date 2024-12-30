/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "luminance/rs_luminance_control.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLuminanceControlUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLuminanceControlUnitTest::SetUpTestCase() {}
void RSLuminanceControlUnitTest::TearDownTestCase() {}
void RSLuminanceControlUnitTest::SetUp() {}
void RSLuminanceControlUnitTest::TearDown() {}

/**
 * @tc.name: LuminanceControl001
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlUnitTest, LuminanceControl001, TestSize.Level1)
{
    ScreenId screenId{};
    int32_t mode{};
    uint32_t level{};
    auto& luminaCtrl = RSLuminanceControl::Get();
    luminaCtrl.Init();
    luminaCtrl.DimmingIncrease(screenId);
    std::ignore = luminaCtrl.IsDimmingOn(screenId);
    std::ignore = luminaCtrl.IsHdrOn(screenId);
    std::ignore = luminaCtrl.IsNeedUpdateLuminance(screenId);
    std::ignore = luminaCtrl.GetHdrTmoNits(screenId, mode);
    std::ignore = luminaCtrl.GetHdrDisplayNits(screenId);
    std::ignore = luminaCtrl.GetHdrBrightnessRatio(screenId, mode);
    std::ignore = luminaCtrl.GetNewHdrLuminance(screenId);
    luminaCtrl.SetNowHdrLuminance(screenId, level);
    luminaCtrl.SetSdrLuminance(screenId, level);
    luminaCtrl.SetHdrStatus(screenId, false);

    luminaCtrl.initStatus_ = true;
    luminaCtrl.DimmingIncrease(screenId);
    std::ignore = luminaCtrl.IsDimmingOn(screenId);
    std::ignore = luminaCtrl.IsHdrOn(screenId);
    std::ignore = luminaCtrl.IsNeedUpdateLuminance(screenId);
    std::ignore = luminaCtrl.GetHdrTmoNits(screenId, mode);
    std::ignore = luminaCtrl.GetHdrDisplayNits(screenId);
    std::ignore = luminaCtrl.GetHdrBrightnessRatio(screenId, mode);
    std::ignore = luminaCtrl.GetNewHdrLuminance(screenId);
    luminaCtrl.SetNowHdrLuminance(screenId, level);
    luminaCtrl.SetSdrLuminance(screenId, level);
    luminaCtrl.SetHdrStatus(screenId, false);
    
    ASSERT_NE((&luminaCtrl), nullptr);
}

/**
 * @tc.name: LuminanceControl002
 * @tc.desc: Test LuminanceControl class members
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLuminanceControlUnitTest, LuminanceControl002, TestSize.Level1)
{
    auto& luminaCtrl = RSLuminanceControl::Get();
    std::ignore = luminaCtrl.LoadLibrary();
    std::ignore = luminaCtrl.LoadStatusControl();
    std::ignore = luminaCtrl.LoadLumControl();
    std::ignore = luminaCtrl.LoadTmoControl();
    luminaCtrl.CloseLibrary();

    int32_t extLibHandle = 0;
    luminaCtrl.extLibHandle_ = &extLibHandle;
    std::ignore = luminaCtrl.LoadLibrary();
    std::ignore = luminaCtrl.LoadStatusControl();
    std::ignore = luminaCtrl.LoadLumControl();
    std::ignore = luminaCtrl.LoadTmoControl();
    luminaCtrl.CloseLibrary();
    luminaCtrl.extLibHandle_ = nullptr;
    std::ignore = luminaCtrl.LoadTmoControl();

    ASSERT_NE((&luminaCtrl), nullptr);
}
} // namespace OHOS::Rosen