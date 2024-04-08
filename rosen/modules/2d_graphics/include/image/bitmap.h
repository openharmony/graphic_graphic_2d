/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef BITMAP_H
#define BITMAP_H

#include "drawing/engine_adapter/impl_interface/bitmap_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct BitmapFormat {
    ColorType colorType;
    AlphaType alphaType;
};

class DRAWING_API Bitmap {
public:
    Bitmap();
    virtual ~Bitmap();
    bool Build(int32_t width, int32_t height, const BitmapFormat& format, int32_t stride = 0);
    bool Build(const ImageInfo& imageInfo, int32_t stride = 0);

    /**
     * @brief Gets the width of Bitmap.
     */
    int GetWidth() const;

    /**
     * @brief Gets the height of Bitmap.
     */
    int GetHeight() const;

    /**
     * @brief Returns row bytes, the interval from one pixel row to the next. Row bytes
     * is at least as large as: GetWidth() * GetImageInfo().GetBytesPerPixel().
     * @return  byte length of pixel row
     */
    int GetRowBytes() const;
    ColorType GetColorType() const;
    AlphaType GetAlphaType() const;
    bool ExtractSubset(Bitmap& dst, const Rect& subset) const;

    /**
     * @brief Copies a Rect of pixels from Bitmap to dstPixels. Copy starts at (srcX, srcY),
     * and does not exceed Bitmap (GetWidth(), GetHeight()).
     *
     * dstInfo specifies width, height, ColorType, AlphaType, and ColorSpace of
     * destination. dstRowBytes specifics the gap from one destination row to the next.
     * Returns true if pixels are copied. Returns false if:
     * - dstInfo has no address
     * - dstRowBytes is less than dstInfo.GetMinRowBytes()
     * - PixelRef is nullptr
     * srcX and srcY may be negative to copy only top or left of source. Returns false
     * if GetWidth() or GetHeight() is zero or negative.
     * Returns false if abs(srcX) >= Bitmap GetWidth(), or if abs(srcY) >= Bitmap GetHeight().
     *
     * @param dstInfo      destination width, height, ColorType, AlphaType, ColorSpace
     * @param dstPixels    destination pixel storage
     * @param dstRowBytes  destination row length
     * @param srcX         column index whose absolute value is less than GetWidth()
     * @param srcY         row index whose absolute value is less than GetHeight()
     * @return             true if pixels are copied to dstPixels
     */
    bool ReadPixels(const ImageInfo& dstInfo, void* dstPixels, size_t dstRowBytes,
                    int32_t srcX, int32_t srcY) const;

    size_t ComputeByteSize() const;

    /**
     * @brief Copies Bitmap pixel address, row bytes, and ImageInfo to pixmap, if address
     * is available, and returns true. If pixel address is not available, return false and
     * leave pixmap unchanged.
     * pixmap contents become invalid on any future change to Bitmap.
     *
     * @param pixmap storage for pixel state if pixels are readable; otherwise, ignored
     * @return true if Bitmap has direct access to pixels
     */
    bool PeekPixels(Pixmap& pixmap) const;

    /**
     * @brief Gets the pointer to Bitmap buffer.
     */
    void* GetPixels() const;

    /**
     * @brief Replaces PixelRef with pixels, preserving ImageInfo and GetRowBytes().
     * Sets SkPixelRef origin to (0, 0). If pixels is nullptr, or if GetImageInfo().GetColorType()
     * equals COLORTYPE_UNKNOWN; release reference to PixelRef, and set PixelRef to nullptr.
     * Caller is responsible for handling ownership pixel memory for the lifetime of Bitmap and PixelRef.
     *
     * @param pixel address of pixel storage, managed by caller
     */
    void SetPixels(void* pixel);

    /**
     * @brief Copies a Rect of pixels from Bitmap to dst Bitmap Pixels.
     * Copy starts at (srcLeft, srcTop), and does not exceed Bitmap (GetWidth(), GetHeight()).
     *
     * dstInfo specifies width, height, ColorType, AlphaType, and ColorSpace of
     * destination. dstRowBytes specifics the gap from one destination row to the next.
     * Returns true if pixels are copied. Returns false if:
     * - dstInfo has no address
     * - dstRowBytes is less than dstInfo.GetMinRowBytes()
     * - PixelRef is nullptr
     * srcX and srcY may be negative to copy only top or left of source. Returns false
     * if GetWidth() or GetHeight() is zero or negative.
     * Returns false if abs(srcLeft) >= Bitmap GetWidth(), or if abs(srcTop) >= Bitmap GetHeight().
     *
     * @param dst     destination Bitmap
     * @param srcLeft column index whose absolute value is less than GetWidth()
     * @param srcTop  row index whose absolute value is less than GetHeight()
     */
    void CopyPixels(Bitmap& dst, int srcLeft, int srcTop) const;
    
