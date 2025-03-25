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

#include <gtest/gtest.h>

#include "util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class UtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void UtilTest::SetUpTestCase() {}
void UtilTest::TearDownTestCase() {}
void UtilTest::SetUp() {}
void UtilTest::TearDown() {}

/**
 * @tc.name: UtilTest_001
 * @tc.desc: Verify the ParseOldConfigFile
 * @tc.type:FUNC
 */
HWTEST_F(UtilTest, UtilTest_001, TestSize.Level1)
{
    std::vector<BootAnimationConfig> configs;

    std::string jsonStr1 = "{}";
    cJSON* jsonData1 = cJSON_Parse(jsonStr1.c_str());
    OHOS::ParseOldConfigFile(jsonData1, configs);

    std::string jsonStr2 = "{\"cust.bootanimation.pics\":1,\"cust.bootanimation.sounds\":1,\
    \"cust.bootanimation.video\":1,\"cust.bootanimation.video.extra\":1,\"cust.bootanimation.rotate.screenid\":1,\
    \"cust.bootanimation.rotate.degree\":1}";
    cJSON* jsonData2 = cJSON_Parse(jsonStr2.c_str());
    OHOS::ParseOldConfigFile(jsonData2, configs);
    EXPECT_EQ(configs.front().rotateDegree, 0);

    std::string jsonStr3 = "{\"cust.bootanimation.pics\":\"abc\",\"cust.bootanimation.sounds\":\"abc\",\
    \"cust.bootanimation.video\":\"abc\",\"cust.bootanimation.video.extra\":\"abc\",\
    \"cust.bootanimation.rotate.screenid\":\"0\", \"cust.bootanimation.rotate.degree\":\"270\"}";
    cJSON* jsonData3 = cJSON_Parse(jsonStr3.c_str());
    OHOS::ParseOldConfigFile(jsonData3, configs);
    EXPECT_EQ(configs.back().rotateDegree, 270);
}

/**
 * @tc.name: UtilTest_002
 * @tc.desc: Verify the ParseNewConfigFile
 * @tc.type:FUNC
 */
HWTEST_F(UtilTest, UtilTest_002, TestSize.Level1)
{
    bool isMultiDisplay = false;
    std::vector<BootAnimationConfig> configs;

    std::string jsonStr1 = "{}";
    cJSON* jsonData1 = cJSON_Parse(jsonStr1.c_str());
    OHOS::ParseNewConfigFile(jsonData1, isMultiDisplay, configs);

    std::string jsonStr2 = "{\"cust.bootanimation.multi_display\":1}";
    cJSON* jsonData2 = cJSON_Parse(jsonStr2.c_str());
    OHOS::ParseNewConfigFile(jsonData2, isMultiDisplay, configs);

    std::string jsonStr3 = "{\"cust.bootanimation.multi_display\":false}";
    cJSON* jsonData3 = cJSON_Parse(jsonStr3.c_str());
    OHOS::ParseNewConfigFile(jsonData3, isMultiDisplay, configs);
    EXPECT_EQ(isMultiDisplay, false);

    std::string jsonStr4 = "{\"cust.bootanimation.multi_display\":true}";
    cJSON* jsonData4 = cJSON_Parse(jsonStr4.c_str());
    OHOS::ParseNewConfigFile(jsonData4, isMultiDisplay, configs);

    std::string jsonStr5 = "{\"screen_config\":[]}";
    cJSON* jsonData5 = cJSON_Parse(jsonStr5.c_str());
    OHOS::ParseNewConfigFile(jsonData5, isMultiDisplay, configs);

    std::string jsonStr6 = "{\"screen_config\":[{}]}";
    cJSON* jsonData6 = cJSON_Parse(jsonStr6.c_str());
    OHOS::ParseNewConfigFile(jsonData6, isMultiDisplay, configs);

    std::string jsonStr7 = "{\"screen_config\":[{\"cust.bootanimation.screen_id\":1,\
    \"cust.bootanimation.pics\":1,\"cust.bootanimation.sounds\":1,\
    \"cust.bootanimation.video_default\":1,\"cust.bootanimation.rotate_degree\":1,\
    \"cust.bootanimation.video_extensions\":1}]}";
    cJSON* jsonData7 = cJSON_Parse(jsonStr7.c_str());
    OHOS::ParseNewConfigFile(jsonData7, isMultiDisplay, configs);

    std::string jsonStr8 = "{\"screen_config\":[{\"cust.bootanimation.screen_id\":\"0\",\
    \"cust.bootanimation.pics\":\"abc\",\"cust.bootanimation.sounds\":\"abc\",\
    \"cust.bootanimation.video_default\":\"abc\",\"cust.bootanimation.rotate_degree\":\"270\",\
    \"cust.bootanimation.video_extensions\":[]}]}";
    cJSON* jsonData8 = cJSON_Parse(jsonStr8.c_str());
    OHOS::ParseNewConfigFile(jsonData8, isMultiDisplay, configs);
    EXPECT_EQ(isMultiDisplay, true);
}

/**
 * @tc.name: UtilTest_003
 * @tc.desc: Verify the ParseVideoExtraPath
 * @tc.type:FUNC
 */
