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

#include "nativewindow_fuzzer.h"

#include <securec.h>
#include <vector>
#include <string>

#include "surface.h"
#include "surface_buffer.h"
#include "surface_buffer_impl.h"
#include "ibuffer_producer.h"
#include "native_window.h"

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
        constexpr int DEFAULT_FENCE = 100;
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;
    }

    class BufferConsumerListener : public IBufferConsumerListener {
    public:
        void OnBufferAvailable() override
        {
        }
    };

    /*
    * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    /*
    * get a string from g_data
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            char tmp = GetData<char>();
            if (tmp == '\0') {
                tmp = '1';
            }
            cstr[i] = tmp;
        }
        std::string str(cstr);
        return str;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // get data
        uint32_t seqNum = GetData<uint32_t>();
        int fenceFd = GetData<int>() % 32768; // maximum fd of linux is 32768
        // fd 0,1,2 represent stdin, stdout and stderr respectively, they should not be closed.
        if (fenceFd >= 0 && fenceFd <= 2) {
            fenceFd = DEFAULT_FENCE;
        }
        Region::Rect rect = GetData<Region::Rect>();
        int32_t rectNumber = GetData<int32_t>();
        Region region = {.rects = &rect, .rectNumber = rectNumber};
        uint32_t sequence = GetData<uint32_t>();
        OHScalingMode scalingMode = GetData<OHScalingMode>();
        OHHDRMetaData metaData = GetData<OHHDRMetaData>();
        OHHDRMetadataKey key = GetData<OHHDRMetadataKey>();
        uint8_t metaData2[STR_LEN];
        for (uint64_t i = 0; i < STR_LEN; i++) {
            metaData2[i] = GetData<uint8_t>();
        }
        uint32_t reserveInts = GetData<uint32_t>() % 0x100000; // no more than 0x100000

        // test
        sptr<OHOS::Surface> cSurface = Surface::CreateSurfaceAsConsumer();
        sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
        cSurface->RegisterConsumerListener(listener);
        sptr<OHOS::IBufferProducer> producer = cSurface->GetProducer();
        sptr<OHOS::Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
        cSurface->SetDefaultWidthAndHeight(0x100, 0x100); // width and height is 0x100
        OHNativeWindow* nativeWindow = CreateNativeWindowFromSurface(&pSurface);
        sptr<OHOS::SurfaceBuffer> sBuffer = new SurfaceBufferImpl(seqNum);
        OHNativeWindowBuffer* nwBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer);
        NativeWindowRequestBuffer(nativeWindow, &nwBuffer, &fenceFd);
        NativeObjectReference(nwBuffer);
        NativeWindowFlushBuffer(nativeWindow, nwBuffer, fenceFd, region);
        NativeWindowCancelBuffer(nativeWindow, nwBuffer);
        GetBufferHandleFromNative(nwBuffer);
        NativeWindowSetScalingMode(nativeWindow, sequence, scalingMode);
        std::vector<OHHDRMetaData> metaDatas = {metaData};
        NativeWindowSetMetaData(nativeWindow, sequence, metaDatas.size(), metaDatas.data());
        NativeWindowSetMetaDataSet(nativeWindow, sequence, key, STR_LEN, metaData2);
        GraphicExtDataHandle *handle = AllocExtDataHandle(reserveInts);
        NativeWindowSetTunnelHandle(nativeWindow, reinterpret_cast<OHExtDataHandle *>(handle));
        FreeExtDataHandle(handle);
        DestoryNativeWindow(nativeWindow);
        DestroyNativeWindowBuffer(nwBuffer);

        return true;
    }
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

