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

#include "bufferqueue_fuzzer.h"

#include <securec.h>

#include "buffer_queue.h"
#include "surface.h"
#include "surface_buffer.h"
#include "surface_buffer_impl.h"
#include "buffer_extra_data.h"
#include "buffer_extra_data_impl.h"
#include "sync_fence.h"

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;
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

    sptr<BufferExtraData> GetBufferExtraDataFromData()
    {
        // get data
        std::string keyInt32 = GetStringFromData(STR_LEN);
        int32_t valueInt32 = GetData<int32_t>();
        std::string keyInt64 = GetStringFromData(STR_LEN);
        int64_t valueInt64 = GetData<int64_t>();
        std::string keyDouble = GetStringFromData(STR_LEN);
        double valueDouble = GetData<double>();
        std::string keyStr = GetStringFromData(STR_LEN);
        std::string valueStr = GetStringFromData(STR_LEN);

        // test
        sptr<BufferExtraData> bedata = new BufferExtraDataImpl();
        bedata->ExtraSet(keyInt32, valueInt32);
        bedata->ExtraSet(keyInt64, valueInt64);
        bedata->ExtraSet(keyDouble, valueDouble);
        bedata->ExtraSet(keyStr, valueStr);

        return bedata;
    }

    void BufferQueueFuzzTest2()
    {
        // get data
        std::string name = GetStringFromData(STR_LEN);
        bool isShared = GetData<bool>();
        uint32_t queueSize = GetData<uint32_t>();
        int32_t width = GetData<int32_t>();
        int32_t height = GetData<int32_t>();
        uint64_t usage = GetData<uint64_t>();
        GraphicTransformType transform = GetData<GraphicTransformType >();
        uint32_t sequence = GetData<uint32_t>();
        std::vector<GraphicHDRMetaData> metaData;
        for (int i = 0; i < 10; i++) { // add 10 elements to the vector
            GraphicHDRMetaData hDRMetaData = GetData<GraphicHDRMetaData>();
            metaData.push_back(hDRMetaData);
        }
        GraphicHDRMetadataKey key = GetData<GraphicHDRMetadataKey>();
        std::vector<uint8_t> metaDataSet;
        for (int i = 0; i < 10; i++) { // add 10 elements to the vector
            uint8_t metaDataElement = GetData<uint8_t>();
            metaDataSet.push_back(metaDataElement);
        }
        bool flag = GetData<bool>();
        std::string result = GetStringFromData(STR_LEN);
        bool status = GetData<bool>();
        uint32_t reserveInts = GetData<uint32_t>() % 0x100000; // no more than 0x100000
        GraphicPresentTimestamp timestamp = GetData<GraphicPresentTimestamp>();

        // test
        sptr<BufferQueue> bufferqueue = new BufferQueue(name, isShared);
        bufferqueue->SetQueueSize(queueSize);
        bufferqueue->SetDefaultWidthAndHeight(width, height);
        bufferqueue->SetDefaultUsage(usage);
        bufferqueue->SetTransform(transform);
        bufferqueue->SetMetaData(sequence, metaData);
        bufferqueue->SetMetaDataSet(sequence, key, metaDataSet);
        bufferqueue->SetProducerCacheCleanFlag(flag);
        bufferqueue->Dump(result);
        bufferqueue->SetStatus(status);
        GraphicExtDataHandle *handle = AllocExtDataHandle(reserveInts);
        sptr<SurfaceTunnelHandle> tunnelHandle = new SurfaceTunnelHandle();
        tunnelHandle->SetHandle(handle);
        bufferqueue->SetTunnelHandle(tunnelHandle);
        FreeExtDataHandle(handle);
        bufferqueue->SetPresentTimestamp(sequence, timestamp);
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
        int32_t timeOut = 0;
        // get data
        std::string name = GetStringFromData(STR_LEN);
        bool isShared = GetData<bool>();
        uint32_t seqNum = GetData<uint32_t>();
        BufferRequestConfig requestConfig = GetData<BufferRequestConfig>();
        OHOS::Rect rect = GetData<OHOS::Rect>();
        int64_t timestamp = GetData<int64_t>();
        BufferFlushConfigWithDamages flushConfig = {.damages =  { rect }, .timestamp = timestamp};
        uint32_t sequence = GetData<uint32_t>();
        std::vector<Rect> damages;

        // test
        sptr<BufferQueue> bufferqueue = new BufferQueue(name, isShared);
        sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(seqNum);
        sptr<BufferExtraData> bedata = GetBufferExtraDataFromData();
        IBufferProducer::RequestBufferReturnValue retval;
        retval.buffer = buffer;
        bufferqueue->RequestBuffer(requestConfig, bedata, retval);
        bufferqueue->ReuseBuffer(requestConfig, bedata, retval);
        bufferqueue->CancelBuffer(sequence, bedata);
        sptr<SyncFence> syncFence = SyncFence::INVALID_FENCE;
        bufferqueue->FlushBuffer(sequence, bedata, syncFence, flushConfig);
        bufferqueue->DoFlushBuffer(sequence, bedata, syncFence, flushConfig);
        bufferqueue->AcquireBuffer(buffer, syncFence, timestamp, damages);
        bufferqueue->ReleaseBuffer(buffer, syncFence);
        bufferqueue->AttachBuffer(buffer, timeOut);
        bufferqueue->DetachBuffer(buffer);

        BufferQueueFuzzTest2();

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

