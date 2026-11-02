/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "common/rs_obj_abs_geometry.h"
#include "effect/rs_render_shader_base.h"
#include "render/rs_effect_luminance_manager.h"
#include "ge_visual_effect_container.h"
#include "drawable/rs_coverage_ng_shader_drawable.h"
#include "effect/rs_render_shape_base.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyDrawableCoverageNGShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyDrawableCoverageNGShaderTest::SetUpTestCase() {}
void RSPropertyDrawableCoverageNGShaderTest::TearDownTestCase() {}
void RSPropertyDrawableCoverageNGShaderTest::SetUp() {}
void RSPropertyDrawableCoverageNGShaderTest::TearDown() {}

} // namespace OHOS::Rosen