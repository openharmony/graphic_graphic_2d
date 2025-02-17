/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "platform/common/rs_log.h"
#include "property/rs_property_trace.h"

using namespace testing;
using namespace testing::Text;

namespace OHOS {
namespace Rosen {
class RSPropertyTraceUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyTraceUnitTest::SetUpTestCase() {}
void RSPropertyTraceUnitTest::TearDownTestCase() {}
void RSPropertyTraceUnitTest::SetUp() {}
void RSPropertyTraceUnitTest::TearDown() {}

/**
 * @tc.name: TestPropertiesDisplayByTrace01
 * @tc.desc: test results of PropertiesDisplayByTrace
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestPropertiesDisplayByTrace01, TestSize.Level1)
{
    auto boundsGeometry = std::make_shared<RSObjAbsGeometry>();
    const NodeId id = 1;
    RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(id, boundsGeometry);
    EXPECT_NE(RSPropertyTrace::GetInstance().IsNeedRefreshConfig(), true);
}

/**
 * @tc.name: TestRefreshNodeTraceInfo01
 * @tc.desc: test results of RefreshNodeTraceInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestRefreshNodeTraceInfo01, TestSize.Level1)
{
    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    EXPECT_NE(RSPropertyTrace::GetInstance().IsNeedRefreshConfig(), true);
}

/**
 * @tc.name: TestTracePropertiesByNodeName01
 * @tc.desc: test results of TracePropertiesByNodeName
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestTracePropertiesByNodeName01, TestSize.Level1)
{
    const NodeId id = 1;
    RSProperties properties;
    std::string nodeName = "";
    RSPropertyTrace::GetInstance().TracePropertiesByNodeName(id, nodeName, properties);
    EXPECT_TRUE(!RSPropertyTrace::GetInstance().IsNeedPropertyTrace(nodeName));

    nodeName = "node";
    std::string info = "NODE_NAME:";
    RSPropertyTrace::GetInstance().DealNodeNameConfig(info);
    RSPropertyTrace::GetInstance().TracePropertiesByNodeName(id, nodeName, properties);
    EXPECT_TRUE(RSPropertyTrace::GetInstance().IsNeedPropertyTrace(nodeName));
}

/**
 * @tc.name: TestInitNodeAndPropertyInfo01
 * @tc.desc: test results of InitNodeAndPropertyInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestInitNodeAndPropertyInfo01, TestSize.Level1)
{
    RSPropertyTrace::GetInstance().InitNodeAndPropertyInfo();
    EXPECT_EQ(RSPropertyTrace::GetInstance().needWriteAllNode_, false);
}

/**
 * @tc.name: TestDealConfigInputInfo01
 * @tc.desc: test results of DealConfigInputInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestDealConfigInputInfo01, TestSize.Level1)
{
    std::string info = "HelloWorld";
    RSPropertyTrace::GetInstance().DealConfigInputInfo(info);

    info = "ID:Hello,World,all";
    RSPropertyTrace::GetInstance().DealConfigInputInfo(info);

    info = "ID:0,1,2";
    RSPropertyTrace::GetInstance().DealConfigInputInfo(info);

    info = "PROPERTY:0,1,2";
    RSPropertyTrace::GetInstance().DealConfigInputInfo(info);
    EXPECT_TRUE(RSPropertyTrace::GetInstance().needWriteAllNode_);
}

/**
 * @tc.name: TestDealNodeNameConfig01
 * @tc.desc: test results of DealNodeNameConfig
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestDealNodeNameConfig01, TestSize.Level1)
{
    std::string info = "NODE_NAME,";
    bool res = RSPropertyTrace::GetInstance().DealNodeNameConfig(info);
    EXPECT_NE(res, true);

    info = "NODE_NAME:";
    res = RSPropertyTrace::GetInstance().DealNodeNameConfig(info);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: TestIsNeedRefreshConfig01
 * @tc.desc: test results of IsNeedRefreshConfig
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestIsNeedRefreshConfig01, TestSize.Level1)
{
    bool res = RSPropertyTrace::GetInstance().IsNeedRefreshConfig();
    EXPECT_NE(res, true);
}

/**
 * @tc.name: TestIsNeedPropertyTrace01
 * @tc.desc: test results of IsNeedPropertyTrace
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestIsNeedPropertyTrace01, TestSize.Level1)
{
    NodeId id = 3;
    std::string info = "ID:0,1,2";
    RSPropertyTrace::GetInstance().DealConfigInputInfo(info);
    bool res = RSPropertyTrace::GetInstance().IsNeedPropertyTrace(id);
    EXPECT_EQ(res, true);

    id = 0;
    res = RSPropertyTrace::GetInstance().IsNeedPropertyTrace(id);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: TestIsNeedPropertyTrace02
 * @tc.desc: test results of IsNeedPropertyTrace
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestIsNeedPropertyTrace02, TestSize.Level1)
{
    std::string nodeName = "";
    bool res = RSPropertyTrace::GetInstance().IsNeedPropertyTrace(nodeName);
    EXPECT_NE(res, true);

    nodeName = "node";
    std::string info = "NODE_NAME:";
    RSPropertyTrace::GetInstance().DealNodeNameConfig(info);
    res = RSPropertyTrace::GetInstance().IsNeedPropertyTrace(nodeName);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: TestAddTraceFlag01
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestAddTraceFlag01, TestSize.Level1)
{
    std::string flag = "flag";
    std::string nodeName = "node";
    std::string info = "NODE_NAME:";
    RSPropertyTrace::GetInstance().AddTraceFlag(flag);
    RSPropertyTrace::GetInstance().DealNodeNameConfig(info);
    bool res = RSPropertyTrace::GetInstance().IsNeedPropertyTrace(nodeName);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: TestClearNodeAndPropertyInfo01
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceUnitTest, TestClearNodeAndPropertyInfo01, TestSize.Level1)
{
    RSPropertyTrace::GetInstance().ClearNodeAndPropertyInfo();
    EXPECT_TRUE(RSPropertyTrace::GetInstance().nodeIdSet_.empty());
}
} // namespace Rosen
} // namespace OHOS