/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "common/rs_common_def.h"
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
#include "pipeline/rs_draw_cmd_list.h"
#include "render/rs_image.h"
#include "property/rs_properties_def.h"
#include <parcel.h>
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;

enum RSOpType : uint16_t {
    OPITEM,
    OPITEMWITHPAINT,
    RECTOPITEM,
    ROUNDRECTOPITEM,
    IMAGEWITHPARMOPITEM,
    DRRECTOPITEM,
    OVALOPITEM,
    REGIONOPITEM,
    ARCOPITEM,
    SAVEOPITEM,
    RESTOREOPITEM,
    FLUSHOPITEM,
    MATRIXOPITEM,
    CLIPRECTOPITEM,
    CLIPRRECTOPITEM,
    CLIPREGIONOPITEM,
    TRANSLATEOPITEM,
    TEXTBLOBOPITEM,
    BITMAPOPITEM,
    BITMAPRECTOPITEM,
    BITMAPLATTICEOPITEM,
    BITMAPNINEOPITEM,
    ADAPTIVERRECTOPITEM,
    CLIPADAPTIVERRECTOPITEM,
    PATHOPITEM,
    CLIPPATHOPITEM,
    PAINTOPITEM,
    CONCATOPITEM,
    SAVELAYEROPITEM,
    DRAWABLEOPITEM,
    PICTUREOPITEM,
    POINTSOPITEM,
    VERTICESOPITEM,
    MULTIPLYALPHAOPITEM,
    SAVEALPHAOPITEM,
    RESTOREALPHAOPITEM,
};

class OpItem : public MemObject, public Parcelable {
public:
    explicit OpItem(size_t size) : MemObject(size) {}
    virtual ~OpItem() {}

    virtual void Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const {};

    virtual RSOpType GetType() const {
        return RSOpType::OPITEM;
    }
    bool Marshalling(Parcel& parcel) const override
    {
        return RSMarshallingHelper::Marshalling(parcel, GetType());
    }
};

class OpItemWithPaint : public OpItem {
public:
    explicit OpItemWithPaint(size_t size) : OpItem(size) {}

    ~OpItemWithPaint() override {}

    RSOpType GetType() const override {
        return RSOpType::OPITEMWITHPAINT;
    }
    bool Marshalling(Parcel& parcel) const override
    {
        return OpItem::Marshalling(parcel);
    }

protected:
    SkPaint paint_;
};

class RectOpItem : public OpItemWithPaint {
public:
    RectOpItem(SkRect rect, const SkPaint& paint);
    ~RectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::RECTOPITEM;
    }

private:
    SkRect rect_;
};

class RoundRectOpItem : public OpItemWithPaint {
public:
    RoundRectOpItem(const SkRRect& rrect, const SkPaint& paint);
    ~RoundRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::ROUNDRECTOPITEM;
    }

private:
    SkRRect rrect_;
};

class ImageWithParmOpItem : public OpItemWithPaint {
public:
    ImageWithParmOpItem(const sk_sp<SkImage> img, int fitNum, int repeatNum, float radius, const SkPaint& paint);
    ImageWithParmOpItem(const sk_sp<SkImage> img, const Rosen::RsImageInfo& rsimageInfo, const SkPaint& paint);

    ~ImageWithParmOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::IMAGEWITHPARMOPITEM;
    }

private:
    std::shared_ptr<RSImage> rsImage_;
};

class DRRectOpItem : public OpItemWithPaint {
public:
    DRRectOpItem(const SkRRect& outer, const SkRRect& inner, const SkPaint& paint);
    ~DRRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::DRRECTOPITEM;
    }

private:
    SkRRect outer_;
    SkRRect inner_;
};

class OvalOpItem : public OpItemWithPaint {
public:
    OvalOpItem(SkRect rect, const SkPaint& paint);
    ~OvalOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::OVALOPITEM;
    }

private:
    SkRect rect_;
};

class RegionOpItem : public OpItemWithPaint {
public:
    RegionOpItem(SkRegion region, const SkPaint& paint);
    ~RegionOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::REGIONOPITEM;
    }

private:
    SkRegion region_;
};

