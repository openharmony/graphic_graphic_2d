/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef PATH_TEST_H
#define PATH_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "../test_base.h"
#include "test_common.h"

class PathTransformWithPerspectiveClip : public TestBase {
public:
    PathTransformWithPerspectiveClip(int type, bool bClip) : TestBase(type), applyPerspectiveClip(bClip)
    {
        fileName_ = "PathTransformWithPerspectiveClip";
    }
    ~PathTransformWithPerspectiveClip() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 358 ms 100次
    bool applyPerspectiveClip;
};

class PathSetFillType : public TestBase {
public:
    PathSetFillType(int type, OH_Drawing_PathFillType fillType) : TestBase(type), fType(fillType)
    {
        fileName_ = "PathSetFillType";
    }
    ~PathSetFillType() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 712 ms 100次
    OH_Drawing_PathFillType fType;
};

class PathGetLength : public TestBase {
public:
    PathGetLength(int type, bool bForceClosed) : TestBase(type), bClosed(bForceClosed)
    {
        fileName_ = "PathGetLength";
    }
    ~PathGetLength() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 768 ms 100次
    bool bClosed;
};

class PathClose : public TestBase {
public:
    explicit PathClose(int type) : TestBase(type)
    {
        fileName_ = "PathClose";
    }
    ~PathClose() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 1544 ms 100次
};

class PathOffset : public TestBase {
public:
    explicit PathOffset(int type) : TestBase(type)
    {
        fileName_ = "PathOffset";
    }
    ~PathOffset() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 454 ms 100次
};

class PathReset : public TestBase {
public:
    explicit PathReset(int type) : TestBase(type)
    {
        fileName_ = "PathReset";
    }
    ~PathReset() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 2613 ms 100次
};

class PathCubicTo : public TestBase {
public:
    explicit PathCubicTo(int type) : TestBase(type)
    {
        fileName_ = "PathCubicTo";
    }
    ~PathCubicTo() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 2041 ms 100次
};

class PathRMoveTo : public TestBase {
public:
    explicit PathRMoveTo(int type) : TestBase(type)
    {
        fileName_ = "PathRMoveTo";
    }
    ~PathRMoveTo() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 1299 ms 100次
};

class PathRLineTo : public TestBase {
public:
    explicit PathRLineTo(int type) : TestBase(type)
    {
        fileName_ = "PathRLineTo";
    }
    ~PathRLineTo() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 767 ms 100次
};

class PathRQuadTo : public TestBase {
public:
    explicit PathRQuadTo(int type) : TestBase(type)
    {
        fileName_ = "PathRQuadTo";
    }
    ~PathRQuadTo() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 1184 ms 100次
};

class PathRConicTo : public TestBase {
public:
    explicit PathRConicTo(int type) : TestBase(type)
    {
        fileName_ = "PathRConicTo";
    }
    ~PathRConicTo() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 931 ms 100次
};

class PathRCubicTo : public TestBase {
public:
    explicit PathRCubicTo(int type) : TestBase(type)
    {
        fileName_ = "PathRCubicTo";
    }
    ~PathRCubicTo() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 1124 ms 100次
};

class PathAddRect : public TestBase {
public:
    explicit PathAddRect(int type) : TestBase(type)
    {
        fileName_ = "PathAddRect";
    }
    ~PathAddRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 727 ms 100次
};

class PathAddRectWithInitialCorner : public TestBase {
public:
    explicit PathAddRectWithInitialCorner(int type) : TestBase(type)
    {
        fileName_ = "PathAddRectWithInitialCorner";
    }
    ~PathAddRectWithInitialCorner() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 739 ms 100次
};

class PathAddRoundRect : public TestBase {
public:
    explicit PathAddRoundRect(int type) : TestBase(type)
    {
        fileName_ = "PathAddRoundRect";
    }
    ~PathAddRoundRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 783 ms 100次
};

class PathAddPath : public TestBase {
public:
    explicit PathAddPath(int type) : TestBase(type)
    {
        fileName_ = "PathAddPath";
    }
    ~PathAddPath() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 2348 ms 100次
};

class PathAddPathWithMode : public TestBase {
public:
    PathAddPathWithMode(int type, OH_Drawing_PathAddMode mode) : TestBase(type), addMode(mode)
    {
        fileName_ = "PathAddPathWithMode";
    }
    ~PathAddPathWithMode() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 2133 ms 100次
    OH_Drawing_PathAddMode addMode;
};

class PathAddPathWithOffsetAndMode : public TestBase {
public:
    PathAddPathWithOffsetAndMode(int type, OH_Drawing_PathAddMode mode) : TestBase(type), addMode(mode)
    {
        fileName_ = "PathAddPathWithOffsetAndMode";
    }
    ~PathAddPathWithOffsetAndMode() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override; // 3547 ms 100次
    OH_Drawing_PathAddMode addMode;
};

class PathEffectCreateDashPathEffect : public TestBase {
public:
    explicit PathEffectCreateDashPathEffect(int type) : TestBase(type) {};
    ~PathEffectCreateDashPathEffect() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathCreate : public TestBase {
public:
    explicit PathCreate(int type) : TestBase(type) {};
    ~PathCreate() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathLineTo : public TestBase {
public:
    explicit PathLineTo(int type) : TestBase(type) {};
    ~PathLineTo() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathArcTo : public TestBase {
public:
    explicit PathArcTo(int type) : TestBase(type) {};
    ~PathArcTo() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathQuadTo : public TestBase {
public:
    explicit PathQuadTo(int type) : TestBase(type) {};
    ~PathQuadTo() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathCopy : public TestBase {
public:
    explicit PathCopy(int type) : TestBase(type) {};
    ~PathCopy() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathConicTo : public TestBase {
public:
    explicit PathConicTo(int type) : TestBase(type) {};
    ~PathConicTo() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathAddPathWithMatrixAndMode : public TestBase {
public:
    explicit PathAddPathWithMatrixAndMode(int type) : TestBase(type) {};
    ~PathAddPathWithMatrixAndMode() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class PathAddOvalWithInitialPoint : public TestBase {
public:
    explicit PathAddOvalWithInitialPoint(int type) : TestBase(type) {};
    ~PathAddOvalWithInitialPoint() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    TestRend renderer;
};

class PathAddArc : public TestBase {
public:
    explicit PathAddArc(int type) : TestBase(type) {};
    ~PathAddArc() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    TestRend renderer;
};
class PathTransform : public TestBase {
public:
    explicit PathTransform(int type) : TestBase(type) {};
    ~PathTransform() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathAddOval : public TestBase {
public:
    explicit PathAddOval(int type) : TestBase(type) {};
    ~PathAddOval() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathContains : public TestBase {
public:
    explicit PathContains(int type) : TestBase(type) {};
    ~PathContains() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif // PATH_TEST_H