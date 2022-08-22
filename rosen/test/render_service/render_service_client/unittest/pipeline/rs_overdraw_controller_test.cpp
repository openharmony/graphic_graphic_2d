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
#include <test_header.h>

#include "overdraw/rs_canvas_listener.h"
#include "overdraw/rs_overdraw_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSOverdrawControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOverdrawControllerTest::SetUpTestCase() {}
void RSOverdrawControllerTest::TearDownTestCase() {}
void RSOverdrawControllerTest::SetUp() {}
void RSOverdrawControllerTest::TearDown() {}

class MockRSPaintFilterCanvas : public RSPaintFilterCanvas {
public:
    explicit MockRSPaintFilterCanvas(SkCanvas *canvas) : RSPaintFilterCanvas(canvas) {}
    MOCK_METHOD2(onDrawRect, void(const SkRect& rect, const SkPaint& paint));
};

/*
 * Function: Singleton is single
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. call GetInstance twice
 *                  2. check instance should be same
 */
HWTEST_F(RSOverdrawControllerTest, Singleton, Function | SmallTest | Level4)
{
    PART("CaseDescription") {
        STEP("1. call GetInstance twice") {
            auto &instance1 = RSOverdrawController::GetInstance();
            auto &instance2 = RSOverdrawController::GetInstance();
            STEP("2. check instance should be same") {
                STEP_ASSERT_EQ(&instance1, &instance2);
            }
        }
    }
}

/*
 * Function: IsEnabled/SetEnable
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. save IsEnabled as e
 *                  2. SetEnable !e
 *                  3. check IsEnabled is !e
 */
HWTEST_F(RSOverdrawControllerTest, Enable, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        bool e = false;
        STEP("1. save IsEnabled as e") {
            e = RSOverdrawController::GetInstance().IsEnabled();
        }

        STEP("2. SetEnable !e") {
            RSOverdrawController::GetInstance().SetEnable(!e);
        }

        STEP("3. save IsEnabled as e") {
            STEP_ASSERT_EQ(RSOverdrawController::GetInstance().IsEnabled(), !e);
        }
    }
}

/*
 * Function: SwitchFunction (true/false)
 * Type: Function
 * EnvConditions: RSOverdrawController disabled
 * CaseDescription: 1. SwitchFunction value=true
 *                  2. IsEnabled is true
 *                  3. SwitchFunction value=false
 *                  4. IsEnabled is false
 */
HWTEST_F(RSOverdrawControllerTest, SwitchFunction, Function | SmallTest | Level2)
{
    PART("EnvConditions") {
        RSOverdrawController::GetInstance().SetEnable(false);
    }

    PART("CaseDescription") {
        STEP("1. SwitchFunction value=true") {
            RSOverdrawController::SwitchFunction("", "true", &RSOverdrawController::GetInstance());
        }

        STEP("2. IsEnabled is true") {
            STEP_ASSERT_EQ(RSOverdrawController::GetInstance().IsEnabled(), true);
        }

        STEP("3. SwitchFunction value=false") {
            RSOverdrawController::SwitchFunction("", "false", &RSOverdrawController::GetInstance());
        }

        STEP("4. IsEnabled is false") {
            STEP_ASSERT_EQ(RSOverdrawController::GetInstance().IsEnabled(), false);
        }
    }
}

class RSValidCanvasListener : public RSCanvasListener {
public:
    explicit RSValidCanvasListener(SkCanvas &canvas) : RSCanvasListener(canvas) {}

    bool IsValid() const override
    {
        return true;
    }

    const char *Name() const override
    {
        return "RSValidCanvasListener";
    }
};

/*
 * Function: SetHook in disabled
 * Type: Function
 * EnvConditions: RSOverdrawController disabled
 * CaseDescription: 1. SetHook<RSCanvasListener>(new MockRSPaintFilterCanvas) == nullptr
 *                  2. SetHook<RSCanvasListener>(nullptr) == nullptr
 *                  3. SetHook<RSValidCanvasListener>(new MockRSPaintFilterCanvas) == nullptr
 */
