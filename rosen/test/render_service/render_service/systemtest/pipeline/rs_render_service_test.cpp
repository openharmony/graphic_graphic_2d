/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "pipeline/rs_render_service.h"

using namespace testing;
using namespace testing::ext;


namespace OHOS::Rosen {
class RSRenderServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    
    const std::string defaultCmd_ = "hidumper -s 10";
};

void RSRenderServiceTest::SetUpTestCase() {}
void RSRenderServiceTest::TearDownTestCase() {}
void RSRenderServiceTest::SetUp() {}
void RSRenderServiceTest::TearDown() {}

/**
 * @tc.name: TestRenderService001
 * @tc.desc: DumpNodesNotOnTheTree test.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceTest, TestRenderService001, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a nodeNotOnTree";
    (void)system(rsCmd.c_str());
}

/**
 * @tc.name: TestRenderService002
 * @tc.desc: DumpAllNodesMemSize test.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceTest, TestRenderService002, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a allSurfacesMem";
    (void)system(rsCmd.c_str());
}

/**
 * @tc.name: TestRenderService003
 * @tc.desc: DumpRSEvenParam test.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceTest, TestRenderService003, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a EventParamList";
    (void)system(rsCmd.c_str());
}

/**
 * @tc.name: TestRenderService004
 * @tc.desc: DumpRenderServiceTree test.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceTest, TestRenderService004, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a RSTree";
    (void)system(rsCmd.c_str());
}
}// namespace OHOS::Rosen
