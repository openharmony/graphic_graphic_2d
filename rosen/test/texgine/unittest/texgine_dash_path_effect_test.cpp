/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>

#include "texgine_dash_path_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineDashPathEffectTest : public testing::Test {
};

/**
 * @tc.name:Make
 * @tc.desc: Verify the Make
 * @tc.type:FUNC
 */
HWTEST_F(TexgineDashPathEffectTest, Make, TestSize.Level1)
{
    float intervals[] = {0.0};
    EXPECT_NO_THROW({
        TexgineDashPathEffect::Make(intervals, 0, 0.0);
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
