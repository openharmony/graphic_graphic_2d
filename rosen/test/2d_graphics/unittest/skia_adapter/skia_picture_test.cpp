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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_picture.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPictureTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaPictureTest::SetUpTestCase() {}
void SkiaPictureTest::TearDownTestCase() {}
void SkiaPictureTest::SetUp() {}
void SkiaPictureTest::TearDown() {}

/**
 * @tc.name: SkiaPicture001
 * @tc.desc: Test SkiaPicture's functions
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPictureTest, SkiaPicture001, TestSize.Level1)
{
    SkiaPicture skiaPicture;
    skiaPicture.Deserialize(nullptr);
    std::shared_ptr<Data> data = std::make_shared<Data>();
    skiaPicture.Deserialize(data);
    skiaPicture.Serialize();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS