/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "draw/path.h"
#include "render/rs_path.h"
#include "utils/matrix.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPathTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPathTest::SetUpTestCase() {}
void RSPathTest::TearDownTestCase() {}
void RSPathTest::SetUp() {}
void RSPathTest::TearDown() {}

static const int START_X = 20;
static const int START_Y = 20;

static Drawing::Path CreateDrawingPath()
{
    Drawing::Path path;
    path.MoveTo(START_X, START_Y);
    return path;
}

/**
 * @tc.name: CreateRSPathTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPathTest, CreateRSPathTest001, TestSize.Level1)
{
    auto rsPath = RSPath::CreateRSPath();
    ASSERT_NE(rsPath, nullptr);
}

/**
 * @tc.name: CreateRSPathTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPathTest, CreateRSPathTest002, TestSize.Level1)
{
    Drawing::Path path1 = CreateDrawingPath();
    auto rsPath = RSPath::CreateRSPath(path1);
    ASSERT_NE(rsPath, nullptr);
}

/**
 * @tc.name: GetDrawingPathTest001
 * @tc.desc: Verify function GetDrawingPath
 * @tc.type:FUNC
 */
HWTEST_F(RSPathTest, GetDrawingPathTest001, TestSize.Level1)
{
    auto rsPath = RSPath::CreateRSPath();
    rsPath->GetDrawingPath();
    ASSERT_NE(rsPath->drPath_, nullptr);
}

/**
 * @tc.name: ReverseTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPathTest, ReverseTest, TestSize.Level1)
{
    Drawing::Path path = CreateDrawingPath();
    auto rsPath = RSPath::CreateRSPath(path);
    auto reveseRsPath = rsPath->Reverse();
    EXPECT_NE(rsPath, reveseRsPath);
}

/**
 * @tc.name: GetDistanceTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPathTest, GetDistanceTest, TestSize.Level1)
{
    Drawing::Path path = CreateDrawingPath();
    auto rsPath = RSPath::CreateRSPath(path);
    EXPECT_EQ(rsPath->GetDistance(), path.GetLength(false));
}

/**
 * @tc.name: SetSkiaPathTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPathTest, SetSkiaPathTest, TestSize.Level1)
{
    Drawing::Path path = CreateDrawingPath();
    RSPath rsPath;
    rsPath.SetDrawingPath(path);
}

/**
 * @tc.name: GetPosTanTest001
 * @tc.desc: Verify function GetPosTan
 * @tc.type:FUNC
 */
HWTEST_F(RSPathTest, GetPosTanTest001, TestSize.Level1)
{
    auto rsPath = RSPath::CreateRSPath();
    Vector2f pos = { 1.0f, 1.0f };
    float degrees = 0.0f;
    EXPECT_FALSE(rsPath->GetPosTan(0.0f, pos, degrees));
}

/**
 * @tc.name: GetPosTanTest002
 * @tc.desc: Verify function GetPosTan
 * @tc.type:FUNC
 */
HWTEST_F(RSPathTest, GetPosTanTest002, TestSize.Level1)
{
    auto rsPath = RSPath::CreateRSPath();
    Vector4f pos = { 1.0f, 1.0f, 1.0f, 1.0f };
    float degrees = 0.0f;
    EXPECT_FALSE(rsPath->GetPosTan(0.0f, pos, degrees));
}
} // namespace OHOS::Rosen
