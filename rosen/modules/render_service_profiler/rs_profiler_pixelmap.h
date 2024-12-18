/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef HRP_PIXELMAP_H
#define HRP_PIXELMAP_H

namespace OHOS::Rosen {

using OHOS::sptr;
using OHOS::SurfaceBuffer;
using OHOS::Media::ImageInfo;
using OHOS::Media::PIXEL_MAP_ERR;
using OHOS::Media::PixelMap;
using OHOS::Media::PixelMemInfo;
using OHOS::Media::AllocatorType;

using ImageData = std::vector<uint8_t>;

class PixelMapStorage final {
public:
    static bool Pull(uint64_t id, const ImageInfo& info, PixelMemInfo& memory, size_t& skipBytes);
    static bool Push(uint64_t id, const ImageInfo& info, const PixelMemInfo& memory, size_t skipBytes);

    static bool Push(uint64_t id, const PixelMap& map);

private:
    static bool Fits(size_t size);

    static bool PullSharedMemory(uint64_t id, const ImageInfo& info, PixelMemInfo& memory, size_t& skipBytes);
    static void PushSharedMemory(uint64_t id, const ImageInfo& info, const PixelMemInfo& memory, size_t skipBytes);
    static void PushSharedMemory(uint64_t id, const PixelMap& map);

    static bool PullDmaMemory(uint64_t id, const ImageInfo& info, PixelMemInfo& memory, size_t& skipBytes);
    static void PushDmaMemory(uint64_t id, const ImageInfo& info, const PixelMemInfo& memory, size_t skipBytes);
    static void PushDmaMemory(uint64_t id, const PixelMap& map);

    static void PushImage(uint64_t id, const ImageData& data, size_t skipBytes, BufferHandle* buffer = nullptr);

    static bool IsSharedMemory(const PixelMap& map);
    static bool IsSharedMemory(const PixelMemInfo& memory);
    static bool IsSharedMemory(AllocatorType type);
    static bool IsDmaMemory(const PixelMap& map);
    static bool IsDmaMemory(const PixelMemInfo& memory);
    static bool IsDmaMemory(AllocatorType type);

    static bool ValidateBufferSize(const PixelMemInfo& memory);
    static uint32_t GetBytesPerPixel(const ImageInfo& info);

    static uint8_t* MapImage(int32_t file, size_t size, int32_t flags);
    static void UnmapImage(void* image, size_t size);

    static SurfaceBuffer* IncrementSurfaceBufferReference(sptr<SurfaceBuffer>& buffer);

    static bool IsDataValid(const void* data, size_t size);

    static bool CopyImageData(const uint8_t* srcImage, size_t srcSize, uint8_t* dstImage, size_t dstSize);
    static bool CopyImageData(const ImageData& data, uint8_t* dstImage, size_t dstSize);
    static bool CopyImageData(const Image* image, uint8_t* dstImage, size_t dstSize);

    static ImageData GenerateRawCopy(const uint8_t* data, size_t size);
    static ImageData GenerateMiniatureAstc(const uint8_t* data, size_t size);
    static ImageData GenerateMiniature(const uint8_t* data, size_t size, uint32_t pixelBytes);
    static ImageData GenerateImageData(const uint8_t* data, size_t size, const PixelMap& map);
    static ImageData GenerateImageData(const ImageInfo& info, const PixelMemInfo& memory);
    static ImageData GenerateImageData(const uint8_t* data, size_t size, bool isAstc, uint32_t pixelBytes);
};

} // OHOS::Rosen

#endif // HRP_PIXELMAP_H