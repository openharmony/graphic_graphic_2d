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

#include <gtest/gtest.h>
#include <test_header.h>
#include "mem_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class MEMParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void MEMParamParseTest::SetUpTestCase() {}
void MEMParamParseTest::TearDownTestCase() {}
void MEMParamParseTest::SetUp() {}
void MEMParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParam001
 * @tc.desc: Test ParseFeatureParam with null node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MEMParamParseTest, ParseFeatureParam001, TestSize.Level1)
{
    MEMParamParse paramParse;
    FeatureParamMapType paramMapType;
    paramMapType[FEATURE_CONFIGS[MEM]] = std::shared_ptr<MEMParam>();
    xmlNode node;
    node.xmlChildrenNode = nullptr;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;

    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("KernelReportEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("true"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    name = "FeatureSingleParam";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("KernelReportAvailableMemLimit"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("10000"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("KernelReportMemInterval"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("200"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    ASSERT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}
} // namespace Rosen
} // namespace OHOS
