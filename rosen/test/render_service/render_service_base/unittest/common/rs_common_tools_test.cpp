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

#include "fstream"
#include "gtest/gtest.h"
#include "pixel_map.h"

#include "common/rs_common_tools.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCommonToolsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCommonToolsTest::SetUpTestCase() {}
void RSCommonToolsTest::TearDownTestCase() {}
void RSCommonToolsTest::SetUp() {}
void RSCommonToolsTest::TearDown() {}

/**
 * @tc.name: GetLocalTimeTest
 * @tc.desc: test results of GetLocalTime
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonToolsTest, GetLocalTimeTest, TestSize.Level1)
{
    std::string currentTime = CommonTools::GetLocalTime();
    EXPECT_FALSE(currentTime.empty());
}

/**
 * @tc.name: SavePixelmapToFileTest
 * @tc.desc: test results of SavePixelmapToFile
 * @tc.type:FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSCommonToolsTest, SavePixelmapToFileTest, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 512;
    opts.size.height = 512;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    EXPECT_FALSE(pixelMap == nullptr);
    auto shpPixelMap = std::shared_ptr<Media::PixelMap>(pixelMap.release());
    std::string dst = "/path/to/nonexistent/directory/";
    CommonTools::SavePixelmapToFile(shpPixelMap, dst);
    dst = "";
    CommonTools::SavePixelmapToFile(shpPixelMap, dst);
    EXPECT_FALSE(shpPixelMap == nullptr);
}
} // namespace OHOS::Rosen