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
    class MyPropertyDrawable : public RSPropertyDrawable {
    public:
        void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override {}
    };
};

void RSRenderContentTest::SetUpTestCase() {}
void RSRenderContentTest::TearDownTestCase() {}
void RSRenderContentTest::SetUp() {}
void RSRenderContentTest::TearDown() {}

/**
 * @tc.name: RegisterTaskDispatchTest001
 * @tc.desc: RegisterTaskDispatchFunc test.
 * @tc.type: FUNC
 * @tc.require: issuesI9J766
 */
HWTEST_F(RSRenderContentTest, RegisterTaskDispatchTest001, TestSize.Level1)
{
    RSRenderContent renderContent;
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas canvas(drawingCanvas.get());

    // propertyDrawablesVec_[0] is nullptr operation
    renderContent.propertyDrawablesVec_[0] = nullptr;
    renderContent.DrawPropertyDrawable(RSPropertyDrawableSlot::INVALID, canvas);

    // not recording canvas, draw directly
    canvas.SetRecordDrawable(false);
    canvas.SetRecordingState(true);
    std::unique_ptr<MyPropertyDrawable> notRecordingCanvas = std::make_unique<MyPropertyDrawable>();
    renderContent.propertyDrawablesVec_[1] = std::move(notRecordingCanvas);
    renderContent.DrawPropertyDrawable(RSPropertyDrawableSlot::SAVE_ALL, canvas);
}

/**
 * @tc.name: DrawPropertyDrawableRangeTest002
 * @tc.desc: DrawPropertyDrawableRange test.
 * @tc.type: FUNC
 * @tc.require: issuesI9J766
 */
HWTEST_F(RSRenderContentTest, DrawPropertyDrawableRangeTest002, TestSize.Level1)
{
    RSRenderContent renderContent;
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas canvas(drawingCanvas.get());

    std::unique_ptr<MyPropertyDrawable> recordingCanvas = std::make_unique<MyPropertyDrawable>();
    renderContent.propertyDrawablesVec_[0] = std::move(recordingCanvas);

    // not recording canvas, draw directly
    canvas.SetRecordDrawable(false);
    canvas.SetRecordingState(true);
    renderContent.DrawPropertyDrawableRange(RSPropertyDrawableSlot::INVALID, RSPropertyDrawableSlot::INVALID, canvas);
}
} // namespace OHOS::Rosen