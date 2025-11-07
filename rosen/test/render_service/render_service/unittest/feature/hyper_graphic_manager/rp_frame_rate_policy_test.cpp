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

#include "feature/hyper_graphic_manager/rp_frame_rate_policy.h"
#include "modifier/rs_render_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int32_t SMALL_SIZE_AREA = 1000;
constexpr int32_t SMALL_SIZE_LENGTH = 500;
constexpr float SCREEN_PPI = 3.9f;
constexpr float SCREEN_XDPI = 3.9f;
constexpr float SCREEN_YDPI = 3.9f;
}

class RPFrameRatePolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RPFrameRatePolicy> rpFrameRatePolicy_ = std::make_shared<RPFrameRatePolicy>();
    static inline std::shared_ptr<RPFrameRatePolicy> rpFrameRatePolicyHasSamll_ = std::make_shared<RPFrameRatePolicy>();
};

void RPFrameRatePolicyTest::SetUpTestCase()
{
    auto data = std::make_shared<RPHgmConfigData>();
    data->SetPpi(SCREEN_PPI);
    data->SetXDpi(SCREEN_XDPI);
    data->SetYDpi(SCREEN_YDPI);

    data->AddAnimDynamicItem({"translate", "1", 800, -1, 90});
    data->AddAnimDynamicItem({"translate", "2", 77, 800, 120});
    data->AddAnimDynamicItem({"translate", "3", 10, 77, 90});
    data->AddAnimDynamicItem({"translate", "4", 0, 10, 60});
    data->AddAnimDynamicItem({"rotation", "1", 0, -1, 90});
    rpFrameRatePolicy_->HgmConfigUpdateCallback(data);

    data->SetSmallSizeArea(SMALL_SIZE_AREA);
    data->SetSmallSizeLength(SMALL_SIZE_LENGTH);
    data->AddSmallSizeAnimDynamicItem({"translate", "1", 802, -1, 90});
    data->AddSmallSizeAnimDynamicItem({"translate", "2", 79, 802, 120});
    data->AddSmallSizeAnimDynamicItem({"translate", "3", 12, 79, 90});
    data->AddSmallSizeAnimDynamicItem({"translate", "4", 2, 12, 72});
    data->AddSmallSizeAnimDynamicItem({"scale", "1", 802, -1, 90});
    data->AddSmallSizeAnimDynamicItem({"scale", "2", 79, 802, 120});
    data->AddSmallSizeAnimDynamicItem({"scale", "3", 12, 79, 90});
    data->AddSmallSizeAnimDynamicItem({"scale", "4", 2, 12, 72});
    rpFrameRatePolicyHasSamll_->HgmConfigUpdateCallback(data);
}
void RPFrameRatePolicyTest::TearDownTestCase() {}
void RPFrameRatePolicyTest::SetUp() {}
void RPFrameRatePolicyTest::TearDown() {}

