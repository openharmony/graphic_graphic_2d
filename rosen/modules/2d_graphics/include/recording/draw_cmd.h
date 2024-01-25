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
#include <utility>
#include <ctime>
#include <chrono>

#include "draw/canvas.h"
#include "draw/paint.h"
#include "recording/cmd_list.h"

#define REGISTER_UNMARSHALLING_FUNC(name, type, func) \
    bool isRegistered##name = OHOS::Rosen::Drawing::UnmarshallingPlayer::RegisterUnmarshallingFunc(type, func)

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawCmdList;
class DRAWING_API DrawOpItem : public OpItem {
public:
    explicit DrawOpItem(uint32_t type) : OpItem(type) {}
    ~DrawOpItem() override = default;

    enum Type : uint32_t {
        OPITEM_HEAD,
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
        SYMBOL_OPITEM,
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
        CLIP_ADAPTIVE_ROUND_RECT_OPITEM,
        ADAPTIVE_IMAGE_OPITEM,
        ADAPTIVE_PIXELMAP_OPITEM,
        IMAGE_WITH_PARM_OPITEM,
        PIXELMAP_WITH_PARM_OPITEM,
        PIXELMAP_RECT_OPITEM,
        REGION_OPITEM,
        PATCH_OPITEM,
        EDGEAAQUAD_OPITEM,
        VERTICES_OPITEM,
        IMAGE_SNAPSHOT_OPITEM,
        SURFACEBUFFER_OPITEM,
        DRAW_FUNC_OPITEM,
    };

    static void BrushHandleToBrush(const BrushHandle& brushHandle, const DrawCmdList& cmdList, Brush& brush);
    static void BrushToBrushHandle(const Brush& brush, DrawCmdList& cmdList, BrushHandle& brushHandle);
    static void GeneratePaintFromHandle(const PaintHandle& paintHandle, const DrawCmdList& cmdList, Paint& paint);
    static void GenerateHandleFromPaint(CmdList& cmdList, const Paint& paint, PaintHandle& paintHandle);

    virtual void Marshalling(DrawCmdList& cmdList) = 0;
    virtual void Playback(Canvas* canvas, const Rect* rect) = 0;

    virtual void SetSymbol() {}
    virtual void SetNodeId(NodeId id) {}
};

class UnmarshallingPlayer {
public:
    using UnmarshallingFunc = std::shared_ptr<DrawOpItem>(*)(const DrawCmdList& cmdList, void* handle);
    DRAWING_API static bool RegisterUnmarshallingFunc(uint32_t type, UnmarshallingFunc func);

    UnmarshallingPlayer(const DrawCmdList& cmdList);
    ~UnmarshallingPlayer() = default;

    std::shared_ptr<DrawOpItem> Unmarshalling(uint32_t type, void* handle);

    const DrawCmdList& cmdList_;

private:
    static std::unordered_map<uint32_t, UnmarshallingFunc> opUnmarshallingFuncLUT_;
};

class GenerateCachedOpItemPlayer {
public:
    GenerateCachedOpItemPlayer(DrawCmdList &cmdList, Canvas* canvas = nullptr, const Rect* rect = nullptr);
    ~GenerateCachedOpItemPlayer() = default;

    bool GenerateCachedOpItem(uint32_t type, void* handle);

    Canvas* canvas_ = nullptr;
    const Rect* rect_;
    DrawCmdList& cmdList_;
};

class DRAWING_API DrawWithPaintOpItem : public DrawOpItem {
public:
    explicit DrawWithPaintOpItem(const Paint& paint, uint32_t type) : DrawOpItem(type), paint_(paint) {}
    DrawWithPaintOpItem(const DrawCmdList& cmdList, const PaintHandle& paintHandle, uint32_t type);
    ~DrawWithPaintOpItem() override = default;
    void Marshalling(DrawCmdList& cmdList) override {}
    void Playback(Canvas* canvas, const Rect* rect) override {}
protected:
    Paint paint_;
};

class DrawPointOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Point& point, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::POINT_OPITEM), point(point), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Point point;
        PaintHandle paintHandle;
    };
    DrawPointOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    explicit DrawPointOpItem(const Point& point, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::POINT_OPITEM), point_(point) {}
    ~DrawPointOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point point_;
};

class DrawPointsOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(PointMode mode, const std::pair<uint32_t, size_t>& pts, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::POINTS_OPITEM), mode(mode), pts(pts), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        PointMode mode;
        std::pair<uint32_t, size_t> pts;
        PaintHandle paintHandle;
    };
    DrawPointsOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawPointsOpItem(PointMode mode, std::vector<Point>& pts, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::POINTS_OPITEM), mode_(mode), pts_(pts) {}
    ~DrawPointsOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    PointMode mode_;
    std::vector<Point> pts_;
};

class DrawLineOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Point& startPt, const Point& endPt, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::LINE_OPITEM), startPt(startPt), endPt(endPt), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Point startPt;
        Point endPt;
        PaintHandle paintHandle;
    };
    DrawLineOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawLineOpItem(const Point& startPt, const Point& endPt, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::LINE_OPITEM), startPt_(startPt), endPt_(endPt) {}
    ~DrawLineOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point startPt_;
    Point endPt_;
};

class DrawRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::RECT_OPITEM), rect(rect), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        PaintHandle paintHandle;
    };
    DrawRectOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    explicit DrawRectOpItem(const Rect& rect, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::RECT_OPITEM), rect_(rect) {}
    ~DrawRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
};

class DrawRoundRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const RoundRect& rrect, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::ROUND_RECT_OPITEM), rrect(rrect), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        RoundRect rrect;
        PaintHandle paintHandle;
    };
    DrawRoundRectOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    explicit DrawRoundRectOpItem(const RoundRect& rrect, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::ROUND_RECT_OPITEM), rrect_(rrect) {}
    ~DrawRoundRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RoundRect rrect_;
};

class DrawNestedRoundRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const RoundRect& outerRRect, const RoundRect& innerRRect, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::NESTED_ROUND_RECT_OPITEM), outerRRect(outerRRect), innerRRect(innerRRect),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        RoundRect outerRRect;
        RoundRect innerRRect;
        PaintHandle paintHandle;
    };
    DrawNestedRoundRectOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawNestedRoundRectOpItem(const RoundRect& outer, const RoundRect& inner, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::NESTED_ROUND_RECT_OPITEM), outerRRect_(outer), innerRRect_(inner) {}
    ~DrawNestedRoundRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RoundRect outerRRect_;
    RoundRect innerRRect_;
};

class DrawArcOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, scalar startAngle, scalar sweepAngle, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::ARC_OPITEM), rect(rect), startAngle(startAngle), sweepAngle(sweepAngle),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        scalar startAngle;
        scalar sweepAngle;
        PaintHandle paintHandle;
    };
    DrawArcOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawArcOpItem(const Rect& oval, scalar startAngle, scalar sweepAngle, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::ARC_OPITEM), rect_(oval), startAngle_(startAngle),
          sweepAngle_(sweepAngle) {}
    ~DrawArcOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class DrawPieOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, scalar startAngle, scalar sweepAngle, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PIE_OPITEM), rect(rect), startAngle(startAngle), sweepAngle(sweepAngle),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        scalar startAngle;
        scalar sweepAngle;
        PaintHandle paintHandle;
    };
    DrawPieOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawPieOpItem(const Rect& oval, scalar startAngle, scalar sweepAngle, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::PIE_OPITEM), rect_(oval), startAngle_(startAngle),
          sweepAngle_(sweepAngle) {}
    ~DrawPieOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class DrawOvalOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::OVAL_OPITEM), rect(rect), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        PaintHandle paintHandle;
    };
    DrawOvalOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    explicit DrawOvalOpItem(const Rect& oval, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::OVAL_OPITEM), rect_(oval) {}
    ~DrawOvalOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
};

class DrawCircleOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Point& centerPt, scalar radius, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::CIRCLE_OPITEM), centerPt(centerPt), radius(radius), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Point centerPt;
        scalar radius;
        PaintHandle paintHandle;
    };
    DrawCircleOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawCircleOpItem(const Point& centerPt, scalar radius, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::CIRCLE_OPITEM), centerPt_(centerPt), radius_(radius) {}
    ~DrawCircleOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point centerPt_;
    scalar radius_;
};

class DrawPathOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& path, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PATH_OPITEM), path(path), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle path;
        PaintHandle paintHandle;
    };
    DrawPathOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    explicit DrawPathOpItem(const Path& path, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::PATH_OPITEM), path_(std::make_shared<Path>(path)) {}
    ~DrawPathOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    DrawBackgroundOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    explicit DrawBackgroundOpItem(const Brush& brush) : DrawOpItem(DrawOpItem::BACKGROUND_OPITEM), brush_(brush) {}
    ~DrawBackgroundOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Brush brush_;
};

class DrawShadowOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& path, const Point3& planeParams, const Point3& devLightPos,
            scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
            : OpItem(DrawOpItem::SHADOW_OPITEM), path(path), planeParams(planeParams), devLightPos(devLightPos),
            lightRadius(lightRadius), ambientColor(ambientColor), spotColor(spotColor), flag(flag) {}
        ~ConstructorHandle() override = default;
        OpDataHandle path;
        Point3 planeParams;
        Point3 devLightPos;
        scalar lightRadius;
        Color ambientColor;
        Color spotColor;
        ShadowFlags flag;
    };
    DrawShadowOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawShadowOpItem(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
        Color ambientColor, Color spotColor, ShadowFlags flag)
        : DrawOpItem(DrawOpItem::SHADOW_OPITEM), planeParams_(planeParams), devLightPos_(devLightPos),
          lightRadius_(lightRadius), ambientColor_(ambientColor), spotColor_(spotColor), flag_(flag),
          path_(std::make_shared<Path>(path)) {}
    ~DrawShadowOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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

class DrawRegionOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& region, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::REGION_OPITEM), region(region), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle region;
        PaintHandle paintHandle;
    };
    DrawRegionOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    explicit DrawRegionOpItem(const Region& region, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::REGION_OPITEM), region_(std::make_shared<Region>(region)) {}
    ~DrawRegionOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::shared_ptr<Region> region_;
};

class DrawVerticesOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& vertices, BlendMode mode, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::VERTICES_OPITEM), vertices(vertices), mode(mode), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle vertices;
        BlendMode mode;
        PaintHandle paintHandle;
    };
    DrawVerticesOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawVerticesOpItem(const Vertices& vertices, BlendMode mode, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::VERTICES_OPITEM), mode_(mode),
          vertices_(std::make_shared<Vertices>(vertices)) {}
    ~DrawVerticesOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    explicit DrawColorOpItem(ConstructorHandle* handle);
    DrawColorOpItem(ColorQuad color, BlendMode mode)
        : DrawOpItem(DrawOpItem::COLOR_OPITEM), color_(color), mode_(mode) {}
    ~DrawColorOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    ColorQuad color_;
    BlendMode mode_;
};

class DrawImageNineOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& image, const RectI& center, const Rect& dst, FilterMode filterMode,
            const BrushHandle& brushHandle, bool hasBrush) : OpItem(DrawOpItem::IMAGE_NINE_OPITEM), image(image),
            center(center), dst(dst), filter(filterMode), brushHandle(brushHandle), hasBrush(hasBrush) {}
        ~ConstructorHandle() override = default;
        OpDataHandle image;
        RectI center;
        Rect dst;
        FilterMode filter;
        BrushHandle brushHandle;
        bool hasBrush;
    };
    DrawImageNineOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawImageNineOpItem(const Image* image, const RectI& center, const Rect& dst, FilterMode filterMode,
        const Brush* brush) : DrawOpItem(DrawOpItem::IMAGE_NINE_OPITEM), center_(center), dst_(dst), filter_(filterMode)
    {
        if (brush) {
            hasBrush_ = true;
            brush_ = *brush;
        }
        image_ = std::make_shared<Image>(*image);
    }
    ~DrawImageNineOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RectI center_;
    Rect dst_;
    FilterMode filter_;
    bool hasBrush_;
    Brush brush_;
    std::shared_ptr<Image> image_;
};

class DrawImageLatticeOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& image, const Lattice& lattice, const Rect& dst, FilterMode filterMode,
            const BrushHandle& brushHandle, bool hasBrush) : OpItem(DrawOpItem::IMAGE_LATTICE_OPITEM), image(image),
            lattice(lattice), dst(dst), filter(filterMode), brushHandle(brushHandle), hasBrush(hasBrush) {}
        ~ConstructorHandle() override = default;
        OpDataHandle image;
        Lattice lattice;
        Rect dst;
        FilterMode filter;
        BrushHandle brushHandle;
        bool hasBrush;
    };
    DrawImageLatticeOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawImageLatticeOpItem(const Image* image, const Lattice& lattice, const Rect& dst, FilterMode filterMode,
        const Brush* brush) : DrawOpItem(DrawOpItem::IMAGE_LATTICE_OPITEM), lattice_(lattice), dst_(dst),
        filter_(filterMode)
    {
        if (brush) {
            hasBrush_ = true;
            brush_ = *brush;
        }
        image_ = std::make_shared<Image>(*image);
    }
    ~DrawImageLatticeOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Lattice lattice_;
    Rect dst_;
    FilterMode filter_;
    bool hasBrush_;
    Brush brush_;
    std::shared_ptr<Image> image_;
};

class DrawBitmapOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& bitmap, scalar px, scalar py, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::BITMAP_OPITEM), bitmap(bitmap), px(px), py(py), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        ImageHandle bitmap;
        scalar px;
        scalar py;
        PaintHandle paintHandle;
    };
    DrawBitmapOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawBitmapOpItem(const Bitmap& bitmap, const scalar px, const scalar py, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::BITMAP_OPITEM), px_(px), py_(py),
          bitmap_(std::make_shared<Bitmap>(bitmap)) {}
    ~DrawBitmapOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;

private:
    scalar px_;
    scalar py_;
    std::shared_ptr<Bitmap> bitmap_;
};

class DrawImageOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& image, scalar px, scalar py, const SamplingOptions& samplingOptions,
            const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::IMAGE_OPITEM), image(image), px(px), py(py), samplingOptions(samplingOptions),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle image;
        scalar px;
        scalar py;
        SamplingOptions samplingOptions;
        PaintHandle paintHandle;
    };
    DrawImageOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawImageOpItem(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling,
        const Paint& paint) : DrawWithPaintOpItem(paint, DrawOpItem::IMAGE_OPITEM), px_(px), py_(py),
        samplingOptions_(sampling), image_(std::make_shared<Image>(image)) {}
    ~DrawImageOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;

private:
    scalar px_;
    scalar py_;
    SamplingOptions samplingOptions_;
    std::shared_ptr<Image> image_;
};

class DrawImageRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
            SrcRectConstraint constraint, const PaintHandle& paintHandle, bool isForeground = false)
            : OpItem(DrawOpItem::IMAGE_RECT_OPITEM), image(image), src(src), dst(dst),
              sampling(sampling), constraint(constraint), paintHandle(paintHandle), isForeground(isForeground) {}
        ~ConstructorHandle() override = default;
        OpDataHandle image;
        Rect src;
        Rect dst;
        SamplingOptions sampling;
        SrcRectConstraint constraint;
        PaintHandle paintHandle;
        bool isForeground;
    };
    DrawImageRectOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawImageRectOpItem(const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
        SrcRectConstraint constraint, const Paint& paint, bool isForeground = false)
        : DrawWithPaintOpItem(paint, DrawOpItem::IMAGE_RECT_OPITEM), src_(src), dst_(dst), sampling_(sampling),
          constraint_(constraint), image_(std::make_shared<Image>(image)), isForeground_(isForeground) {}
    ~DrawImageRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect src_;
    Rect dst_;
    SamplingOptions sampling_;
    SrcRectConstraint constraint_;
    std::shared_ptr<Image> image_;
    bool isForeground_ = false;
};

class DrawPictureOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& picture) : OpItem(DrawOpItem::PICTURE_OPITEM), picture(picture) {}
        ~ConstructorHandle() override = default;
        OpDataHandle picture;
    };
    DrawPictureOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    explicit DrawPictureOpItem(const Picture& picture)
        : DrawOpItem(DrawOpItem::PICTURE_OPITEM), picture_(std::make_shared<Picture>(picture)) {}
    ~DrawPictureOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::shared_ptr<Picture> picture_;
};

class DrawTextBlobOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& textBlob, scalar x, scalar y, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::TEXT_BLOB_OPITEM), textBlob(textBlob), x(x), y(y), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        static bool GenerateCachedOpItem(DrawCmdList& cmdList, const TextBlob* textBlob, scalar x, scalar y, Paint& p);
        bool GenerateCachedOpItem(DrawCmdList& cmdList, Canvas* canvas);
        OpDataHandle textBlob;
        scalar x;
        scalar y;
        PaintHandle paintHandle;
    };
    DrawTextBlobOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawTextBlobOpItem(const TextBlob* blob, const scalar x, const scalar y, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::TEXT_BLOB_OPITEM), x_(x), y_(y),
          textBlob_(std::make_shared<TextBlob>(*blob)) {}
    ~DrawTextBlobOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;

    std::shared_ptr<DrawImageRectOpItem> GenerateCachedOpItem(Canvas* canvas);
protected:
    void DrawHighContrast(Canvas* canvas) const;
private:
    scalar x_;
    scalar y_;
    std::shared_ptr<TextBlob> textBlob_;
    std::shared_ptr<DrawImageRectOpItem> cacheImage_;
    bool callFromCacheFunc_ = false;
};

using DrawSymbolAnimation = struct DrawSymbolAnimation {
    // all animation need
    double startValue = 0;
    double curValue = 0;
    double endValue = 1;
    double speedValue = 0.01;
    uint32_t number = 0; // animate times when reach the destination
    // hierarchy animation need
    long long startDuration = 0;
    std::chrono::milliseconds curTime; // frame timestamp
};

class DrawSymbolOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const SymbolOpHandle& symbolHandle, Point locate, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::SYMBOL_OPITEM), symbolHandle(symbolHandle), locate(locate), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        SymbolOpHandle symbolHandle;
        Point locate;
        PaintHandle paintHandle;
    };
    DrawSymbolOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawSymbolOpItem(const DrawingHMSymbolData& symbol, Point locate, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::SYMBOL_OPITEM), symbol_(symbol), locate_(locate) {}
    ~DrawSymbolOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;

    void SetSymbol() override;

    void InitialScale();

    void InitialVariableColor();

    void SetScale(size_t index);

    void SetVariableColor(size_t index);

    static void UpdateScale(const double cur, Path& path);

    void UpdataVariableColor(const double cur, size_t index);
private:
    static void MergeDrawingPath(
        Path& multPath, DrawingRenderGroup& group, std::vector<Path>& pathLayers);
    DrawingHMSymbolData symbol_;
    Point locate_;

    std::vector<DrawSymbolAnimation> animation_;
    uint32_t number_ = 2; // one animation means a back and forth
    bool startAnimation_ = false; // update animation_ if true
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
    explicit ClipRectOpItem(ConstructorHandle* handle);
    ClipRectOpItem(const Rect& rect, ClipOp op, bool doAntiAlias)
        : DrawOpItem(DrawOpItem::CLIP_RECT_OPITEM), rect_(rect), clipOp_(op), doAntiAlias_(doAntiAlias) {}
    ~ClipRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    explicit ClipIRectOpItem(ConstructorHandle* handle);
    ClipIRectOpItem(const RectI& rect, ClipOp op)
        : DrawOpItem(DrawOpItem::CLIP_IRECT_OPITEM), rect_(rect), clipOp_(op) {}
    ~ClipIRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    explicit ClipRoundRectOpItem(ConstructorHandle* handle);
    ClipRoundRectOpItem(const RoundRect& roundRect, ClipOp op, bool doAntiAlias)
        : DrawOpItem(DrawOpItem::CLIP_ROUND_RECT_OPITEM), rrect_(roundRect), clipOp_(op), doAntiAlias_(doAntiAlias) {}
    ~ClipRoundRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RoundRect rrect_;
    ClipOp clipOp_;
    bool doAntiAlias_;
};

class ClipPathOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& path, ClipOp clipOp, bool doAntiAlias)
            : OpItem(DrawOpItem::CLIP_PATH_OPITEM), path(path), clipOp(clipOp), doAntiAlias(doAntiAlias) {}
        ~ConstructorHandle() override = default;
        OpDataHandle path;
        ClipOp clipOp;
        bool doAntiAlias;
    };
    ClipPathOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    ClipPathOpItem(const Path& path, ClipOp op, bool doAntiAlias)
        : DrawOpItem(DrawOpItem::CLIP_PATH_OPITEM), clipOp_(op), doAntiAlias_(doAntiAlias),
          path_(std::make_shared<Path>(path)) {}
    ~ClipPathOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    ClipOp clipOp_;
    bool doAntiAlias_;
    std::shared_ptr<Path> path_;
};

class ClipRegionOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& region, ClipOp clipOp = ClipOp::INTERSECT)
            : OpItem(DrawOpItem::CLIP_REGION_OPITEM), region(region), clipOp(clipOp) {}
        ~ConstructorHandle() override = default;
        OpDataHandle region;
        ClipOp clipOp;
    };
    ClipRegionOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    ClipRegionOpItem(const Region& region, ClipOp op)
        : DrawOpItem(DrawOpItem::CLIP_REGION_OPITEM), clipOp_(op), region_(std::make_shared<Region>(region)) {}
    ~ClipRegionOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    explicit SetMatrixOpItem(ConstructorHandle* handle);
    explicit SetMatrixOpItem(const Matrix& matrix) : DrawOpItem(DrawOpItem::SET_MATRIX_OPITEM), matrix_(matrix) {}
    ~SetMatrixOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    explicit ConcatMatrixOpItem(ConstructorHandle* handle);
    explicit ConcatMatrixOpItem(const Matrix& matrix) : DrawOpItem(DrawOpItem::CONCAT_MATRIX_OPITEM), matrix_(matrix) {}
    ~ConcatMatrixOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    explicit TranslateOpItem(ConstructorHandle* handle);
    TranslateOpItem(scalar dx, scalar dy) : DrawOpItem(DrawOpItem::TRANSLATE_OPITEM), dx_(dx), dy_(dy) {}
    ~TranslateOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    explicit ScaleOpItem(ConstructorHandle* handle);
    ScaleOpItem(scalar sx, scalar sy) : DrawOpItem(DrawOpItem::SCALE_OPITEM), sx_(sx), sy_(sy) {}
    ~ScaleOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    explicit RotateOpItem(ConstructorHandle* handle);
    RotateOpItem(scalar deg, scalar sx, scalar sy)
        : DrawOpItem(DrawOpItem::ROTATE_OPITEM), deg_(deg), sx_(sx), sy_(sy) {}
    ~RotateOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    explicit ShearOpItem(ConstructorHandle* handle);
    ShearOpItem(scalar sx, scalar sy) : DrawOpItem(DrawOpItem::SHEAR_OPITEM), sx_(sx), sy_(sy) {}
    ~ShearOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class ClearOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(ColorQuad color) : OpItem(DrawOpItem::CLEAR_OPITEM), color(color) {}
        ~ConstructorHandle() override = default;
        ColorQuad color;
    };
    explicit ClearOpItem(ConstructorHandle* handle);
    explicit ClearOpItem(ColorQuad color) : DrawOpItem(DrawOpItem::CLEAR_OPITEM), color_(color) {}
    ~ClearOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class SaveLayerOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, bool hasBrush, const BrushHandle& brushHandle, uint32_t saveLayerFlags)
            : OpItem(DrawOpItem::SAVE_LAYER_OPITEM), rect(rect), hasBrush(hasBrush), brushHandle(brushHandle),
              saveLayerFlags(saveLayerFlags) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        bool hasBrush;
        BrushHandle brushHandle;
        uint32_t saveLayerFlags;
    };
    SaveLayerOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    SaveLayerOpItem(const SaveLayerOps& saveLayerOps)
        : DrawOpItem(DrawOpItem::SAVE_LAYER_OPITEM), saveLayerFlags_(saveLayerOps.GetSaveLayerFlags())
    {
        if (saveLayerOps.GetBounds() != nullptr) {
            rect_ = *saveLayerOps.GetBounds();
        }
        const Brush* brush = saveLayerOps.GetBrush();
        if (brush != nullptr) {
            hasBrush_ = true;
            brush_ = *brush;
        }
    }
    ~SaveLayerOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    uint32_t saveLayerFlags_;
    Rect rect_;
    bool hasBrush_ = false;
    Brush brush_;
};

class RestoreOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::RESTORE_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    RestoreOpItem();
    ~RestoreOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
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
    ClipAdaptiveRoundRectOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    explicit ClipAdaptiveRoundRectOpItem(const std::vector<Point>& radiusData)
        : DrawOpItem(DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM),  radiusData_(radiusData) {}
    ~ClipAdaptiveRoundRectOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::vector<Point> radiusData_;
};

class DrawAdaptiveImageOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& image, const AdaptiveImageInfo& rsImageInfo,
            const SamplingOptions& sampling, const bool isImage, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::ADAPTIVE_IMAGE_OPITEM), image(image), rsImageInfo(rsImageInfo), sampling(sampling),
              isImage(isImage), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle image;
        AdaptiveImageInfo rsImageInfo;
        SamplingOptions sampling;
        bool isImage;
        PaintHandle paintHandle;
    };
    DrawAdaptiveImageOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawAdaptiveImageOpItem(const std::shared_ptr<Image>& image, const std::shared_ptr<Data>& data,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::ADAPTIVE_IMAGE_OPITEM), rsImageInfo_(rsImageInfo), sampling_(sampling)
    {
        if (data != nullptr) {
            data_ = data;
            isImage_ = false;
        } else if (image != nullptr) {
            image_ = image;
            isImage_ = true;
        }
    }
    ~DrawAdaptiveImageOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    AdaptiveImageInfo rsImageInfo_;
    SamplingOptions sampling_;
    bool isImage_;
    std::shared_ptr<Image> image_;
    std::shared_ptr<Data> data_;
};

class DrawAdaptivePixelMapOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& pixelMap, const AdaptiveImageInfo& imageInfo,
            const SamplingOptions& sampling, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::ADAPTIVE_PIXELMAP_OPITEM), pixelMap(pixelMap),
              imageInfo(imageInfo), sampling(sampling), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle pixelMap;
        AdaptiveImageInfo imageInfo;
        SamplingOptions sampling;
        PaintHandle paintHandle;
    };
    DrawAdaptivePixelMapOpItem(const DrawCmdList& cmdList, ConstructorHandle* handle);
    DrawAdaptivePixelMapOpItem(const std::shared_ptr<Media::PixelMap>& pixelMap, const AdaptiveImageInfo& rsImageInfo,
        const SamplingOptions& sampling, const Paint& paint)
        : DrawWithPaintOpItem(paint, DrawOpItem::ADAPTIVE_PIXELMAP_OPITEM), imageInfo_(rsImageInfo),
          sampling_(sampling), pixelMap_(pixelMap) {}
    ~DrawAdaptivePixelMapOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const DrawCmdList& cmdList, void* handle);
    void Marshalling(DrawCmdList& cmdList) override;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    AdaptiveImageInfo imageInfo_;
    SamplingOptions sampling_;
    std::shared_ptr<Media::PixelMap> pixelMap_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
