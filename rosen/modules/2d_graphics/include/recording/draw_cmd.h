/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef DRAW_CMD_H
#define DRAW_CMD_H

#include <unordered_map>
#include <functional>
#include <stack>

#include "draw/canvas.h"
#include "draw/pen.h"
#include "recording/cmd_list.h"
#include "recording/recording_canvas.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct BrushHandle {
    Color color;
    BlendMode mode;
    bool isAntiAlias;
    Filter::FilterQuality filterQuality;
    ImageHandle colorSpaceHandle;
    FlattenableHandle shaderEffectHandle;
    FlattenableHandle colorFilterHandle;
    FlattenableHandle imageFilterHandle;
    FlattenableHandle maskFilterHandle;
};

struct PenHandle {
    Color color;
    scalar width;
    scalar miterLimit;
    Pen::CapStyle capStyle;
    Pen::JoinStyle joinStyle;
    BlendMode mode;
    bool isAntiAlias;
    Filter::FilterQuality filterQuality;
    FlattenableHandle pathEffectHandle;
    ImageHandle colorSpaceHandle;
    FlattenableHandle shaderEffectHandle;
    FlattenableHandle colorFilterHandle;
    FlattenableHandle imageFilterHandle;
    FlattenableHandle maskFilterHandle;
};

class UnmarshallingPlayer {
public:
    UnmarshallingPlayer(const CmdList& cmdList);
    ~UnmarshallingPlayer() = default;

    std::shared_ptr<DrawOpItem> Unmarshalling(uint32_t type, void* handle);

    const CmdList& cmdList_;

    using UnmarshallingFunc = std::shared_ptr<DrawOpItem>(*)(const CmdList& cmdList, void* handle);
private:
    static std::unordered_map<uint32_t, UnmarshallingFunc> opUnmarshallingFuncLUT_;
};

class GenerateCachedOpItemPlayer {
public:
    GenerateCachedOpItemPlayer(CmdList &cmdList, Canvas* canvas = nullptr, const Rect* rect = nullptr);
    ~GenerateCachedOpItemPlayer() = default;

    bool GenerateCachedOpItem(uint32_t type, void* handle);
    
    Canvas* canvas_;
    const Rect* rect_;
    CmdList& cmdList_;
    void* brushOpHandle_ = nullptr;
    void* penOpHandle_ = nullptr;
};

class DrawOpItem : public OpItem {
public:
    explicit DrawOpItem(uint32_t type) : OpItem(type) {}
    ~DrawOpItem() override = default;

    enum Type : uint32_t {
        OPITEM_HEAD,
        CMD_LIST_OPITEM,
        POINT_OPITEM,
        POINTS_OPITEM,
        LINE_OPITEM,
        RECT_OPITEM,
        ROUND_RECT_OPITEM,
        NESTED_ROUND_RECT_OPITEM,
        ARC_OPITEM,
        PIE_OPITEM,
        OVAL_OPITEM,
        CIRCLE_OPITEM,
        COLOR_OPITEM,
        IMAGE_NINE_OPITEM,
        IMAGE_ANNOTATION_OPITEM,
        IMAGE_LATTICE_OPITEM,
        PATH_OPITEM,
        BACKGROUND_OPITEM,
        SHADOW_OPITEM,
        BITMAP_OPITEM,
        IMAGE_OPITEM,
        IMAGE_RECT_OPITEM,
        PICTURE_OPITEM,
        TEXT_BLOB_OPITEM,
        CLIP_RECT_OPITEM,
        CLIP_IRECT_OPITEM,
        CLIP_ROUND_RECT_OPITEM,
        CLIP_PATH_OPITEM,
        CLIP_REGION_OPITEM,
        SET_MATRIX_OPITEM,
        RESET_MATRIX_OPITEM,
        CONCAT_MATRIX_OPITEM,
        TRANSLATE_OPITEM,
        SCALE_OPITEM,
        ROTATE_OPITEM,
        SHEAR_OPITEM,
        FLUSH_OPITEM,
        CLEAR_OPITEM,
        SAVE_OPITEM,
        SAVE_LAYER_OPITEM,
        RESTORE_OPITEM,
        DISCARD_OPITEM,
        ATTACH_PEN_OPITEM,
        ATTACH_BRUSH_OPITEM,
        DETACH_PEN_OPITEM,
        DETACH_BRUSH_OPITEM,
        CLIP_ADAPTIVE_ROUND_RECT_OPITEM,
        ADAPTIVE_IMAGE_OPITEM,
        ADAPTIVE_PIXELMAP_OPITEM,
        IMAGE_WITH_PARM_OPITEM,
        EXTEND_PIXELMAP_OPITEM,
        PIXELMAP_RECT_OPITEM,
        REGION_OPITEM,
        PATCH_OPITEM,
        EDGEAAQUAD_OPITEM,
        VERTICES_OPITEM,
        IMAGE_SNAPSHOT_OPITEM,
    };

