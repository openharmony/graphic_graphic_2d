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
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    sptr<BufferExtraData> GetBufferExtraDataFromData()
    {
        // get data
        std::string key_Int32 = GetStringFromData(STR_LEN);
        int32_t value_Int32 = GetData<int32_t>();
        std::string key_Int64 = GetStringFromData(STR_LEN);
        int64_t value_Int64 = GetData<int64_t>();
        std::string key_Double = GetStringFromData(STR_LEN);
        double value_Double = GetData<double>();
        std::string key_Str = GetStringFromData(STR_LEN);
        std::string value_Str = GetStringFromData(STR_LEN);

        // test
        sptr<BufferExtraData> bedata = new BufferExtraDataImpl();
        bedata->ExtraSet(key_Int32, value_Int32);
        bedata->ExtraSet(key_Int64, value_Int64);
        bedata->ExtraSet(key_Double, value_Double);
        bedata->ExtraSet(key_Str, value_Str);

        return bedata;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < 0) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        std::string name = GetStringFromData(STR_LEN);
        bool isShared = GetData<bool>();
        BufferRequestConfig requestConfig = GetData<BufferRequestConfig>();
        BufferFlushConfig flushConfig = GetData<BufferFlushConfig>();
        uint32_t sequence = GetData<uint32_t>();
        int64_t timestamp = GetData<int64_t>();
        Rect damage = GetData<Rect>();
        uint32_t queueSize = GetData<uint32_t>();
        int32_t width = GetData<int32_t>();
        int32_t height = GetData<int32_t>();
        uint32_t usage = GetData<uint32_t>();
        TransformType transform = GetData<TransformType>();
        std::vector<HDRMetaData> metaData;
        for (int i = 0; i < 10; i++) { // add 10 elements to the vector
            HDRMetaData data = GetData<HDRMetaData>();
            metaData.push_back(data);
        }
        HDRMetadataKey key = GetData<HDRMetadataKey>();
        std::vector<uint8_t> metaDataSet;
        for (int i = 0; i < 10; i++) { // add 10 elements to the vector
            uint8_t data = GetData<uint8_t>();
            metaDataSet.push_back(data);
        }
        // test
        sptr<BufferQueue> bufferqueue = new BufferQueue(name, isShared);
        sptr<BufferExtraData> bedata = GetBufferExtraDataFromData();
        IBufferProducer::RequestBufferReturnValue retval;
        bufferqueue->RequestBuffer(requestConfig, bedata, retval);
        bufferqueue->CancelBuffer(sequence, bedata);
        sptr<SyncFence> syncFence = SyncFence::INVALID_FENCE;
        sptr<OHOS::SurfaceBuffer> buffer = nullptr;
        bufferqueue->FlushBuffer(sequence, bedata, syncFence, flushConfig);
        bufferqueue->AcquireBuffer(buffer, syncFence, timestamp, damage);
        bufferqueue->ReleaseBuffer(buffer, syncFence);
        bufferqueue->AttachBuffer(buffer);
        bufferqueue->DetachBuffer(buffer);
        bufferqueue->SetQueueSize(queueSize);
        bufferqueue->SetDefaultWidthAndHeight(width, height);
        bufferqueue->SetDefaultUsage(usage);
        bufferqueue->SetTransform(transform);
        bufferqueue->SetMetaData(sequence, metaData);
        bufferqueue->SetMetaDataSet(sequence, key, metaDataSet);
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

