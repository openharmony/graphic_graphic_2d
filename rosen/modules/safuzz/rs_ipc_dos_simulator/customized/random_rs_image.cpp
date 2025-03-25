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

#include "customized/random_rs_image.h"

#include "customized/random_pixel_map.h"
#include "draw/paint.h"
#include "random/random_data.h"
#include "random/random_engine.h"
#include "utils/point.h"

namespace OHOS {
namespace Rosen {
namespace {
int GetRandomImageFit()
{
    static constexpr int IMAGE_FIT_INDEX_MAX = 17;
    return RandomEngine::GetRandomIndex(IMAGE_FIT_INDEX_MAX);
}

int GetRandomImageRepeat()
{
    static constexpr int IMAGE_REPEAT_INDEX_MAX = 3;
    return RandomEngine::GetRandomIndex(IMAGE_REPEAT_INDEX_MAX);
}
} // namespace

std::shared_ptr<RSImage> RandomRSImage::GetRandomRSImage()
{
    std::shared_ptr<RSImage> rsImage = std::make_shared<RSImage>();
    rsImage->SetImageRepeat(GetRandomImageRepeat());
    rsImage->SetImageFit(GetRandomImageFit());
    rsImage->SetImageRotateDegree(RandomData::GetRandomInt32());
    if (RandomData::GetRandomBool()) {
        rsImage->SetFitMatrix(RandomData::GetRandomDrawingMatrix());
    }
    rsImage->SetDynamicRangeMode(RandomData::GetRandomUint32());
    rsImage->SetFrameRect(RandomData::GetRandomRectF());
    rsImage->SetNodeId(RandomData::GetRandomUint64());
    rsImage->SetScale(RandomData::GetRandomFloat());
    std::vector<Drawing::Point> radius = {
        RandomData::GetRandomDrawingPoint(),
        RandomData::GetRandomDrawingPoint(),
        RandomData::GetRandomDrawingPoint(),
        RandomData::GetRandomDrawingPoint(),
    };
    rsImage->SetRadius(radius);
    if (RandomData::GetRandomBool()) {
        rsImage->SetInnerRect(RandomData::GetRandomDrawingRectI());
    }
    rsImage->SetDstRect(RandomData::GetRandomRectF());
    rsImage->SetSrcRect(RandomData::GetRandomRectF());
    if (RandomData::GetRandomBool()) {
        auto image = std::make_shared<Drawing::Image>();
        rsImage->SetDmaImage(image);
    }
    Drawing::Paint paint;
    rsImage->SetPaint(paint);
    rsImage->SetPixelMap(RandomPixelMap::GetRandomPixelMap());
    return rsImage;
}

std::vector<std::shared_ptr<RSImage>> RandomRSImage::GetRandomRSImageVector()
{
    std::vector<std::shared_ptr<RSImage>> images;
    int vectorLength = RandomEngine::GetRandomSmallVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        images.emplace_back(GetRandomRSImage());
    }
    return images;
}
} // namespace Rosen
} // namespace OHOS
