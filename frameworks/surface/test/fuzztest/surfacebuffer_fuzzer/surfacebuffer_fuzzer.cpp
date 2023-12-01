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

#include "surfacebuffer_fuzzer.h"

#include <securec.h>

#include "surface_buffer.h"
#include "surface_buffer_impl.h"
#include "buffer_extra_data.h"
#include "buffer_extra_data_impl.h"
#include <message_parcel.h>
#include <iostream>

namespace OHOS {
    namespace {
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;
        constexpr size_t STR_LEN = 10;
        constexpr int32_t MAX_SIZE = 1024;
    }

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
        GraphicColorGamut colorGamut = GetData<GraphicColorGamut>();
        GraphicTransformType transform = GetData<GraphicTransformType>();
        int32_t width = GetData<int32_t>();
        int32_t height = GetData<int32_t>();
        width = width > MAX_SIZE ? MAX_SIZE : width;
        width = width < 0 ? 0 : width;
        height = height > MAX_SIZE ? MAX_SIZE : height;
        height = height < 0 ? 0 : height;
        BufferRequestConfig config = GetData<BufferRequestConfig>();
        std::string keyInt32 = GetStringFromData(STR_LEN);
        int32_t valueInt32 = GetData<int32_t>();
        std::string keyInt64 = GetStringFromData(STR_LEN);
        int64_t valueInt64 = GetData<int64_t>();
        std::string keyDouble = GetStringFromData(STR_LEN);
        double valueDouble = GetData<double>();
        std::string keyStr = GetStringFromData(STR_LEN);
        std::string valueStr = GetStringFromData(STR_LEN);

        // test
        sptr<SurfaceBuffer> surfaceBuffer = new SurfaceBufferImpl(seqNum);
        surfaceBuffer->SetSurfaceBufferColorGamut(colorGamut);
        surfaceBuffer->SetSurfaceBufferTransform(transform);
        surfaceBuffer->SetSurfaceBufferWidth(width);
        surfaceBuffer->SetSurfaceBufferHeight(height);
        surfaceBuffer->Alloc(config);
        sptr<BufferExtraData> bedata = new BufferExtraDataImpl();
        bedata->ExtraSet(keyInt32, valueInt32);
        bedata->ExtraSet(keyInt64, valueInt64);
        bedata->ExtraSet(keyDouble, valueDouble);
        bedata->ExtraSet(keyStr, valueStr);
        surfaceBuffer->SetExtraData(bedata);
        MessageParcel parcel;
        surfaceBuffer->WriteToMessageParcel(parcel);
        surfaceBuffer->ReadFromMessageParcel(parcel);

        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

