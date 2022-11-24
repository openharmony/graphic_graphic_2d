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

#include <gtest/gtest.h>

#include "screen_manager/rs_screen_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSScreenDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenDataTest::SetUpTestCase() {}
void RSScreenDataTest::TearDownTestCase() {}
void RSScreenDataTest::SetUp() {}
void RSScreenDataTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenDataTest, Marshalling001, TestSize.Level1)
{
    RSScreenData screenData;
    Parcel parcel;
    ASSERT_TRUE(screenData.Marshalling(parcel));
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenDataTest, Unmarshalling001, TestSize.Level1)
{
    RSScreenData screenData;
    Parcel parcel;
    screenData.Unmarshalling(parcel);
}

} // namespace Rosen
} // namespace OHOS
