/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include <message_parcel.h>
#include <securec.h>
#include <surface_buffer.h>
#include <sys/mman.h>

#include "media_errors.h"
#include "pixel_map.h"
#include "rs_profiler.h"
#include "rs_profiler_utils.h"

#include "transaction/rs_marshalling_helper.h"

namespace OHOS::Media {

struct UnmarshallingContext {
    UnmarshallingContext(uint64_t id, Parcel& parcel) : id { id }, parcel { parcel } {}

    static constexpr int headerLength = 24;                  // NOLINT
    static constexpr int maxPictureSize = 600 * 1024 * 1024; // NOLINT

    bool GatherImageFromFile(Rosen::ImageCacheRecord* record)
    {
        if (bufferSize <= 0 || bufferSize > UnmarshallingContext::maxPictureSize) {
            return false;
        }
        base = static_cast<uint8_t*>(malloc(bufferSize));
        if (!base) {
            return false;
        }
        if (memmove_s(base, bufferSize, record->image.get(), bufferSize) != 0) {
            delete base;
            base = nullptr;
            return false;
        }

        context = nullptr;
        return true;
    }

    uint64_t id = 0;
    Parcel& parcel;
    std::unique_ptr<PixelMap> pixelMap = std::make_unique<Media::PixelMap>();
    ImageInfo imgInfo;
    AllocatorType allocType = AllocatorType::DEFAULT;
    int32_t bufferSize = 0;
    uint8_t* base = nullptr;
    void* context = nullptr;
};

// This class has to be 'reimplemented' here to get access to the PixelMap's private functions.
// It works ONLY thanks to the 'friend class ImageSource' in PixelMap.
class ImageSource {
public:
    // See PixelMap::Unmarshalling
    // foundation/multimedia/image_framework/frameworks/innerkitsimpl/common/src/pixel_map.cpp
    static PixelMap* Unmarshalling(uint64_t id, Parcel& parcel);

private:
    // See foundation/multimedia/image_framework/frameworks/innerkitsimpl/utils/include/image_utils.h
    static int32_t GetPixelBytes(const PixelFormat& pixelFormat);
    static int32_t SurfaceBufferReference(void* buffer);

