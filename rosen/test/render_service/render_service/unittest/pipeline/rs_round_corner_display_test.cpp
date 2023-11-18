/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "common/rs_singleton.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/round_corner_display/rs_sub_thread_rcd.h"
#include "pipeline/round_corner_display/rs_round_corner_display.h"
#include "pipeline/rs_display_render_node.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRoundCornerDisplayTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;
};

void RSRoundCornerDisplayTest::SetUpTestCase() {}
void RSRoundCornerDisplayTest::TearDownTestCase() {}
void RSRoundCornerDisplayTest::SetUp() {}
void RSRoundCornerDisplayTest::TearDown() {}

/*
 * @tc.name: RSSubThreadRCDTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RSSubThreadRCDTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSSubThreadRCDTest, TestSize.Level1)
{
    auto context = new RenderContext();
    auto threadRcd = &(RSSingleton<RSSubThreadRCD>::GetInstance());
    threadRcd->Start(context);
    delete context;
    context = nullptr;
}

/*
 * @tc.name: RCDLoadConfigFileTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RCDLoadConfigFileTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RCDLoadConfigFileTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.LoadConfigFile();
}

/*
 * @tc.name: RCDInitTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RCDInitTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RCDInitTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
}

/*
 * @tc.name: UpdateParameterTest
 * @tc.desc: Test RSRoundCornerDisplayTest.UpdateParameterTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, UpdateParameterTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();

    ScreenRotation curOrientation = ScreenRotation::ROTATION_0;
    rcdInstance.UpdateOrientationStatus(curOrientation);

    int notchStatus = 0;
    rcdInstance.UpdateNotchStatus(notchStatus);

    uint32_t width = 1344;
    uint32_t height = 2772;
    rcdInstance.UpdateDisplayParameter(width, height);

    std::map<std::string, bool> updateFlag = {
        {"display", true},
        {"notch", true},
        {"orientation", true}
    };
    rcdInstance.UpdateParameter(updateFlag);
}

/*
 * @tc.name: RSDrawRoundCornerTest
 * @tc.desc: Test RSRoundCornerDisplayTest.RSDrawRoundCornerTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, RSDrawRoundCornerTest, TestSize.Level1)
{
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();

    ScreenRotation curOrientation = ScreenRotation::ROTATION_0;
    rcdInstance.UpdateOrientationStatus(curOrientation);

    int notchStatus = 0;
    rcdInstance.UpdateNotchStatus(notchStatus);

    uint32_t width = 1344;
    uint32_t height = 2772;
    rcdInstance.UpdateDisplayParameter(width, height);

    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(width, height);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
    ASSERT_NE(canvas, nullptr);
    rcdInstance.DrawRoundCorner(canvas);
}

}
