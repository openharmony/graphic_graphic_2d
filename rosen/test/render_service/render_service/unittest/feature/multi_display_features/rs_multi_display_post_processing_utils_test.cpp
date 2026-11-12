/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <fstream>
#include <thread>
#include <vector>

#include "draw/surface.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "rs_layer_transaction_data.h"
#include "rs_multi_display_feature_param.h"
#include "rs_multi_display_interpolation_filter.h"
#include "rs_multi_display_post_processing_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t FAKE_WIDTH = 200;
constexpr uint32_t FAKE_HEIGHT = 120;
constexpr uint32_t PHYS_WIDTH = 200;
constexpr uint32_t PHYS_HEIGHT = 180;
constexpr uint32_t PARAM_A_UINT = 2;
constexpr uint32_t PARAM_N_UINT = 3;
constexpr float PARAM_A = 2.0f;
constexpr float PARAM_N = 3.0f;
constexpr ScreenId TEST_SCREEN_ID = 0;
constexpr ScreenId CONFIGED_SCREEN_ID = 0;
constexpr ScreenId UNCONFIGED_SCREEN_ID = 16;
const std::string TEST_XML_PATH = "rs_multi_display_interpolation_ut_config.xml";

ComposerScreenInfo MakeComposerScreenInfo(uint32_t width, uint32_t height, uint32_t phyWidth, uint32_t phyHeight)
{
    ComposerScreenInfo screenInfo;
    screenInfo.id = TEST_SCREEN_ID;
    screenInfo.width = width;
    screenInfo.height = height;
    screenInfo.phyWidth = phyWidth;
    screenInfo.phyHeight = phyHeight;
    return screenInfo;
}

RSMultiDisplayInterpolationFilterParams MakeValidFilterParams()
{
    RSMultiDisplayInterpolationFilterParams params;
    params.filterOn = true;
    params.paramA = PARAM_A;
    params.paramN = PARAM_N;
    params.realWidth = static_cast<float>(FAKE_WIDTH);
    params.realHeight = static_cast<float>(FAKE_HEIGHT);
    return params;
}

bool WriteAndLoadConfig(const bool interpSwitch, const bool withParams)
{
    std::string interpSwitchStr = interpSwitch ? "true" : "false";
    std::string paramsNode =
        withParams ? "<params displayId=\"0\" realWidth=\"200\" realHeight=\"120\" paramA=\"2\" paramN=\"3\"/>" : "";
    std::string content = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Configs>"
        "<multi_display_features>"
        "<interp_screen_feature name=\"switch\" value=\"" + interpSwitchStr + "\">" +
        paramsNode + "</interp_screen_feature>" +
        "</multi_display_features></Configs>";
    std::ofstream file(TEST_XML_PATH);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    file.close();
    return RSMultiDisplayFeatureParam::Load(TEST_XML_PATH);
}
} // namespace

class RSMultiDisplayPostProcessingUtilsTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override
    {
        surface_ = Drawing::Surface::MakeRasterN32Premul(FAKE_WIDTH, FAKE_HEIGHT);
        ASSERT_TRUE(surface_ != nullptr);
        canvas_ = std::make_unique<RSPaintFilterCanvas>(surface_.get());
        ASSERT_TRUE(canvas_ != nullptr);
    }
    void TearDown() override {}

protected:
    std::shared_ptr<Drawing::Surface> surface_;
    std::unique_ptr<RSPaintFilterCanvas> canvas_;
};

/**
 * @tc.name: Prepare001
 * @tc.desc: filterOn=false (non-interpolation screen), short-circuit and return false directly without touching canvas
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayPostProcessingUtilsTest, Prepare001, TestSize.Level1)
{
    auto filterParams = MakeValidFilterParams();
    filterParams.filterOn = false;
    auto screenInfo = MakeComposerScreenInfo(FAKE_WIDTH, FAKE_HEIGHT, PHYS_WIDTH, PHYS_HEIGHT);

    ASSERT_FALSE(RSMultiDisplayPostProcessingUtils::Prepare(*canvas_, filterParams, screenInfo));
    ASSERT_EQ(filterParams.targetWidth, 0.0f);
    ASSERT_EQ(filterParams.targetHeight, 0.0f);
}

/**
 * @tc.name: Prepare002
 * @tc.desc: Return false on validation failure if paramA/paramN is invalid (≤0)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayPostProcessingUtilsTest, Prepare002, TestSize.Level1)
{
    auto screenInfo = MakeComposerScreenInfo(FAKE_WIDTH, FAKE_HEIGHT, PHYS_WIDTH, PHYS_HEIGHT);
    auto invalidA = MakeValidFilterParams();
    invalidA.paramA = 0.0f;
    ASSERT_FALSE(RSMultiDisplayPostProcessingUtils::Prepare(*canvas_, invalidA, screenInfo));

    auto invalidN = MakeValidFilterParams();
    invalidN.paramN = -1.0f;
    ASSERT_FALSE(RSMultiDisplayPostProcessingUtils::Prepare(*canvas_, invalidN, screenInfo));
}

/**
 * @tc.name: Prepare003
 * @tc.desc: Mock consistency check, return false if render resolution (width/height) mismatches configured realW/H
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayPostProcessingUtilsTest, Prepare003, TestSize.Level1)
{
    auto screenInfo = MakeComposerScreenInfo(FAKE_WIDTH + 1, FAKE_HEIGHT, PHYS_WIDTH, PHYS_HEIGHT);
    auto filterParams = MakeValidFilterParams();
    ASSERT_FALSE(RSMultiDisplayPostProcessingUtils::Prepare(*canvas_, filterParams, screenInfo));

    screenInfo = MakeComposerScreenInfo(FAKE_WIDTH, FAKE_HEIGHT + 1, PHYS_WIDTH, PHYS_HEIGHT);
    ASSERT_FALSE(RSMultiDisplayPostProcessingUtils::Prepare(*canvas_, filterParams, screenInfo));
}

/**
 * @tc.name: Prepare004
 * @tc.desc: Reverse physical check, return false if phyW!=realW or phyH!=realH*N/A (prevent HDI config conflict)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayPostProcessingUtilsTest, Prepare004, TestSize.Level1)
{
    auto filterParams = MakeValidFilterParams();
    auto badPhyWidth = MakeComposerScreenInfo(FAKE_WIDTH, FAKE_HEIGHT, PHYS_WIDTH + 1, PHYS_HEIGHT);
    ASSERT_FALSE(RSMultiDisplayPostProcessingUtils::Prepare(*canvas_, filterParams, badPhyWidth));

    auto badPhyHeight = MakeComposerScreenInfo(FAKE_WIDTH, FAKE_HEIGHT, PHYS_WIDTH, PHYS_HEIGHT + 1);
    ASSERT_FALSE(RSMultiDisplayPostProcessingUtils::Prepare(*canvas_, filterParams, badPhyHeight));
}

/**
 * @tc.name: Prepare005
 * @tc.desc: Prepare succeeds with valid params: targetW/H filled with physical size, main canvas replaced by offscreen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayPostProcessingUtilsTest, Prepare005, TestSize.Level1)
{
    auto filterParams = MakeValidFilterParams();
    auto screenInfo = MakeComposerScreenInfo(FAKE_WIDTH, FAKE_HEIGHT, PHYS_WIDTH, PHYS_HEIGHT);
    auto* mainSurface = canvas_->GetSurface();
    ASSERT_TRUE(mainSurface != nullptr);

    ASSERT_TRUE(RSMultiDisplayPostProcessingUtils::Prepare(*canvas_, filterParams, screenInfo));
    ASSERT_EQ(filterParams.targetWidth, static_cast<float>(PHYS_WIDTH));
    ASSERT_EQ(filterParams.targetHeight, static_cast<float>(PHYS_HEIGHT));
    ASSERT_TRUE(canvas_->GetSurface() != nullptr);
    ASSERT_NE(canvas_->GetSurface(), mainSurface);

    RSMultiDisplayPostProcessingUtils::Process(*canvas_, filterParams, true);
}

/**
 * @tc.name: Process001
 * @tc.desc: No operation in Process if needPostProcessing=false, canvas surface remains unchanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayPostProcessingUtilsTest, Process001, TestSize.Level1)
{
    auto* surfaceBefore = canvas_->GetSurface();
    RSMultiDisplayInterpolationFilterParams filterParams;
    RSMultiDisplayPostProcessingUtils::Process(*canvas_, filterParams, false);
    ASSERT_EQ(canvas_->GetSurface(), surfaceBefore);
}

/**
 * @tc.name: Process002
 * @tc.desc: After matched Prepare→Process, main canvas restores original surface
 *           (raster backend shader fallback no crash)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayPostProcessingUtilsTest, Process002, TestSize.Level2)
{
    auto filterParams = MakeValidFilterParams();
    auto screenInfo = MakeComposerScreenInfo(FAKE_WIDTH, FAKE_HEIGHT, PHYS_WIDTH, PHYS_HEIGHT);
    auto* mainSurface = canvas_->GetSurface();
    ASSERT_TRUE(RSMultiDisplayPostProcessingUtils::Prepare(*canvas_, filterParams, screenInfo));
    ASSERT_NE(canvas_->GetSurface(), mainSurface);

    auto offscreenCanvas = canvas_->GetSurface()->GetCanvas();
    ASSERT_TRUE(offscreenCanvas != nullptr);
    offscreenCanvas->DrawRect(Drawing::Rect(0.0f, 0.0f, FAKE_WIDTH, FAKE_HEIGHT));

    RSMultiDisplayPostProcessingUtils::Process(*canvas_, filterParams, true);
    ASSERT_EQ(canvas_->GetSurface(), mainSurface);
}

/**
 * @tc.name: Process003
 * @tc.desc: Concurrent DrawInterpolation on independent canvases: the shared RuntimeEffect
 *           must be initialized exactly once and thread-safely (magic static), no data race
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayPostProcessingUtilsTest, Process003, TestSize.Level1)
{
    constexpr uint32_t threadNum = 8;
    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < threadNum; i++) {
        threads.emplace_back([]() {
            auto surface = Drawing::Surface::MakeRasterN32Premul(FAKE_WIDTH, FAKE_HEIGHT);
            ASSERT_TRUE(surface != nullptr);
            auto canvas = surface->GetCanvas();
            ASSERT_TRUE(canvas != nullptr);
            canvas->DrawRect(Drawing::Rect(0.0f, 0.0f, FAKE_WIDTH, FAKE_HEIGHT));
            auto image = surface->GetImageSnapshot();
            ASSERT_TRUE(image != nullptr);
            RSMultiDisplayInterpolationFilter filter(PARAM_A, PARAM_N, FAKE_WIDTH, FAKE_HEIGHT);
            filter.DrawInterpolation(*canvas, image, Drawing::Rect(0.0f, 0.0f, FAKE_WIDTH, FAKE_HEIGHT));
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
}

/**
 * @tc.name: FilterDescription001
 * @tc.desc: GetDescription contains all shader inputs for DFX to diagnose parameter mismatch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayPostProcessingUtilsTest, FilterDescription001, TestSize.Level1)
{
    RSMultiDisplayInterpolationFilter filter(PARAM_A, PARAM_N, FAKE_WIDTH, FAKE_HEIGHT);
    auto description = filter.GetDescription();
    ASSERT_NE(description.find("2.000000"), std::string::npos);
    ASSERT_NE(description.find("3.000000"), std::string::npos);
    ASSERT_NE(description.find("200.000000"), std::string::npos);
    ASSERT_NE(description.find("120.000000"), std::string::npos);
}
class RSMultiDisplayInterpolationParamTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase()
    {
        WriteAndLoadConfig(false, false);
    }
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: GetActiveInterpolationParams001
 * @tc.desc: Check1: short-circuit return nullopt if interp disabled (zero-overhead path for normal products)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayInterpolationParamTest, GetActiveInterpolationParams001, TestSize.Level1)
{
    ASSERT_TRUE(WriteAndLoadConfig(false, true));
    auto param =
        RSMultiDisplayPostProcessingUtils::GetActiveInterpolationParams(CONFIGED_SCREEN_ID, FAKE_WIDTH, FAKE_HEIGHT);
    ASSERT_FALSE(param.has_value());
}

/**
 * @tc.name: GetActiveInterpolationParams002
 * @tc.desc: Check1: config without split_screen_feature node (removed), interpolation still works
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayInterpolationParamTest, GetActiveInterpolationParams002, TestSize.Level1)
{
    ASSERT_TRUE(WriteAndLoadConfig(true, true));
    auto param =
        RSMultiDisplayPostProcessingUtils::GetActiveInterpolationParams(CONFIGED_SCREEN_ID, FAKE_WIDTH, FAKE_HEIGHT);
    ASSERT_TRUE(param.has_value());
}

/**
 * @tc.name: GetActiveInterpolationParams003
 * @tc.desc: Check2: return nullopt if enabled but screen unconfigured (per-frame path for normal screens)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayInterpolationParamTest, GetActiveInterpolationParams003, TestSize.Level1)
{
    ASSERT_TRUE(WriteAndLoadConfig(true, true));
    auto param =
        RSMultiDisplayPostProcessingUtils::GetActiveInterpolationParams(UNCONFIGED_SCREEN_ID, FAKE_WIDTH, FAKE_HEIGHT);
    ASSERT_FALSE(param.has_value());
}

/**
 * @tc.name: GetActiveInterpolationParams004
 * @tc.desc: Check3: mock consistency check, return nullopt if render resolution mismatches realW/H,
 *           avoid physical buffer-coord mismatch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayInterpolationParamTest, GetActiveInterpolationParams004, TestSize.Level1)
{
    ASSERT_TRUE(WriteAndLoadConfig(true, true));
    auto param = RSMultiDisplayPostProcessingUtils::GetActiveInterpolationParams(
        CONFIGED_SCREEN_ID, FAKE_WIDTH, FAKE_HEIGHT + 1);
    ASSERT_FALSE(param.has_value());
}

/**
 * @tc.name: GetActiveInterpolationParams005
 * @tc.desc: Return configured params if all 3 checks pass, fields match XML (for buffer rewrite & offscreen mapping)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayInterpolationParamTest, GetActiveInterpolationParams005, TestSize.Level1)
{
    ASSERT_TRUE(WriteAndLoadConfig(true, true));
    auto param =
        RSMultiDisplayPostProcessingUtils::GetActiveInterpolationParams(CONFIGED_SCREEN_ID, FAKE_WIDTH, FAKE_HEIGHT);
    ASSERT_TRUE(param.has_value());
    ASSERT_EQ(param->displayId, CONFIGED_SCREEN_ID);
    ASSERT_EQ(param->realWidth, FAKE_WIDTH);
    ASSERT_EQ(param->realHeight, FAKE_HEIGHT);
    ASSERT_EQ(param->paramA, PARAM_A_UINT);
    ASSERT_EQ(param->paramN, PARAM_N_UINT);
}

/**
 * @tc.name: GetActiveInterpolationParams006
 * @tc.desc: Load non-existent file returns false without altering current state (boot tolerance)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayInterpolationParamTest, GetActiveInterpolationParams006, TestSize.Level1)
{
    ASSERT_TRUE(WriteAndLoadConfig(true, true));
    ASSERT_FALSE(RSMultiDisplayFeatureParam::Load("not_exist_interpolation_ut_config.xml"));
    auto param =
        RSMultiDisplayPostProcessingUtils::GetActiveInterpolationParams(CONFIGED_SCREEN_ID, FAKE_WIDTH, FAKE_HEIGHT);
    ASSERT_TRUE(param.has_value());
}

/**
 * @tc.name: AdjustBufferConfigForInterpolation001
 * @tc.desc: bufferConfig updated to physical resolution when valid (A:N mapping target)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayInterpolationParamTest, AdjustBufferConfigForInterpolation001, TestSize.Level1)
{
    BufferRequestConfig bufferConfig;
    bufferConfig.width = FAKE_WIDTH;
    bufferConfig.height = FAKE_HEIGHT;

    ASSERT_TRUE(RSMultiDisplayPostProcessingUtils::AdjustBufferConfigForInterpolation(
        PHYS_WIDTH, PHYS_HEIGHT, bufferConfig));
    ASSERT_EQ(bufferConfig.width, PHYS_WIDTH);
    ASSERT_EQ(bufferConfig.height, PHYS_HEIGHT);
}

/**
 * @tc.name: AdjustBufferConfigForInterpolation002
 * @tc.desc: Return false and keep config unchanged if physical size invalid (HDI reports w/h=0)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMultiDisplayInterpolationParamTest, AdjustBufferConfigForInterpolation002, TestSize.Level1)
{
    BufferRequestConfig bufferConfig;
    bufferConfig.width = FAKE_WIDTH;
    bufferConfig.height = FAKE_HEIGHT;

    ASSERT_FALSE(
        RSMultiDisplayPostProcessingUtils::AdjustBufferConfigForInterpolation(0, PHYS_HEIGHT, bufferConfig));
    ASSERT_FALSE(
        RSMultiDisplayPostProcessingUtils::AdjustBufferConfigForInterpolation(PHYS_WIDTH, 0, bufferConfig));
    ASSERT_EQ(bufferConfig.width, FAKE_WIDTH);
    ASSERT_EQ(bufferConfig.height, FAKE_HEIGHT);
}
} // namespace OHOS::Rosen