class ArcOpItem : public OpItemWithPaint {
public:
    ArcOpItem(const SkRect& rect, float startAngle, float sweepAngle, bool useCenter, const SkPaint& paint);
    ~ArcOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::ARCOPITEM;
    }

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

    RSOpType GetType() const override {
        return RSOpType::SAVEOPITEM;
    }

    bool Marshalling(Parcel& parcel) const override
    {
        return OpItem::Marshalling(parcel);
    }
    static SaveOpItem* Unmarshalling(Parcel& parcel)
    {
        return new SaveOpItem();
    }
};

class RestoreOpItem : public OpItem {
public:
    RestoreOpItem();
    ~RestoreOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::RESTOREOPITEM;
    }

    bool Marshalling(Parcel& parcel) const override
    {
        return OpItem::Marshalling(parcel);
    }
    static RestoreOpItem* Unmarshalling(Parcel& parcel)
    {
        return new RestoreOpItem();
    }
};

class FlushOpItem : public OpItem {
public:
    FlushOpItem();
    ~FlushOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::FLUSHOPITEM;
    }

};

class MatrixOpItem : public OpItem {
public:
    MatrixOpItem(const SkMatrix& matrix);
    ~MatrixOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::MATRIXOPITEM;
    }

private:
    SkMatrix matrix_;
};

class ClipRectOpItem : public OpItem {
public:
    ClipRectOpItem(const SkRect& rect, SkClipOp op, bool doAA);
    ~ClipRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::CLIPRECTOPITEM;
    }

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

    RSOpType GetType() const override {
        return RSOpType::CLIPRRECTOPITEM;
    }

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

    RSOpType GetType() const override {
        return RSOpType::CLIPREGIONOPITEM;
    }

private:
    SkRegion region_;
    SkClipOp clipOp_;
};

class TranslateOpItem : public OpItem {
public:
    TranslateOpItem(float distanceX, float distanceY);
    ~TranslateOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::TRANSLATEOPITEM;
    }

private:
    float distanceX_;
    float distanceY_;
};

class TextBlobOpItem : public OpItemWithPaint {
public:
    TextBlobOpItem(const sk_sp<SkTextBlob> textBlob, float x, float y, const SkPaint& paint);
    ~TextBlobOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override{
        return RSOpType::TEXTBLOBOPITEM;
    }
    bool Marshalling(Parcel& parcel) const override
    {
        bool success = true;
        success &= OpItemWithPaint::Marshalling(parcel);
        success &= RSMarshallingHelper::Marshalling(parcel, textBlob_);
        success &= RSMarshallingHelper::Marshalling(parcel, x_);
        success &= RSMarshallingHelper::Marshalling(parcel, y_);
        return success;
    }
    static TextBlobOpItem* Unmarshalling(Parcel& parcel)
    {
        sk_sp<SkTextBlob> textBlob;
        float x;
        float y;
        if (!RSMarshallingHelper::Unmarshalling(parcel, textBlob)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, x)) {
            return nullptr;
        }
        if (!RSMarshallingHelper::Unmarshalling(parcel, y)) {
            return nullptr;
        }

        SkPaint paint;
        return new TextBlobOpItem(textBlob, x, y, paint);
    }

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

    RSOpType GetType() const override {
        return RSOpType::BITMAPOPITEM;
    }

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

    RSOpType GetType() const override {
        return RSOpType::BITMAPRECTOPITEM;
    }

private:
    SkRect rectSrc_;
    SkRect rectDst_;
    sk_sp<SkImage> bitmapInfo_;
};

class BitmapLatticeOpItem : public OpItemWithPaint {
public:
    BitmapLatticeOpItem(
        const sk_sp<SkImage> bitmapInfo, const SkCanvas::Lattice& lattice, const SkRect& rect, const SkPaint* paint);
    ~BitmapLatticeOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::BITMAPLATTICEOPITEM;
    }

private:
    SkRect rect_;
    SkCanvas::Lattice lattice_;
    sk_sp<SkImage> bitmapInfo_;
};

