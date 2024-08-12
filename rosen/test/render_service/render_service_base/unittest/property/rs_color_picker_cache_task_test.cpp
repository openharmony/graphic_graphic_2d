/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "property/rs_color_picker_cache_task.h"
#include "skia_image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSColorPickerCacheTaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorPickerCacheTaskTest::SetUpTestCase() {}
void RSColorPickerCacheTaskTest::TearDownTestCase() {}
void RSColorPickerCacheTaskTest::SetUp() {}
void RSColorPickerCacheTaskTest::TearDown() {}

/**
 * @tc.name: RSColorPickerCacheTaskTest001
 * @tc.desc: RSColorPickerCacheTaskTest001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerCacheTaskTest, RSColorPickerCacheTaskTest001, TestSize.Level1)
{
    auto task = std::make_shared<RSColorPickerCacheTask>();
    EXPECT_FALSE(task->InitTask(nullptr));
    auto image = std::make_shared<Drawing::Image>();
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>();
    EXPECT_FALSE(task->InitTask(image));
    task->imageSnapshotCache_ = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(task->InitTask(image));
    task->SetWaitRelease(true);
    EXPECT_TRUE(task->GetWaitRelease());
    task->SetWaitRelease(false);
    EXPECT_FALSE(task->GetWaitRelease());
    task->SetShadowColorStrategy(true);
    EXPECT_TRUE(task->shadowColorStrategy_);
    task->SetShadowColorStrategy(false);
    EXPECT_FALSE(task->shadowColorStrategy_);
    task->SetIsShadow(true);
    EXPECT_TRUE(task->isShadow_);
    task->SetIsShadow(false);
    EXPECT_FALSE(task->isShadow_);
    EXPECT_FALSE(task->GetFirstGetColorFinished());
    int deviceWidth = 10;
    int deviceHeight = 10;
    EXPECT_FALSE(task->GetDeviceSize(deviceWidth, deviceHeight));
    task->SetDeviceSize(deviceWidth, deviceHeight);
    EXPECT_EQ(task->deviceWidth_, 10);
    EXPECT_EQ(task->deviceHeight_, 10);
    EXPECT_TRUE(task->GetDeviceSize(deviceWidth, deviceHeight));
    RSColor color;
    task->GetColorAverage(color);
    EXPECT_EQ(color.GetRed(), 0);
    for (int i = 0; i < 21; i++) {
        if (i == 0) {
            task->colorArray_.emplace_back(RSColor(255, 255, 255, 255));
        } else {
            task->colorArray_.emplace_back(RSColor(0, 0, 0, 0));
        }
    }
    task->GetColorAverage(color);
    EXPECT_EQ(color.GetRed(), 12);
    task->colorArray_.clear();
    EXPECT_EQ(task->colorArray_.size(), 0);
}

static std::shared_ptr<Drawing::Image> CreateDrawingImage(int width, int height)
{
    const Drawing::ImageInfo info(width, height, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE);
    auto surface(Drawing::Surface::MakeRaster(info));
    auto canvas = surface->GetCanvas();
    Drawing::Brush brush;
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(0, 0, width, height));
    canvas->DetachBrush();
    return surface->GetImageSnapshot();
}

/**
 * @tc.name: RSColorPickerCacheTaskTest002
 * @tc.desc: RSColorPickerCacheTaskTest002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerCacheTaskTest, RSColorPickerCacheTaskTest002, TestSize.Level1)
{
    auto task = std::make_shared<RSColorPickerCacheTask>();
    task->Reset();
    EXPECT_TRUE(task->waitRelease_);
    task->imageSnapshotCache_ = std::make_shared<Drawing::Image>();
    task->Reset();
    EXPECT_TRUE(task->waitRelease_);

    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    Drawing::Canvas canvas;
    auto paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    auto image = CreateDrawingImage(10, 10);
    auto pixelMap = std::make_shared<Drawing::Pixmap>();
    EXPECT_FALSE(task->GpuScaleImage(nullptr, image, pixelMap));
    EXPECT_FALSE(task->GpuScaleImage(paintFilterCanvas, image, pixelMap));
    image = CreateDrawingImage(150, 150);
    EXPECT_FALSE(task->GpuScaleImage(paintFilterCanvas, image, pixelMap));
    RSColor color;
    EXPECT_FALSE(task->GetColor(color));
}

/**
 * @tc.name: RSColorPickerCacheTaskTest003
 * @tc.desc: RSColorPickerCacheTaskTest003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerCacheTaskTest, RSColorPickerCacheTaskTest003, TestSize.Level1)
{
    auto task = std::make_shared<RSColorPickerCacheTask>();
    EXPECT_FALSE(task->Render());
    const Drawing::ImageInfo info(10, 10, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE);
    auto surface(Drawing::Surface::MakeRaster(info));
    task->cacheSurface_ = surface;
    EXPECT_FALSE(task->Render());
    task->cacheProcessStatus_.store(CacheProcessStatus::WAITING);
    EXPECT_FALSE(task->Render());
    task->cacheProcessStatus_.store(CacheProcessStatus::DONE);
    EXPECT_FALSE(task->Render());
    auto image = CreateDrawingImage(10, 10);
    task->imageSnapshotCache_ = CreateDrawingImage(10, 10);
    EXPECT_TRUE(task->InitTask(image));
    task->cacheProcessStatus_.store(CacheProcessStatus::DONE);
    task->cacheBackendTexture_ = Drawing::BackendTexture(true);
    EXPECT_FALSE(task->Render());
}

/**
 * @tc.name: RSColorPickerCacheTaskTest004
 * @tc.desc: RSColorPickerCacheTaskTest004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerCacheTaskTest, RSColorPickerCacheTaskTest004, TestSize.Level1)
{
    auto task = std::make_shared<RSColorPickerCacheTask>();
    EXPECT_FALSE(task->PostPartialColorPickerTask(nullptr, nullptr));
    RSColorPickerCacheTask::postColorPickerTask = [this](std::weak_ptr<RSColorPickerCacheTask> task) {};
    EXPECT_FALSE(task->PostPartialColorPickerTask(nullptr, nullptr));
    auto colorPickerCacheTask = std::make_shared<RSColorPickerCacheTask>();
    EXPECT_FALSE(task->PostPartialColorPickerTask(colorPickerCacheTask, nullptr));
    auto imageSnapshot = CreateDrawingImage(10, 10);
    EXPECT_FALSE(task->PostPartialColorPickerTask(colorPickerCacheTask, imageSnapshot));
    colorPickerCacheTask->imageSnapshotCache_ = CreateDrawingImage(10, 10);
    EXPECT_FALSE(task->PostPartialColorPickerTask(colorPickerCacheTask, imageSnapshot));
    colorPickerCacheTask->SetStatus(CacheProcessStatus::DONE);
    EXPECT_TRUE(task->PostPartialColorPickerTask(colorPickerCacheTask, imageSnapshot));
    colorPickerCacheTask->SetStatus(CacheProcessStatus::DOING);
    EXPECT_FALSE(task->PostPartialColorPickerTask(colorPickerCacheTask, imageSnapshot));
    task = std::make_shared<RSColorPickerCacheTask>();
    colorPickerCacheTask = std::make_shared<RSColorPickerCacheTask>();
    colorPickerCacheTask->imageSnapshotCache_ = CreateDrawingImage(10, 10);
    colorPickerCacheTask->SetStatus(CacheProcessStatus::DONE);
    EXPECT_TRUE(task->PostPartialColorPickerTask(colorPickerCacheTask, imageSnapshot));
}
} // namespace Rosen
} // namespace OHOS