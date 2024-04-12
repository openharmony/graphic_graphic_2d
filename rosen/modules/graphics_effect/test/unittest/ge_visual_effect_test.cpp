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

#include "ge_visual_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GEVisualEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEVisualEffectTest::SetUpTestCase(void) {}
void GEVisualEffectTest::TearDownTestCase(void) {}

void GEVisualEffectTest::SetUp() {}
void GEVisualEffectTest::TearDown() {}

/**
 * @tc.name: SetParam001
 * @tc.desc: Verify the SetParam
 * @tc.type: FUNC
 */
HWTEST_F(GEVisualEffectTest, SetParam001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam001 start";

    auto visualEffect = std::make_shared<GEVisualEffect>("abc");
    visualEffect->SetParam("abc", 1);
    visualEffect->SetParam("abc", 1.0f);
    visualEffect->SetParam("abc", "bca");

    GTEST_LOG_(INFO) << "GEVisualEffectTest SetParam001 end";
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
