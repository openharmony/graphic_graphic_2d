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

#ifdef ROSEN_OHOS
#include "native_window.h"
#endif
#include "utils/canvas_utils.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

#ifdef ROSEN_OHOS
static std::unordered_map<Canvas*, OHNativeWindow*> canvasWindow_;
static std::mutex canvasWindowMutex_;
#endif

Canvas* CanvasUtils::CreateLockCanvas(OHNativeWindow* nativeWindow)
{
#ifdef ROSEN_OHOS
    OHNativeWindowBuffer* buffer = nullptr;
    int32_t ret = NativeWindowLockBuffer(nativeWindow, {}, &buffer);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("CreateLockCanvas NativeWindowLockBuffer failed");
        return nullptr;
    }
    if (buffer->sfbuffer == nullptr) {
        return nullptr;
    }

    ImageInfo imageInfo(buffer->sfbuffer->GetWidth(), buffer->sfbuffer->GetHeight(),
        ColorType::COLORTYPE_RGBA_8888, AlphaType::ALPHATYPE_PREMUL);
    Bitmap bitmap;
    bool result = bitmap.InstallPixels(imageInfo, buffer->sfbuffer->GetVirAddr(), buffer->sfbuffer->GetStride());
    if (!result) {
        LOGE("CreateLockCanvas Create bitmap failed");
        return nullptr;
    }

    Canvas* canvas = new Canvas();
    canvas->Bind(bitmap);
    std::lock_guard<std::mutex> lock(canvasWindowMutex_);
    canvasWindow_.insert({canvas, nativeWindow});
    return canvas;
#else
    return nullptr;
#endif
}

bool CanvasUtils::UnlockCanvas(Canvas* canvas, OHNativeWindow* nativeWindow)
{
#ifdef ROSEN_OHOS
    {
        std::lock_guard<std::mutex> lock(canvasWindowMutex_);
        auto iter = canvasWindow_.find(canvas);
        if (iter == canvasWindow_.end() || iter->second != nativeWindow) {
            LOGE("UnlockCanvas canvas is invalid");
            return false;
        }
        canvasWindow_.erase(iter);
    }

    int32_t ret = NativeWindowUnlockAndFlushBuffer(nativeWindow);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("UnlockCanvas NativeWindowUnlockAndFlushBuffer failed");
        return false;
    }

    return true;
#else
    return false;
#endif
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS