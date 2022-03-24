/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "image/picture.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PictureTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PictureTest::SetUpTestCase() {}
void PictureTest::TearDownTestCase() {}
void PictureTest::SetUp() {}
void PictureTest::TearDown() {}

/**
 * @tc.name: PictureCreateAndDestory001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PictureTest, PictureCreateAndDestory001, TestSize.Level1)
{
    // The best way to create Picture.
    std::unique_ptr<Picture> picture = std::make_unique<Picture>();
    ASSERT_TRUE(picture != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS