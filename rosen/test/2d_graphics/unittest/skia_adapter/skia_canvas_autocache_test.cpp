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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_canvas_autocache.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaCanvasAutoCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaCanvasAutoCacheTest::SetUpTestCase() {}
void SkiaCanvasAutoCacheTest::TearDownTestCase() {}
void SkiaCanvasAutoCacheTest::SetUp() {}
void SkiaCanvasAutoCacheTest::TearDown() {}

/**
 * @tc.name: SkiaCanvasAutoCache001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, SkiaCanvasAutoCache001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache != nullptr);
}

/**
 * @tc.name: Init001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, Init001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    SkMatrix m;
    skiaCanvasAutoCache->Init(m);
}

/**
 * @tc.name: OpCanCache001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, OpCanCache001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    auto v = skiaCanvasAutoCache->GetOpListDrawArea();
    ASSERT_TRUE(v.size() == 0);
}

/**
 * @tc.name: GetOpUnionRect001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, GetOpUnionRect001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache->GetOpUnionRect().isEmpty());
}

/**
 * @tc.name: GetOpsNum001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, GetOpsNum001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache->GetOpsNum() == 0);
}

/**
 * @tc.name: GetOpsPercent001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, GetOpsPercent001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache->GetOpsPercent() == 0);
}

/**
 * @tc.name: ShowDrawResult001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, ShowDrawResult001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    SkRect bound;
    skiaCanvasAutoCache->ShowDrawResult(bound);
}

/**
 * @tc.name: getBaseLayerSize001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, getBaseLayerSize001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    SkISize size = skiaCanvasAutoCache->getBaseLayerSize();
    ASSERT_TRUE(size.area() == 0);
}

/**
 * @tc.name: recordingContext001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, recordingContext001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache->recordingContext() == nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS