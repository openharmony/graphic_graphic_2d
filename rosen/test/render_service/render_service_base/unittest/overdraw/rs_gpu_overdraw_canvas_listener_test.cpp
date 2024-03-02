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

#include "third_party/skia/include/core/SkRRect.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkRegion.h"
#include "third_party/skia/include/core/SkScalar.h"
#include "third_party/skia/include/core/SkBlendMode.h"

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
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, Create001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    delete listener;
}

/**
 * @tc.name: onDrawRect
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawRect001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawRect(rect);
    delete listener;
}

/**
 * @tc.name: onDrawOval
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawOval001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawOval(rect);
    delete listener;
}

/**
 * @tc.name: onDrawArc
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawArc001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    Drawing::scalar startAngle = 0.0;
    Drawing::scalar sweepAngle = 0.0;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawArc(rect, startAngle, sweepAngle);
    delete listener;
}

/**
 * @tc.name: onDrawArc
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawArc002, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    Drawing::scalar startAngle = 0.0;
    Drawing::scalar sweepAngle = 0.0;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawArc(rect, startAngle, sweepAngle);
    delete listener;
}

/**
 * @tc.name: onDrawPath
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawPath001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Path path;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawPath(path);
    delete listener;
}

/**
 * @tc.name: onDrawRegion
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawRegion001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Region region;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawRegion(region);
    delete listener;
}

/**
 * @tc.name: onDrawPicture
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:#I8K89C
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawPicture001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Picture picture;
    auto listener = new RSGPUOverdrawCanvasListener(canvas);
    listener->DrawPicture(picture);
    delete listener;
}
} // namespace Rosen
} // namespace OHOS
