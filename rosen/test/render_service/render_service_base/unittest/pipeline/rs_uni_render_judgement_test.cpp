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

#include <fstream>
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/rs_uni_render_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniRenderJudgementTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniRenderJudgementTest::SetUpTestCase() {}
void RSUniRenderJudgementTest::TearDownTestCase() {}
void RSUniRenderJudgementTest::SetUp() {}
void RSUniRenderJudgementTest::TearDown() {}

/**
 * @tc.name: TestUniRenderJudgement001
 * @tc.desc: GetUniRenderEnabledType test.
 * @tc.type: FUNC
 * @tc.require: issueI9J766
 */
HWTEST_F(RSUniRenderJudgementTest, TestUniRenderJudgement001, TestSize.Level1)
{
    UniRenderEnabledType uniType = RSUniRenderJudgement::GetUniRenderEnabledType();
    ASSERT_TRUE(uniType == UniRenderEnabledType::UNI_RENDER_DISABLED ||
                uniType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
}

/**
 * @tc.name: TestUniRenderJudgement002
 * @tc.desc: IsUniRender test.
 * @tc.type: FUNC
 * @tc.require: issueI9J766
 */
HWTEST_F(RSUniRenderJudgementTest, TestUniRenderJudgement002, TestSize.Level1)
{
    UniRenderEnabledType uniType = RSUniRenderJudgement::GetUniRenderEnabledType();
    RSUniRenderJudgement::InitUniRenderConfig();
    bool isUni = RSUniRenderJudgement::IsUniRender();
    if (uniType == UniRenderEnabledType::UNI_RENDER_DISABLED) {
        ASSERT_EQ(false, isUni);
    } else {
        ASSERT_EQ(true, isUni);
    }
}

/**
 * @tc.name: TestUniRenderJudgement003
 * @tc.desc: SafeGetLine test.
 * @tc.type: FUNC
 * @tc.require: issueI9J766
 */
HWTEST_F(RSUniRenderJudgementTest, TestUniRenderJudgement003, TestSize.Level1)
{
    const std::string configPath = "/data/";
    const std::string configName = "test.config";
    std::string configFilePath = configPath + configName;
    // check empty config
    std::ifstream configFile = std::ifstream(configFilePath.c_str());
    std::string line;
    (void)RSUniRenderJudgement::SafeGetLine(configFile, line);
    ASSERT_EQ(line, "");
    // add lines to file
    std::string rsCmd = "touch " + configFilePath;
    (void)system(rsCmd.c_str());
    std::ofstream writeFile = std::ofstream(configFilePath.c_str());
    std::string newLine = "add test line";
    writeFile << newLine;
    writeFile.close();
    // check test config
    configFile = std::ifstream(configFilePath.c_str());
    (void)RSUniRenderJudgement::SafeGetLine(configFile, line);
    ASSERT_EQ(line, newLine);
    // safely remove file
    rsCmd = "rm " + configFilePath;
    (void)system(rsCmd.c_str());
}
} // namespace OHOS::Rosen