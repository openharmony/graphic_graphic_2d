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

#include "texgine_path_1d_path_effect.h"

using namespace testing;
using namespace testing::ext;

struct MockVars {
    std::shared_ptr<RSPathEffect> skPathEffect_ = nullptr;
};

namespace {
struct MockVars g_tp1peMockvars;

void InitTp1peMockVars(struct MockVars &&vars)
{
    g_tp1peMockvars = std::move(vars);
}
} // namespace


namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexginePath1DPathEffectTest : public testing::Test {
};

/**
 * @tc.name:Make
 * @tc.desc: Verify the Make
 * @tc.type:FUNC
 */
HWTEST_F(TexginePath1DPathEffectTest, Make, TestSize.Level1)
{
    std::shared_ptr<TexginePath> tp = std::make_shared<TexginePath>();
    EXPECT_NO_THROW({
        InitTp1peMockVars({});
        std::shared_ptr<TexginePathEffect> tpe =
            TexginePath1DPathEffect::Make(*tp, 0.0, 0.0, TexginePath1DPathEffect::Style::K_TRANSLATE_STYLE);
        EXPECT_NE(tpe->GetPathEffect(), g_tp1peMockvars.skPathEffect_);
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
