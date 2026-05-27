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
#include <fstream>

#include "util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    constexpr int32_t TEST_DURATION = 60;
    constexpr int32_t TEST_SIDE_LEN = 720;
    constexpr int32_t TEST_VP = 12;
    constexpr float TEST_RATIO = 360.0f;
    constexpr unsigned long TEST_FILE_SIZE = 1000;
    constexpr int32_t TEST_ROTATE_DEGREE = 270;
}

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

HWTEST_F(UtilTest, UtilTest_001, TestSize.Level1)
{
    std::vector<BootAnimationConfig> configs;

    std::string jsonStr1 = "{}";
    cJSON* jsonData1 = cJSON_Parse(jsonStr1.c_str());
    OHOS::ParseOldConfigFile(jsonData1, configs);

    std::string jsonStr2 = "{\"cust.bootanimation.pics\":1,\"cust.bootanimation.sounds\":1,\
\"cust.bootanimation.video\":1,\"cust.bootanimation.video.extra\":1,\"cust.bootanimation.rotate.screenid\":1,\
\"cust.bootanimation.rotate.degree\":1,\"cust.bootanimation.frame_rate_enabled\":true}";
    cJSON* jsonData2 = cJSON_Parse(jsonStr2.c_str());
    OHOS::ParseOldConfigFile(jsonData2, configs);
    EXPECT_EQ(configs.front().rotateDegree, 0);

    std::string jsonStr3 = "{\"cust.bootanimation.pics\":\"abc\",\"cust.bootanimation.sounds\":\"abc\",\
\"cust.bootanimation.video\":\"abc\",\"cust.bootanimation.video.extra\":\"abc\",\
\"cust.bootanimation.rotate.screenid\":\"0\", \"cust.bootanimation.rotate.degree\":\"270\"}";
    cJSON* jsonData3 = cJSON_Parse(jsonStr3.c_str());
    OHOS::ParseOldConfigFile(jsonData3, configs);
    EXPECT_EQ(configs.back().rotateDegree, TEST_ROTATE_DEGREE);
}

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
\"cust.bootanimation.video_extensions\":[],\"cust.bootanimation.frame_rate_enabled\":true}]}";
    cJSON* jsonData8 = cJSON_Parse(jsonStr8.c_str());
    OHOS::ParseNewConfigFile(jsonData8, isMultiDisplay, configs);
    EXPECT_EQ(isMultiDisplay, true);
}

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

HWTEST_F(UtilTest, UtilTest_005, TestSize.Level1)
{
    std::string filePath = "";
    bool isFileExist = OHOS::IsFileExisted(filePath);
    EXPECT_EQ(false, isFileExist);

    filePath = "/sys_prod/etc/bootanimation/bootanimation_custom_config.json1";
    isFileExist = OHOS::IsFileExisted(filePath);
    EXPECT_EQ(false, isFileExist);
}

