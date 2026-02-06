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

#include "feature/hyper_graphic_manager/hgm_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class HgmContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void HgmContextTest::SetUpTestCase() {}
void HgmContextTest::TearDownTestCase() {}
void HgmContextTest::SetUp() {}
void HgmContextTest::TearDown() {}

/**
 * @tc.name: TestInitHgmConfig
 * @tc.desc: test HgmContextTest.InitHgmConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, TestInitHgmConfig, TestSize.Level1)
{
    HgmContext hgmContext;
    std::unordered_map<std::string, std::string> sourceTuningConfig;
    std::unordered_map<std::string, std::string> solidLayerConfig;
    std::vector<std::string> appBufferList;
    hgmContext.InitHgmConfig(sourceTuningConfig, solidLayerConfig, appBufferList);
    EXPECT_EQ(hgmContext.frameRateFunctions_.frameRateGetFunc(static_cast<RSPropertyUnit>(0xff), 0.f, 1000.f, 0.f), 0);
}

/**
 * @tc.name: TestInitHgmUpdateCallback
 * @tc.desc: test HgmContext.InitHgmUpdateCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, TestInitHgmUpdateCallback, TestSize.Level1)
{
    HgmContext hgmContext;
    hgmContext.InitHgmUpdateCallback();
    auto generator = CreateVSyncGenerator();
    sptr<VSyncController> rsVSyncController = new VSyncController(generator, 0);
    sptr<VSyncDistributor> rsVSyncDistributor = new VSyncDistributor(rsVSyncController, "rs");
    hgmContext.ProcessHgmFrameRate(0, rsVSyncDistributor, 1);
    hgmContext.rpHgmConfigDataChange_ = true;
    hgmContext.ProcessHgmFrameRate(1, rsVSyncDistributor, 2);
    EXPECT_EQ(hgmContext.rpHgmConfigDataChange_, false);
    usleep(110000);
}
} // namespace OHOS::Rosen

