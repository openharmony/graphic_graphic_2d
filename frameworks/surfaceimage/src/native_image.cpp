/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "native_image.h"
#include "surface_image.h"
#include "window.h"

using namespace OHOS;

struct OH_NativeImage {
    OHOS::sptr<OHOS::SurfaceImage> consumer;
    OHOS::sptr<OHOS::IBufferProducer> producer;
};

OH_NativeImage* OH_NativeImage_Create(uint32_t textureId, uint32_t textureTarget)
{
    OHOS::sptr<OHOS::SurfaceImage> surfaceImage = new SurfaceImage(textureId, textureTarget);
    sptr<OHOS::IBufferProducer> producer = surfaceImage->GetProducer();
    OH_NativeImage* nativeImage = new OH_NativeImage();
    nativeImage->consumer = surfaceImage;
    nativeImage->producer = producer;
    sptr<IBufferConsumerListener> listener = new SurfaceImageListener(surfaceImage);
    nativeImage->consumer->RegisterConsumerListener(listener);
    return nativeImage;
}

OHNativeWindow* OH_NativeImage_AcquireNativeWindow(OH_NativeImage* image)
{
    if (image == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return nullptr;
    }
    sptr<OHOS::IBufferProducer> producer = image->producer;
    sptr<OHOS::Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    struct NativeWindow* nativeWindow = CreateNativeWindowFromSurface(&pSurface);
    return nativeWindow;
}

int32_t OH_NativeImage_AttachContext(OH_NativeImage* image, uint32_t textureId)
{
    if (image == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return SURFACE_ERROR_ERROR;
    }
    return image->consumer->AttachContext(textureId);
}

int32_t OH_NativeImage_DetachContext(OH_NativeImage* image)
{
    if (image == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return SURFACE_ERROR_ERROR;
    }
    return image->consumer->DetachContext();
}

int32_t OH_NativeImage_UpdateSurfaceImage(OH_NativeImage* image)
{
    if (image == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return SURFACE_ERROR_ERROR;
    }
    return image->consumer->UpdateSurfaceImage();
}

int64_t OH_NativeImage_GetTimestamp(OH_NativeImage* image)
{
    if (image == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return SURFACE_ERROR_ERROR;
    }
    return image->consumer->GetTimeStamp();
}

int32_t OH_NativeImage_GetTransformMatrix(OH_NativeImage* image, float matrix[16])
{
    if (image == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return SURFACE_ERROR_ERROR;
    }
    return image->consumer->GetTransformMatrix(matrix);
}

void OH_NativeImage_Destroy(OH_NativeImage** image)
{
    if (image == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return;
    }
    delete *image;
    *image = nullptr;
}

