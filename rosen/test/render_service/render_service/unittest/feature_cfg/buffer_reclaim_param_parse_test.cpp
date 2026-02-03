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
#include "buffer_reclaim_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferReclaimParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BufferReclaimParamParseTest::SetUpTestCase() {}
void BufferReclaimParamParseTest::TearDownTestCase() {}
void BufferReclaimParamParseTest::SetUp() {}
void BufferReclaimParamParseTest::TearDown() {}

HWTEST_F(BufferReclaimParamParseTest, ParseFeatureParamTest001, TestSize.Level1)
{
    BufferReclaimParamParse paramParse;
    FeatureParamMapType invalidParamMapType;
    xmlNode node;
    BufferReclaimParam::GetInstance().SetBufferReclaimEnable(false);

    auto res = paramParse.ParseFeatureParam(invalidParamMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), false);

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;
    res = paramParse.ParseFeatureParam(invalidParamMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_INTERNAL_FAIL);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), false);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("BufferReclaimEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseFeatureParam(invalidParamMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_INTERNAL_FAIL);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), false);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("BufferReclaimEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("true"));
    res = paramParse.ParseFeatureParam(invalidParamMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_INTERNAL_FAIL);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), false);
}

HWTEST_F(BufferReclaimParamParseTest, ParseFeatureParamTest002, TestSize.Level1)
{
    BufferReclaimParamParse paramParse;
    FeatureParamMapType featureParam;
    featureParam["BufferReclaimConfig"] = std::make_shared<BufferReclaimParam>();

    xmlNode node;
    BufferReclaimParam::GetInstance().SetBufferReclaimEnable(false);

    auto res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_GET_CHILD_FAIL);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), false);

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    xmlNode nextNode;
    nextNode.type = xmlElementType::XML_ELEMENT_NODE;
    string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;
    res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), false);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("BufferReclaimEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), false);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("BufferReclaimEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("true"));
    res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), true);

    string invalidName = "FeatureSingleParam";
    nextNode.name = reinterpret_cast<const xmlChar*>(invalidName.c_str());
    BufferReclaimParam::GetInstance().SetBufferReclaimEnable(false);
    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("BufferReclaimEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("true"));
    res = paramParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
    EXPECT_EQ(BufferReclaimParam::GetInstance().IsBufferReclaimEnable(), false);
}
}