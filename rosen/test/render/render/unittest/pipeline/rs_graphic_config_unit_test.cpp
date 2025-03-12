/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "pipeline/main_thread/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using ConfigItem = RSGraphicConfig::ConfigItem;
const std::string XML_STR = R"(<?xml version='1.0' encoding="utf-8"?>
<Configs>
    <testMap>
        <testBooL enable="true"></testBooL>
    </testMap>
    <testString>testString</testString>
    <testStrings>testStrings1 testStrings2 testStrings3</testStrings>
    <testInts>1 2 3</testInts>
    <testFloats>0.1 0.2 -0.3</testFloats>
    <testPositiveFloats>0.1 0.2 0.3</testPositiveFloats>
    <testInvalidPositiveFloats>0.1 0.2 -0.3</testInvalidPositiveFloats>
    <testUndifined></testUndifined>
</Configs>
)";

const std::string XML_INVALID_INPUT_STR = R"(<?xml version='1.0' encoding="utf-8"?>
<Configs>
    <testMap>
        <testBooL></testBooL>
    </testMap>
    <testString></testString>
    <testStrings></testStrings>
    <testInts>a b c</testInts>
    <testFloats>a b c</testFloats>
    <testPositiveFloats>a b c</testPositiveFloats>
</Configs>
)";

class RsGraphicConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    ConfigItem ReadConfig(const std::string& xmlStr);
};

void RsGraphicConfigTest::SetUpTestCase() {}

void RsGraphicConfigTest::TearDownTestCase() {}

void RsGraphicConfigTest::SetUp() {}

void RsGraphicConfigTest::TearDown() {}

void SetConfigMap(std::map<std::string, RSGraphicConfig::ValueType>* nonConstConfigMap)
{
    *nonConstConfigMap = {
        { "testMap",                        RSGraphicConfig::ValueType::MAP },
        { "testBooL",                       RSGraphicConfig::ValueType::UNDIFINED },
        { "testString",                     RSGraphicConfig::ValueType::STRING },
        { "testStrings",                    RSGraphicConfig::ValueType::STRINGS },
        { "testInts",                       RSGraphicConfig::ValueType::INTS },
        { "testFloats",                     RSGraphicConfig::ValueType::FLOATS },
        { "testPositiveFloats",             RSGraphicConfig::ValueType::POSITIVE_FLOATS },
        { "testInvalidPositiveFloats",      RSGraphicConfig::ValueType::POSITIVE_FLOATS },
        { "testUndifined",                  RSGraphicConfig::ValueType::UNDIFINED },
    };
}

void ResetConfigMap(std::map<std::string, RSGraphicConfig::ValueType>* nonConstConfigMap)
{
    *nonConstConfigMap = {
        { "blurEffect",                     RSGraphicConfig::ValueType::MAP },
        { "blurSwitchOpen",                 RSGraphicConfig::ValueType::UNDIFINED },
    };
}

ConfigItem RsGraphicConfigTest::ReadConfig(const std::string& xmlStr)
{
    ConfigItem config;
    xmlDocPtr docPtr = xmlParseMemory(xmlStr.c_str(), xmlStr.length() + 1);
    if (docPtr == nullptr) {
        return config;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        xmlFreeDoc(docPtr);
        return config;
    }

    std::map<std::string, ConfigItem> configMap;
    config.SetValue(configMap);
    RSGraphicConfig::ReadConfig(rootPtr, *config.mapValue);
    xmlFreeDoc(docPtr);
    return config;
}

