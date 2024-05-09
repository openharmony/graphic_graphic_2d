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
#include "rs_profiler_cache.h"
#include "rs_profiler_utils.h"

#include "transaction/rs_marshalling_helper.h"

namespace OHOS::Media {

struct UnmarshallingContext {
public:
    static constexpr int headerLength = 24; // NOLINT

public:
    explicit UnmarshallingContext(Parcel& parcel) : parcel(parcel) {}

    bool GatherImageFromFile(const Rosen::Image* image)
    {
        if ((size <= 0) || (size > Rosen::Image::maxSize)) {
            return false;
        }

        base = static_cast<uint8_t*>(malloc(size));
        if (!base) {
            return false;
        }

        if (!Rosen::Utils::Move(base, size, image->data.data(), size)) {
            delete base;
            base = nullptr;
            return false;
        }

        context = nullptr;
        return true;
    }

    bool GatherDmaImageFromFile(const Rosen::Image* image)
    {
        if ((size <= 0) || (size > Rosen::Image::maxSize)) {
            return false;
        }

        auto* buffer = reinterpret_cast<const BufferHandle*>(image->data.data());
        if (!buffer || (buffer->width == 0) || (buffer->height == 0)) {
            base = nullptr;
            context = nullptr;
            return false;
        }

        BufferRequestConfig config = {};
        config.width = buffer->width;
        config.strideAlignment = buffer->stride;
        config.height = buffer->height;
        config.format = buffer->format;
        config.usage = buffer->usage;

        sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
        surfaceBuffer->Alloc(config);

        base = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
        Rosen::Utils::Move(base, buffer->size, image->data.data() + sizeof(BufferHandle), buffer->size);

        void* nativeBuffer = surfaceBuffer.GetRefPtr();
        auto* ref = reinterpret_cast<OHOS::RefBase*>(nativeBuffer);
        ref->IncStrongRef(ref);
        context = nativeBuffer;
        return true;
    }

public:
    Parcel& parcel;
    std::unique_ptr<PixelMap> map = std::make_unique<Media::PixelMap>();
    ImageInfo info;
    AllocatorType allocType = AllocatorType::DEFAULT;
    size_t rowPitch = 0;
    size_t size = 0;
    uint8_t* base = nullptr;
    void* context = nullptr;
};

// This class has to be 'reimplemented' here to get access to the PixelMap's private functions.
// It works ONLY thanks to the 'friend class ImageSource' in PixelMap.
class ImageSource {
public:
    // See PixelMap::Unmarshalling
    // foundation/multimedia/image_framework/frameworks/innerkitsimpl/common/src/pixel_map.cpp
    static PixelMap* Unmarshal(Parcel& parcel);
    static bool Marshal(Parcel& parcel, PixelMap& map);

private:
    static uint64_t NewImageId();
    static bool CacheImage(uint64_t id, const void* data, size_t size, size_t skipBytes);
    static bool CacheImage(uint64_t id, std::vector<uint8_t> && data, size_t skipBytes);
    static Rosen::Image* GetCachedImage(uint64_t id);

    static uint8_t* MapImage(int32_t file, size_t size, int32_t flags);
    static void UnmapImage(void* image, size_t size);

    // See foundation/multimedia/image_framework/frameworks/innerkitsimpl/utils/include/image_utils.h
    static bool IsValidFormat(const PixelFormat& format);
    static void SurfaceBufferReference(void* buffer);

    static void OnClientMarshalling(PixelMap& map, uint64_t id);

