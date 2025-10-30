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

#include "feature/hyper_graphic_manager/rp_hgm_xml_parser.h"
#include "hgm_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr const char* HGM_POLICY_CONFIG = "/sys_prod/etc/graphic/hgm_policy_config.xml";
constexpr const char* INVALID_CONFIG = "/sys_prod/etc/graphic/invalid_config.xml";
}
class RPHgmXMLParser : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RPHgmXMLParser::SetUpTestCase() {}
void RPHgmXMLParser::TearDownTestCase() {}
void RPHgmXMLParser::SetUp() {}
void RPHgmXMLParser::TearDown() {}

static xmlDocPtr StringToXmlDoc(const std::string& xmlContent)
{
    xmlDocPtr docPtr = xmlReadMemory(xmlContent.c_str(), xmlContent.size(), nullptr, nullptr, 0);
    return docPtr;
}

/**
 * @tc.name: TestLoadConfiguration
 * @tc.desc: test RPHgmXMLParser.LoadConfiguration
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RPHgmXmlParserTest, TestLoadConfiguration, TestSize.Level1)
{
    auto parser = std::make_unique<RPHgmXMLParser>();
    parser->LoadConfiguration(HGM_POLICY_CONFIG);
    EXPECT_EQ(parser->LoadConfiguration(INVALID_CONFIG), XML_FILE_LOAD_FAIL);
}

/**
 * @tc.name: TestParse
 * @tc.desc: test RPHgmXMLParser.Parse
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RPHgmXmlParserTest, TestParse, TestSize.Level1)
{
    std::string xmlContent = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
                                <HgmConfig version="1.0" xmlns:xi="http://www.w3.org/2001/XInclude">
                                    <Param name="default_mode" value="-1"/>
                                    <Params name="additional_touch_rate_config">
                                        <Test name="AAAA" value="1"/>
                                        <Test name="BBBB" value="2"/>
                                        XXXX
                                    </Params>
                                    <Params name="source_tuning_for_yuv420">
                                        <Test name="AAAA" value="1"/>
                                        <Test name="BBBB" value="2"/>
                                        XXXX
                                    </Params>
                                    <Params name="rs_solid_color_layer_config">
                                        <Test name="AAAA" value="1"/>
                                        <Test name="BBBB"/>
                                    </Params>
                                </HgmConfig>)";

    std::string noChildNodeXmlContent = (R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>)");

    std::string noChildNodeXmlContent2 = (R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
                                <HgmConfig version="1.0" xmlns:xi="http://www.w3.org/2001/XInclude">
                                    <Params name="additional_touch_rate_config">
                                    </Params>         
                                    <Params name="source_tuning_for_yuv420">
                                    </Params>                           
                                </HgmConfig>)");

    auto parser = std::make_unique<RPHgmXMLParser>();
    EXPECT_EQ(parser->Parse(), XML_GET_ROOT_FAIL);
    parser->xmlDocument_ = StringToXmlDoc(noChildNodeXmlContent);
    EXPECT_EQ(parser->Parse(), XML_GET_ROOT_FAIL);

    xmlFreeDoc(parser->xmlDocument_);
    parser->xmlDocument_ = StringToXmlDoc(noChildNodeXmlContent2);
    EXPECT_EQ(parser->Parse(), EXEC_SUCCESS);

    xmlFreeDoc(parser->xmlDocument_);
    parser->xmlDocument_ = StringToXmlDoc(xmlContent);
    EXPECT_EQ(parser->Parse(), EXEC_SUCCESS);
    EXPECT_EQ(parser->appBufferList_.size(), 2);
    EXPECT_EQ(parser->sourceTuningConfig_.size(), 2);
    EXPECT_EQ(parser->solidLayerConfig_.size(), 2);
    EXPECT_EQ(parser->solidLayerConfig_["BBBB"], "");
}

} // namespace OHOS::Rosen
