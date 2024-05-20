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
#include <test_header.h>

#include "platform/ohos/overdraw/rs_canvas_listener.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr uint32_t SIZE1 = 5;
constexpr uint32_t SIZE2 = 6;
const char* SET_KEY = "overdraw-color";
const char* SET_VALUE1 = "1 2 3 4";
const char* SET_VALUE2 = "true";
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
    explicit MockRSPaintFilterCanvas(Drawing::Canvas *canvas) : RSPaintFilterCanvas(canvas) {}
    MOCK_METHOD1(DrawRect, void(const Drawing::Rect& rect));
};

class RSDelegateTest : public RSDelegate {
public:
    void Repaint()
    {
        return;
    }
};

/*
 * Function: Singleton is single
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. call GetInstance twice
 *                  2. check instance should be same
 */
HWTEST_F(RSOverdrawControllerTest, Singleton, Function | SmallTest | Level0)
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
HWTEST_F(RSOverdrawControllerTest, Enable, Function | SmallTest | Level0)
{
    PART("CaseDescription") {
        bool e = false;
        STEP("1. set enable as fasle") {
            RSOverdrawController::GetInstance().SetEnable(false);
            e = RSOverdrawController::GetInstance().IsEnabled();
            STEP_ASSERT_EQ(e, false);
        }

        STEP("2. set enable as false when enable is false") {
            RSOverdrawController::GetInstance().SetEnable(false);
            e = RSOverdrawController::GetInstance().IsEnabled();
            STEP_ASSERT_EQ(e, false);
        }

        STEP("3. set enable as true when enable is false") {
            RSOverdrawController::GetInstance().SetEnable(true);
            e = RSOverdrawController::GetInstance().IsEnabled();
            STEP_ASSERT_EQ(e, true);
        }

        STEP("4. set enable as true when enable is true") {
            RSOverdrawController::GetInstance().SetEnable(true);
            e = RSOverdrawController::GetInstance().IsEnabled();
            STEP_ASSERT_EQ(e, true);
        }

        STEP("5. set enable as false when enable is true") {
            RSOverdrawController::GetInstance().SetEnable(false);
            e = RSOverdrawController::GetInstance().IsEnabled();
            STEP_ASSERT_EQ(e, false);
        }
    }
}

