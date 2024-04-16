/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

void RSPropertyTraceTest::SetUpTestCase() {}
void RSPropertyTraceTest::TearDownTestCase() {}
void RSPropertyTraceTest::SetUp() {}
void RSPropertyTraceTest::TearDown() {}

/**
 * @tc.name: PropertiesDisplayByTrace001
 * @tc.desc: test results of PropertiesDisplayByTrace
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, PropertiesDisplayByTrace001, TestSize.Level1)
{
    RSProperties properties;
    const NodeId id = 1;
    RSPropertyTrace::GetInstance().PropertiesDisplayByTrace(id, properties);
}

/**
 * @tc.name: RefreshNodeTraceInfo001
 * @tc.desc: test results of RefreshNodeTraceInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, RefreshNodeTraceInfo001, TestSize.Level1)
{
    RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
}

/**
 * @tc.name: TracePropertiesByNodeName001
 * @tc.desc: test results of TracePropertiesByNodeName
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, TracePropertiesByNodeName001, TestSize.Level1)
{
    const NodeId id = 1;
    RSProperties properties;
    std::string nodeName = "";
    RSPropertyTrace::GetInstance().TracePropertiesByNodeName(id, nodeName, properties);
    EXPECT_EQ(id, 1);

    nodeName = "node";
    std::string info = "NODE_NAME:";
    RSPropertyTrace::GetInstance().DealNodeNameConfig(info);
    RSPropertyTrace::GetInstance().TracePropertiesByNodeName(id, nodeName, properties);
    EXPECT_EQ(id, 1);
}

/**
 * @tc.name: InitNodeAndPropertyInfo001
 * @tc.desc: test results of InitNodeAndPropertyInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, InitNodeAndPropertyInfo001, TestSize.Level1)
{
    RSPropertyTrace::GetInstance().InitNodeAndPropertyInfo();
    EXPECT_EQ(RSPropertyTrace::GetInstance().needWriteAllNode_, false);
}

/**
 * @tc.name: DealConfigInputInfo001
 * @tc.desc: test results of DealConfigInputInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, DealConfigInputInfo001, TestSize.Level1)
{
    std::string info = "HelloWorld";
    RSPropertyTrace::GetInstance().DealConfigInputInfo(info);

    info = "ID:Hello,World,all";
    RSPropertyTrace::GetInstance().DealConfigInputInfo(info);

    info = "ID:0,1,2";
    RSPropertyTrace::GetInstance().DealConfigInputInfo(info);

    info = "PROPERTY:0,1,2";
    RSPropertyTrace::GetInstance().DealConfigInputInfo(info);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DealNodeNameConfig001
 * @tc.desc: test results of DealNodeNameConfig
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, DealNodeNameConfig001, TestSize.Level1)
{
    std::string info = "NODE_NAME,";
    bool res = RSPropertyTrace::GetInstance().DealNodeNameConfig(info);
    EXPECT_NE(res, true);

    info = "NODE_NAME:";
    res = RSPropertyTrace::GetInstance().DealNodeNameConfig(info);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: IsNeedRefreshConfig001
 * @tc.desc: test results of IsNeedRefreshConfig
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, IsNeedRefreshConfig001, TestSize.Level1)
{
    bool res = RSPropertyTrace::GetInstance().IsNeedRefreshConfig();
    EXPECT_NE(res, true);
}

/**
 * @tc.name: IsNeedPropertyTrace001
 * @tc.desc: test results of IsNeedPropertyTrace
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, IsNeedPropertyTrace001, TestSize.Level1)
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
 * @tc.name: IsNeedPropertyTrace002
 * @tc.desc: test results of IsNeedPropertyTrace
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyTraceTest, IsNeedPropertyTrace002, TestSize.Level1)
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
} // namespace Rosen
} // namespace OHOS