    static bool InitUnmarshalling(UnmarshallingContext& context);
    static bool UnmarshalFromSharedMemory(UnmarshallingContext& context, uint64_t id);
    static bool UnmarshalFromDMA(UnmarshallingContext& context, uint64_t id);
    static bool UnmarshalFromData(UnmarshallingContext& context);
    static PixelMap* FinalizeUnmarshalling(UnmarshallingContext& context);
};

uint64_t ImageSource::NewImageId()
{
    return Rosen::ImageCache::New();
}

bool ImageSource::CacheImage(uint64_t id, const void* data, size_t size, size_t skipBytes)
{
    Rosen::Image image(reinterpret_cast<const uint8_t*>(data), size, skipBytes);
    return Rosen::ImageCache::Add(id, std::move(image));
}

bool ImageSource::CacheImage(uint64_t id, std::vector<uint8_t>&& data, size_t skipBytes)
{
    Rosen::Image image(std::move(data), skipBytes);
    return Rosen::ImageCache::Add(id, std::move(image));
}

Rosen::Image* ImageSource::GetCachedImage(uint64_t id)
{
    return Rosen::ImageCache::Get(id);
}

uint8_t* ImageSource::MapImage(int32_t file, size_t size, int32_t flags)
{
    auto image = ::mmap(nullptr, size, flags, MAP_SHARED, file, 0);
    return (image != MAP_FAILED) ? reinterpret_cast<uint8_t*>(image) : nullptr; // NOLINT
}

void ImageSource::UnmapImage(void* image, size_t size)
{
    if (image && size > 0) {
        ::munmap(image, size);
    }
}

bool ImageSource::IsValidFormat(const PixelFormat& format)
{
    return (format == PixelFormat::ARGB_8888) || (format == PixelFormat::BGRA_8888) ||
           (format == PixelFormat::RGBA_8888) || (format == PixelFormat::CMYK) || (format == PixelFormat::ALPHA_8) ||
           (format == PixelFormat::RGB_888) || (format == PixelFormat::RGB_565) || (format == PixelFormat::RGBA_F16) ||
           (format == PixelFormat::NV21) || (format == PixelFormat::NV12) || (format == PixelFormat::ASTC_4x4) ||
           (format == PixelFormat::ASTC_6x6) || (format == PixelFormat::ASTC_8x8);
}

void ImageSource::SurfaceBufferReference(void* buffer)
{
    if (buffer) {
        auto object = reinterpret_cast<OHOS::RefBase*>(buffer);
        object->IncStrongRef(object);
    }
}

bool ImageSource::InitUnmarshalling(UnmarshallingContext& context)
{
    if (!context.map) {
        return false;
    }

    if (!context.map->ReadImageInfo(context.parcel, context.info)) {
        return false;
    }

    context.map->SetEditable(context.parcel.ReadBool());

    const bool isAstc = context.parcel.ReadBool();
    context.map->SetAstc(isAstc);

    context.allocType = static_cast<AllocatorType>(context.parcel.ReadInt32());
    context.parcel.ReadInt32(); // unused csm
    context.rowPitch = context.parcel.ReadInt32();
    context.size = context.parcel.ReadInt32();

    const size_t rawSize = context.rowPitch * context.info.size.height;
    return IsValidFormat(context.info.pixelFormat) && (isAstc || (context.size == rawSize));
}

bool ImageSource::UnmarshalFromSharedMemory(UnmarshallingContext& context, uint64_t id)
{
    constexpr int32_t invalidFile = -1;
    const int32_t file =
        Rosen::RSProfiler::IsParcelMock(context.parcel) ? invalidFile : context.map->ReadFileDescriptor(context.parcel);
    if (file < 0) {
        auto image = GetCachedImage(id);
        if (image && (static_cast<uint32_t>(context.size) == image->data.size())) {
            if (context.GatherImageFromFile(image)) {
                context.parcel.SkipBytes(UnmarshallingContext::headerLength);
                return true;
            }
        }
        return false;
    }

    auto image = MapImage(file, context.size, PROT_READ | PROT_WRITE);
    if (!image) {
        image = MapImage(file, context.size, PROT_READ);
        if (!image) {
            ::close(file);
            return false;
        }
    }

    CacheImage(id, image, context.size, UnmarshallingContext::headerLength);

    context.context = new int32_t();
    if (!context.context) {
        UnmapImage(image, context.size);
        ::close(file);
        return false;
    }
    *static_cast<int32_t*>(context.context) = file;
    context.base = image;
    return true;
}

bool ImageSource::UnmarshalFromDMA(UnmarshallingContext& context, uint64_t id)
{
    auto image = Rosen::RSProfiler::IsParcelMock(context.parcel) ? GetCachedImage(id) : nullptr;
    if (image) {
        context.parcel.SkipBytes(image->skipBytes);
        return context.GatherDmaImageFromFile(image);
    }

    const size_t readPosition = context.parcel.GetReadPosition();

    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    surfaceBuffer->ReadFromMessageParcel(static_cast<MessageParcel&>(context.parcel));
    auto* virAddr = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
    void* nativeBuffer = surfaceBuffer.GetRefPtr();
    SurfaceBufferReference(nativeBuffer);
    context.base = virAddr;
    context.context = nativeBuffer;

    if (auto buffer = surfaceBuffer->GetBufferHandle()) {
        std::vector<uint8_t> data;
        data.resize(sizeof(BufferHandle) + buffer->size);
        Rosen::Utils::Move(data.data(), sizeof(BufferHandle), buffer, sizeof(BufferHandle));
        Rosen::Utils::Move(data.data() + sizeof(BufferHandle), buffer->size, virAddr, buffer->size);
        CacheImage(id, std::move(data), context.parcel.GetReadPosition() - readPosition);
    }

    return true;
}

bool ImageSource::UnmarshalFromData(UnmarshallingContext& context)
{
    context.base = context.map->ReadImageData(context.parcel, context.size);
    return (context.base != nullptr);
}

PixelMap* ImageSource::FinalizeUnmarshalling(UnmarshallingContext& context)
{
    if (context.map->SetImageInfo(context.info) != SUCCESS) {
        if (context.map->freePixelMapProc_) {
            context.map->freePixelMapProc_(context.base, context.context, context.size);
        }
        context.map->ReleaseMemory(context.allocType, context.base, context.context, context.size);
        if (context.context && (context.allocType == AllocatorType::SHARE_MEM_ALLOC)) {
            delete static_cast<int32_t*>(context.context);
        }

        return nullptr;
    }
    context.map->SetPixelsAddr(context.base, context.context, context.size, context.allocType, nullptr);
    if (!context.map->ReadTransformData(context.parcel, context.map.get())) {
        return nullptr;
    }
    if (!context.map->ReadAstcRealSize(context.parcel, context.map.get())) {
        return nullptr;
    }

    return context.map.release();
}

PixelMap* ImageSource::Unmarshal(Parcel& parcel)
{
    const uint64_t id = parcel.ReadUint64();
    UnmarshallingContext context { parcel };

    if (!InitUnmarshalling(context)) {
        return nullptr;
    }

#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (context.allocType == AllocatorType::SHARE_MEM_ALLOC) {
        if (!UnmarshalFromSharedMemory(context, id)) {
            return nullptr;
        }
    } else if (context.allocType == AllocatorType::DMA_ALLOC) {
        if (!UnmarshalFromDMA(context, id)) {
            return nullptr;
        }
    } else {
        if (!UnmarshalFromData(context)) {
            return nullptr;
        }
    }
#else
    if (!UnmarshalFromData(context)) {
        return nullptr;
    }
#endif

