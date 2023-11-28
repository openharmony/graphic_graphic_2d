/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <test_header.h>

#include "hgm_vsync_generator_controller.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_render_frame_rate_linker.h"
#include "vsync_controller.h"
#include "vsync_generator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    sptr<VSyncGenerator> vsyncGenerator = CreateVSyncGenerator();
    sptr<VSyncController> rsController = new VSyncController(vsyncGenerator, 0);
    sptr<VSyncController> appController = new VSyncController(vsyncGenerator, 0);
    std::shared_ptr<HgmVSyncGeneratorController> controller =
        std::make_shared<HgmVSyncGeneratorController>(rsController, appController, vsyncGenerator);
}
class HgmVSyncGeneratorControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HgmVSyncGeneratorControllerTest::SetUpTestCase() {}
void HgmVSyncGeneratorControllerTest::TearDownTestCase() {}
void HgmVSyncGeneratorControllerTest::SetUp() {}
void HgmVSyncGeneratorControllerTest::TearDown() {}

/*
 * @tc.name: ChangeGeneratorRate
 * @tc.desc: Test ChangeGeneratorRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmVSyncGeneratorControllerTest, ChangeGeneratorRate, TestSize.Level1)
{
    ASSERT_NE(controller, nullptr);
    std::vector<std::pair<FrameRateLinkerId, uint32_t>> appChangeData = {{1, 30}, {2, 60}, {3, 120}};
    uint32_t controllerRate1 = 30;
    controller->ChangeGeneratorRate(controllerRate1, appChangeData);
    uint32_t controllerRate2 = 60;
    controller->ChangeGeneratorRate(controllerRate2, appChangeData);
    uint32_t controllerRate3 = 90;
    controller->ChangeGeneratorRate(controllerRate3, appChangeData);
    uint32_t controllerRate4 = 120;
    controller->ChangeGeneratorRate(controllerRate4, appChangeData);
}

/*
 * @tc.name: GetCurrentOffset
 * @tc.desc: Test ChangeGeneratorRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmVSyncGeneratorControllerTest, GetCurrentOffset, TestSize.Level1)
{
    ASSERT_NE(controller, nullptr);
    std::vector<std::pair<FrameRateLinkerId, uint32_t>> appChangeData = {{1, 30}, {2, 60}, {3, 120}};
    controller->ChangeGeneratorRate(30, appChangeData);
    EXPECT_EQ(controller->GetCurrentOffset(), 0);
}

/*
 * @tc.name: GetCurrentRate
 * @tc.desc: Test GetCurrentRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmVSyncGeneratorControllerTest, GetCurrentRate, TestSize.Level1)
{
    ASSERT_NE(controller, nullptr);
    std::vector<std::pair<FrameRateLinkerId, uint32_t>> appChangeData = {{1, 30}, {2, 60}, {3, 120}};
    controller->ChangeGeneratorRate(60, appChangeData);
    EXPECT_EQ(controller->GetCurrentRate(), 60);
}
} // namespace Rosen
} // namespace OHOS
