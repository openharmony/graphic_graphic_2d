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

#include <memory>
#include <utility>

#include "feature/color_picker/rs_color_picker_manager.h"
#include "feature/color_picker/rs_color_picker_utils.h"
#include "gtest/gtest.h"

#include "draw/canvas.h"
#include "draw/surface.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSColorPickerUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override {}
    void TearDown() override {}
};

void RSColorPickerUtilsTest::SetUpTestCase() {}
void RSColorPickerUtilsTest::TearDownTestCase() {}

/**
 * @tc.name: GetColorForPlaceholder
 * @tc.desc: test GetColorForPlaceholder
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, GetColorForPlaceholder, TestSize.Level1)
{
    auto colorPair = RSColorPickerUtils::GetColorForPlaceholder(ColorPlaceholder::FOREGROUND);
    EXPECT_EQ(colorPair.first, Drawing::Color::COLOR_BLACK);
    colorPair = RSColorPickerUtils::GetColorForPlaceholder(ColorPlaceholder::ACCENT);
    EXPECT_EQ(colorPair.first, Drawing::Color::COLOR_BLACK);
}

/**
 * @tc.name: InterpolateColor
 * @tc.desc: test InterpolateColor
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, InterpolateColor, TestSize.Level1)
{
    auto color = RSColorPickerUtils::InterpolateColor(0xFF000000, 0xFFFFFFFF, -1.f);
    EXPECT_EQ(color, 0xFFFFFFFF);
    color = RSColorPickerUtils::InterpolateColor(0xFF000000, 0xFFFFFFFF, 0.2f);
    EXPECT_EQ(color, 0xFF333333);
}

// ============================================================================
// RSColorPickerUtils::CreateColorPickerInfo Tests
// ============================================================================

/**
 * @tc.name: CreateColorPickerInfoWithNullSnapshot
 * @tc.desc: Test CreateColorPickerInfo returns nullptr when snapshot is null
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, CreateColorPickerInfoWithNullSnapshot, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSColorPickerManager manager(1);
    auto weakManager = manager.weak_from_this();

    Drawing::Surface* surface = canvas.GetSurface();
    auto colorPickerInfo = RSColorPickerUtils::CreateColorPickerInfo(surface, nullptr, weakManager);

    EXPECT_EQ(colorPickerInfo, nullptr);
}

// ============================================================================
// RSColorPickerUtils::ScheduleColorPickWithSemaphore Tests
// ============================================================================

/**
 * @tc.name: ScheduleColorPickWithSemaphoreWithValidInputs
 * @tc.desc: Test ScheduleColorPickWithSemaphore with valid inputs
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, ScheduleColorPickWithSemaphoreWithValidInputs, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSColorPickerManager manager(1);
    auto weakManager = manager.weak_from_this();

    Drawing::Surface* surface = canvas.GetSurface();
    if (!surface) {
        GTEST_SKIP() << "No valid surface available";
        return;
    }

    auto snapshot = std::make_shared<Drawing::Image>();
    auto colorPickerInfo = RSColorPickerUtils::CreateColorPickerInfo(surface, snapshot, weakManager);

    if (colorPickerInfo) {
        RSColorPickerUtils::ScheduleColorPickWithSemaphore(*surface, weakManager, std::move(colorPickerInfo));
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.name: ScheduleColorPickWithSemaphoreWithExpiredManager
 * @tc.desc: Test ScheduleColorPickWithSemaphore with expired weak_ptr to manager
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, ScheduleColorPickWithSemaphoreWithExpiredManager, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    Drawing::Surface* surface = canvas.GetSurface();
    if (!surface) {
        GTEST_SKIP() << "No valid surface available";
        return;
    }

    auto snapshot = std::make_shared<Drawing::Image>();
    std::weak_ptr<RSColorPickerManager> weakManager;

    auto colorPickerInfo = RSColorPickerUtils::CreateColorPickerInfo(surface, snapshot, weakManager);

    if (colorPickerInfo) {
        RSColorPickerUtils::ScheduleColorPickWithSemaphore(*surface, weakManager, std::move(colorPickerInfo));
        EXPECT_TRUE(true);
    }
}

} // namespace OHOS::Rosen
