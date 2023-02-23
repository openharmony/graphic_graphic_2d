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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_H

#include "core/SkDrawShadowInfo.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkDrawable.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageFilter.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPicture.h"
#include "include/core/SkRRect.h"
#include "include/core/SkRect.h"
#include "include/core/SkRegion.h"
#include "include/core/SkTextBlob.h"
#include "pixel_map.h"

#include "common/rs_common_def.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "property/rs_properties_def.h"
#include "render/rs_image.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;

enum RSOpType : uint16_t {
    OPITEM,
    OPITEM_WITH_PAINT,
    RECT_OPITEM,
    ROUND_RECT_OPITEM,
    IMAGE_WITH_PARM_OPITEM,
    DRRECT_OPITEM,
    OVAL_OPITEM,
    REGION_OPITEM,
    ARC_OPITEM,
    SAVE_OPITEM,
    RESTORE_OPITEM,
    FLUSH_OPITEM,
    MATRIX_OPITEM,
    CLIP_RECT_OPITEM,
    CLIP_RRECT_OPITEM,
    CLIP_REGION_OPITEM,
    TRANSLATE_OPITEM,
    TEXTBLOB_OPITEM,
    BITMAP_OPITEM,
    BITMAP_RECT_OPITEM,
    BITMAP_NINE_OPITEM,
    PIXELMAP_OPITEM,
    PIXELMAP_RECT_OPITEM,
    ADAPTIVE_RRECT_OPITEM,
    ADAPTIVE_RRECT_SCALE_OPITEM,
    CLIP_ADAPTIVE_RRECT_OPITEM,
    CLIP_OUTSET_RECT_OPITEM,
    PATH_OPITEM,
    CLIP_PATH_OPITEM,
    PAINT_OPITEM,
    CONCAT_OPITEM,
    SAVE_LAYER_OPITEM,
    DRAWABLE_OPITEM,
    PICTURE_OPITEM,
    POINTS_OPITEM,
    VERTICES_OPITEM,
    SHADOW_REC_OPITEM,
    MULTIPLY_ALPHA_OPITEM,
    SAVE_ALPHA_OPITEM,
    RESTORE_ALPHA_OPITEM,
};

class OpItem : public MemObject, public Parcelable {
public:
    explicit OpItem(size_t size) : MemObject(size) {}
    virtual ~OpItem() {}

    virtual void Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const {};
    virtual RSOpType GetType() const = 0;

    std::unique_ptr<OpItem> GenerateCachedOpItem(SkSurface* surface) const;
    virtual std::optional<SkRect> GetCacheBounds() const
    {
        // not cacheable by default
        return std::nullopt;
    }

    bool Marshalling(Parcel& parcel) const override
    {
        return true;
    }
};

class OpItemWithPaint : public OpItem {
public:
    explicit OpItemWithPaint(size_t size) : OpItem(size) {}
    ~OpItemWithPaint() override {}

protected:
    SkPaint paint_;
};

class RectOpItem : public OpItemWithPaint {
public:
    RectOpItem(SkRect rect, const SkPaint& paint);
    ~RectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRect rect_;
};

class RoundRectOpItem : public OpItemWithPaint {
public:
    RoundRectOpItem(const SkRRect& rrect, const SkPaint& paint);
    ~RoundRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::ROUND_RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRRect rrect_;
};

class ImageWithParmOpItem : public OpItemWithPaint {
public:
    ImageWithParmOpItem(
        const sk_sp<SkImage> img, const sk_sp<SkData> data, const RsImageInfo& rsimageInfo, const SkPaint& paint);
    ImageWithParmOpItem(
        const std::shared_ptr<Media::PixelMap>& pixelmap, const RsImageInfo& rsimageInfo, const SkPaint& paint);
    ImageWithParmOpItem(const std::shared_ptr<RSImage>& rsImage, const SkPaint& paint);

    ~ImageWithParmOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::IMAGE_WITH_PARM_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    std::shared_ptr<RSImage> rsImage_;
};

class DRRectOpItem : public OpItemWithPaint {
public:
    DRRectOpItem(const SkRRect& outer, const SkRRect& inner, const SkPaint& paint);
    ~DRRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::DRRECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRRect outer_;
    SkRRect inner_;
};

class OvalOpItem : public OpItemWithPaint {
public:
    OvalOpItem(SkRect rect, const SkPaint& paint);
    ~OvalOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;
    std::optional<SkRect> GetCacheBounds() const override
    {
        return rect_;
    }

    RSOpType GetType() const override
    {
        return RSOpType::OVAL_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRect rect_;
};

class RegionOpItem : public OpItemWithPaint {
public:
    RegionOpItem(SkRegion region, const SkPaint& paint);
    ~RegionOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::REGION_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRegion region_;
};

class ArcOpItem : public OpItemWithPaint {
public:
    ArcOpItem(const SkRect& rect, float startAngle, float sweepAngle, bool useCenter, const SkPaint& paint);
    ~ArcOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::ARC_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRect rect_;
    float startAngle_;
    float sweepAngle_;
    bool useCenter_;
};

class SaveOpItem : public OpItem {
public:
    SaveOpItem();
    ~SaveOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::SAVE_OPITEM;
    }

    static OpItem* Unmarshalling(Parcel& parcel);
};

class RestoreOpItem : public OpItem {
public:
    RestoreOpItem();
    ~RestoreOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::RESTORE_OPITEM;
    }

    static OpItem* Unmarshalling(Parcel& parcel);
};

class FlushOpItem : public OpItem {
public:
    FlushOpItem();
    ~FlushOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::FLUSH_OPITEM;
    }

    static OpItem* Unmarshalling(Parcel& parcel);
};

class MatrixOpItem : public OpItem {
public:
    MatrixOpItem(const SkMatrix& matrix);
    ~MatrixOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::MATRIX_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkMatrix matrix_;
};

class ClipRectOpItem : public OpItem {
public:
    ClipRectOpItem(const SkRect& rect, SkClipOp op, bool doAA);
    ~ClipRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRect rect_;
    SkClipOp clipOp_;
    bool doAA_;
};

class ClipRRectOpItem : public OpItem {
public:
    ClipRRectOpItem(const SkRRect& rrect, SkClipOp op, bool doAA);
    ~ClipRRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_RRECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRRect rrect_;
    SkClipOp clipOp_;
    bool doAA_;
};

class ClipRegionOpItem : public OpItem {
public:
    ClipRegionOpItem(const SkRegion& region, SkClipOp op);
    ~ClipRegionOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_REGION_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRegion region_;
    SkClipOp clipOp_;
};

class TranslateOpItem : public OpItem {
public:
    TranslateOpItem(float distanceX, float distanceY);
    ~TranslateOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::TRANSLATE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    float distanceX_;
    float distanceY_;
};

class TextBlobOpItem : public OpItemWithPaint {
public:
    TextBlobOpItem(const sk_sp<SkTextBlob> textBlob, float x, float y, const SkPaint& paint);
    ~TextBlobOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;
    std::optional<SkRect> GetCacheBounds() const override
    {
        // bounds of textBlob_, with additional offset [x_, y_]. textBlob_ should never be null but we should check.
        return textBlob_ ? std::make_optional<SkRect>(textBlob_->bounds().makeOffset(x_, y_)) : std::nullopt;
    }

    RSOpType GetType() const override
    {
        return RSOpType::TEXTBLOB_OPITEM;
    }
    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    sk_sp<SkTextBlob> textBlob_;
    float x_;
    float y_;
};

class BitmapOpItem : public OpItemWithPaint {
public:
    BitmapOpItem(const sk_sp<SkImage> bitmapInfo, float left, float top, const SkPaint* paint);
    ~BitmapOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::BITMAP_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    float left_;
    float top_;
    sk_sp<SkImage> bitmapInfo_;
};

class BitmapRectOpItem : public OpItemWithPaint {
public:
    BitmapRectOpItem(
        const sk_sp<SkImage> bitmapInfo, const SkRect* rectSrc, const SkRect& rectDst, const SkPaint* paint);
    ~BitmapRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::BITMAP_RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRect rectSrc_;
    SkRect rectDst_;
    sk_sp<SkImage> bitmapInfo_;
};

class PixelMapOpItem : public OpItemWithPaint {
public:
    PixelMapOpItem(const std::shared_ptr<Media::PixelMap>& pixelmap, float left, float top, const SkPaint* paint);
    ~PixelMapOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::PIXELMAP_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    std::shared_ptr<Media::PixelMap> pixelmap_;
    float left_;
    float top_;
};

class PixelMapRectOpItem : public OpItemWithPaint {
public:
    PixelMapRectOpItem(
        const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& src, const SkRect& dst, const SkPaint* paint);
    ~PixelMapRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::PIXELMAP_RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    std::shared_ptr<Media::PixelMap> pixelmap_;
    SkRect src_;
    SkRect dst_;
};

class BitmapNineOpItem : public OpItemWithPaint {
public:
    BitmapNineOpItem(
        const sk_sp<SkImage> bitmapInfo, const SkIRect& center, const SkRect& rectDst, const SkPaint* paint);
    ~BitmapNineOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::BITMAP_NINE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkIRect center_;
    SkRect rectDst_;
    sk_sp<SkImage> bitmapInfo_;
};

class AdaptiveRRectOpItem : public OpItemWithPaint {
public:
    AdaptiveRRectOpItem(float radius, const SkPaint& paint);
    ~AdaptiveRRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::ADAPTIVE_RRECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    float radius_;
    SkPaint paint_;
};