/**
 * @tc.name: TestHgmConfigUpdateCallback
 * @tc.desc: test RPFrameRatePolicy.HgmConfigUpdateCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RPFrameRatePolicyTest, TestHgmConfigUpdateCallback, TestSize.Level1)
{
    RPFrameRatePolicy rpFrameRatePolicy;
    rpFrameRatePolicy.HgmConfigUpdateCallback(nullptr);
    EXPECT_EQ(rpFrameRatePolicy.animAttributes_.size(), 0);

    auto data = std::make_shared<RPHgmConfigData>();
    data->SetSmallSizeArea(SMALL_SIZE_AREA);
    rpFrameRatePolicy.HgmConfigUpdateCallback(data);
    EXPECT_NE(rpFrameRatePolicy.smallSizeArea_, SMALL_SIZE_AREA);

    data->AddSmallSizeAnimDynamicItem({"translate", "", 0, -1, 60});
    rpFrameRatePolicy.HgmConfigUpdateCallback(data);
    EXPECT_EQ(rpFrameRatePolicy.smallSizeAnimAttributes_.size(), 0);

    data->smallSizeConfigData_.clear();
    data->AddSmallSizeAnimDynamicItem({"", "1", 0, -1, 60});
    rpFrameRatePolicy.HgmConfigUpdateCallback(data);
    EXPECT_EQ(rpFrameRatePolicy.smallSizeAnimAttributes_.size(), 0);

    data->smallSizeConfigData_.clear();
    data->AddSmallSizeAnimDynamicItem({"", "", 0, -1, 60});
    rpFrameRatePolicy.HgmConfigUpdateCallback(data);
    EXPECT_EQ(rpFrameRatePolicy.smallSizeAnimAttributes_.size(), 0);

    data->smallSizeConfigData_.clear();
    data->AddSmallSizeAnimDynamicItem({"translate", "1", 0, -1, 60});
    rpFrameRatePolicy.HgmConfigUpdateCallback(data);
    EXPECT_EQ(rpFrameRatePolicy.smallSizeAnimAttributes_.size(), 1);
    EXPECT_EQ(rpFrameRatePolicy.smallSizeArea_, SMALL_SIZE_AREA);

    data->smallSizeConfigData_.clear();
    data->AddAnimDynamicItem({"translate", "", 0, -1, 60});
    rpFrameRatePolicy.HgmConfigUpdateCallback(data);
    EXPECT_EQ(rpFrameRatePolicy.animAttributes_.size(), 0);

    data->configData_.clear();
    data->AddAnimDynamicItem({"", "1", 0, -1, 60});
    rpFrameRatePolicy.HgmConfigUpdateCallback(data);
    EXPECT_EQ(rpFrameRatePolicy.animAttributes_.size(), 0);

    data->configData_.clear();
    data->AddAnimDynamicItem({"", "", 0, -1, 60});
    rpFrameRatePolicy.HgmConfigUpdateCallback(data);
    EXPECT_EQ(rpFrameRatePolicy.animAttributes_.size(), 0);

    data->configData_.clear();
    data->AddAnimDynamicItem({"translate", "1", 0, -1, 60});
    rpFrameRatePolicy.HgmConfigUpdateCallback(data);
    EXPECT_EQ(rpFrameRatePolicy.animAttributes_.size(), 1);
}

/**
 * @tc.name: TestGetExpectedFrameRate
 * @tc.desc: test RPFrameRatePolicy.GetExpectedFrameRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RPFrameRatePolicyTest, TestGetExpectedFrameRate, TestSize.Level1)
{
    RPFrameRatePolicy rpFrameRatePolicy;
    rpFrameRatePolicy.ppi_ = -1.f;
    EXPECT_EQ(rpFrameRatePolicy.GetExpectedFrameRate(RSPropertyUnit::PIXEL_SIZE, 0.f, 1000.f, 0.f), 0);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetExpectedFrameRate(RSPropertyUnit::UNKNOWN, 0.f, 1000.f, 0.f), 0);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetExpectedFrameRate(RSPropertyUnit::ANGLE_ROTATION, 0.f, 1000.f, 0.f), 0);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetExpectedFrameRate(RSPropertyUnit::PIXEL_SIZE, 0.f, 1000.f, 0.f), 0);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetExpectedFrameRate(RSPropertyUnit::PIXEL_POSITION, 0.f, 1000.f, 0.f), 0);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetExpectedFrameRate(RSPropertyUnit::RATIO_SCALE, 0.f, 1000.f, 0.f), 0);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetExpectedFrameRate(static_cast<RSPropertyUnit>(0xff), 0.f, 1000.f, 0.f), 0);
}

/**
 * @tc.name: TestGetPreferredFps
 * @tc.desc: test RPFrameRatePolicy.GetPreferredFps
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RPFrameRatePolicyTest, TestGetPreferredFps, TestSize.Level1)
{
    EXPECT_EQ(rpFrameRatePolicy_->GetPreferredFps("translate", 800.1f, 0.f, 0.f), 90);
    EXPECT_EQ(rpFrameRatePolicy_->GetPreferredFps("translate", -1.f, 0.f, 0.f), 0);
    EXPECT_EQ(rpFrameRatePolicy_->GetPreferredFps("scale", 800.1f, 0.f, 0.f), 0);

    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("translate", 800.1f, 100.f, 100.f), 120);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("translate", 3.f, 100.f, 100.f), 72);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("translate", 1.f, 100.f, 100.f), 60);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("rotation", 800.1f, 100.f, 100.f), 90);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("rotation", 800.1f, 100.f, SMALL_SIZE_LENGTH + 1.f), 90);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("rotation", 800.1f, 100.f, -1.f), 90);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("rotation", 800.1f, -1.f, -1.f), 90);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("rotation", 800.1f, SMALL_SIZE_AREA + 1.f, -1.f), 90);
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("scale", 800.1f, -1.f, -1.f), 0);

    rpFrameRatePolicyHasSamll_->smallSizeLength_ = -1.f;
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("rotation", 800.1f, 100.f, 100.f), 90);
    rpFrameRatePolicyHasSamll_->smallSizeLength_ = SMALL_SIZE_LENGTH;

    rpFrameRatePolicyHasSamll_->smallSizeArea_ = -1.f;
    EXPECT_EQ(rpFrameRatePolicyHasSamll_->GetPreferredFps("rotation", 800.1f, 100.f, 100.f), 90);
    rpFrameRatePolicyHasSamll_->smallSizeArea_ = SMALL_SIZE_AREA;
}
} // namespace OHOS::Rosen