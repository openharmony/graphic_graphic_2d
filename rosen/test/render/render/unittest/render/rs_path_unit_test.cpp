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

#include "draw/path.h"
#include "render/rs_path.h"
#include "utils/matrix.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPathUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPathUnitTest::SetUpTestCase() {}
void RSPathUnitTest::TearDownTestCase() {}
void RSPathUnitTest::SetUp() {}
void RSPathUnitTest::TearDown() {}

static const int START_X = 20;
static const int START_Y = 20;

static Drawing::Path CreateDrawingPath()
{
    Drawing::Path path;
    path.MoveTo(START_X, START_Y);
    return path;
}

/**
 * @tc.name: TestCreateRSPath01
 * @tc.desc: Verify function CreateRSPath
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPathUnitTest, TestCreateRSPath01, TestSize.Level1)
{
    auto testPath = RSPath::CreateRSPath();
    ASSERT_NE(testPath, nullptr);
}

/**
 * @tc.name: TestCreateRSPath02
 * @tc.desc: Verify function CreateRSPath
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPathUnitTest, TestCreateRSPath02, TestSize.Level1)
{
    Drawing::Path path1 = CreateDrawingPath();
    auto testPath = RSPath::CreateRSPath(path1);
    ASSERT_NE(testPath, nullptr);
}

/**
 * @tc.name: TestGetDrawingPath01
 * @tc.desc: Verify function GetDrawingPath
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPathUnitTest, TestGetDrawingPath01, TestSize.Level1)
{
    auto testPath = RSPath::CreateRSPath();
    testPath->GetDrawingPath();
    ASSERT_NE(testPath->drPath_, nullptr);
}

/**
 * @tc.name: TestReverse01
 * @tc.desc: Verify function Reverse
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPathUnitTest, TestReverse01, TestSize.Level1)
{
    Drawing::Path path = CreateDrawingPath();
    auto testPath = RSPath::CreateRSPath(path);
    auto reveseRsPath = testPath->Reverse();
    EXPECT_NE(testPath, reveseRsPath);
}

/**
 * @tc.name: TestGetDistance01
 * @tc.desc: Verify function GetDistance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPathUnitTest, TestGetDistance01, TestSize.Level1)
{
    Drawing::Path path = CreateDrawingPath();
    auto testPath = RSPath::CreateRSPath(path);
    EXPECT_EQ(testPath->GetDistance(), path.GetLength(false));
}

/**
 * @tc.name: TestSetSkiaPath01
 * @tc.desc: Verify function SetSkiaPath
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPathUnitTest, TestSetSkiaPath01, TestSize.Level1)
{
    Drawing::Path path = CreateDrawingPath();
    RSPath testPath;
    testPath.SetDrawingPath(path);
    ASSERT_NE(&path, testPath.drPath_);
}

/**
 * @tc.name: TestGetPosTan01
 * @tc.desc: Verify function GetPosTan
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPathUnitTest, TestGetPosTan01, TestSize.Level1)
{
    auto testPath = RSPath::CreateRSPath();
    Vector2f pos = { 1.0f, 1.0f };
    float degrees = 0.0f;
    EXPECT_FALSE(testPath->GetPosTan(0.0f, pos, degrees));
}

/**
 * @tc.name: TestGetPosTan02
 * @tc.desc: Verify function GetPosTan
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPathUnitTest, TestGetPosTan02, TestSize.Level1)
{
    auto testPath = RSPath::CreateRSPath();
    Vector4f pos = { 1.0f, 1.0f, 1.0f, 1.0f };
    float degrees = 0.0f;
    EXPECT_FALSE(testPath->GetPosTan(0.0f, pos, degrees));
}

/**
 * @tc.name: TestReverse02
 * @tc.desc: Verify function Reverse
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPathUnitTest, TestReverse02, TestSize.Level1)
{
    RSPath testPath;
    EXPECT_NE(testPath.Reverse(), nullptr);
}

/**
 * @tc.name: TestSetDrawingPath01
 * @tc.desc: Verify function SetDrawingPath
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPathUnitTest, TestSetDrawingPath01, TestSize.Level2)
{
    Drawing::Path path = CreateDrawingPath();
    auto testPath = RSPath::CreateRSPath();
    EXPECT_NE(testPath, nullptr);

    testPath->drPath_ = nullptr;
    testPath->SetDrawingPath(path);
    EXPECT_NE(testPath->drPath_, nullptr);
}
} // namespace OHOS::Rosen
