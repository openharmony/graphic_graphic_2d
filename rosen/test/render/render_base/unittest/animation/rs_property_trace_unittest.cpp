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

#include "gtest/gtest.h"

#include <fstream>

#include "property/rs_property_trace.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSPropertyTraceUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyTraceUnitTest::SetUpTestCase()
{
    system("mount -o remount,rw /");
    system("mkdir /etc/rosen/");
}
void RSPropertyTraceUnitTest::TearDownTestCase()
{
    system("rm -rf /etc/rosen");
}
void RSPropertyTraceUnitTest::SetUp() {}
void RSPropertyTraceUnitTest::TearDown() {}

/**
 * @tc.name: RSPropertyTraceUnitTest002
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceUnitTest, RSPropertyTraceUnitTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceUnitTest RSPropertyTraceUnitTest002 start";
    const std::string configStrUt = R"("ID:all;")";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStrUt.c_str() << std::endl;
    outFile.close();

    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();

    auto rectUt = std::make_shared<RSObjAbsGeometry>();
    EXPECT_TRUE(rectUt != nullptr);
    RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(1000, rectUt);
    GTEST_LOG_(INFO) << "RSPropertyTraceUnitTest RSPropertyTraceUnitTest002 end";
}

/**
 * @tc.name: RSPropertyTraceUnitTest003
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceUnitTest, RSPropertyTraceUnitTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceUnitTest RSPropertyTraceUnitTest003 start";
    const std::string configStrUt = R"("ID:1000;")";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStrUt.c_str() << std::endl;
    outFile.close();

    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    auto rectUt = std::make_shared<RSObjAbsGeometry>();
    EXPECT_TRUE(rectUt != nullptr);
    RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(1000, rectUt);
    RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(1001, rectUt);
    GTEST_LOG_(INFO) << "RSPropertyTraceUnitTest RSPropertyTraceUnitTest003 end";
}

/**
 * @tc.name: RSPropertyTraceUnitTest004
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceUnitTest, RSPropertyTraceUnitTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceUnitTest RSPropertyTraceUnitTest004 start";
    const std::string configStrUt = R"("id:1000;")";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStrUt.c_str() << std::endl;
    outFile.close();

    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    GTEST_LOG_(INFO) << "RSPropertyTraceUnitTest RSPropertyTraceUnitTest004 end";
}

/**
 * @tc.name: RSPropertyTraceUnitTest005
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceUnitTest, RSPropertyTraceUnitTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceUnitTest RSPropertyTraceUnitTest005 start";
    const std::string configStrUt = R"(":1000;")";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStrUt.c_str() << std::endl;
    outFile.close();

    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    GTEST_LOG_(INFO) << "RSPropertyTraceUnitTest RSPropertyTraceUnitTest005 end";
}

/**
 * @tc.name: RSPropertyTraceUnitTest006
 * @tc.desc: Verify the trace
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyTraceUnitTest, RSPropertyTraceUnitTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPropertyTraceUnitTest RSPropertyTraceUnitTest006 start";
    const std::string configStrUt = R"(";")";
    std::ofstream outFile;
    outFile.open("/etc/rosen/property.config");
    outFile << configStrUt.c_str() << std::endl;
    outFile.close();

    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    GTEST_LOG_(INFO) << "RSPropertyTraceUnitTest RSPropertyTraceUnitTest006 end";
}

} // namespace Rosen
} // namespace OHOS