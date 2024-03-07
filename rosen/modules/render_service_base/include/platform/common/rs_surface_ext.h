/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_SURFACE_EXT_H
#define RENDER_SERVICE_BASE_COMMON_RS_SURFACE_EXT_H
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <unistd.h>

#include "image/image.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSSurfaceExt : public std::enable_shared_from_this<RSSurfaceExt> {
public:
    static inline constexpr RSSurfaceExtType Type = RSSurfaceExtType::NONE;
    RSSurfaceExtType GetType() const
    {
        return Type;
    }
    RSSurfaceExt() = default;
    virtual ~RSSurfaceExt() = default;

    RSSurfaceExt(const RSSurfaceExt&) = delete;
    RSSurfaceExt(const RSSurfaceExt&&) = delete;
    RSSurfaceExt& operator=(const RSSurfaceExt&) = delete;
    RSSurfaceExt& operator=(const RSSurfaceExt&&) = delete;

#ifdef USE_SURFACE_TEXTURE
    virtual void DrawTextureImage(RSPaintFilterCanvas& canvas, bool freeze, const Drawing::Rect& clipRect) = 0;
    virtual void UpdateSurfaceDefaultSize(float width, float height) = 0;
    virtual void SetAttachCallback(const RSSurfaceTextureAttachCallBack& attachCallback) = 0;
    virtual void SetUpdateCallback(const RSSurfaceTextureUpdateCallBack& updateCallback) = 0;
    virtual void MarkUiFrameAvailable(bool available) = 0;
    virtual bool IsUiFrameAvailable() const = 0;
#endif
};

#ifdef USE_SURFACE_TEXTURE
using RSSurfaceTexture = RSSurfaceExt;
#endif
}
}
#endif // #define RENDER_SERVICE_BASE_COMMON_RS_SURFACE_EXT_H

