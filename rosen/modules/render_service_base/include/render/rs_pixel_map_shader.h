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

#ifndef RENDER_SERVICE_BASE_PIXEL_MAP_SHADER_H
#define RENDER_SERVICE_BASE_PIXEL_MAP_SHADER_H
#include "effect/shader_effect.h"
#include "pixel_map.h"
#include "utils/extend_object.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSPixelMapShader : public Drawing::ExtendObject {
public:
    RSPixelMapShader() = default;
    RSPixelMapShader(
        std::shared_ptr<Media::PixelMap> pixelMap, Drawing::TileMode tileX, Drawing::TileMode tileY,
        const Drawing::SamplingOptions& sampling, const Drawing::Matrix& matrix);
    ~RSPixelMapShader() override = default;
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override;
    bool Unmarshalling(Parcel& parcel) override;
#endif
    // Be Careful: this Function will return an Object Ptr which should be managed by Caller.
    void* GenerateBaseObject() override;
private:
    std::shared_ptr<Media::PixelMap> pixelMap_;
    Drawing::TileMode tileX_;
    Drawing::TileMode tileY_;
    Drawing::SamplingOptions sampling_;
    Drawing::Matrix matrix_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIXEL_MAP_SHADER_H