    static bool UnmarshallingInitContext(UnmarshallingContext& context);
    static bool UnmarshallFromSharedMem(UnmarshallingContext& context);
    static bool UnmarshallFromDMA(UnmarshallingContext& context);
    static PixelMap* UnmarshallingFinalize(UnmarshallingContext& context);
};

int32_t ImageSource::GetPixelBytes(const PixelFormat& pixelFormat)
{
    constexpr int32_t alphA8Bytes = 1;
    constexpr int32_t rgB565Bytes = 2;
    constexpr int32_t rgB888Bytes = 3;
    constexpr int32_t argB8888Bytes = 4;
    constexpr uint8_t rgbaF16Bytes = 8;
    constexpr int32_t astc4X4Bytes = 1;
    constexpr int32_t nV21Bytes = 2; // Each pixel is sorted on 3/2 bytes.

    int pixelBytes = 0;
    switch (pixelFormat) {
        case PixelFormat::ARGB_8888:
        case PixelFormat::BGRA_8888:
        case PixelFormat::RGBA_8888:
        case PixelFormat::CMYK:
            pixelBytes = argB8888Bytes;
            break;
        case PixelFormat::ALPHA_8:
            pixelBytes = alphA8Bytes;
            break;
        case PixelFormat::RGB_888:
            pixelBytes = rgB888Bytes;
            break;
        case PixelFormat::RGB_565:
            pixelBytes = rgB565Bytes;
            break;
        case PixelFormat::RGBA_F16:
            pixelBytes = rgbaF16Bytes;
            break;
        case PixelFormat::NV21:
        case PixelFormat::NV12:
            pixelBytes = nV21Bytes; // perl pixel 1.5 Bytes but return int so return 2
            break;
        case PixelFormat::ASTC_4x4:
        case PixelFormat::ASTC_6x6:
        case PixelFormat::ASTC_8x8:
            pixelBytes = astc4X4Bytes;
            break;
        default:
            break;
    }
    return pixelBytes;
}

int32_t ImageSource::SurfaceBufferReference(void* buffer)
{
    if (buffer == nullptr) {
        return ERR_SURFACEBUFFER_REFERENCE_FAILED;
    }
    auto* ref = reinterpret_cast<OHOS::RefBase*>(buffer);
    ref->IncStrongRef(ref);
    return SUCCESS;
}

bool ImageSource::UnmarshallingInitContext(UnmarshallingContext& context)
{
    if (!context.pixelMap) {
        return false;
    }

    if (!context.pixelMap->ReadImageInfo(context.parcel, context.imgInfo)) {
        return false;
    }

    context.pixelMap->SetEditable(context.parcel.ReadBool());

    const bool isAstc = context.parcel.ReadBool();
    context.pixelMap->SetAstc(isAstc);

    context.allocType = static_cast<AllocatorType>(context.parcel.ReadInt32());
    context.parcel.SkipBytes(sizeof(int32_t)); // unused csm
    int32_t rowDataSize = context.parcel.ReadInt32();
    context.bufferSize = context.parcel.ReadInt32();
    int32_t bytesPerPixel = GetPixelBytes(context.imgInfo.pixelFormat);
    if (bytesPerPixel == 0) {
        return false;
    }
    if ((!isAstc) && context.bufferSize != rowDataSize * context.imgInfo.size.height) {
        return false;
    }

    return true;
}

bool ImageSource::UnmarshallFromSharedMem(UnmarshallingContext& context)
{
    constexpr int32_t invalidFileDescriptor = -1;
    const int32_t fileDescriptor = Rosen::RSProfiler::IsParcelMock(context.parcel)
                                       ? invalidFileDescriptor
                                       : context.pixelMap->ReadFileDescriptor(context.parcel);
    if (fileDescriptor < 0) {
        auto imageCacheRecord = Rosen::RSProfiler::GetImage(context.id);
        if (imageCacheRecord && static_cast<uint32_t>(context.bufferSize) == imageCacheRecord->imageSize) {
            if (!context.GatherImageFromFile(imageCacheRecord)) {
                return false;
            }
            context.parcel.SkipBytes(UnmarshallingContext::headerLength);
        } else {
            return false;
        }
    } else {
        void* ptr = ::mmap(nullptr, context.bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0);
        if (ptr == MAP_FAILED) { // NOLINT
            ptr = ::mmap(nullptr, context.bufferSize, PROT_READ, MAP_SHARED, fileDescriptor, 0);
            if (ptr == MAP_FAILED) { // NOLINT
                ::close(fileDescriptor);
                return false;
            }
        }

        Rosen::RSProfiler::AddImage(context.id, ptr, context.bufferSize, context.headerLength);

        context.context = new int32_t();
        if (context.context == nullptr) {
            ::munmap(ptr, context.bufferSize);
            ::close(fileDescriptor);
            return false;
        }
        *static_cast<int32_t*>(context.context) = fileDescriptor;
        context.base = static_cast<uint8_t*>(ptr);
    }
    return true;
}

bool ImageSource::UnmarshallFromDMA(UnmarshallingContext& context)
{
    auto imageCacheRecord =
        Rosen::RSProfiler::IsParcelMock(context.parcel) ? Rosen::RSProfiler::GetImage(context.id) : nullptr;
    if (!imageCacheRecord) {
        const size_t size1 = context.parcel.GetReadableBytes();

        // original code
        sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
        surfaceBuffer->ReadFromMessageParcel(static_cast<MessageParcel&>(context.parcel));
        auto* virAddr = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
        void* nativeBuffer = surfaceBuffer.GetRefPtr();
        SurfaceBufferReference(nativeBuffer);
        context.base = virAddr;
        context.context = nativeBuffer;

        const size_t size2 = context.parcel.GetReadableBytes();

        Rosen::RSProfiler::AddImage(context.id, virAddr, context.bufferSize, size1 - size2);
    } else {
        context.allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
        context.parcel.SkipBytes(imageCacheRecord->skipBytes);
        if (!context.GatherImageFromFile(imageCacheRecord)) {
            return false;
        }
    }

    return true;
}

PixelMap* ImageSource::UnmarshallingFinalize(UnmarshallingContext& context)
{
    uint32_t ret = context.pixelMap->SetImageInfo(context.imgInfo);
    if (ret != SUCCESS) {
        if (context.pixelMap->freePixelMapProc_ != nullptr) {
            context.pixelMap->freePixelMapProc_(context.base, context.context, context.bufferSize);
        }
        context.pixelMap->ReleaseMemory(context.allocType, context.base, context.context, context.bufferSize);
        if (context.allocType == AllocatorType::SHARE_MEM_ALLOC && context.context != nullptr) {
            delete static_cast<int32_t*>(context.context);
        }

        return nullptr;
    }
    context.pixelMap->SetPixelsAddr(context.base, context.context, context.bufferSize, context.allocType, nullptr);
    if (!context.pixelMap->ReadTransformData(context.parcel, context.pixelMap.get())) {
        return nullptr;
    }
    if (!context.pixelMap->ReadAstcRealSize(context.parcel, context.pixelMap.get())) {
        return nullptr;
    }

    return context.pixelMap.release();
}

PixelMap* ImageSource::Unmarshalling(uint64_t id, Parcel& parcel)
{
    UnmarshallingContext context { id, parcel };

    if (!UnmarshallingInitContext(context)) {
        return nullptr;
    }

#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (context.allocType == AllocatorType::SHARE_MEM_ALLOC) {
        if (!UnmarshallFromSharedMem(context)) {
            return nullptr;
        }
    } else if (context.allocType == AllocatorType::DMA_ALLOC) {
        if (!UnmarshallFromDMA(context)) {
            return nullptr;
        }
    } else {
        context.base = context.pixelMap->ReadImageData(parcel, context.bufferSize);
        if (context.base == nullptr) {
            return nullptr;
        }
    }
#else
    base = ReadImageData(parcel, bufferSize);
    if (base == nullptr) {
        return false;
    }
#endif

