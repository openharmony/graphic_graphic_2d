/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "rs_canvas_render_node_drawable.h"
#include "rs_effect_render_node_drawable.h"
#include "rs_logical_display_render_node_drawable.h"
#include "rs_render_node_drawable.h"
#include "rs_surface_render_node_drawable.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSubtreeTest : public testing:Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSubtreeTest::SetUpTestCase() {}
void RSSubtreeTest::TearDownTestCase() {}
void RSSubtreeTest::SetUp() {}
void RSSubtreeTest::TearDown() {}
}
