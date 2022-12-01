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

#include <gtest/gtest.h>

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

} // namespace Rosen
} // namespace OHOS