HWTEST_F(UtilTest, UtilTest_003, TestSize.Level1)
{
    BootAnimationConfig config;
    std::string jsonStr1 = "{}";
    cJSON* jsonData1 = cJSON_Parse(jsonStr1.c_str());
    OHOS::ParseVideoExtraPath(jsonData1, config);

    std::string jsonStr2 = "[]";
    cJSON* jsonData2 = cJSON_Parse(jsonStr2.c_str());
    OHOS::ParseVideoExtraPath(jsonData2, config);

    std::string jsonStr3 = "{\"1\":\"abc\"}";
    cJSON* jsonData3 = cJSON_Parse(jsonStr3.c_str());
    OHOS::ParseVideoExtraPath(jsonData3, config);
    EXPECT_EQ(config.videoExtPath.size(), 1);
}

/**
 * @tc.name: UtilTest_004
 * @tc.desc: Verify the ParseBootDuration
 * @tc.type:FUNC
 */
HWTEST_F(UtilTest, UtilTest_004, TestSize.Level1)
{
    BootAnimationConfig config;
    int32_t duration;
    std::string jsonStr1 = "{}";
    cJSON* jsonData1 = cJSON_Parse(jsonStr1.c_str());
    OHOS::ParseBootDuration(jsonData1, duration);

    std::string jsonStr2 = "{\"cust.bootanimation.duration\":10}";
    cJSON* jsonData2 = cJSON_Parse(jsonStr2.c_str());
    OHOS::ParseBootDuration(jsonData2, duration);

    std::string jsonStr3 = "{\"cust.bootanimation.duration\":\"10\"}";
    cJSON* jsonData3 = cJSON_Parse(jsonStr3.c_str());
    OHOS::ParseBootDuration(jsonData3, duration);
    EXPECT_EQ(duration, 10);
}

/**
 * @tc.name: UtilTest_005
 * @tc.desc: Verify the IsFileExisted
 * @tc.type:FUNC
 */
HWTEST_F(UtilTest, UtilTest_005, TestSize.Level1)
{
    std::string filePath = "";
    bool isFileExist = OHOS::IsFileExisted(filePath);
    EXPECT_EQ(false, isFileExist);

    filePath = "/sys_prod/etc/bootanimation/bootanimation_custom_config.json1";
    isFileExist = OHOS::IsFileExisted(filePath);
    EXPECT_EQ(false, isFileExist);
}

/**
 * @tc.name: UtilTest_006
 * @tc.desc: Verify the ParseBootConfig
 * @tc.type:FUNC
 */
HWTEST_F(UtilTest, UtilTest_006, TestSize.Level1)
{
    std::string filePath = "";
    std::vector<BootAnimationConfig> animationConfigs;
    bool isMultiDisplay = false;
    bool isCompatible = false;
    int32_t duration = 60;
    bool parseResult = OHOS::ParseBootConfig(filePath, duration, isCompatible, isMultiDisplay, animationConfigs);
    EXPECT_EQ(false, parseResult);

    filePath = "/sys_prod/etc/bootanimation/bootanimation_custom_config.json1";
    parseResult = OHOS::ParseBootConfig(filePath, duration, isCompatible, isMultiDisplay, animationConfigs);
    EXPECT_EQ(false, parseResult);

    filePath = "/sys_prod/etc/bootanimation/bootanimation_custom_config.json";
    parseResult = OHOS::ParseBootConfig(filePath, duration, isCompatible, isMultiDisplay, animationConfigs);
    bool isFileExist = OHOS::IsFileExisted(filePath);
    EXPECT_EQ(isFileExist ? true : false, parseResult);
}

/**
 * @tc.name: UtilTest_007
 * @tc.desc: Verify the ReadFile
 * @tc.type:FUNC
 */
HWTEST_F(UtilTest, UtilTest_007, TestSize.Level1)
{
    std::string filePath = "";
    std::string content = OHOS::ReadFile(filePath);
    EXPECT_EQ(0, content.length());

    filePath = "/sys_prod/etc/bootanimation/bootanimation_custom_config.json1";
    content = OHOS::ReadFile(filePath);
    EXPECT_EQ(0, content.length());

    filePath = "/sys_prod/etc/bootanimation/bootanimation_custom_config.json";
    content = OHOS::ReadFile(filePath);
    bool isFileExist = OHOS::IsFileExisted(filePath);
    EXPECT_EQ(content.empty(), isFileExist ? false : true);
}

/**
 * @tc.name: UtilTest_008
 * @tc.desc: Verify the GetHingeStatus
 * @tc.type:FUNC
 */
HWTEST_F(UtilTest, UtilTest_008, TestSize.Level1)
{
    std::string HING_STATUS_INFO_PATH = "/sys/class/sensors/hinge_sensor/hinge_status_info";
    std::string content = OHOS::GetHingeStatus();
    if (OHOS::IsFileExisted(HING_STATUS_INFO_PATH)) {
        EXPECT_NE(0, content.length());
    } else {
        EXPECT_EQ(0, content.length());
    }
}
}
