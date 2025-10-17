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

#include "video_metadata_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class VideoMetadataParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void VideoMetadataParamTest::SetUpTestCase() {}
void VideoMetadataParamTest::TearDownTestCase() {}
void VideoMetadataParamTest::SetUp() {}
void VideoMetadataParamTest::TearDown() {}

/**
 * @tc.name: GetVideoMetadataAppMap_001
 * @tc.desc: Verify the GetVideoMetadataAppMap and AddVideoMetadataApp function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VideoMetadataParamTest, GetVideoMetadataAppMap_001, Function | SmallTest | Level1)
{
    std::string appName = "testApp";
    std::string val = "1";
    VideoMetadataParam::AddVideoMetadataApp(appName, val);
    const auto& vidoMetadataAppMap = VideoMetadataParam::GetVideoMetadataAppMap();
    EXPECT_EQ(vidoMetadataAppMap.at(appName), "1");
}
} // namespace Rosen
} // namespace OHOS