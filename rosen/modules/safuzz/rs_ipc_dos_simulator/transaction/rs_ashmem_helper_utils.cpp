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

#include "transaction/rs_ashmem_helper_utils.h"

#include <sys/mman.h>

#include "common/safuzz_log.h"
#include "random/random_data_basic_type.h"
#include "random/random_engine.h"
#include "transaction/rs_ashmem_helper.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<MessageParcel> RSAshmemHelperVariant::CreateAshmemParcel(std::shared_ptr<MessageParcel>& dataParcel)
{
    static constexpr float CHOOSE_RANDOM_PROBABILITY = 0.5;

    size_t dataSize = dataParcel->GetDataSize();

    // 1. save origin parcel data to a memfd and record the fd to new parcel
    // 2. save all fds and their offsets in new parcel, erase fd payloads in the memfd copy
    // 3. seal the memfd so the content is immutable for everyone
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    if (!ashmemAllocator) {
        SAFUZZ_LOGE("RSAshmemHelperVariant::CreateAshmemParcel failed, ashmemAllocator is nullptr");
        return nullptr;
    }
    if (!ashmemAllocator->WriteToAshmem(reinterpret_cast<void*>(dataParcel->GetData()), dataSize)) {
        SAFUZZ_LOGE("RSAshmemHelperVariant::CreateAshmemParcel: WriteToAshmem failed");
        return nullptr;
    }
    // 1. save data
    int fd = ashmemAllocator->GetFd();
    std::shared_ptr<MessageParcel> ashmemParcel = std::make_shared<MessageParcel>();
    ashmemParcel->WriteInt32(1); // 1: indicate ashmem parcel
    if (RandomEngine::ChooseByProbability(CHOOSE_RANDOM_PROBABILITY)) {
        ashmemParcel->WriteUint32(RandomDataBasicType::GetRandomUint32());
    } else {
        ashmemParcel->WriteUint32(dataSize);
    }
    ashmemParcel->WriteFileDescriptor(fd);

    // 2. save fds and their offsets
    size_t offsetSize = dataParcel->GetOffsetsSize();
    if (RandomEngine::ChooseByProbability(CHOOSE_RANDOM_PROBABILITY)) {
        ashmemParcel->WriteInt32(RandomDataBasicType::GetRandomInt32());
    } else {
        ashmemParcel->WriteInt32(offsetSize);
    }
    if (offsetSize > 0) {
        // save array that record the offsets of all fds
        ashmemParcel->WriteBuffer(
            reinterpret_cast<void*>(dataParcel->GetObjectOffsets()), sizeof(binder_size_t) * offsetSize);
        // save all fds of origin parcel; refuse ashmem parcel for non-fd objects
        if (!RSAshmemHelper::CopySupportedObjectsToParcel(ashmemParcel.get(), dataParcel)) {
            SAFUZZ_LOGE("RSAshmemHelperVariant::CreateAshmemParcel: CopySupportedObjectsToParcel failed");
            return nullptr;
        }
        // erase fd payloads in the memfd copy
        RSAshmemHelper::EraseSupportedObjectsInAshmem(ashmemAllocator->GetData(), *dataParcel);
    }

    // 3. seal the memfd to keep deep parse paths reachable on the receiver side
    if (!ashmemAllocator->Seal()) {
        SAFUZZ_LOGE("RSAshmemHelperVariant::CreateAshmemParcel: Seal failed");
        return nullptr;
    }

    return ashmemParcel;
}
} // namespace Rosen
} // namespace OHOS
