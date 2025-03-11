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

#ifndef RS_GRAPHIC_TEST_IMG_H
#define RS_GRAPHIC_TEST_IMG_H

#include "image_source.h"
#include "pixel_map.h"
#include "rs_graphic_test.h"

#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_modifier.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<OHOS::Media::PixelMap> DecodePixelMap(
    const std::string& pathName, const OHOS::Media::AllocatorType& allocatorType);
std::shared_ptr<OHOS::Rosen::RSCanvasNode> SetUpNodeBgImage(
    const std::string& pathName, const OHOS::Rosen::Vector4f bounds);
std::shared_ptr<OHOS::Rosen::RSCanvasNode> SetUpNodeBgImage(
    const uint8_t* data, uint32_t size, const OHOS::Rosen::Vector4f bounds);

class ImageCustomModifier : public OHOS::Rosen::RSContentStyleModifier {
public:
    ~ImageCustomModifier() = default;

    void Draw(OHOS::Rosen::RSDrawingContext& context) const;
    void SetWidth(int32_t width);
    void SetHeight(int32_t height);
    void SetPixelMapPath(std::string pathName);

private:
    std::shared_ptr<OHOS::Rosen::RSProperty<int32_t>> width_;
    std::shared_ptr<OHOS::Rosen::RSProperty<int32_t>> height_;
    std::shared_ptr<OHOS::Rosen::RSProperty<std::string>> pathName_;
};
} // namespace Rosen
} // namespace OHOS

#endif