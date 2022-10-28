/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef GRAPHIC_STANDARD_COMPOSER_LAYER_CONTEXT_MOCK_H
#define GRAPHIC_STANDARD_COMPOSER_LAYER_CONTEXT_MOCK_H

#include "hdi_layer.h"

namespace OHOS {
namespace Rosen {
namespace MockSys {
class HdiLayerContext {
public:
    HdiLayerContext(IRect dstRect, IRect srcRect, uint32_t zOrder);
    virtual ~HdiLayerContext();
    std::shared_ptr<HdiLayerInfo> GetHdiLayer();
    GSError DrawBufferColor();
    GSError FillHdiLayer();
private:
    std::shared_ptr<HdiLayerInfo> hdiLayer_;
    sptr<Surface> pSurface_;
    sptr<Surface> cSurface_;
    IRect srcRect_;
    IRect dstRect_;
    uint32_t zOrder_ = 0;
    static void DrawColor(void *image, uint32_t width, uint32_t height);
};
} // namespace MockSys
} // namespace Rosen
} // namespace OHOS
#endif // GRAPHIC_STANDARD_COMPOSER_LAYER_CONTEXT_MOCK_H