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
#include "common/rs_threshold_detector.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSThresholdDetectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSThresholdDetectorTest::SetUpTestCase() {}
void RSThresholdDetectorTest::TearDownTestCase() {}
void RSThresholdDetectorTest::SetUp() {}
void RSThresholdDetectorTest::TearDown() {}

/**
 * @tc.name: RSThresholdDetectorTest001
 * @tc.desc: test of RSThresholdDetector
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSThresholdDetectorTest, RSThresholdDetectorTest001, TestSize.Level1)
{
    // data tests whether callback is trigger
    uint32_t data[] = {10, 500, 501, 502, 200, 100, 99, 0, 200, 501, 0};
    uint32_t thresholdLow = 100; // low threshold : 100
    uint32_t thresholdHigh = 500; // high threshold : 500
    // default low flag
    auto thresholdDetector = std::make_shared<RSThresholdDetector<uint32_t>>(thresholdLow, thresholdHigh);

    // 1. data[0]:10 < thresholdLow, not callback
    auto notCallback = [] (const uint32_t value, bool isHigh) {
        ASSERT_TRUE(false);
    };
    thresholdDetector->Detect(data[0], notCallback);
    // 2. thresholdLow < data[1]:500 <= thresholdHigh, not callback
    thresholdDetector->Detect(data[1], notCallback);
    // 3. data[2]:501 > thresholdHigh firstly, callback and trigger high flag
    auto callback2 = [data] (const uint32_t value, bool isHigh) {
        ASSERT_EQ(value, data[2]);
        ASSERT_TRUE(isHigh);
    };
    thresholdDetector->Detect(data[2], callback2);
    // 4. after high flag, data[3]:502 > thresholdHigh secondly, not callback
    thresholdDetector->Detect(data[3], notCallback);
    // 5. after high flag, thresholdLow < data[4]:200 <= thresholdHigh, not callback
    thresholdDetector->Detect(data[4], notCallback);
    // 6. after high flag, data[5]:100 <= thresholdLow firstly, callback and trigger low flag
    auto callback5 = [data] (const uint32_t value, bool isHigh) {
        ASSERT_EQ(value, data[5]);
        ASSERT_TRUE(!isHigh);
    };
    thresholdDetector->Detect(data[5], callback5);
    // 7. after low flag, data[6]:99 <= thresholdLow, not callback
    thresholdDetector->Detect(data[6], notCallback);
    // 8. after low flag, data[7]:0 <= thresholdLow, not callback
    thresholdDetector->Detect(data[7], notCallback);
    // 9. after low flag, data[8]:200 <= thresholdHigh, not callback
    thresholdDetector->Detect(data[8], notCallback);
    // 10. after low flag, data[9]:501 > thresholdHigh firstly, callback and trigger high flag
    auto callback9 = [data] (const uint32_t value, bool isHigh) {
        ASSERT_EQ(value, data[9]);
        ASSERT_TRUE(isHigh);
    };
    thresholdDetector->Detect(data[9], callback9);
    // 11. after high flag, data[10]:0 <= thresholdLow firstly, callback and trigger low flag
    auto callback10 = [data] (const uint32_t value, bool isHigh) {
        ASSERT_EQ(value, data[10]);
        ASSERT_TRUE(!isHigh);
    };
    thresholdDetector->Detect(data[10], callback10);
}
} // namespace OHOS::Rosen