HWTEST_F(RSOverdrawControllerTest, SetHookDisable, Function | SmallTest | Level2)
{
    PART("EnvConditions") {
        RSOverdrawController::GetInstance().SetEnable(false);
    }

    PART("CaseDescription") {
        auto skCanvas = std::make_unique<SkCanvas>();
        RSPaintFilterCanvas *canvas = nullptr;
        STEP("1. SetHook<RSCanvasListener>(new MockRSPaintFilterCanvas) == nullptr") {
            auto mockRSPaintFilterCanvas = new MockRSPaintFilterCanvas(skCanvas.get());
            canvas = mockRSPaintFilterCanvas;
            auto listener = RSOverdrawController::GetInstance().SetHook<RSCanvasListener>(canvas);
            STEP_ASSERT_EQ(listener, nullptr);
            STEP_ASSERT_EQ(canvas, mockRSPaintFilterCanvas);
            delete canvas;
        }

        STEP("2. SetHook<RSCanvasListener>(nullptr) == nullptr") {
            canvas = nullptr;
            auto listener = RSOverdrawController::GetInstance().SetHook<RSCanvasListener>(canvas);
            STEP_ASSERT_EQ(listener, nullptr);
            STEP_ASSERT_EQ(canvas, nullptr);
        }

        STEP("3. SetHook<RSValidCanvasListener>(new MockRSPaintFilterCanvas) == nullptr") {
            auto mockRSPaintFilterCanvas = new MockRSPaintFilterCanvas(skCanvas.get());
            canvas = mockRSPaintFilterCanvas;
            auto listener = RSOverdrawController::GetInstance().SetHook<RSValidCanvasListener>(canvas);
            STEP_ASSERT_EQ(listener, nullptr);
            STEP_ASSERT_EQ(canvas, mockRSPaintFilterCanvas);
            delete canvas;
        }
    }
}

/*
 * Function: SetHook in enabled
 * Type: Function
 * EnvConditions: RSOverdrawController enabled
 * CaseDescription: 1. SetHook<RSCanvasListener>(nullptr) == nullptr
 *                  2. SetHook<RSCanvasListener>(new MockRSPaintFilterCanvas) == nullptr
 *                  3. SetHook<RSValidCanvasListener>(new MockRSPaintFilterCanvas) != nullptr
 */
HWTEST_F(RSOverdrawControllerTest, SetHookEnable, Function | SmallTest | Level2)
{
    PART("EnvConditions") {
        RSOverdrawController::GetInstance().SetEnable(true);
    }

    PART("CaseDescription") {
        auto skCanvas = std::make_unique<SkCanvas>();
        RSPaintFilterCanvas *canvas = nullptr;
        STEP("1. SetHook<RSCanvasListener>(nullptr) == nullptr") {
            canvas = nullptr;
            auto listener = RSOverdrawController::GetInstance().SetHook<RSCanvasListener>(canvas);
            STEP_ASSERT_EQ(listener, nullptr);
            STEP_ASSERT_EQ(canvas, nullptr);
        }

        STEP("2. SetHook<RSCanvasListener>(new MockRSPaintFilterCanvas) == nullptr") {
            auto mockRSPaintFilterCanvas = new MockRSPaintFilterCanvas(skCanvas.get());
            canvas = mockRSPaintFilterCanvas;
            auto listener = RSOverdrawController::GetInstance().SetHook<RSCanvasListener>(canvas);
            STEP_ASSERT_EQ(listener, nullptr);
            STEP_ASSERT_EQ(canvas, mockRSPaintFilterCanvas);
            delete canvas;
        }

        STEP("3. SetHook<RSValidCanvasListener>(new MockRSPaintFilterCanvas) != nullptr") {
            auto mockRSPaintFilterCanvas = new MockRSPaintFilterCanvas(skCanvas.get());
            canvas = mockRSPaintFilterCanvas;
            auto listener = RSOverdrawController::GetInstance().SetHook<RSValidCanvasListener>(canvas);
            STEP_ASSERT_NE(listener, nullptr);
            STEP_ASSERT_NE(canvas, mockRSPaintFilterCanvas);
            delete canvas;
        }
    }
}
} // namespace Rosen
} // namespace OHOS
