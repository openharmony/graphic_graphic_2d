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
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
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
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
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
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
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
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}


/**
 * @tc.name: TestRenderService005
 * @tc.desc: DumpHelpInfo test.
 * @tc.type: FUNC
 * @tc.require: issueI5ZCVU
 */
HWTEST_F(RSRenderServiceTest, TestRenderService005, TestSize.Level1)
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
 * @tc.name: TestRenderService006
 * @tc.desc: Surface test.
 * @tc.type: FUNC
 * @tc.require: issueI5ZCVU
 */
HWTEST_F(RSRenderServiceTest, TestRenderService006, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a surface";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService007
 * @tc.desc: Screen test.
 * @tc.type: FUNC
 * @tc.require: issueI5ZCVU
 */
HWTEST_F(RSRenderServiceTest, TestRenderService007, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a screen";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService008
 * @tc.desc: allInfo test.
 * @tc.type: FUNC
 * @tc.require: issueI5ZCVU
 */
HWTEST_F(RSRenderServiceTest, TestRenderService008, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a allInfo";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService009
 * @tc.desc: FPS test.
 * @tc.type: FUNC
 * @tc.require: issueI5ZCVU
 */
HWTEST_F(RSRenderServiceTest, TestRenderService009, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a 'composer fps'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService010
 * @tc.desc: surfacenode test.
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSRenderServiceTest, TestRenderService010, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a 'surfacenode 0'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);

    const std::string rsCmd1 = defaultCmd_ + " -a 'surfacenode'";
    ret = system(rsCmd1.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService011
 * @tc.desc: DisplayNode fps test.
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSRenderServiceTest, TestRenderService011, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a 'DisplayNode fps'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService012
 * @tc.desc: DisplayNode fpsClear test.
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSRenderServiceTest, TestRenderService012, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a 'DisplayNode fpsClear'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService013
 * @tc.desc: dumpMem test.
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSRenderServiceTest, TestRenderService013, TestSize.Level1)
{
    const std::string rsCmd = defaultCmd_ + " -a 'dumpMem'";
    int ret = system(rsCmd.c_str());
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: TestRenderService014
 * @tc.desc: trimMem test.
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSRenderServiceTest, TestRenderService014, TestSize.Level1)
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
