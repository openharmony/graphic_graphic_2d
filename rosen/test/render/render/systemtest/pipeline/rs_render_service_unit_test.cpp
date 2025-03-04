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
#include "pipeline/main_thread/rs_render_service.h"

using namespace testing;
using namespace testing::ext;


namespace OHOS::Rosen {
class RsRenderServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    
    const std::string defaultCmd_ = "hidumper -s 10";
};

void RsRenderServiceTest::SetUpTestCase() {}
void RsRenderServiceTest::TearDownTestCase() {}
void RsRenderServiceTest::SetUp() {}
void RsRenderServiceTest::TearDown() {}

/**
 * @tc.name: TestRenderService01
 * @tc.desc: DumpNodesNotOnTheTree test.
 * @tc.type: FUNC
 * @tc.require: issueI5X0T1
 */
HWTEST_F(RsRenderServiceTest, TestRenderService01, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a allSurfacesMem";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService02
 * @tc.desc: DumpAllNodesMemSize test.
 * @tc.type: FUNC
 * @tc.require: issueI5X0T1
 */
HWTEST_F(RsRenderServiceTest, TestRenderService02, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a nodeNotOnTree";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService03
 * @tc.desc: DumpRSEvenParam test.
 * @tc.type: FUNC
 * @tc.require: issueI5X0T1
 */
HWTEST_F(RsRenderServiceTest, TestRenderService03, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a EventParamList";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService04
 * @tc.desc: DumpRenderServiceTree test.
 * @tc.type: FUNC
 * @tc.require: issueI5X0T1
 */
HWTEST_F(RsRenderServiceTest, TestRenderService04, TestSize.Level1)
{
    const std::string rsCmd1 = defaultCmd_;
    int ret = system(rsCmd1.c_str());
    ASSERT_EQ(ret, 0);
    const std::string rsCmd2 = defaultCmd_ + " -a h";
    ret = system(rsCmd2.c_str());
    ASSERT_EQ(ret, 0);
    const std::string rsCmd3 = defaultCmd_ + " ";
    ret = system(rsCmd3.c_str());
    ASSERT_EQ(ret, 0);
    const std::string rsCmd4 = defaultCmd_ + " -h";
    ret = system(rsCmd4.c_str());
    ASSERT_EQ(ret, 0);
}


/**
 * @tc.name: TestRenderService05
 * @tc.desc: DumpHelpInfo test.
 * @tc.type: FUNC
 * @tc.require: issueI5ZCV1
 */
HWTEST_F(RsRenderServiceTest, TestRenderService05, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a RSTree";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService06
 * @tc.desc: Surface test.
 * @tc.type: FUNC
 * @tc.require: issueI5ZCV1
 */
HWTEST_F(RsRenderServiceTest, TestRenderService06, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a surface";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService07
 * @tc.desc: Screen test.
 * @tc.type: FUNC
 * @tc.require: issueI5ZCV1
 */
HWTEST_F(RsRenderServiceTest, TestRenderService07, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a allInfo";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService08
 * @tc.desc: allInfo test.
 * @tc.type: FUNC
 * @tc.require: issueI5ZCV1
 */
HWTEST_F(RsRenderServiceTest, TestRenderService08, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a screen";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService09
 * @tc.desc: FPS test.
 * @tc.type: FUNC
 * @tc.require: issueI5ZCV1
 */
HWTEST_F(RsRenderServiceTest, TestRenderService09, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a 'composer fps'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService10
 * @tc.desc: surfacenode test.
 * @tc.type: FUNC
 * @tc.require: issueI78T31
 */
HWTEST_F(RsRenderServiceTest, TestRenderService10, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a 'surfacenode 0'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);

    const std::string rsCmd1 = defaultCmd_ + " -a 'surfacenode'";
    ret = system(rsCmd1.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService11
 * @tc.desc: DisplayNode fps test.
 * @tc.type: FUNC
 * @tc.require: issueI78T31
 */
HWTEST_F(RsRenderServiceTest, TestRenderService11, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a 'DisplayNode fpsClear'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService12
 * @tc.desc: DisplayNode fpsClear test.
 * @tc.type: FUNC
 * @tc.require: issueI78T31
 */
HWTEST_F(RsRenderServiceTest, TestRenderService12, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a 'DisplayNode fps'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService13
 * @tc.desc: dumpMem test.
 * @tc.type: FUNC
 * @tc.require: issueI78T31
 */
HWTEST_F(RsRenderServiceTest, TestRenderService13, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a 'dumpMem'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService14
 * @tc.desc: trimMem test.
 * @tc.type: FUNC
 * @tc.require: issueI78T31
 */
HWTEST_F(RsRenderServiceTest, TestRenderService14, TestSize.Level1)
{
    std::string rsCmd = defaultCmd_ + " -a 'trimMem cpu'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
    rsCmd = defaultCmd_ + " -a 'trimMem gpu'";
    ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
    rsCmd = defaultCmd_ + " -a 'trimMem shader'";
    ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
    rsCmd = defaultCmd_ + " -a 'trimMem uihidden'";
    ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}
} // namespace OHOS::Rosen
