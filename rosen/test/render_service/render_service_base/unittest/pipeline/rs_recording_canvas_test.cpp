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

#ifndef USE_ROSEN_DRAWING
#include <gtest/gtest.h>
#include <include/core/SkTextBlob.h>

#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_recording_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRecordingCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRecordingCanvasTest::SetUpTestCase() {}
void RSRecordingCanvasTest::TearDownTestCase() {}
void RSRecordingCanvasTest::SetUp() {}
void RSRecordingCanvasTest::TearDown() {}

/**
 * @tc.name: Clear001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRecordingCanvasTest, Clear001, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    RSRecordingCanvas canvas(width, height);
    canvas.Clear();
}

/**
 * @tc.name: willRestore001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRecordingCanvasTest, willRestore001, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    RSRecordingCanvas canvas(width, height);
    canvas.willRestore();
}

/**
 * @tc.name: onDrawPath001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRecordingCanvasTest, onDrawPath001, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    RSRecordingCanvas canvas(width, height);
    SkPath path;
    SkPaint paint;
    canvas.onDrawPath(path, paint);
}

/**
 * @tc.name: RSRecordingCanvas001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRecordingCanvasTest, RSRecordingCanvas001, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    RSRecordingCanvas canvas(width, height);
    canvas.willRestore();
    SkPaint skPaint;
    canvas.onDrawBehind(skPaint);
    canvas.Clear();
}

/**
 * @tc.name: RSRecordingCanvas002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRecordingCanvasTest, RSRecordingCanvas002, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    RSRecordingCanvas canvas(width, height);
    canvas.willSave();
    canvas.willRestore();
    sk_sp<SkTextBlob> blob = SkTextBlob::MakeFromString("TextBlob", SkFont(nullptr, 24.0f, 1.0f, 0.0f));
    SkScalar x = 1.0;
    SkScalar y = 1.0;
    SkPaint paint;
    canvas.SetIsCustomTextType(false);
    ASSERT_FALSE(canvas.IsCustomTextType());
    canvas.onDrawTextBlob(blob.get(), x, y, paint);
    canvas.SetIsCustomTextType(true);
    ASSERT_TRUE(canvas.IsCustomTextType());
    canvas.onDrawTextBlob(blob.get(), x, y, paint);
    canvas.Clear();
}
} // namespace Rosen
} // namespace OHOS
#endif
