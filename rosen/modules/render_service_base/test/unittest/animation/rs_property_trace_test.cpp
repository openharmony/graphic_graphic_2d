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

#include <fstream>

#include "property/rs_property_trace.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSPropertyTraceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyTraceTest::SetUpTestCase()
{
    system("mount -o remount,rw /");
    system("mkdir /etc/rosen/");
}
void RSPropertyTraceTest::TearDownTestCase()
{
    system("rm -rf /etc/rosen");
}
void RSPropertyTraceTest::SetUp() {}
void RSPropertyTraceTest::TearDown() {}

/**
 * @tc.name: RSPropertyTraceTest002
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceTest, RSPropertyTraceTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest002 start";
    const std::string configStr = R"("ID:all;")";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStr.c_str() << std::endl;
    outFile.close();

    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();

    RSProperties properties;
    RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(1000, properties);
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest002 end";
}

/**
 * @tc.name: RSPropertyTraceTest003
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceTest, RSPropertyTraceTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest003 start";
    const std::string configStr = R"("ID:1000;")";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStr.c_str() << std::endl;
    outFile.close();

    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    RSProperties properties;
    RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(1000, properties);
    RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(1001, properties);
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest003 end";
}

/**
 * @tc.name: RSPropertyTraceTest004
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceTest, RSPropertyTraceTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest004 start";
    const std::string configStr = R"("id:1000;")";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStr.c_str() << std::endl;
    outFile.close();

    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest004 end";
}

/**
 * @tc.name: RSPropertyTraceTest005
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceTest, RSPropertyTraceTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest005 start";
    const std::string configStr = R"(":1000;")";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStr.c_str() << std::endl;
    outFile.close();

    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest005 end";
}

/**
 * @tc.name: RSPropertyTraceTest006
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceTest, RSPropertyTraceTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest006 start";
    const std::string configStr = R"(";")";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStr.c_str() << std::endl;
    outFile.close();

    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest006 end";
}

/**
 * @tc.name: RSPropertyTraceTest007
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceTest, RSPropertyTraceTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest007 start";
    const std::string configStr = "ID:all;\nPROPERTY:alpha,corner,shadow;";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStr.c_str() << std::endl;
    outFile.close();
    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();

    RSProperties properties;
    RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(1000, properties);
    GTEST_LOG_(INFO) << "RSPropertyTraceTest RSPropertyTraceTest007 end";
}

} // namespace Rosen
} // namespace OHOS