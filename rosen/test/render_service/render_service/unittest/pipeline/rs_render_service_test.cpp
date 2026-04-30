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

#include "gtest/gtest.h"
#include "limit_number.h"
#include "parameters.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "render_server/rs_render_service.h"
#include "screen_manager/rs_screen_manager.h"
#include "gfx/dump/rs_dump_manager.h"
#include "want.h"

using namespace testing;
using namespace testing::ext;


namespace OHOS::Rosen {
class RSRenderServiceUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceUnitTest::SetUpTestCase() {}
void RSRenderServiceUnitTest::TearDownTestCase() {}
void RSRenderServiceUnitTest::SetUp() {}
void RSRenderServiceUnitTest::TearDown() {}

sptr<RSRenderService> GetAndInitRenderService()
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    renderService->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    return renderService;
}

std::string GetDumpResult(sptr<RSRenderService> renderService, std::u16string arg)
{
    std::string dumpString = "";
    std::unordered_set<std::u16string> argSet = { arg };
    return dumpString;
}

/**
 * @tc.name: DoDump003
 * @tc.desc: test DoDump, with different single arg.
 * @tc.type: FUNC
 * @tc.require: issueIAJCOS
 */
HWTEST_F(RSRenderServiceUnitTest, DoDump003, TestSize.Level1)
{
    auto renderService = GetAndInitRenderService();
    ASSERT_NE(renderService, nullptr);
    ASSERT_NE(renderService->screenManager_, nullptr);
}

/**
 * @tc.name: GameFrameHandlerTest
 * @tc.desc: test GameFrameHandler.
 * @tc.type: FUNC
 * @tc.require: issue21752
 */
HWTEST_F(RSRenderServiceUnitTest, GameFrameHandler, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    ASSERT_NE(renderService, nullptr);
    ASSERT_EQ(renderService->GetGameFrameHandler(), nullptr);
    renderService->FeatureComponentInit();
    ASSERT_NE(renderService->GetGameFrameHandler(), nullptr);
}
} // namespace OHOS::Rosen