class AdaptiveRRectScaleOpItem : public OpItemWithPaint {
public:
    AdaptiveRRectScaleOpItem(float radiusRatio, const SkPaint& paint);
    ~AdaptiveRRectScaleOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::ADAPTIVE_RRECT_SCALE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    float radiusRatio_;
    SkPaint paint_;
};

class ClipAdaptiveRRectOpItem : public OpItem {
public:
    ClipAdaptiveRRectOpItem(const SkVector radius[]);
    ~ClipAdaptiveRRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_ADAPTIVE_RRECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkVector radius_[4];
};

class ClipOutsetRectOpItem : public OpItem {
public:
    ClipOutsetRectOpItem(float dx, float dy);
    ~ClipOutsetRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_OUTSET_RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    float dx_;
    float dy_;
};

class PathOpItem : public OpItemWithPaint {
public:
    PathOpItem(const SkPath& path, const SkPaint& paint);
    ~PathOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;
    std::optional<SkRect> GetCacheBounds() const override
    {
        return path_.getBounds();
    }

    RSOpType GetType() const override
    {
        return RSOpType::PATH_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkPath path_;
};

class ClipPathOpItem : public OpItem {
public:
    ClipPathOpItem(const SkPath& path, SkClipOp clipOp, bool doAA);
    ~ClipPathOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_PATH_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkPath path_;
    SkClipOp clipOp_;
    bool doAA_;
};

class PaintOpItem : public OpItemWithPaint {
public:
    PaintOpItem(const SkPaint& paint);
    ~PaintOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::PAINT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);
};

class ConcatOpItem : public OpItem {
public:
    ConcatOpItem(const SkMatrix& matrix);
    ~ConcatOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::CONCAT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkMatrix matrix_;
};

class SaveLayerOpItem : public OpItemWithPaint {
public:
    SaveLayerOpItem(const SkCanvas::SaveLayerRec& rec);
    ~SaveLayerOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::SAVE_LAYER_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRect* rectPtr_ = nullptr;
    SkRect rect_ = SkRect::MakeEmpty();
    sk_sp<SkImageFilter> backdrop_;
    sk_sp<SkImage> mask_;
    SkMatrix matrix_;
    SkCanvas::SaveLayerFlags flags_;
};

class DrawableOpItem : public OpItem {
public:
    DrawableOpItem(SkDrawable* drawable, const SkMatrix* matrix);
    ~DrawableOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::DRAWABLE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    sk_sp<SkDrawable> drawable_;
    SkMatrix matrix_ = SkMatrix::I();
};

class PictureOpItem : public OpItemWithPaint {
public:
    PictureOpItem(const sk_sp<SkPicture> picture, const SkMatrix* matrix, const SkPaint* paint);
    ~PictureOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::PICTURE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    sk_sp<SkPicture> picture_ { nullptr };
    SkMatrix matrix_;
};

class PointsOpItem : public OpItemWithPaint {
public:
    PointsOpItem(SkCanvas::PointMode mode, int count, const SkPoint processedPoints[], const SkPaint& paint);
    ~PointsOpItem() override
    {
        delete[] processedPoints_;
    }
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::POINTS_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkCanvas::PointMode mode_;
    int count_;
    SkPoint* processedPoints_;
};

class VerticesOpItem : public OpItemWithPaint {
public:
    VerticesOpItem(const SkVertices* vertices, const SkVertices::Bone bones[],
        int boneCount, SkBlendMode mode, const SkPaint& paint);
    ~VerticesOpItem() override;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::VERTICES_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    sk_sp<SkVertices> vertices_;
    SkVertices::Bone* bones_;
    int boneCount_;
    SkBlendMode mode_;
};

class ShadowRecOpItem : public OpItem {
public:
    ShadowRecOpItem(const SkPath& path, const SkDrawShadowRec& rec);
    ~ShadowRecOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::SHADOW_REC_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkPath path_;
    SkDrawShadowRec rec_;
};

class MultiplyAlphaOpItem : public OpItem {
public:
    MultiplyAlphaOpItem(float alpha);
    ~MultiplyAlphaOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::MULTIPLY_ALPHA_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    float alpha_;
};

class SaveAlphaOpItem : public OpItem {
public:
    SaveAlphaOpItem();
    ~SaveAlphaOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::SAVE_ALPHA_OPITEM;
    }

    static OpItem* Unmarshalling(Parcel& parcel);
};

class RestoreAlphaOpItem : public OpItem {
public:
    RestoreAlphaOpItem();
    ~RestoreAlphaOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override
    {
        return RSOpType::RESTORE_ALPHA_OPITEM;
    }

    static OpItem* Unmarshalling(Parcel& parcel);
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_H