class RSValidCanvasListener : public RSCanvasListener {
public:
    explicit RSValidCanvasListener(Drawing::Canvas &canvas) : RSCanvasListener(canvas) {}
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
 * Function: CreateListener in disabled
 * Type: Function
 * EnvConditions: RSOverdrawController disabled
 * CaseDescription: 1. CreateListener<RSCanvasListener>(MockRSPaintFilterCanvas) == nullptr
 *                  2. CreateListener<RSCanvasListener>(nullptr) == nullptr
 *                  3. CreateListener<RSValidCanvasListener>(MockRSPaintFilterCanvas) == nullptr
 */
HWTEST_F(RSOverdrawControllerTest, CreateListenerDisable, Function | SmallTest | Level2)
{
    auto &controller = RSOverdrawController::GetInstance();
    PART("EnvConditions") {
        controller.SetEnable(false);
    }

    PART("CaseDescription") {
        auto tmpCanvas = std::make_unique<Drawing::Canvas>();
        std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
        STEP("1. CreateListener<RSCanvasListener>(MockRSPaintFilterCanvas) == nullptr") {
            auto mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(tmpCanvas.get());
            canvas = mockRSPaintFilterCanvas;
            auto listener = controller.CreateListener<RSCanvasListener>(canvas.get());
            STEP_ASSERT_EQ(listener, nullptr);
        }

        STEP("2. CreateListener<RSCanvasListener>(nullptr) == nullptr") {
            canvas = nullptr;
            auto listener = controller.CreateListener<RSCanvasListener>(canvas.get());
            STEP_ASSERT_EQ(listener, nullptr);
        }

        STEP("3. CreateListener<RSValidCanvasListener>(MockRSPaintFilterCanvas) == nullptr") {
            auto mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(tmpCanvas.get());
            canvas = mockRSPaintFilterCanvas;
            auto listener = controller.CreateListener<RSValidCanvasListener>(canvas.get());
            STEP_ASSERT_EQ(listener, nullptr);
        }
    }
}

/*
 * Function: CreateListener in enabled
 * Type: Function
 * EnvConditions: RSOverdrawController enabled
 * CaseDescription: 1. CreateListener<RSCanvasListener>(nullptr) == nullptr
 *                  2. CreateListener<RSCanvasListener>(MockRSPaintFilterCanvas) == nullptr
 *                  3. CreateListener<RSValidCanvasListener>(MockRSPaintFilterCanvas) != nullptr
 */
HWTEST_F(RSOverdrawControllerTest, CreateListenerEnable, Function | SmallTest | Level2)
{
    auto &controller = RSOverdrawController::GetInstance();
    PART("EnvConditions") {
        controller.SetEnable(true);
    }

    PART("CaseDescription") {
        auto tmpCanvas = std::make_unique<Drawing::Canvas>();
        std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
        STEP("1. CreateListener<RSCanvasListener>(nullptr) == nullptr") {
            canvas = nullptr;
            auto listener = controller.CreateListener<RSCanvasListener>(canvas.get());
            STEP_ASSERT_EQ(listener, nullptr);
        }

        STEP("2. CreateListener<RSCanvasListener>(MockRSPaintFilterCanvas) == nullptr") {
            auto mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(tmpCanvas.get());
            canvas = mockRSPaintFilterCanvas;
            auto listener = controller.CreateListener<RSCanvasListener>(canvas.get());
            STEP_ASSERT_EQ(listener, nullptr);
        }

        STEP("3. CreateListener<RSValidCanvasListener>(MockRSPaintFilterCanvas) != nullptr") {
            auto mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(tmpCanvas.get());
            canvas = mockRSPaintFilterCanvas;
            auto listener = controller.CreateListener<RSValidCanvasListener>(canvas.get());
            STEP_ASSERT_NE(listener, nullptr);
        }
    }
}
/**
 * @tc.name: OnColorChangeTest001
 * @tc.desc: OnColorChange Test
 * @tc.type:FUNC
 * @tc.require:issueI9NDED
 */
HWTEST_F(RSOverdrawControllerTest, OnColorChangeTest001, TestSize.Level1)
{
    auto& controller = RSOverdrawController::GetInstance();
    controller.SetEnable(false);
    EXPECT_FALSE(controller.IsEnabled());
    controller.OnColorChange(SET_KEY, SET_VALUE1, &controller);

    auto colorArrayTest = controller.GetColorArray();
    auto ColorMap = controller.GetColorMap();
    EXPECT_EQ(ColorMap.size(), SIZE1);
    EXPECT_EQ(colorArrayTest.size(), SIZE2);
}

/**
 * @tc.name: SwitchFunctionTest002
 * @tc.desc: SwitchFunctio Test
 * @tc.type:FUNC
 * @tc.require:issueI9NDED
 */
HWTEST_F(RSOverdrawControllerTest, SwitchFunctionTest002, TestSize.Level1)
{
    auto& controller = RSOverdrawController::GetInstance();
    controller.SetEnable(true);
    EXPECT_TRUE(controller.IsEnabled());

    std::shared_ptr<RSDelegateTest> delegate = std::make_shared<RSDelegateTest>();
    EXPECT_TRUE(delegate);

    controller.SetDelegate(delegate);
    EXPECT_TRUE(controller.delegate_);
    controller.SwitchFunction(SET_KEY, SET_VALUE2, &controller);
    EXPECT_TRUE(controller.IsEnabled());
    controller.SwitchFunction(SET_KEY, "", &controller);
    EXPECT_FALSE(controller.IsEnabled());
}
} // namespace Rosen
} // namespace OHOS
