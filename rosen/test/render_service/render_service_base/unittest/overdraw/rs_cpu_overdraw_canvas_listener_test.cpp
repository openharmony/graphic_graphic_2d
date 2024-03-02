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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <include/core/SkDrawable.h>
#include <include/core/SkMatrix.h>
#include <include/core/SkPath.h>
#include <include/core/SkPicture.h>
#include <include/core/SkRRect.h>
#include <include/core/SkRegion.h>
#include <include/core/SkTextBlob.h>
#include <test_header.h>

#include "platform/ohos/overdraw/rs_cpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_listened_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSCPUOverdrawCanvasListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCPUOverdrawCanvasListenerTest::SetUpTestCase() {}
void RSCPUOverdrawCanvasListenerTest::TearDownTestCase() {}
void RSCPUOverdrawCanvasListenerTest::SetUp() {}
void RSCPUOverdrawCanvasListenerTest::TearDown() {}

class MockDrawingCanvas : public Drawing::Canvas {
public:
    MOCK_METHOD1(DrawRegion, void(const Drawing::Region& region));
};

/*
 * Function: mock draw 3 regions that do not intersect each other
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. new mock MockDrawingCanvas
 *                  2. expect MockDrawingCanvas call DrawRegion once
 *                  3. new RSCPUOverdrawCanvasListener from MockDrawingCanvas
 *                  4. call RSCPUOverdrawCanvasListener's onDrawRect 3 times
 *                      - rect{  0,   0, 100, 100}
 *                      - rect{200, 200, 100, 100}
 *                      - rect{400, 400, 100, 100}
 */
HWTEST_F(RSCPUOverdrawCanvasListenerTest, NoIntersect, Function | SmallTest | Level2)
{
    PART("CaseDescription")
    {
        std::unique_ptr<MockDrawingCanvas> mockDrawingCanvas = nullptr;
        STEP("1. new mock MockDrawingCanvas")
        {
            mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
        }

        STEP("2. expect MockDrawingCanvas call DrawRegion once")
        {
            EXPECT_CALL(*mockDrawingCanvas, DrawRegion(_)).Times(1);
        }

        std::unique_ptr<RSCPUOverdrawCanvasListener> rsOverdrawCanvasListener = nullptr;
        STEP("3. new RSCPUOverdrawCanvasListener from MockDrawingCanvas")
        {
            rsOverdrawCanvasListener = std::make_unique<RSCPUOverdrawCanvasListener>(*mockDrawingCanvas);
        }

        STEP("4. call RSCPUOverdrawCanvasListener's onDrawRect 3 times")
        {
            rsOverdrawCanvasListener->DrawRect(Drawing::Rect(0, 0, 100, 100));
            rsOverdrawCanvasListener->DrawRect(Drawing::Rect(200, 200, 300, 300));
            rsOverdrawCanvasListener->DrawRect(Drawing::Rect(400, 400, 500, 500));
            rsOverdrawCanvasListener->Draw();
        }
    }
}
} // namespace Rosen
} // namespace OHOS