HWTEST_F(UtilTest, UtilTest_006, TestSize.Level1)
{
    std::string filePath = "";
    std::vector<BootAnimationConfig> animationConfigs;
    bool isMultiDisplay = false;
    bool isCompatible = false;
    int32_t duration = TEST_DURATION;
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

HWTEST_F(UtilTest, UtilTest_008, TestSize.Level1)
{
    std::string hingStatusInfoPath = "/sys/class/sensors/hinge_sensor/hinge_status_info";
    std::string content = OHOS::GetHingeStatus();
    if (OHOS::IsFileExisted(hingStatusInfoPath)) {
        EXPECT_NE(0, content.length());
    } else {
        EXPECT_EQ(0, content.length());
    }
}

HWTEST_F(UtilTest, UtilTest_009, TestSize.Level1)
{
    const char* fileBuffer;
    int totalsize = 0;
    FrameRateConfig frameConfig;
    bool result = OHOS::ParseImageConfig(fileBuffer, totalsize, frameConfig);
    EXPECT_FALSE(result);
}

HWTEST_F(UtilTest, UtilTest_010, TestSize.Level1)
{
    std::string filename = "abc";
    std::shared_ptr<ImageStruct> imageStruct = std::make_shared<ImageStruct>();
    int32_t bufferLen = 0;
    ImageStructVec imgVec;
    bool result = OHOS::CheckImageData(filename, imageStruct, bufferLen, imgVec);
    EXPECT_FALSE(result);
    unsigned long fileSize = TEST_FILE_SIZE;
    imageStruct->memPtr.SetBufferSize(fileSize);
    result = OHOS::CheckImageData(filename, imageStruct, bufferLen, imgVec);
    EXPECT_TRUE(result);
}

HWTEST_F(UtilTest, UtilTest_011, TestSize.Level2)
{
    std::string filePath = "/sys_prod/etc/bootanimation/bootanimation_custom_config.json1";
    std::map<int32_t, BootAnimationProgressConfig> configs;
    OHOS::ParseProgressConfig(filePath, configs);
    EXPECT_EQ(configs.empty(), true);
}

HWTEST_F(UtilTest, UtilTest_012, TestSize.Level1)
{
    std::map<int32_t, BootAnimationProgressConfig> configs;
    std::string jsonStr = "{\"1\":\"abc\"}";
    cJSON* jsonData = cJSON_Parse(jsonStr.c_str());
    OHOS::ParseProgressData(jsonData, configs);
    EXPECT_EQ(configs.empty(), true);

    jsonStr = "{\"progress_config\":[]}";
    jsonData = cJSON_Parse(jsonStr.c_str());
    OHOS::ParseProgressData(jsonData, configs);
    EXPECT_EQ(configs.empty(), true);

    jsonStr = "{\"progress_config\":[{\"cust.bootanimation.screen_id\":\"0\"}]}";
    jsonData = cJSON_Parse(jsonStr.c_str());
    OHOS::ParseProgressData(jsonData, configs);
    EXPECT_EQ(configs.empty(), false);

    jsonStr = "{\"progress_config\":[{\"cust.bootanimation.progress_screen_id\":\"1\", "
    "\"cust.bootanimation.progress_font_size\":\"3\", \"cust.bootanimation.progress_radius_size\":\"10\", "
    "\"cust.bootanimation.progress_x_offset\":\"-1088\", \"cust.bootanimation.progress_degree90\", "
    "\"cust.bootanimation.progress_height\":\"2232\", \"cust.bootanimation.progress_frame_height\":\"112\"}]}";
    jsonData = cJSON_Parse(jsonStr.c_str());
    OHOS::ParseProgressData(jsonData, configs);
    EXPECT_EQ(configs.empty(), false);
}

HWTEST_F(UtilTest, UtilTest_013, TestSize.Level2)
{
    std::string str = "";
    EXPECT_EQ(OHOS::StringToInt32(str), 0);
    str = "1234567890";
    EXPECT_EQ(OHOS::StringToInt32(str), 0);
    str = "-123";
    EXPECT_EQ(OHOS::StringToInt32(str), -123);
    str = "123ab";
    EXPECT_EQ(OHOS::StringToInt32(str), 0);
    str = "123";
    EXPECT_EQ(OHOS::StringToInt32(str), 123);
}

HWTEST_F(UtilTest, UtilTest_014, TestSize.Level1)
{
    int32_t sideLen = TEST_SIDE_LEN;
    int32_t vp = TEST_VP;
    int32_t result = OHOS::TranslateVp2Pixel(sideLen, vp);
    EXPECT_GT(result, 0);
}

HWTEST_F(UtilTest, UtilTest_015, TestSize.Level1)
{
    int32_t sideLen = TEST_SIDE_LEN;
    int32_t vp = TEST_VP;
    float ratio = 0;
    int32_t result = OHOS::TranslateVp2Pixel(sideLen, vp, ratio);
    EXPECT_EQ(result, 0);
}

HWTEST_F(UtilTest, UtilTest_016, TestSize.Level1)
{
    int32_t sideLen = TEST_SIDE_LEN;
    int32_t vp = TEST_VP;
    float ratio = TEST_RATIO;
    int32_t result = OHOS::TranslateVp2Pixel(sideLen, vp, ratio);
    EXPECT_GT(result, 0);
}

HWTEST_F(UtilTest, UtilTest_017, TestSize.Level1)
{
    OHOS::PostTask([]() {}, 0);
    EXPECT_TRUE(true);
}

HWTEST_F(UtilTest, UtilTest_018, TestSize.Level1)
{
    int32_t result = OHOS::GetSystemCurrentTime();
    EXPECT_GT(result, 0);
}

HWTEST_F(UtilTest, UtilTest_019, TestSize.Level1)
{
    std::string deviceType = OHOS::GetDeviceType();
    EXPECT_NE(deviceType.empty(), true);
}

HWTEST_F(UtilTest, UtilTest_020, TestSize.Level1)
{
    ImageStructVec imgVec;
    OHOS::SortZipFile(imgVec);
    EXPECT_EQ(imgVec.size(), 0);
}

HWTEST_F(UtilTest, UtilTest_021, TestSize.Level1)
{
    std::string invalidPath = "/invalid/path/test.zip";
    ImageStructVec imgVec;
    FrameRateConfig frameConfig;
    bool result = OHOS::ReadZipFile(invalidPath, imgVec, frameConfig);
    EXPECT_EQ(result, false);
}

HWTEST_F(UtilTest, UtilTest_022, TestSize.Level1)
{
    unzFile zipFile = nullptr;
    bool result = OHOS::CloseZipFile(zipFile, false);
    EXPECT_EQ(result, false);
}

HWTEST_F(UtilTest, UtilTest_023, TestSize.Level1)
{
    unzFile zipFile = nullptr;
    std::string fileName = "test.png";
    ImageStructVec imgVec;
    FrameRateConfig frameConfig;
    bool result = OHOS::ReadImageFile(zipFile, fileName, imgVec, frameConfig, TEST_FILE_SIZE);
    EXPECT_EQ(result, false);
}

HWTEST_F(UtilTest, UtilTest_024, TestSize.Level1)
{
    std::string jsonStr = "{\"FrameRate\":60}";
    cJSON* jsonData = cJSON_Parse(jsonStr.c_str());
    FrameRateConfig frameConfig;
    char* buffer = jsonStr.data();
    bool result = OHOS::ParseImageConfig(buffer, jsonStr.length(), frameConfig);
    cJSON_Delete(jsonData);
    EXPECT_TRUE(result);
}

HWTEST_F(UtilTest, UtilTest_025, TestSize.Level1)
{
    cJSON* overallData = nullptr;
    int32_t duration = 0;
    OHOS::ParseBootDuration(overallData, duration);
    EXPECT_EQ(duration, 0);
}

HWTEST_F(UtilTest, UtilTest_026, TestSize.Level1)
{
    std::string path = "";
    cJSON* result = OHOS::ParseFileConfig(path);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(UtilTest, UtilTest_027, TestSize.Level1)
{
    BootAnimationConfig config;
    std::string jsonStr = "{\"screen1\":\"path1\",\"screen2\":\"path2\"}";
    cJSON* jsonData = cJSON_Parse(jsonStr.c_str());
    OHOS::ParseVideoExtraPath(jsonData, config);
    cJSON_Delete(jsonData);
    EXPECT_EQ(config.videoExtPath.size(), 2);
}

/**
 * @tc.name: StringToInt32_BoundaryMaxLength_ReturnCorrectValue
 * @tc.desc: Verify the StringToInt32 function with maximum length string.
 * @tc.type: FUNC
 */
HWTEST_F(UtilTest, StringToInt32_BoundaryMaxLength_ReturnCorrectValue, TestSize.Level1)
{
    std::string str = "123456789";
    EXPECT_EQ(OHOS::StringToInt32(str), 123456789);
}

/**
 * @tc.name: StringToInt32_ExceedMaxLength_ReturnZero
 * @tc.desc: Verify the StringToInt32 function with string exceeding maximum length.
 * @tc.type: FUNC
 */
HWTEST_F(UtilTest, StringToInt32_ExceedMaxLength_ReturnZero, TestSize.Level1)
{
    std::string str = "1234567890";
    EXPECT_EQ(OHOS::StringToInt32(str), 0);
}

/**
 * @tc.name: TranslateVp2Pixel_ZeroRatio_ReturnZero
 * @tc.desc: Verify the TranslateVp2Pixel function with zero ratio.
 * @tc.type: FUNC
 */
HWTEST_F(UtilTest, TranslateVp2Pixel_ZeroRatio_ReturnZero, TestSize.Level1)
{
    int32_t sideLen = TEST_SIDE_LEN;
    int32_t vp = TEST_VP;
    float ratio = 0;
    int32_t result = OHOS::TranslateVp2Pixel(sideLen, vp, ratio);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: IsFileExisted_EmptyPath_ReturnFalse
 * @tc.desc: Verify the IsFileExisted function with empty path.
 * @tc.type: FUNC
 */
HWTEST_F(UtilTest, IsFileExisted_EmptyPath_ReturnFalse, TestSize.Level1)
{
    std::string filePath = "";
    bool isFileExist = OHOS::IsFileExisted(filePath);
    EXPECT_EQ(false, isFileExist);
}

/**
 * @tc.name: ParseBootConfig_EmptyPath_ReturnFalse
 * @tc.desc: Verify the ParseBootConfig function with empty path.
 * @tc.type: FUNC
 */
HWTEST_F(UtilTest, ParseBootConfig_EmptyPath_ReturnFalse, TestSize.Level1)
{
    std::string filePath = "";
    std::vector<BootAnimationConfig> animationConfigs;
    bool isMultiDisplay = false;
    bool isCompatible = false;
    int32_t duration = TEST_DURATION;
    bool parseResult = OHOS::ParseBootConfig(filePath, duration, isCompatible, isMultiDisplay, animationConfigs);
    EXPECT_EQ(false, parseResult);
}

/**
 * @tc.name: CheckImageData_NullBuffer_ReturnFalse
 * @tc.desc: Verify the CheckImageData function with null buffer.
 * @tc.type: FUNC
 */
HWTEST_F(UtilTest, CheckImageData_NullBuffer_ReturnFalse, TestSize.Level1)
{
    std::string filename = "abc";
    std::shared_ptr<ImageStruct> imageStruct = std::make_shared<ImageStruct>();
    int32_t bufferLen = 0;
    ImageStructVec imgVec;
    bool result = OHOS::CheckImageData(filename, imageStruct, bufferLen, imgVec);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ReadZipFile_NullZipFile_ReturnFalse
 * @tc.desc: Verify the ReadZipFile function with invalid zip path.
 * @tc.type: FUNC
 */
HWTEST_F(UtilTest, ReadZipFile_NullZipFile_ReturnFalse, TestSize.Level1)
{
    std::string invalidPath = "/invalid/path/test.zip";
    ImageStructVec imgVec;
    FrameRateConfig frameConfig;
    bool result = OHOS::ReadZipFile(invalidPath, imgVec, frameConfig);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: ParseImageConfig_NullBuffer_ReturnFalse
 * @tc.desc: Verify the ParseImageConfig function with null buffer.
 * @tc.type: FUNC
 */
HWTEST_F(UtilTest, ParseImageConfig_NullBuffer_ReturnFalse, TestSize.Level1)
{
    const char* fileBuffer;
    int totalsize = 0;
    FrameRateConfig frameConfig;
    bool result = OHOS::ParseImageConfig(fileBuffer, totalsize, frameConfig);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ReadFile_NullPath_ReturnEmpty
 * @tc.desc: Verify the ReadFile function with null path.
 * @tc.type: FUNC
 */
HWTEST_F(UtilTest, ReadFile_NullPath_ReturnEmpty, TestSize.Level1)
{
    std::string filePath = "";
    std::string content = OHOS::ReadFile(filePath);
    EXPECT_EQ(0, content.length());
}
}