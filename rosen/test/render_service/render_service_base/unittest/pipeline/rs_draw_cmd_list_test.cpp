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

#include "pipeline/rs_draw_cmd_list.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DrawCmdListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DrawCmdListTest::SetUpTestCase() {}
void DrawCmdListTest::TearDownTestCase() {}
void DrawCmdListTest::SetUp() {}
void DrawCmdListTest::TearDown() {}

/**
 * @tc.name: ClearDrawCmdList001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(DrawCmdListTest, ClearDrawCmdList001, TestSize.Level1)
{
    NodeId id = 1;
    DrawCmdListManager::Instance().ClearDrawCmdList(id);
}

} // namespace Rosen
} // namespace OHOS
