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

#include "video_metadata_param_parse.h"

using namespace testing;
using namespace testing::ext;

namespace {
const std::string TEST_GRAPHIC_CONFIG_XML = (R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <FeatureModule version = "1.0" xmlns:xi="http://www.w3.org/2001/XInclude" name="VideoMetadataConfig">
        <FeatureMultiParam name = "SdpInfoAppList">
            <App name="com.target.app" value="1"/>
        </FeatureMultiParam>
    </FeatureModule>)");

xmlDoc* StringToXmlDoc(const std::string& xmlContent)
{
    xmlDoc* docPtr = xmlReadMemory(xmlContent.c_str(), xmlContent.size(), nullptr, nullptr, 0);
    return docPtr;
}
}

namespace OHOS {
namespace Rosen {
class VideoMetadataParamParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void VideoMetadataParamParseTest::SetUpTestCase() {}
void VideoMetadataParamParseTest::TearDownTestCase() {}
void VideoMetadataParamParseTest::SetUp() {}
void VideoMetadataParamParseTest::TearDown() {}

/**
 * @tc.name: VideoMetadataParamParse
 * @tc.desc: Verify the VideoMetadataParamParse
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VideoMetadataParamParseTest, ParseFeatureParam, Function | SmallTest | Level1)
{
    FeatureParamMapType featureParam;
    featureParam["VideoMetadataConfig"] = std::make_shared<VideoMetadataParam>();
    VideoMetadataParamParse videoMetadataParamParse;
    xmlNode node;
    node.xmlChildrenNode = nullptr;
    int32_t result = videoMetadataParamParse.ParseFeatureParam(featureParam, node);
    EXPECT_EQ(result, PARSE_GET_CHILD_FAIL);

    xmlDoc* doc = StringToXmlDoc(TEST_GRAPHIC_CONFIG_XML);
    ASSERT_NE(doc, nullptr);
    xmlNode* featureRoot = xmlDocGetRootElement(doc);
    ASSERT_NE(featureRoot, nullptr);
    int32_t result2 = videoMetadataParamParse.ParseFeatureParam(featureParam, *featureRoot);
    EXPECT_EQ(result2, PARSE_EXEC_SUCCESS);

    xmlNode node2;
    node2.xmlChildrenNode = nullptr;
    int32_t result3 = videoMetadataParamParse.ParseVideoMetadataInternal(node2);
    EXPECT_EQ(result3, PARSE_EXEC_SUCCESS);

    xmlNode node3;
    xmlNode* childNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("FeatureMultiParam"));
    if (childNode != nullptr) {
        childNode->type = XML_ELEMENT_NODE;
    }
    node3.xmlChildrenNode = childNode;
    int32_t result4 = videoMetadataParamParse.ParseFeatureParam(featureParam, node3);
    EXPECT_NE(result4, PARSE_EXEC_SUCCESS);

    xmlFreeNode(childNode);
    childNode = nullptr;
    xmlFreeDoc(doc);
    doc = nullptr;
}
} // namespace Rosen
} // namespace OHOS