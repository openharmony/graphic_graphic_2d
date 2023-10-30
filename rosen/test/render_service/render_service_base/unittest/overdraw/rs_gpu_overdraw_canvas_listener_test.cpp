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
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, Create001, TestSize.Level1)
{
    SkCanvas skCanvas;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    delete listener;
}

/**
 * @tc.name: onDrawRect
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawRect001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkRect rect;
    SkPaint paint;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawRect(rect, paint);
    delete listener;
}

/**
 * @tc.name: onDrawRRect
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawRRect001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkRRect rect;
    SkPaint paint;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawRRect(rect, paint);
    delete listener;
}

/**
 * @tc.name: onDrawDRRect
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawDRRect001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkRRect outer;
    SkRRect inner;
    SkPaint paint;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawDRRect(outer, inner, paint);
    delete listener;
}

/**
 * @tc.name: onDrawOval
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawOval001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkRect rect;
    SkPaint paint;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawOval(rect, paint);
    delete listener;
}

/**
 * @tc.name: onDrawArc
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawArc001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkRect rect;
    SkScalar startAngle = 0.0;
    SkScalar sweepAngle = 0.0;
    SkPaint paint;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawArc(rect, startAngle, sweepAngle, true, paint);
    delete listener;
}

/**
 * @tc.name: onDrawArc
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawArc002, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkRect rect;
    SkScalar startAngle = 0.0;
    SkScalar sweepAngle = 0.0;
    SkPaint paint;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawArc(rect, startAngle, sweepAngle, false, paint);
    delete listener;
}

/**
 * @tc.name: onDrawPath
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawPath001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkPath path;
    SkPaint paint;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawPath(path, paint);
    delete listener;
}

/**
 * @tc.name: onDrawRegion
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawRegion001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkRegion region;
    SkPaint paint;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawRegion(region, paint);
    delete listener;
}

/**
 * @tc.name: onDrawTextBlob
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawTextBlob001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkTextBlob *blob;
    SkScalar x = 0.0;
    SkScalar y = 0.0;
    SkPaint paint;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawTextBlob(blob, x, y, paint);
    delete listener;
}

/**
 * @tc.name: onDrawAnnotation
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawAnnotation001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkRect rect;
    char key[10];
    SkData* value = nullptr;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawAnnotation(rect, key, value);
    delete listener;
}

/**
 * @tc.name: onDrawDrawable
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawDrawable001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkDrawable* drawable = nullptr;
    SkMatrix* matrix;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawDrawable(drawable, matrix);
    delete listener;
}

/**
 * @tc.name: onDrawPicture
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSGPUOverdrawCanvasListenerTest, onDrawPicture001, TestSize.Level1)
{
    SkCanvas skCanvas;
    SkPicture* picture;
    SkMatrix* matrix;
    SkPaint* paint;
    auto listener = new RSGPUOverdrawCanvasListener(skCanvas);
    listener->onDrawPicture(picture, matrix, paint);
    delete listener;
}
} // namespace Rosen
} // namespace OHOS
