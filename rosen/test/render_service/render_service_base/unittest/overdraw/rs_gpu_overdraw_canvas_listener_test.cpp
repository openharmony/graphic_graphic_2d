/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_listened_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSGPUOverdrawCanvasListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSGPUOverdrawCanvasListenerTest::SetUpTestCase() {}
void RSGPUOverdrawCanvasListenerTest::TearDownTestCase() {}
void RSGPUOverdrawCanvasListenerTest::SetUp() {}
void RSGPUOverdrawCanvasListenerTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, Create001, TestSize.Level1)
{
    SkCanvas skCanvas;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    delete listener;
}

} // namespace Rosen
} // namespace OHOS
