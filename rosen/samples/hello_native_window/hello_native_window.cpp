/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <iostream>
#include "external_window.h"
#include "surface.h"

using namespace OHOS;

namespace {
class BufferConsumerListenerTest : public ::OHOS::IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};
}

int32_t main(int32_t argc, const char *argv[])
{
    std::cout << "sample start" << std::endl;

    sptr<OHOS::Surface> cSurface = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListenerTest();
    cSurface->RegisterConsumerListener(listener);
    sptr<OHOS::IBufferProducer> producer = cSurface->GetProducer();
    sptr<OHOS::Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);

    OHNativeWindow* nativeWindow = OH_NativeWindow_CreateNativeWindow(&pSurface);

    int code = SET_USAGE;
    int32_t usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
    int32_t ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, usage);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeWindowHandleOpt SET_USAGE faile" << std::endl;
    }
    code = SET_BUFFER_GEOMETRY;
    int32_t width = 0x100;
    int32_t height = 0x100;
    ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, width, height);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeWindowHandleOpt SET_BUFFER_GEOMETRY failed" << std::endl;
    }
    code = SET_STRIDE;
    int32_t stride = 0x8;
    ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, stride);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeWindowHandleOpt SET_STRIDE failed" << std::endl;
    }
    code = SET_FORMAT;
    int32_t format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, format);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeWindowHandleOpt SET_FORMAT failed" << std::endl;
    }

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeWindowRequestBuffer failed" << std::endl;
    }

    if (OH_NativeWindow_GetBufferHandleFromNative(nativeWindowBuffer) == nullptr) {
        std::cout << "OH_NativeWindow_GetBufferHandleFromNative failed" << std::endl;
    }

    ret = OH_NativeWindow_NativeObjectReference(nativeWindowBuffer);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeObjectReference failed" << std::endl;
    }

    ret = OH_NativeWindow_NativeObjectUnreference(nativeWindowBuffer);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeObjectUnreference failed" << std::endl;
    }

    struct Region *region = new Region();
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0x100;
    rect->y = 0x100;
    rect->w = 0x100;
    rect->h = 0x100;
    region->rects = rect;
    region->rectNumber = 1;

    ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow, nativeWindowBuffer, fenceFd, *region);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeWindowFlushBuffer failed" << std::endl;
    }
    delete region;
    delete rect;

    int32_t sequence = 0;
    OHScalingMode scalingMode = OHScalingMode::OH_SCALING_MODE_SCALE_TO_WINDOW;
    ret = OH_NativeWindow_NativeWindowSetScalingMode(nativeWindow, sequence, scalingMode);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeWindowSetScalingMode failed" << std::endl;
    }

    int32_t size = 1;
    const OHHDRMetaData metaData[] = {{OH_METAKEY_RED_PRIMARY_X, 0}};
    ret = OH_NativeWindow_NativeWindowSetMetaData(nativeWindow, sequence, size, metaData);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeWindowSetMetaData failed" << std::endl;
    }

    OHHDRMetadataKey key = OHHDRMetadataKey::OH_METAKEY_HDR10_PLUS;
    const uint8_t metaDataSet[] = {0};
    ret = OH_NativeWindow_NativeWindowSetMetaDataSet(nativeWindow, sequence, key, size, metaDataSet);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeWindowSetMetaDataSet failed" << std::endl;
    }

    OHExtDataHandle *handle = new OHExtDataHandle();
    handle->fd = -1;
    handle->reserveInts = 1;
    handle->reserve[0] = 1;
    ret = OH_NativeWindow_NativeWindowSetTunnelHandle(nativeWindow, handle);
    if (ret != OHOS::GSERROR_OK) {
        std::cout << "OH_NativeWindow_NativeWindowSetTunnelHandle failed" << std::endl;
    }
    delete handle;
    std::cout << "sample end" << std::endl;
    return 0;
}