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

#include "gtest/gtest.h"
#include "pipeline/rs_render_content.h"
#include "pipeline/rs_recording_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderContentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderContentTest::SetUpTestCase() {}
void RSRenderContentTest::TearDownTestCase() {}
void RSRenderContentTest::SetUp() {}
void RSRenderContentTest::TearDown() {}

class MyPropertyDrawable : public RSPropertyDrawable {
public:
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override
    {
        return;
    }
};

/**
 * @tc.name: RegisterTaskDispatchTest001
 * @tc.desc: RegisterTaskDispatchFunc test.
 * @tc.type: FUNC
 * @tc.require: issueI9VT6E
 */
HWTEST_F(RSRenderContentTest, RegisterTaskDispatchTest001, TestSize.Level1)
{
    std::shared_ptr<RSRenderContent> renderContent = std::make_shared<RSRenderContent>();
    ASSERT_NE(renderContent, nullptr);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas canvas(drawingCanvas.get());
    // propertyDrawablesVec_[0] is nullptr operation
    renderContent->propertyDrawablesVec_[0] = nullptr;
    renderContent->DrawPropertyDrawable(RSPropertyDrawableSlot::INVALID, canvas);

    // not recording canvas, draw directly
    canvas.SetRecordingState(false);
    canvas.SetRecordDrawable(false);
    std::unique_ptr<MyPropertyDrawable> notRecordingCanvasTest1 = std::make_unique<MyPropertyDrawable>();
    renderContent->propertyDrawablesVec_[1] = std::move(notRecordingCanvasTest1);
    renderContent->DrawPropertyDrawable(RSPropertyDrawableSlot::SAVE_ALL, canvas);

    canvas.SetRecordingState(true);
    canvas.SetRecordDrawable(false);
    std::unique_ptr<MyPropertyDrawable> notRecordingCanvasTest2 = std::make_unique<MyPropertyDrawable>();
    renderContent->propertyDrawablesVec_[2] = std::move(notRecordingCanvasTest2);
    renderContent->DrawPropertyDrawable(RSPropertyDrawableSlot::BOUNDS_MATRIX, canvas);

    canvas.SetRecordingState(true);
    canvas.SetRecordDrawable(true);
    std::unique_ptr<MyPropertyDrawable> notRecordingCanvasTest3 = std::make_unique<MyPropertyDrawable>();
    renderContent->propertyDrawablesVec_[3] = std::move(notRecordingCanvasTest3);
    ExtendRecordingCanvas extendRecordingCanvas(0, 1);
    extendRecordingCanvas.addDrawOpImmediate_ = false;
    canvas.canvas_ = &extendRecordingCanvas;
    renderContent->DrawPropertyDrawable(RSPropertyDrawableSlot::ALPHA, canvas);
}

/**
 * @tc.name: DrawPropertyDrawableRangeTest002
 * @tc.desc: DrawPropertyDrawableRange test.
 * @tc.type: FUNC
 * @tc.require: issueI9VT6E
 */
HWTEST_F(RSRenderContentTest, DrawPropertyDrawableRangeTest002, TestSize.Level1)
{
    std::shared_ptr<RSRenderContent> renderContent = std::make_shared<RSRenderContent>();
    ASSERT_NE(renderContent, nullptr);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas canvas(drawingCanvas.get());

    std::unique_ptr<MyPropertyDrawable> recordingCanvas = std::make_unique<MyPropertyDrawable>();
    renderContent->propertyDrawablesVec_[0] = std::move(recordingCanvas);

    // not recording canvas, draw directly
    canvas.SetRecordingState(false);
    canvas.SetRecordDrawable(false);
    renderContent->DrawPropertyDrawableRange(RSPropertyDrawableSlot::INVALID, RSPropertyDrawableSlot::INVALID, canvas);

    canvas.SetRecordingState(true);
    canvas.SetRecordDrawable(false);
    std::unique_ptr<MyPropertyDrawable> notRecordingCanvasTest2 = std::make_unique<MyPropertyDrawable>();
    renderContent->propertyDrawablesVec_[2] = std::move(notRecordingCanvasTest2);
    renderContent->DrawPropertyDrawableRange(
        RSPropertyDrawableSlot::BOUNDS_MATRIX, RSPropertyDrawableSlot::BOUNDS_MATRIX, canvas);

    canvas.SetRecordDrawable(true);
    canvas.SetRecordingState(true);
    std::unique_ptr<MyPropertyDrawable> notRecordingCanvasTest3 = std::make_unique<MyPropertyDrawable>();
    renderContent->propertyDrawablesVec_[3] = std::move(notRecordingCanvasTest3);
    ExtendRecordingCanvas extendRecordingCanvas(0, 1);
    extendRecordingCanvas.addDrawOpImmediate_ = false;
    canvas.canvas_ = &extendRecordingCanvas;
    renderContent->DrawPropertyDrawableRange(RSPropertyDrawableSlot::ALPHA, RSPropertyDrawableSlot::ALPHA, canvas);
}
} // namespace OHOS::Rosen