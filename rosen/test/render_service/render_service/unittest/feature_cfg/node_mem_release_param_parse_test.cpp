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
 
#include "node_mem_release_param_parse.h"
 
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
 
class NodeMemReleaseParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
 
void NodeMemReleaseParamParseTest::SetUpTestCase() {}
void NodeMemReleaseParamParseTest::TearDownTestCase() {}
void NodeMemReleaseParamParseTest::SetUp() {}
void NodeMemReleaseParamParseTest::TearDown() {}
 
/**
 * @tc.name: ParseFeatureParamTest
 * @tc.desc: Test ParseFeatureParam
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NodeMemReleaseParamParseTest, ParseFeatureParamTest, TestSize.Level1)
{
    FeatureParamMapType featureParam;
    xmlNode node;
    node.xmlChildrenNode = nullptr;
    NodeMemReleaseParamParse nodeMemReleaseParamParse;
    int32_t result = nodeMemReleaseParamParse.ParseFeatureParam(featureParam, node);
    ASSERT_EQ(result, PARSE_GET_CHILD_FAIL);
 
    xmlNode attributeNode;
    attributeNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &attributeNode;
    result = nodeMemReleaseParamParse.ParseFeatureParam(featureParam, node);
    ASSERT_EQ(result, PARSE_EXEC_SUCCESS);
 
    xmlNode childNode;
    childNode.type = xmlElementType::XML_ELEMENT_NODE;
    std::string name = "FeatureSingleParam";
    childNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode = &childNode;
    result = nodeMemReleaseParamParse.ParseFeatureParam(featureParam, node);
    ASSERT_EQ(result, PARSE_INTERNAL_FAIL);
 
    name = "FeatureSwitch";
    childNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)("Other"));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    result = nodeMemReleaseParamParse.ParseFeatureParam(featureParam, node);
    ASSERT_EQ(result, PARSE_EXEC_SUCCESS);
 
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)("NodeOffTreeMemReleaseEnabled"));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    result = nodeMemReleaseParamParse.ParseFeatureParam(featureParam, node);
    ASSERT_EQ(result, PARSE_EXEC_SUCCESS);
 
    xmlSetProp(&childNode, (const xmlChar*)("name"), (const xmlChar*)("CanvasDrawingNodeDMAMemEnabled"));
    xmlSetProp(&childNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    result = nodeMemReleaseParamParse.ParseFeatureParam(featureParam, node);
    ASSERT_EQ(result, PARSE_EXEC_SUCCESS);
}
 
} // namespace Rosen
} // namespace OHOS