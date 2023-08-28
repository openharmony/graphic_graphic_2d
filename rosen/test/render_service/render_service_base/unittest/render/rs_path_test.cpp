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
#include "render/rs_path.h"
#include "include/core/SkPath.h"
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkPath.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPathMeasure.h"
#include "include/utils/SkParsePath.h"
#else
#include "recording/recording_path.h"
#include "draw/path.h"
#include "utils/matrix.h"
#include "utils/scalar.h"
#endif

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

static const int startX = 20;
static const int startY = 20;

static SkPath CreateSkPath()
{
    SkPath path;
    path.moveTo(startX, startY);
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
#ifndef USE_ROSEN_DRAWING
    SkPath path1 = CreateSkPath();
    auto rsPath = RSPath::CreateRSPath(path1);
    ASSERT_NE(rsPath, nullptr);
#else
    Drawing::Path path2;
    auto rsPath = RSPath::CreateRSPath(path2);
    ASSERT_NE(rsPath, nullptr);
#endif
}

/**
 * @tc.name: SetSkiaPathTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPathTest, SetSkiaPathTest, TestSize.Level1)
{
    SkPath path = CreateSkPath();
    RSPath rsPath;
    rsPath.SetSkiaPath(path);
}
} // namespace OHOS::Rosen
