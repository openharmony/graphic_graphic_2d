
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

#include "feature/image_enhance/image_enhance_manager.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImageEnhanceManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RSImageEnhanceManagerTest::SetUpTestCase() {}
void RSImageEnhanceManagerTest::TearDownTestCase() {}
void RSImageEnhanceManagerTest::SetUp() {}
void RSImageEnhanceManagerTest::TearDown() {}

/**
 * @tc.name: CheckPackageInConflgList
 * @tc.desc: Test CheckPackageInConflgList
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSImageEnhanceManagerTest, CheckPackageInConflgListTest, TestSize.Level1)
{
    std::vector<std::string> testPkgs = { "test1", "test2", "test3" };
    ImageEnhanceManager imageEnhanceMgr;
    imageEnhanceMgr.pkgs_ = testPkgs;
    imageEnhanceMgr.CheckPackageInConflgList(testPkgs);
    imageEnhanceMgr.pkgs_ = {};
    imageEnhanceMgr.CheckPackageInConflgList(testPkgs);
    EXPECT_TRUE(imageEnhanceMgr.pkgs_ == testPkgs);
}

/**
 * @tc.name: AnalyzePkgParam
 * @tc.desc: Test AnalyzePkgParam
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSImageEnhanceManagerTest, AnalyzePkgParamTest, TestSize.Level1)
{
    ImageEnhanceManager imageEnhanceMgr;
    std::string testStr = "test";
    EXPECT_TRUE(imageEnhanceMgr.AnalyzePkgParam(testStr) == std::make_tuple("test", pid_t(0), int32_t(-1)));
    testStr = "test:test";
    EXPECT_TRUE(imageEnhanceMgr.AnalyzePkgParam(testStr) == std::make_tuple("test", pid_t(0), int32_t(-1)));
    testStr = "test:1";
    EXPECT_TRUE(imageEnhanceMgr.AnalyzePkgParam(testStr) == std::make_tuple("test", pid_t(1), int32_t(-1)));
    testStr = "test:test:test";
    EXPECT_TRUE(imageEnhanceMgr.AnalyzePkgParam(testStr) == std::make_tuple("test", pid_t(0), int32_t(-1)));
    testStr = "test:1:test";
    EXPECT_TRUE(imageEnhanceMgr.AnalyzePkgParam(testStr) == std::make_tuple("test", pid_t(1), int32_t(-1)));
    testStr = "test:1:2";
    EXPECT_TRUE(imageEnhanceMgr.AnalyzePkgParam(testStr) == std::make_tuple("test", pid_t(1), int32_t(2)));
}

/**
 * @tc.name: CheckImageEnhanceList
 * @tc.desc: Test CheckImageEnhanceList
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSImageEnhanceManagerTest, CheckImageEnhanceListTest, TestSize.Level1)
{
    ImageEnhanceManager imageEnhanceMgr;
    std::string testPkg = "test";
    std::unordered_set<pid_t> imageEnhancePidList = {};
    imageEnhanceMgr.CheckImageEnhanceList(testPkg, 0, imageEnhancePidList);
    imageEnhanceMgr.mImageEnhanceScene_ = { "test1" };
    imageEnhanceMgr.CheckImageEnhanceList(testPkg, 0, imageEnhancePidList);
    imageEnhanceMgr.mImageEnhanceScene_ = { "test" };
    imageEnhanceMgr.CheckImageEnhanceList(testPkg, 0, imageEnhancePidList);
    EXPECT_TRUE(imageEnhancePidList.count(0) == 1);
}

/**
 * @tc.name: IsNumber
 * @tc.desc: Test IsNumber
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSImageEnhanceManagerTest, IsNumberTest, TestSize.Level1)
{
    ImageEnhanceManager imageEnhanceMgr;
    std::string testStr = "";
    EXPECT_FALSE(imageEnhanceMgr.IsNumber(testStr));
    testStr = "123456789";
    EXPECT_FALSE(imageEnhanceMgr.IsNumber(testStr));
    testStr = "1234567";
    EXPECT_TRUE(imageEnhanceMgr.IsNumber(testStr));
}
} // namespace OHOS::Rosen
