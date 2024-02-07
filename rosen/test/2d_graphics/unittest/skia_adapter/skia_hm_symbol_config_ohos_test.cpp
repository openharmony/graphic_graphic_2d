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
#include "skia_adapter/skia_hm_symbol_config_ohos.h"
#include "src/ports/skia_ohos/HmSymbolConfig_ohos.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaHmSymbolConfigOhosTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaHmSymbolConfigOhosTest::SetUpTestCase() {}
void SkiaHmSymbolConfigOhosTest::TearDownTestCase() {}
void SkiaHmSymbolConfigOhosTest::SetUp() {}
void SkiaHmSymbolConfigOhosTest::TearDown() {}

/**
 * @tc.name: GetSymbolLayersGroups001
 * @tc.desc: Test GetSymbolLayersGroups
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaHmSymbolConfigOhosTest, GetSymbolLayersGroups001, TestSize.Level1)
{
    RenderGroup renderGroup;
    std::map<SymbolRenderingStrategy, std::vector<RenderGroup>> renderModeGroups {
        { SymbolRenderingStrategy::INVALID_RENDERING_STRATEGY, { renderGroup } }
    };
    AnimationSetting animationSetting;
    SymbolLayersGroups group {1, {}, renderModeGroups, { animationSetting } };
    HmSymbolConfig_OHOS::getInstance()->getHmSymbolConfig()->insert({1, group});
    auto groups = SkiaHmSymbolConfigOhos::GetSymbolLayersGroups(1);
    ASSERT_TRUE(groups != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS