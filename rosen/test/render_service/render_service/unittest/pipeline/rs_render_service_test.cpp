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

#include "feature/param_manager/rs_param_manager.h"
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
    sptr<RSRenderService> renderService(new RSRenderService());
    if (renderService) {
        renderService->mainThread_ = RSMainThread::Instance();
        renderService->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    }
    if (renderService->mainThread_) {
        renderService->mainThread_->handler_ =
            std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::Create(true));
    }
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
    ASSERT_NE(renderService->mainThread_, nullptr);
    ASSERT_NE(renderService->screenManager_, nullptr);
}

/**
 * @tc.name: RSParamManager001
 * @tc.desc: test RSParamManager
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceUnitTest, RSParamManager001, TestSize.Level1)
{
    RSParamManager& paramManager = RSParamManager::GetInstance();
    paramManager.SubscribeEvent();
    ASSERT_NE(paramManager.handleEventFunc_.size(), 0);
    paramManager.UnSubscribeEvent();
}

/**
 * @tc.name: RSParamManager003
 * @tc.desc: test RSParamManager
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceUnitTest, RSParamManager003, TestSize.Level1)
{
    RSParamManager& paramManager = RSParamManager::GetInstance();
    paramManager.SubscribeEvent();
    OHOS::AAFwk::Want want;
    want.SetAction("unexpected_action");
    paramManager.OnReceiveEvent(want);
    paramManager.HandleParamUpdate(want);
    int restartParameter = std::atoi(system::GetParameter("debug.graphic.cloudpushrestart", "0").c_str());
    int cloudParameter = std::atoi(system::GetParameter("persist.rosen.disableddgr.enabled", "0").c_str());
    ASSERT_EQ(restartParameter, 0);
    ASSERT_EQ(cloudParameter, 0);
    paramManager.UnSubscribeEvent();
}

/**
 * @tc.name: RSParamManager004
 * @tc.desc: test RSParamManager
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceUnitTest, RSParamManager004, TestSize.Level1)
{
    RSParamManager& paramManager = RSParamManager::GetInstance();
    bool isCloud = paramManager.IsCloudDisableDDGR();
    ASSERT_FALSE(isCloud);
    paramManager.GetHigherVersionPath();
    std::string filePath = "";
    paramManager.GetVersionNums(filePath);
    std::string versionStr = "version=5.0.0.0";
    std::vector<std::string> versionStrSplit = paramManager.SplitString(versionStr, '=');
    ASSERT_EQ(versionStrSplit.size(), 2);
    std::vector<std::string> localVersion = {"4", "0", "0", "0"};
    std::vector<std::string> cloudVersion = {"5", "0", "0", "0"};
    bool versionCompare = paramManager.CompareVersion(localVersion, cloudVersion);
    ASSERT_TRUE(versionCompare);
}

/**
 * @tc.name  : GRSParamManager005
 * @tc.desc  : test RSParamManager
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceUnitTest, RSParamManager005, TestSize.Level1)
{
    RSParamManager& paramManager = RSParamManager::GetInstance();
    std::vector<std::string> localVersion1 = {"5", "0", "0", "0"};
    std::vector<std::string> cloudVersion1 = {"4", "0", "0", "0"};
    bool versionCompare1 = paramManager.CompareVersion(localVersion1, cloudVersion1);
    ASSERT_FALSE(versionCompare1);
    std::vector<std::string> localVersion2 = {"5", "0", "0", "0"};
    std::vector<std::string> cloudVersion2 = {"5", "a", "0", "0"};
    bool versionCompare2 = paramManager.CompareVersion(localVersion2, cloudVersion2);
    ASSERT_FALSE(versionCompare2);
}

/**
 * @tc.name  : RemoveConnectionTest001
 * @tc.desc  : test RemoveConnection
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceUnitTest, RemoveConnectionTest001, TestSize.Level1)
{
    sptr<RSRenderService> renderService(new RSRenderService());
    ASSERT_NE(renderService, nullptr);
    bool isConnectionRemoved = renderService->RemoveConnection(nullptr);
    EXPECT_FALSE(isConnectionRemoved);

    auto token = new IRemoteStub<RSIConnectionToken>();
    auto tokenObj = token->AsObject();
    ASSERT_NE(tokenObj, nullptr);
    isConnectionRemoved = renderService->RemoveConnection(token);
    EXPECT_FALSE(isConnectionRemoved);
}
} // namespace OHOS::Rosen