    virtual void Playback(Canvas* canvas, const Rect* rect) = 0;
};

class AttachPenOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const PenHandle& penHandle) : OpItem(DrawOpItem::ATTACH_PEN_OPITEM), penHandle(penHandle) {}
        ~ConstructorHandle() override = default;
        PenHandle penHandle;
    };
    AttachPenOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~AttachPenOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Pen pen_;
};

class AttachBrushOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const BrushHandle& brushHandle)
            : OpItem(DrawOpItem::ATTACH_BRUSH_OPITEM), brushHandle(brushHandle) {}
        ~ConstructorHandle() override = default;
        BrushHandle brushHandle;
    };
    AttachBrushOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~AttachBrushOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Brush brush_;
};

class DetachPenOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::DETACH_PEN_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    DetachPenOpItem();
    ~DetachPenOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class DetachBrushOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::DETACH_BRUSH_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    DetachBrushOpItem();
    ~DetachBrushOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class ImageSnapshotOpItem : public DrawOpItem {
public:
    ImageSnapshotOpItem(std::shared_ptr<Image> image, const Rect& src, const Rect& dst,
        const SamplingOptions& sampling, SrcRectConstraint constraint);
    ~ImageSnapshotOpItem() override = default;
    void Playback(Canvas* canvas, const Rect* rect) override;

private:
    std::shared_ptr<Image> image_;
    Rect src_;
    Rect dst_;
    SamplingOptions sampling_;
    SrcRectConstraint constraint_;
};

class DrawCmdListOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const CmdListHandle& handle) : OpItem(DrawOpItem::CMD_LIST_OPITEM), handle(handle) {}
        ~ConstructorHandle() override = default;
        CmdListHandle handle;
    };
    DrawCmdListOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawCmdListOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::vector<std::shared_ptr<DrawOpItem>> opItems_;
};

class DrawPointOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Point& point) : OpItem(DrawOpItem::POINT_OPITEM), point(point) {}
        ~ConstructorHandle() override = default;
        Point point;
    };
    DrawPointOpItem(ConstructorHandle* handle);
    ~DrawPointOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point point_;
};

class DrawPointsOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(PointMode mode, const std::pair<uint32_t, size_t>& pts)
            : OpItem(DrawOpItem::POINTS_OPITEM), mode(mode), pts(pts) {}
        ~ConstructorHandle() override = default;
        PointMode mode;
        std::pair<uint32_t, size_t> pts;
    };
    DrawPointsOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPointsOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    PointMode mode_;
    std::vector<Point> pts_;
};

class DrawLineOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Point& startPt, const Point& endPt)
            : OpItem(DrawOpItem::LINE_OPITEM), startPt(startPt), endPt(endPt) {}
        ~ConstructorHandle() override = default;
        Point startPt;
        Point endPt;
    };
    DrawLineOpItem(ConstructorHandle* handle);
    ~DrawLineOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point startPt_;
    Point endPt_;
};

class DrawRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect) : OpItem(DrawOpItem::RECT_OPITEM), rect(rect) {}
        ~ConstructorHandle() override = default;
        Rect rect;
    };
    DrawRectOpItem(ConstructorHandle* handle);
    ~DrawRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
};

class DrawRoundRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const RoundRect& rrect) : OpItem(DrawOpItem::ROUND_RECT_OPITEM), rrect(rrect) {}
        ~ConstructorHandle() override = default;
        RoundRect rrect;
    };
    DrawRoundRectOpItem(ConstructorHandle* handle);
    ~DrawRoundRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RoundRect rrect_;
};

class DrawNestedRoundRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const RoundRect& outerRRect, const RoundRect& innerRRect)
            : OpItem(DrawOpItem::NESTED_ROUND_RECT_OPITEM), outerRRect(outerRRect), innerRRect(innerRRect) {}
        ~ConstructorHandle() override = default;
        RoundRect outerRRect;
        RoundRect innerRRect;
    };
    DrawNestedRoundRectOpItem(ConstructorHandle* handle);
    ~DrawNestedRoundRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RoundRect outerRRect_;
    RoundRect innerRRect_;
};

class DrawArcOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, scalar startAngle, scalar sweepAngle)
            : OpItem(DrawOpItem::ARC_OPITEM), rect(rect), startAngle(startAngle), sweepAngle(sweepAngle) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        scalar startAngle;
        scalar sweepAngle;
    };
    DrawArcOpItem(ConstructorHandle* handle);
    ~DrawArcOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class DrawPieOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, scalar startAngle, scalar sweepAngle)
            : OpItem(DrawOpItem::PIE_OPITEM), rect(rect), startAngle(startAngle), sweepAngle(sweepAngle) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        scalar startAngle;
        scalar sweepAngle;
    };
    DrawPieOpItem(ConstructorHandle* handle);
    ~DrawPieOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class DrawOvalOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect) : OpItem(DrawOpItem::OVAL_OPITEM), rect(rect) {}
        ~ConstructorHandle() override = default;
        Rect rect;
    };
    DrawOvalOpItem(ConstructorHandle* handle);
    ~DrawOvalOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
};

class DrawCircleOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Point& centerPt, scalar radius)
            : OpItem(DrawOpItem::CIRCLE_OPITEM), centerPt(centerPt), radius(radius) {}
        ~ConstructorHandle() override = default;
        Point centerPt;
        scalar radius;
    };
    DrawCircleOpItem(ConstructorHandle* handle);
    ~DrawCircleOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point centerPt_;
    scalar radius_;
};

class DrawPathOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& path) : OpItem(DrawOpItem::PATH_OPITEM), path(path) {}
        ~ConstructorHandle() override = default;
        ImageHandle path;
    };
    DrawPathOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPathOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::shared_ptr<Path> path_;
};

class DrawBackgroundOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const BrushHandle& brushHandle)
            : OpItem(DrawOpItem::BACKGROUND_OPITEM), brushHandle(brushHandle) {}
        ~ConstructorHandle() override = default;
        BrushHandle brushHandle;
    };
    DrawBackgroundOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawBackgroundOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Brush brush_;
};

class DrawShadowOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& path, const Point3& planeParams, const Point3& devLightPos,
            scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
            : OpItem(DrawOpItem::SHADOW_OPITEM), path(path), planeParams(planeParams), devLightPos(devLightPos),
            lightRadius(lightRadius), ambientColor(ambientColor), spotColor(spotColor), flag(flag) {}
        ~ConstructorHandle() override = default;
        ImageHandle path;
        Point3 planeParams;
        Point3 devLightPos;
        scalar lightRadius;
        Color ambientColor;
        Color spotColor;
        ShadowFlags flag;
    };
    DrawShadowOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawShadowOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point3 planeParams_;
    Point3 devLightPos_;
    scalar lightRadius_;
    Color ambientColor_;
    Color spotColor_;
    ShadowFlags flag_;
    std::shared_ptr<Path> path_;
};

class DrawRegionOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const CmdListHandle& region) : OpItem(DrawOpItem::REGION_OPITEM), region(region) {}
        ~ConstructorHandle() override = default;
        CmdListHandle region;
    };
    DrawRegionOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawRegionOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::shared_ptr<Region> region_;
};

class DrawPatchOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const std::pair<uint32_t, size_t> cubics, const std::pair<uint32_t, size_t> colors,
            const std::pair<uint32_t, size_t> texCoords, BlendMode mode)
            : OpItem(DrawOpItem::PATCH_OPITEM), cubics(cubics), colors(colors), texCoords(texCoords), mode(mode) {}
        ~ConstructorHandle() override = default;
        std::pair<uint32_t, size_t> cubics;
        std::pair<uint32_t, size_t> colors;
        std::pair<uint32_t, size_t> texCoords;
        BlendMode mode;
    };
    explicit DrawPatchOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPatchOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    BlendMode mode_;
    std::vector<Point> cubics_;
    std::vector<ColorQuad> colors_;
    std::vector<Point> texCoords_;
};

class DrawEdgeAAQuadOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, const std::pair<uint32_t, size_t> clipQuad, QuadAAFlags aaFlags,
            ColorQuad color, BlendMode mode) : OpItem(DrawOpItem::EDGEAAQUAD_OPITEM), rect(rect), clipQuad(clipQuad),
            aaFlags(aaFlags), color(color), mode(mode) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        std::pair<uint32_t, size_t> clipQuad;
        QuadAAFlags aaFlags;
        ColorQuad color;
        BlendMode mode;
    };
    explicit DrawEdgeAAQuadOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawEdgeAAQuadOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    QuadAAFlags aaFlags_;
    ColorQuad color_;
    BlendMode mode_;
    std::vector<Point> clipQuad_;
};

class DrawVerticesOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const VerticesHandle& vertices, BlendMode mode)
            : OpItem(DrawOpItem::VERTICES_OPITEM), vertices(vertices), mode(mode) {}
        ~ConstructorHandle() override = default;
        VerticesHandle vertices;
        BlendMode mode;
    };
    DrawVerticesOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawVerticesOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    BlendMode mode_;
    std::shared_ptr<Vertices> vertices_;
};

class DrawColorOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(ColorQuad color, BlendMode mode)
            : OpItem(DrawOpItem::COLOR_OPITEM), color(color), mode(mode) {}
        ~ConstructorHandle() override = default;
        ColorQuad color;
        BlendMode mode;
    };
    DrawColorOpItem(ConstructorHandle* handle);
    ~DrawColorOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    ColorQuad color_;
    BlendMode mode_;
};

class DrawImageNineOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& image, const RectI& center, const Rect& dst, FilterMode filterMode,
            const BrushHandle& brushHandle, bool hasBrush) : OpItem(DrawOpItem::IMAGE_NINE_OPITEM), image(image),
            center(center), dst(dst), filter(filterMode), brushHandle(brushHandle), hasBrush(hasBrush) {}
        ~ConstructorHandle() override = default;
        ImageHandle image;
        RectI center;
        Rect dst;
        FilterMode filter;
        BrushHandle brushHandle;
        bool hasBrush;
    };
    explicit DrawImageNineOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawImageNineOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RectI center_;
    Rect dst_;
    FilterMode filter_;
    bool hasBrush_;
    Brush brush_;
    std::shared_ptr<Image> image_;
};

class DrawAnnotationOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, const char* key, const ImageHandle& data)
            : OpItem(DrawOpItem::IMAGE_ANNOTATION_OPITEM), rect(rect), key(key), data(data) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        const char* key;
        const ImageHandle data;
    };
    DrawAnnotationOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawAnnotationOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    const char* key_;
    std::shared_ptr<Data> data_;
};

class DrawImageLatticeOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& image, const Lattice& lattice, const Rect& dst, FilterMode filterMode,
            const BrushHandle& brushHandle, bool hasBrush) : OpItem(DrawOpItem::IMAGE_LATTICE_OPITEM), image(image),
            lattice(lattice), dst(dst), filter(filterMode), brushHandle(brushHandle), hasBrush(hasBrush) {}
        ~ConstructorHandle() override = default;
        ImageHandle image;
        Lattice lattice;
        Rect dst;
        FilterMode filter;
        BrushHandle brushHandle;
        bool hasBrush;
    };
    explicit DrawImageLatticeOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawImageLatticeOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Lattice lattice_;
    Rect dst_;
    FilterMode filter_;
    bool hasBrush_;
    Brush brush_;
    std::shared_ptr<Image> image_;
};

class DrawBitmapOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& bitmap, scalar px, scalar py)
            : OpItem(DrawOpItem::BITMAP_OPITEM), bitmap(bitmap), px(px), py(py) {}
        ~ConstructorHandle() override = default;
        ImageHandle bitmap;
        scalar px;
        scalar py;
    };
    DrawBitmapOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawBitmapOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;

private:
    scalar px_;
    scalar py_;
    std::shared_ptr<Bitmap> bitmap_;
};

class DrawImageOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& image, scalar px, scalar py, const SamplingOptions& samplingOptions)
            : OpItem(DrawOpItem::IMAGE_OPITEM), image(image), px(px), py(py), samplingOptions(samplingOptions) {}
        ~ConstructorHandle() override = default;
        ImageHandle image;
        scalar px;
        scalar py;
        SamplingOptions samplingOptions;
    };
    DrawImageOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawImageOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;

private:
    scalar px_;
    scalar py_;
    SamplingOptions samplingOptions_;
    std::shared_ptr<Image> image_;
};

class DrawImageRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
            SrcRectConstraint constraint) : OpItem(DrawOpItem::IMAGE_RECT_OPITEM), image(image), src(src), dst(dst),
            sampling(sampling), constraint(constraint) {}
        ~ConstructorHandle() override = default;
        ImageHandle image;
        Rect src;
        Rect dst;
        SamplingOptions sampling;
        SrcRectConstraint constraint;
    };
    DrawImageRectOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawImageRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect src_;
    Rect dst_;
    SamplingOptions sampling_;
    SrcRectConstraint constraint_;
    std::shared_ptr<Image> image_;
};

class DrawPictureOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& picture) : OpItem(DrawOpItem::PICTURE_OPITEM), picture(picture) {}
        ~ConstructorHandle() override = default;
        ImageHandle picture;
    };
    DrawPictureOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPictureOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::shared_ptr<Picture> picture_;
};

class DrawTextBlobOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& textBlob, scalar x, scalar y) : OpItem(DrawOpItem::TEXT_BLOB_OPITEM),
            textBlob(textBlob), x(x), y(y) {}
        ~ConstructorHandle() override = default;
        bool GenerateCachedOpItem(std::shared_ptr<CmdList> cacheCmdList, const TextBlob* textBlob);
        bool GenerateCachedOpItem(CmdList& cmdList, Canvas* canvas, AttachPenOpItem::ConstructorHandle* penOpHandle,
            AttachBrushOpItem::ConstructorHandle* brushOpHandle);
        ImageHandle textBlob;
        scalar x;
        scalar y;
    };
    DrawTextBlobOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawTextBlobOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;

    std::shared_ptr<ImageSnapshotOpItem> GenerateCachedOpItem(Canvas* canvas,
        AttachPenOpItem* penOpItem, AttachBrushOpItem* brushOpItem);
private:
    scalar x_;
    scalar y_;
    std::shared_ptr<TextBlob> textBlob_;
};

class ClipRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, ClipOp op, bool doAntiAlias)
            : OpItem(DrawOpItem::CLIP_RECT_OPITEM), rect(rect), clipOp(op), doAntiAlias(doAntiAlias) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        ClipOp clipOp;
        bool doAntiAlias;
    };
    ClipRectOpItem(ConstructorHandle* handle);
    ~ClipRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    ClipOp clipOp_;
    bool doAntiAlias_;
};

class ClipIRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const RectI& rect, ClipOp op = ClipOp::INTERSECT)
            : OpItem(DrawOpItem::CLIP_IRECT_OPITEM), rect(rect), clipOp(op) {}
        ~ConstructorHandle() override = default;
        RectI rect;
        ClipOp clipOp;
    };
    ClipIRectOpItem(ConstructorHandle* handle);
    ~ClipIRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RectI rect_;
    ClipOp clipOp_;
};

class ClipRoundRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const RoundRect& rrect, ClipOp op, bool doAntiAlias)
            : OpItem(DrawOpItem::CLIP_ROUND_RECT_OPITEM), rrect(rrect), clipOp(op), doAntiAlias(doAntiAlias) {}
        ~ConstructorHandle() override = default;
        RoundRect rrect;
        ClipOp clipOp;
        bool doAntiAlias;
    };
    ClipRoundRectOpItem(ConstructorHandle* handle);
    ~ClipRoundRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RoundRect rrect_;
    ClipOp clipOp_;
    bool doAntiAlias_;
};

class ClipPathOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& path, ClipOp clipOp, bool doAntiAlias)
            : OpItem(DrawOpItem::PATH_OPITEM), path(path), clipOp(clipOp), doAntiAlias(doAntiAlias) {}
        ~ConstructorHandle() override = default;
        ImageHandle path;
        ClipOp clipOp;
        bool doAntiAlias;
    };
    ClipPathOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~ClipPathOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    ClipOp clipOp_;
    bool doAntiAlias_;
    std::shared_ptr<Path> path_;
};

class ClipRegionOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const CmdListHandle& region, ClipOp clipOp = ClipOp::INTERSECT)
            : OpItem(DrawOpItem::CLIP_REGION_OPITEM), region(region), clipOp(clipOp) {}
        ~ConstructorHandle() override = default;
        CmdListHandle region;
        ClipOp clipOp;
    };
    ClipRegionOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~ClipRegionOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    ClipOp clipOp_;
    std::shared_ptr<Region> region_;
};

class SetMatrixOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Matrix::Buffer& matrixBuffer)
            : OpItem(DrawOpItem::SET_MATRIX_OPITEM), matrixBuffer(matrixBuffer) {}
        ~ConstructorHandle() override = default;
        Matrix::Buffer matrixBuffer;
    };
    SetMatrixOpItem(ConstructorHandle* handle);
    ~SetMatrixOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Matrix matrix_;
};

class ResetMatrixOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::RESET_MATRIX_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    ResetMatrixOpItem();
    ~ResetMatrixOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class ConcatMatrixOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Matrix::Buffer& matrixBuffer)
            : OpItem(DrawOpItem::CONCAT_MATRIX_OPITEM), matrixBuffer(matrixBuffer) {}
        ~ConstructorHandle() override = default;
        Matrix::Buffer matrixBuffer;
    };
    ConcatMatrixOpItem(ConstructorHandle* handle);
    ~ConcatMatrixOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Matrix matrix_;
};

class TranslateOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(scalar dx, scalar dy) : OpItem(DrawOpItem::TRANSLATE_OPITEM), dx(dx), dy(dy) {}
        ~ConstructorHandle() override = default;
        scalar dx;
        scalar dy;
    };
    TranslateOpItem(ConstructorHandle* handle);
    ~TranslateOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    scalar dx_;
    scalar dy_;
};

class ScaleOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(scalar sx, scalar sy) : OpItem(DrawOpItem::SCALE_OPITEM), sx(sx), sy(sy) {}
        ~ConstructorHandle() override = default;
        scalar sx;
        scalar sy;
    };
    ScaleOpItem(ConstructorHandle* handle);
    ~ScaleOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    scalar sx_;
    scalar sy_;
};

class RotateOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(scalar deg, scalar sx, scalar sy)
            : OpItem(DrawOpItem::ROTATE_OPITEM), deg(deg), sx(sx), sy(sy) {}
        ~ConstructorHandle() override = default;
        scalar deg;
        scalar sx;
        scalar sy;
    };
    RotateOpItem(ConstructorHandle* handle);
    ~RotateOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    scalar deg_;
    scalar sx_;
    scalar sy_;
};

class ShearOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(scalar sx, scalar sy) : OpItem(DrawOpItem::SHEAR_OPITEM), sx(sx), sy(sy) {}
        ~ConstructorHandle() override = default;
        scalar sx;
        scalar sy;
    };
    ShearOpItem(ConstructorHandle* handle);
    ~ShearOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    scalar sx_;
    scalar sy_;
};

class FlushOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::FLUSH_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    FlushOpItem();
    ~FlushOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class ClearOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(ColorQuad color) : OpItem(DrawOpItem::CLEAR_OPITEM), color(color) {}
        ~ConstructorHandle() override = default;
        ColorQuad color;
    };
    ClearOpItem(ConstructorHandle* handle);
    ~ClearOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    ColorQuad color_;
};

class SaveOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::SAVE_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    SaveOpItem();
    ~SaveOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class SaveLayerOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, bool hasBrush, const BrushHandle& brushHandle,
            const CmdListHandle& imageFilter, uint32_t saveLayerFlags) : OpItem(DrawOpItem::SAVE_LAYER_OPITEM),
            rect(rect), hasBrush(hasBrush), brushHandle(brushHandle), imageFilter(imageFilter),
            saveLayerFlags(saveLayerFlags) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        bool hasBrush;
        BrushHandle brushHandle;
        CmdListHandle imageFilter;
        uint32_t saveLayerFlags;
    };
    SaveLayerOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~SaveLayerOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    uint32_t saveLayerFlags_;
    bool hasBrush_;
    Brush brush_;
    std::shared_ptr<ImageFilter> imageFilter_;
};

class RestoreOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::RESTORE_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    RestoreOpItem();
    ~RestoreOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class DiscardOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::DISCARD_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    DiscardOpItem();
    ~DiscardOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class ClipAdaptiveRoundRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const std::pair<uint32_t, size_t>& radiusData)
            : OpItem(DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM), radiusData(radiusData) {}
        ~ConstructorHandle() override = default;
        std::pair<uint32_t, size_t> radiusData;
    };
    ClipAdaptiveRoundRectOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~ClipAdaptiveRoundRectOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::vector<Point> radiusData_;
};

class DrawAdaptiveImageOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& image, const AdaptiveImageInfo& rsImageInfo,
            const SamplingOptions& sampling, const bool isImage) : OpItem(DrawOpItem::ADAPTIVE_IMAGE_OPITEM),
            image(image), rsImageInfo(rsImageInfo), sampling(sampling), isImage(isImage) {}
        ~ConstructorHandle() override = default;
        ImageHandle image;
        AdaptiveImageInfo rsImageInfo;
        SamplingOptions sampling;
        bool isImage;
    };
    DrawAdaptiveImageOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawAdaptiveImageOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    AdaptiveImageInfo rsImageInfo_;
    SamplingOptions sampling_;
    bool isImage_;
    std::shared_ptr<Image> image_;
    std::shared_ptr<Data> data_;
};

class DrawAdaptivePixelMapOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& pixelMap, const AdaptiveImageInfo& imageInfo,
            const SamplingOptions& smapling) : OpItem(DrawOpItem::ADAPTIVE_PIXELMAP_OPITEM), pixelMap(pixelMap),
            imageInfo(imageInfo), smapling(smapling) {}
        ~ConstructorHandle() override = default;
        ImageHandle pixelMap;
        AdaptiveImageInfo imageInfo;
        SamplingOptions smapling;
    };
    DrawAdaptivePixelMapOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawAdaptivePixelMapOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    AdaptiveImageInfo imageInfo_;
    SamplingOptions smapling_;
    std::shared_ptr<Media::PixelMap> pixelMap_;
};

class DrawImageWithParmOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& objectHandle, const SamplingOptions& sampling)
            : OpItem(DrawOpItem::IMAGE_WITH_PARM_OPITEM), objectHandle(objectHandle), sampling(sampling) {}
        ~ConstructorHandle() override = default;
        ImageHandle objectHandle;
        SamplingOptions sampling;
    };
    DrawImageWithParmOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawImageWithParmOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageObject> objectHandle_;
};

class DrawExtendPixelMapOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& objectHandle, const SamplingOptions& sampling)
            : OpItem(DrawOpItem::EXTEND_PIXELMAP_OPITEM), objectHandle(objectHandle), sampling(sampling) {}
        ~ConstructorHandle() override = default;
        ImageHandle objectHandle;
        SamplingOptions sampling;
    };
    DrawExtendPixelMapOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawExtendPixelMapOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageObject> objectHandle_;
};

class DrawExtendPixelMapRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& objectHandle, const SamplingOptions& sampling)
            : OpItem(DrawOpItem::PIXELMAP_RECT_OPITEM), objectHandle(objectHandle), sampling(sampling) {}
        ~ConstructorHandle() override = default;
        ImageHandle objectHandle;
        SamplingOptions sampling;
    };
    DrawExtendPixelMapRectOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawExtendPixelMapRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageBaseOj> objectHandle_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
