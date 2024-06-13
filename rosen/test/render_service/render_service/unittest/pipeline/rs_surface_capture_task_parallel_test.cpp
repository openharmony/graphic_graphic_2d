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

#include "pipeline/rs_context.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_capture_task_parallel.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceCaptureTaskParallelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceCaptureTaskParallelTest::SetUpTestCase() {}
void RSSurfaceCaptureTaskParallelTest::TearDownTestCase() {}
void RSSurfaceCaptureTaskParallelTest::SetUp() {}
void RSSurfaceCaptureTaskParallelTest::TearDown() {}

/*
 * @tc.name: CalPixelMapRotation
 * @tc.desc: function test
 * @tc.type: FUNC
 * @tc.require: issueI9PKY5
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CalPixelMapRotation, TestSize.Level2)
{
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    RSSurfaceCaptureTaskParallel task(0, scaleX, scaleY);
    task.screenCorrection_ = ScreenRotation::ROTATION_90;
    task.screenRotation_ = ScreenRotation::ROTATION_270;
    ASSERT_EQ(task.CalPixelMapRotation(), RS_ROTATION_180);
}
}
}
