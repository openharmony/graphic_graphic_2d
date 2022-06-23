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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <include/core/SkRegion.h>
#include <include/core/SkRRect.h>
#include <test_header.h>

#include "pipeline/overdraw/rs_cpu_overdraw_canvas_listener.h"
#include "pipeline/overdraw/rs_listened_canvas.h"

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

class MockSkCanvas : public SkCanvas {
public:
    MOCK_METHOD2(onDrawRegion, void(const SkRegion& region, const SkPaint& paint));
};

/*
 * Function: mock draw 3 intersecting regions
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. new mock MockSkCanvas
 *                  2. expect MockSkCanvas call onDrawRegion 3 times
 *                  3. new RSCPUOverdrawCanvasListener from MockSkCanvas
 *                  4. call RSCPUOverdrawCanvasListener's onDrawRect 3 times
 *                      - rect{  0,   0, 600, 600}
 *                      - rect{100, 100, 400, 400}
 *                      - rect{200, 200, 200, 200}
 */
HWTEST_F(RSCPUOverdrawCanvasListenerTest, Intersect, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        std::unique_ptr<MockSkCanvas> mockSkCanvas = nullptr;
        STEP("1. new mock MockSkCanvas") {
            mockSkCanvas = std::make_unique<MockSkCanvas>();
        }

        STEP("2. expect MockSkCanvas call onDrawRegion 3 times") {
            EXPECT_CALL(*mockSkCanvas, onDrawRegion(_, _)).Times(3);
        }

        std::unique_ptr<RSCPUOverdrawCanvasListener> rsOverdrawCanvasListener = nullptr;
        STEP("3. new RSCPUOverdrawCanvasListener from MockSkCanvas") {
            rsOverdrawCanvasListener = std::make_unique<RSCPUOverdrawCanvasListener>(*mockSkCanvas);
        }

        STEP("4. call RSCPUOverdrawCanvasListener's onDrawRect 3 times") {
            rsOverdrawCanvasListener->onDrawRRect(SkRRect::MakeOval(SkRect::MakeXYWH(  0,   0, 600, 600)), {});
            rsOverdrawCanvasListener->onDrawRRect(SkRRect::MakeOval(SkRect::MakeXYWH(100, 100, 400, 400)), {});
            rsOverdrawCanvasListener->onDrawRRect(SkRRect::MakeOval(SkRect::MakeXYWH(200, 200, 200, 200)), {});
            rsOverdrawCanvasListener->Draw();
        }
    }
}

/*
 * Function: mock draw 3 regions that do not intersect each other
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. new mock MockSkCanvas
 *                  2. expect MockSkCanvas call onDrawRegion once
 *                  3. new RSCPUOverdrawCanvasListener from MockSkCanvas
 *                  4. call RSCPUOverdrawCanvasListener's onDrawRect 3 times
 *                      - rect{  0,   0, 100, 100}
 *                      - rect{200, 200, 100, 100}
 *                      - rect{400, 400, 100, 100}
 */
HWTEST_F(RSCPUOverdrawCanvasListenerTest, NoIntersect, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        std::unique_ptr<MockSkCanvas> mockSkCanvas = nullptr;
        STEP("1. new mock MockSkCanvas") {
            mockSkCanvas = std::make_unique<MockSkCanvas>();
        }

        STEP("2. expect MockSkCanvas call onDrawRegion once") {
            EXPECT_CALL(*mockSkCanvas, onDrawRegion(_, _)).Times(1);
        }

        std::unique_ptr<RSCPUOverdrawCanvasListener> rsOverdrawCanvasListener = nullptr;
        STEP("3. new RSCPUOverdrawCanvasListener from MockSkCanvas") {
            rsOverdrawCanvasListener = std::make_unique<RSCPUOverdrawCanvasListener>(*mockSkCanvas);
        }

        STEP("4. call RSCPUOverdrawCanvasListener's onDrawRect 3 times") {
            rsOverdrawCanvasListener->onDrawRect(SkRect::MakeXYWH(  0,   0, 100, 100), {});
            rsOverdrawCanvasListener->onDrawRect(SkRect::MakeXYWH(200, 200, 100, 100), {});
            rsOverdrawCanvasListener->onDrawRect(SkRect::MakeXYWH(400, 400, 100, 100), {});
            rsOverdrawCanvasListener->Draw();
        }
    }
}
} // namespace Rosen
} // namespace OHOS