class BitmapNineOpItem : public OpItemWithPaint {
public:
    BitmapNineOpItem(
        const sk_sp<SkImage> bitmapInfo, const SkIRect& center, const SkRect& rectDst, const SkPaint* paint);
    ~BitmapNineOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::BITMAPNINEOPITEM;
    }

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

    RSOpType GetType() const override {
        return RSOpType::ADAPTIVERRECTOPITEM;
    }

private:
    float radius_;
    SkPaint paint_;
};

class ClipAdaptiveRRectOpItem : public OpItemWithPaint {
public:
    ClipAdaptiveRRectOpItem(float radius);
    ~ClipAdaptiveRRectOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::CLIPADAPTIVERRECTOPITEM;
    }

private:
    float radius_;
};

class PathOpItem : public OpItemWithPaint {
public:
    PathOpItem(const SkPath& path, const SkPaint& paint);
    ~PathOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::PATHOPITEM;
    }

private:
    SkPath path_;
};

class ClipPathOpItem : public OpItem {
public:
    ClipPathOpItem(const SkPath& path, SkClipOp clipOp, bool doAA);
    ~ClipPathOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::CLIPPATHOPITEM;
    }

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

    RSOpType GetType() const override {
        return RSOpType::PAINTOPITEM;
    }

};

class ConcatOpItem : public OpItem {
public:
    ConcatOpItem(const SkMatrix& matrix);
    ~ConcatOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::CONCATOPITEM;
    }

private:
    SkMatrix matrix_;
};

class SaveLayerOpItem : public OpItemWithPaint {
public:
    SaveLayerOpItem(const SkCanvas::SaveLayerRec& rec);
    ~SaveLayerOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::SAVELAYEROPITEM;
    }

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

    RSOpType GetType() const override {
        return RSOpType::DRAWABLEOPITEM;
    }

private:
    sk_sp<SkDrawable> drawable_;
    SkMatrix matrix_ = SkMatrix::I();
};

class PictureOpItem : public OpItemWithPaint {
public:
    PictureOpItem(const sk_sp<SkPicture> picture, const SkMatrix* matrix, const SkPaint* paint);
    ~PictureOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::PICTUREOPITEM;
    }

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

    RSOpType GetType() const override {
        return RSOpType::POINTSOPITEM;
    }

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

    RSOpType GetType() const override {
        return RSOpType::VERTICESOPITEM;
    }

private:
    sk_sp<SkVertices> vertices_;
    SkVertices::Bone* bones_;
    int boneCount_;
    SkBlendMode mode_;
};

class MultiplyAlphaOpItem : public OpItem {
public:
    MultiplyAlphaOpItem(float alpha);
    ~MultiplyAlphaOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::MULTIPLYALPHAOPITEM;
    }

    bool Marshalling(Parcel& parcel) const override
    {
        bool success = true;
        success &= OpItem::Marshalling(parcel);
        success &= RSMarshallingHelper::Marshalling(parcel, alpha_);
        return success;
    }
    static MultiplyAlphaOpItem* Unmarshalling(Parcel& parcel)
    {
        float alpha;
        if (!RSMarshallingHelper::Unmarshalling(parcel, alpha)) {
            return nullptr;
        }
        return new MultiplyAlphaOpItem(alpha);
    }
private:
    float alpha_;
};

class SaveAlphaOpItem : public OpItem {
public:
    SaveAlphaOpItem();
    ~SaveAlphaOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::SAVEALPHAOPITEM;
    }

    bool Marshalling(Parcel& parcel) const override
    {
        return OpItem::Marshalling(parcel);
    }
    static SaveAlphaOpItem* Unmarshalling(Parcel& parcel)
    {
        return new SaveAlphaOpItem();
    }
};

class RestoreAlphaOpItem : public OpItem {
public:
    RestoreAlphaOpItem();
    ~RestoreAlphaOpItem() override {}
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    RSOpType GetType() const override {
        return RSOpType::RESTOREALPHAOPITEM;
    }

    bool Marshalling(Parcel& parcel) const override
    {
        return OpItem::Marshalling(parcel);
    }
    static RestoreAlphaOpItem* Unmarshalling(Parcel& parcel)
    {
        return new RestoreAlphaOpItem();
    }
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_H
