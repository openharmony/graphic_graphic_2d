/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <securec.h>
#include <gtest/gtest.h>

#include "screen_manager/rs_screen_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenPropertyTest::SetUpTestCase() {}
void RSScreenPropertyTest::TearDownTestCase() {}
void RSScreenPropertyTest::SetUp() {}
void RSScreenPropertyTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenPropertyTest, Marshalling001, TestSize.Level1)
{
    RSScreenProperty srcProperty;
    MessageParcel parcel;
    srcProperty.Marshalling(parcel);

    auto dstProperty = sptr<RSScreenProperty>(RSScreenProperty::Unmarshalling(parcel));
    ASSERT_NE(dstProperty, nullptr);
}
}