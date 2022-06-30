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

#include "draw/canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class CanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CanvasTest::SetUpTestCase() {}
void CanvasTest::TearDownTestCase() {}
void CanvasTest::SetUp() {}
void CanvasTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CreateAndDestroy001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    EXPECT_TRUE(nullptr != canvas);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