    return UnmarshallingFinalize(context);
}

} // namespace OHOS::Media

namespace OHOS::Rosen {

using PixelMapHelper = Media::ImageSource;

Media::PixelMap* RSProfiler::UnmarshalPixelMap(Parcel& parcel)
{
    if (!IsEnabled()) {
        return Media::PixelMap::Unmarshalling(parcel);
    }
    const uint64_t id = Utils::ComposeNodeId(parcel.ReadInt32(), parcel.ReadInt32());
    return PixelMapHelper::Unmarshalling(id, parcel);
}

bool RSProfiler::MarshalPixelMap(Parcel& parcel, const std::shared_ptr<Media::PixelMap>& map)
{
    if (!map) {
        return false;
    }

    if (!IsEnabled()) {
        return map->Marshalling(parcel);
    }

    const pid_t pid = getpid();
    const uint32_t imageId = GenerateImageId();
    if (!(parcel.WriteInt32(pid) && parcel.WriteInt32(imageId))) {
        return false;
    }

    constexpr size_t imageSizeOffsetCorrection = 44;
    const size_t imageSizeOffset = parcel.GetWritePosition() + imageSizeOffsetCorrection;

    if (!map->Marshalling(parcel)) {
        return false;
    }

    if (!RSMarshallingHelper::GetUseSharedMem(std::this_thread::get_id())) {
        if (void* context = map->GetFd()) {
            const int32_t fileDescriptor = *static_cast<int32_t*>(context);
            const uint32_t imageSize = *reinterpret_cast<uint32_t*>(parcel.GetData() + imageSizeOffset);
            void* image = ::mmap(nullptr, imageSize, PROT_READ, MAP_SHARED, fileDescriptor, 0);
            if (image != MAP_FAILED) {
                constexpr uint32_t skipBytes = 24;
                AddImage(Utils::ComposeNodeId(pid, imageId), image, imageSize, skipBytes);
                ::munmap(image, imageSize);
            }
        }
    }

    return true;
}

} // namespace OHOS::Rosen