/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drm_param_parse.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class DrmParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DrmParamParseTest::SetUpTestCase() {}
void DrmParamParseTest::TearDownTestCase() {}
void DrmParamParseTest::SetUp() {}
void DrmParamParseTest::TearDown() {}

/**
 * @tc.name: ParseFeatureParam
 * @tc.desc: Verify the ParseFeatureParam function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DrmParamParseTest, ParseFeatureParamTest001, Function | SmallTest | Level1)
{
    FeatureParamMapType paramMapType;
    DRMParamParse paramParse;
    xmlNode node;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    EXPECT_EQ(res, PARSE_GET_CHILD_FAIL);

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlNode nextNode;
    std::string name = "FeatureSwitch";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("DrmEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("DrmMarkAllParentBlurEnabled"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("false"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}

/**
 * @tc.name: ParseMultiParam
 * @tc.desc: Verify the ParseFeatureParam function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(DrmParamParseTest, ParseFeatureParamTest002, Function | SmallTest | Level1)
{
    FeatureParamMapType paramMapType;
    DRMParamParse paramParse;
    xmlNode node;
    auto res = paramParse.ParseFeatureParam(paramMapType, node);
    EXPECT_EQ(res, PARSE_GET_CHILD_FAIL);

    xmlNode childNode;
    childNode.type = xmlElementType::XML_ATTRIBUTE_NODE;
    node.xmlChildrenNode = &childNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlNode nextNode;
    std::string name = "FeatureMultiParam";
    nextNode.name = reinterpret_cast<const xmlChar*>(name.c_str());
    node.xmlChildrenNode->next = &nextNode;
    res = paramParse.ParseFeatureParam(paramMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("SCBVolumePanel"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("0"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);

    xmlSetProp(&nextNode, (const xmlChar*)("name"), (const xmlChar*)("SCBCloneWallpaper"));
    xmlSetProp(&nextNode, (const xmlChar*)("value"), (const xmlChar*)("1"));
    res = paramParse.ParseFeatureParam(paramMapType, node);
    EXPECT_EQ(res, ParseErrCode::PARSE_EXEC_SUCCESS);
}
} // namespace Rosen
} // namespace OHOS