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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_path_effect.h"
#include "effect/path_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPathEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaPathEffectTest::SetUpTestCase() {}
void SkiaPathEffectTest::TearDownTestCase() {}
void SkiaPathEffectTest::SetUp() {}
void SkiaPathEffectTest::TearDown() {}

/**
 * @tc.name: InitWithPathDash001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPathEffectTest, InitWithPathDash001, TestSize.Level1)
{
    Path path;
    scalar advance = 12.0f;
    scalar phase = 10.0f;
    SkiaPathEffect skiaPathEffect;
    skiaPathEffect.InitWithPathDash(path, advance, phase, PathDashStyle::TRANSLATE);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS