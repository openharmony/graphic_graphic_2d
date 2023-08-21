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

#include "message_parcel.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class FileUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FileUtilsTest::SetUpTestCase() {}
void FileUtilsTest::TearDownTestCase() {}
void FileUtilsTest::SetUp() {}
void FileUtilsTest::TearDown() {}

/**
 * @tc.name: WriteToFileNoSuchFile
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(FileUtilsTest, WriteToFileNoSuchFile, TestSize.Level1)
{
    std::string testFile = "/NoSuchFile";
    MessageParcel messageParcel;
    messageParcel.WriteInt32(0);
    bool ret = WriteToFile(messageParcel.GetData(), messageParcel.GetDataSize(), testFile);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: WriteStringToFileNoSuchFile
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(FileUtilsTest, WriteStringToFileNoSuchFile, TestSize.Level1)
{
    std::string testFile = "/NoSuchFile";
    std::string line = "line1";
    bool ret = WriteToFile(line, testFile);
    EXPECT_EQ(ret, false);
}


/**
 * @tc.name: CreateFileTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(FileUtilsTest, CreateFileTest001, TestSize.Level1)
{
    std::string testFile1 = "";
    bool res = CreateFile(testFile1);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: CreateFileTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(FileUtilsTest, CreateFileTest002, TestSize.Level1)
{
    std::string testFile1 = "/test";
    bool res = CreateFile(testFile1);
    EXPECT_TRUE(res);
}
}