    return FinalizeUnmarshalling(context);
}

void ImageSource::OnClientMarshalling(Media::PixelMap& map, uint64_t id)
{
    if (Rosen::RSMarshallingHelper::GetUseSharedMem(std::this_thread::get_id())) {
        return;
    }

    if (auto file = static_cast<const int32_t*>(map.GetFd())) {
        const size_t size = map.isAstc_ ? map.pixelsSize_ : map.rowDataSize_ * map.imageInfo_.size.height;
        if (auto image = MapImage(*file, size, PROT_READ)) {
            CacheImage(id, image, size, UnmarshallingContext::headerLength);
            UnmapImage(image, size);
        }
    }
}

bool ImageSource::Marshal(Parcel& parcel, Media::PixelMap& map)
{
    const uint64_t id = NewImageId();
    if (!parcel.WriteUint64(id) || !map.Marshalling(parcel)) {
        return false;
    }

    OnClientMarshalling(map, id);
    return true;
}

} // namespace OHOS::Media

namespace OHOS::Rosen {

using PixelMapHelper = Media::ImageSource;

Media::PixelMap* RSProfiler::UnmarshalPixelMap(Parcel& parcel)
{
    if (!IsEnabled()) {
        return Media::PixelMap::Unmarshalling(parcel);
    }

    return PixelMapHelper::Unmarshal(parcel);
}

bool RSProfiler::MarshalPixelMap(Parcel& parcel, const std::shared_ptr<Media::PixelMap>& map)
{
    if (!map) {
        return false;
    }

    if (!IsEnabled()) {
        return map->Marshalling(parcel);
    }

    return PixelMapHelper::Marshal(parcel, *map);
}

} // namespace OHOS::Rosen