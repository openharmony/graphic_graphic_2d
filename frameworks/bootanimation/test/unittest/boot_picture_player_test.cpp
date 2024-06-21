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

#include "boot_picture_player.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BootPicturePlayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootPicturePlayerTest::SetUpTestCase() {}
void BootPicturePlayerTest::TearDownTestCase() {}
void BootPicturePlayerTest::SetUp() {}
void BootPicturePlayerTest::TearDown() {}

/**
 * @tc.name: BootPicturePlayerTest_001
 * @tc.desc: Verify the ReadPicZipFile
 * @tc.type:FUNC
 */
HWTEST_F(BootPicturePlayerTest, BootPicturePlayerTest_001, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    ImageStructVec imgVec;
    int32_t freq = 30;
    EXPECT_EQ(player->ReadPicZipFile(imgVec, freq), true);
}

/**
 * @tc.name: BootPicturePlayerTest_002
 * @tc.desc: Verify the CheckFrameRateValid
 * @tc.type:FUNC
 */
HWTEST_F(BootPicturePlayerTest, BootPicturePlayerTest_002, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    int32_t frameRate = 0;
    EXPECT_EQ(player->CheckFrameRateValid(frameRate), false);
}

/**
 * @tc.name: BootPicturePlayerTest_003
 * @tc.desc: Verify the CheckFrameRateValid
 * @tc.type:FUNC
 */
HWTEST_F(BootPicturePlayerTest, BootPicturePlayerTest_003, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    int32_t frameRate = 30;
    EXPECT_EQ(player->CheckFrameRateValid(frameRate), true);
}
}