namespace {
/* *
 * @tc.name: LoadConfigXml
 * @tc.desc: test LoadConfigXml.
 * @tc.type: FUNC
 */
HWTEST_F(RsGraphicConfigTest, LoadConfigXml, TestSize.Level2)
{
    ASSERT_TRUE(RSGraphicConfig::LoadConfigXml());
}

/* *
 * @tc.name: ReadConfig
 * @tc.desc: test ReadConfig.
 * @tc.type: FUNC
 */
HWTEST_F(RsGraphicConfigTest, ReadConfig, TestSize.Level2)
{
    auto nonConstConfigMap =
        const_cast<std::map<std::string, RSGraphicConfig::ValueType>*>(&RSGraphicConfig::configItemTypeMap_);
    SetConfigMap(nonConstConfigMap);
    RSGraphicConfig::config_ = ReadConfig(XML_STR);
    RSGraphicConfig::ConfigItem item = RSGraphicConfig::config_["testMap"];
    ASSERT_TRUE(item.IsMap());

    item = RSGraphicConfig::config_["testMap"]["testBooL"].GetProp("enable");
    ASSERT_TRUE(item.boolValue);
    ASSERT_TRUE(item.IsBool());

    item = RSGraphicConfig::config_["testString"];
    ASSERT_EQ("testString", item.stringValue);
    ASSERT_TRUE(item.IsString());

    item = RSGraphicConfig::config_["testStrings"];
    ASSERT_TRUE(item.IsStrings());
    ASSERT_TRUE(item.stringsValue->size() == 3);
    auto stringValues = *item.stringsValue;
    ASSERT_EQ("testStrings1", stringValues[0]);
    ASSERT_EQ("testStrings2", stringValues[1]);
    ASSERT_EQ("testStrings3", stringValues[2]);

    item = RSGraphicConfig::config_["testFloats"];
    ASSERT_TRUE(item.IsFloats());
    ASSERT_TRUE(item.floatsValue->size() == 3);
    auto floatValues = *item.floatsValue;
    ASSERT_TRUE(std::abs(0.1 - floatValues[0]) < std::numeric_limits<float>::epsilon());
    ASSERT_TRUE(std::abs(0.2 - floatValues[1]) < std::numeric_limits<float>::epsilon());
    ASSERT_TRUE(std::abs(-0.3 - floatValues[2]) < std::numeric_limits<float>::epsilon());

    item = RSGraphicConfig::config_["testInts"];
    ASSERT_TRUE(item.IsInts());
    ASSERT_TRUE(item.intsValue->size() == 3);
    auto intValues = *item.intsValue;
    ASSERT_EQ(1, intValues[0]);
    ASSERT_EQ(2, intValues[1]);
    ASSERT_EQ(3, intValues[2]);

    item = RSGraphicConfig::config_["testPositiveFloats"];
    ASSERT_TRUE(item.IsFloats());
    ASSERT_TRUE(item.floatsValue->size() == 3);
    floatValues = *item.floatsValue;
    ASSERT_TRUE(std::abs(0.1 - floatValues[0]) < std::numeric_limits<float>::epsilon());
    ASSERT_TRUE(std::abs(0.2 - floatValues[1]) < std::numeric_limits<float>::epsilon());
    ASSERT_TRUE(std::abs(0.3 - floatValues[2]) < std::numeric_limits<float>::epsilon());

    item = RSGraphicConfig::config_["testInvalidPositiveFloats"];
    ASSERT_TRUE(item.IsFloats());
    ASSERT_TRUE(item.floatsValue->size() == 0);

    item = RSGraphicConfig::config_["testUndifined"];
    ASSERT_TRUE(item.type == RSBaseXmlConfig::ValueType::UNDIFINED);
    ResetConfigMap(nonConstConfigMap);
}

/* *
 * @tc.name: ReadConfig01
 * @tc.desc: test ReadConfig invalid value input.
 * @tc.type: FUNC
 */
HWTEST_F(RsGraphicConfigTest, ReadConfig01, TestSize.Level2)
{
    auto nonConstConfigMap =
        const_cast<std::map<std::string, RSGraphicConfig::ValueType>*>(&RSGraphicConfig::configItemTypeMap_);
    SetConfigMap(nonConstConfigMap);
    RSGraphicConfig::config_ = ReadConfig(XML_INVALID_INPUT_STR);
    RSGraphicConfig::ConfigItem item = RSGraphicConfig::config_["testMap"];
    ASSERT_TRUE(item.IsMap());

    item = RSGraphicConfig::config_["testMap"]["testBooL"].GetProp("enable");
    ASSERT_FALSE(item.IsBool());

    item = RSGraphicConfig::config_["testString"];
    ASSERT_TRUE(item.IsString());
    ASSERT_EQ("", item.stringValue);

    item = RSGraphicConfig::config_["testStrings"];
    ASSERT_TRUE(item.IsStrings());
    ASSERT_TRUE(item.stringsValue->size() == 0);

    item = RSGraphicConfig::config_["testInts"];
    ASSERT_TRUE(item.IsInts());
    ASSERT_TRUE(item.intsValue->size() == 0);

    item = RSGraphicConfig::config_["testFloats"];
    ASSERT_TRUE(item.IsFloats());
    ASSERT_TRUE(item.floatsValue->size() == 0);

    item = RSGraphicConfig::config_["testPositiveFloats"];
    ASSERT_TRUE(item.IsFloats());
    ASSERT_TRUE(item.floatsValue->size() == 0);

    ResetConfigMap(nonConstConfigMap);
}

/* *
 * @tc.name: DumpConfig01
 * @tc.desc: test DumpConfig.
 * @tc.type: FUNC
 */
HWTEST_F(RsGraphicConfigTest, DumpConfig01, TestSize.Level2)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<blurEffect>"
        "<blurSwitchOpen enable=\"true\"></blurSwitchOpen>"
        "</blurEffect>"
        "</Configs>";
    RSGraphicConfig::config_ = ReadConfig(xmlStr);
    RSGraphicConfig::DumpConfig(*RSGraphicConfig::GetConfig().mapValue);
}

/* *
 * @tc.name: ReadProperty01
 * @tc.desc: test ReadProperty name and enable.
 * @tc.type: FUNC
 */
HWTEST_F(RsGraphicConfigTest, ReadProperty01, TestSize.Level2)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<blurEffect>"
        "<blurSwitchOpen name=\"testName\" enable=\"true\"></blurSwitchOpen>"
        "</blurEffect>"
        "</Configs>";
    RSGraphicConfig::config_ = ReadConfig(xmlStr);
}

/* *
 * @tc.name: BlurEffectConfig01
 * @tc.desc: set blurEffect.blurSwitchOpen true.
 * @tc.type: FUNC
 */
HWTEST_F(RsGraphicConfigTest, BlurEffectConfig01, TestSize.Level2)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<blurEffect>"
        "<blurSwitchOpen enable=\"true\"></blurSwitchOpen>"
        "</blurEffect>"
        "</Configs>";
    RSGraphicConfig::config_ = ReadConfig(xmlStr);
    RSMainThread::Instance()->ConfigureRenderService();
    ASSERT_TRUE(RSMainThread::Instance()->IsBlurSwitchOpen());
}

/* *
 * @tc.name: BlurEffectConfig02
 * @tc.desc: set blurEffect.blurSwitchOpen false.
 * @tc.type: FUNC
 */
HWTEST_F(RsGraphicConfigTest, BlurEffectConfig02, TestSize.Level2)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<blurEffect>"
        "<blurSwitchOpen enable=\"false\"></blurSwitchOpen>"
        "</blurEffect>"
        "</Configs>";
    RSGraphicConfig::config_ = ReadConfig(xmlStr);
    RSMainThread::Instance()->ConfigureRenderService();
    ASSERT_FALSE(RSMainThread::Instance()->IsBlurSwitchOpen());
}
} // namespace
} // namespace Rosen
} // namespace OHOS