    /**
     * @brief Sets ImageInfo to info following the rules in setInfo(), and creates PixelRef
     * containing pixels and rowBytes. releaseProc, if not nullptr, is called immediately on failure
     * or when pixels are no longer referenced. context may be nullptr. If ImageInfo could not be
     * set, or rowBytes is less than info.GetMinRowBytes():
     * calls releaseProc if present, calls Free(), and returns false.
     * Otherwise, if pixels equals nullptr: sets SkImageInfo, calls releaseProc if present, returns true.
     * If ImageInfo is set, pixels is not nullptr, and releaseProc is not nullptr:
     * when pixels are no longer referenced, calls releaseProc with pixels and context as parameters.
     * @param info         contains width, height, AlphaType, ColorType, ColorSpace
     * @param pixels       address or pixel storage; may be nullptr
     * @param rowBytes     size of pixel row or larger
     * @param releaseProc  function called when pixels can be deleted; may be nullptr
     * @param context      caller state passed to releaseProc; may be nullptr
     * @return             true if ImageInfo is set to info
     */
    bool InstallPixels(const ImageInfo& info, void* pixels, size_t rowBytes,
                       ReleaseProc releaseProc = nullptr, void* context = nullptr);
    /**
     * @brief Returns true if pixels can not change. Most
     * immutable Bitmap checks trigger an assert only on debug builds.
     * @return true if pixels are immutable
     */
    bool IsImmutable();

    /**
     * @brief Sets internal flag to mark Bitmap as immutable. Once set, pixels can not change.
     * Any other bitmap sharing the same PixelRef are also marked as immutable. Once PixelRef
     * is marked immutable, the setting cannot be cleared. Writing to immutable Bitmap pixels
     * triggers an assert on debug builds.
     */
    void SetImmutable();

    /** 
     * @brief Replaces pixel values with c, interpreted as being in the sRGB ColorSpace.
     * @param color unpremultiplied color
     */
    void ClearWithColor(const ColorQuad& color) const;

    /**
     * @brief Returns true if GetWidth() or GetHeight() are zero, or if PixelRef is nullptr.
     * If true, Bitmap has no effect when drawn or drawn into.
     * @return true if drawing has no effect
     */
    bool IsValid() const;

    /**
     * @brief Returns true if either width() or height() are zero.
     * Does not check if SkPixelRef is nullptr; call IsValid() to check GetWidth(), GetHeight(), and PixelRef.
     * @return true if dimensions do not enclose area
     */
    bool IsEmpty() const;

    /**
     * @brief Returns pixel at (x, y) as unpremultiplied color. Returns black with
     * alpha if ColorType is COLORTYPE_ALPHA_8. ColorSpace in ImageInfo is ignored.
     * Some color precision may be lost in the conversion to unpremultiplied color;
     * original pixel data may have additional precision.
     *
     * @param x column index, zero or greater, and less than GetWidth()
     * @param y row index, zero or greater, and less than GetHeight()
     * @return pixel converted to unpremultiplied color
     */
    ColorQuad GetColor(int x, int y) const;


    /**
     * @brief Resets to its initial state; all fields are set to zero, as if Bitmap
     * had been initialized by Bitmap(). Sets width, height, row bytes to zero; pixel
     * address to nullptr; ColorType to COLORTYPE_UNKNOWN; and AlphaType to ALPHATYPE_UNKNOWN.
     * If PixelRef is allocated, its reference count is decreased by one, releasing its memory
     * if Bitmap is the sole owner.
     */
    void Free();
    BitmapFormat GetFormat() const;
    void SetFormat(const BitmapFormat& format);

    void SetInfo(const ImageInfo& info);

    /**
     * @brief Gets Image info which contains width, height, AlphaType, ColorType, and ColorSpace.
     * @return Returns ImageInfo describing this Bitmap
     */
    ImageInfo GetImageInfo() const;

    /**
     * @brief Gets a constant reference to the Pixmap holding the Bitmap pixel
     * address, row bytes, and ImageInfo.
     * @return Returns Pixmap describing this Bitmap
     */
    Pixmap GetPixmap() const;

    /*
     * @brief  Make new image from Bitmap but never copy Pixels
     * @note the function never copy Pixels, make sure Pixels is available during using the image
     */
    std::shared_ptr<Image> MakeImage() const;

    /**
     * @brief Sets SkImageInfo to info following the rules in setInfo() and allocates pixel memory.
     * Returns false and calls reset() if SkImageInfo could not be set, or memory could not be allocated.
     * On most platforms, allocating pixel memory may succeed even though there is not sufficient memory
     * to hold pixels; allocation does not take place until the pixels are written to. The actual behavior
     * depends on the platform implementation of malloc().
     * @param info contains width, height, AlphaType, ColorType, ColorSpace
     * @return true if pixel storage is allocated
     */
    bool TryAllocPixels(const ImageInfo& info);
    template<typename T>
    T* GetImpl() const
    {
        return bmpImplPtr->DowncastingTo<T>();
    }

    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);

private:
    std::shared_ptr<BitmapImpl> bmpImplPtr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
