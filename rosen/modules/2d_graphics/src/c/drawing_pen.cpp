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

#include "c/drawing_pen.h"

#include "draw/pen.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Pen* CastToPen(OH_Drawing_Pen* cPen)
{
    return reinterpret_cast<Pen*>(cPen);
}

static const Pen& CastToPen(const OH_Drawing_Pen& cPen)
{
    return reinterpret_cast<const Pen&>(cPen);
}

static OH_Drawing_PenLineCapStyle CapCastToCCap(Pen::CapStyle cap)
{
    OH_Drawing_PenLineCapStyle cCap = LINE_FLAT_CAP;
    switch (cap) {
        case Pen::CapStyle::FLAT_CAP:
            cCap = LINE_FLAT_CAP;
            break;
        case Pen::CapStyle::SQUARE_CAP:
            cCap = LINE_SQUARE_CAP;
            break;
        case Pen::CapStyle::ROUND_CAP:
            cCap = LINE_ROUND_CAP;
            break;
        default:
            break;
    }
    return cCap;
}

static Pen::CapStyle CCapCastToCap(OH_Drawing_PenLineCapStyle cCap)
{
    Pen::CapStyle cap = Pen::CapStyle::FLAT_CAP;
    switch (cCap) {
        case LINE_FLAT_CAP:
            cap = Pen::CapStyle::FLAT_CAP;
            break;
        case LINE_SQUARE_CAP:
            cap = Pen::CapStyle::SQUARE_CAP;
            break;
        case LINE_ROUND_CAP:
            cap = Pen::CapStyle::ROUND_CAP;
            break;
        default:
            break;
    }
    return cap;
}

static OH_Drawing_PenLineJoinStyle JoinCastToCJoin(Pen::JoinStyle join)
{
    OH_Drawing_PenLineJoinStyle cJoin = LINE_MITER_JOIN;
    switch (join) {
        case Pen::JoinStyle::MITER_JOIN:
            cJoin = LINE_MITER_JOIN;
            break;
        case Pen::JoinStyle::ROUND_JOIN:
            cJoin = LINE_ROUND_JOIN;
            break;
        case Pen::JoinStyle::BEVEL_JOIN:
            cJoin = LINE_BEVEL_JOIN;
            break;
        default:
            break;
    }
    return cJoin;
}

static Pen::JoinStyle CJoinCastToJoin(OH_Drawing_PenLineJoinStyle cJoin)
{
    Pen::JoinStyle join = Pen::JoinStyle::MITER_JOIN;
    switch (cJoin) {
        case LINE_MITER_JOIN:
            join = Pen::JoinStyle::MITER_JOIN;
            break;
        case LINE_ROUND_JOIN:
            join = Pen::JoinStyle::ROUND_JOIN;
            break;
        case LINE_BEVEL_JOIN:
            join = Pen::JoinStyle::BEVEL_JOIN;
            break;
        default:
            break;
    }
    return join;
}

OH_Drawing_Pen* OH_Drawing_PenCreate()
{
    return (OH_Drawing_Pen*)new Pen;
}

void OH_Drawing_PenDestroy(OH_Drawing_Pen* cPen)
{
    delete CastToPen(cPen);
}

bool OH_Drawing_PenIsAntiAlias(const OH_Drawing_Pen* cPen)
{
    return CastToPen(*cPen).IsAntiAlias();
}

void OH_Drawing_PenSetAntiAlias(OH_Drawing_Pen* cPen, bool aa)
{
    CastToPen(cPen)->SetAntiAlias(aa);
}

uint32_t OH_Drawing_PenGetColor(const OH_Drawing_Pen* cPen)
{
    return CastToPen(*cPen).GetColor().CastToColorQuad();
}

void OH_Drawing_PenSetColor(OH_Drawing_Pen* cPen, uint32_t color)
{
    CastToPen(cPen)->SetColor(color);
}

float OH_Drawing_PenGetWidth(const OH_Drawing_Pen* cPen)
{
    return CastToPen(*cPen).GetWidth();
}

void OH_Drawing_PenSetWidth(OH_Drawing_Pen* cPen, float width)
{
    CastToPen(cPen)->SetWidth(width);
}

float OH_Drawing_PenGetMiterLimit(const OH_Drawing_Pen* cPen)
{
    return CastToPen(*cPen).GetMiterLimit();
}

void OH_Drawing_PenSetMiterLimit(OH_Drawing_Pen* cPen, float miter)
{
    CastToPen(cPen)->SetMiterLimit(miter);
}

OH_Drawing_PenLineCapStyle OH_Drawing_PenGetCap(const OH_Drawing_Pen* cPen)
{
    Pen::CapStyle cap = CastToPen(*cPen).GetCapStyle();
    OH_Drawing_PenLineCapStyle cCap = CapCastToCCap(cap);
    return cCap;
}

void OH_Drawing_PenSetCap(OH_Drawing_Pen* cPen, OH_Drawing_PenLineCapStyle cCap)
{
    Pen::CapStyle cap = CCapCastToCap(cCap);
    CastToPen(cPen)->SetCapStyle(cap);
}

OH_Drawing_PenLineJoinStyle OH_Drawing_PenGetJoin(const OH_Drawing_Pen* cPen)
{
    Pen::JoinStyle join = CastToPen(*cPen).GetJoinStyle();
    OH_Drawing_PenLineJoinStyle cJoin = JoinCastToCJoin(join);
    return cJoin;
}

void OH_Drawing_PenSetJoin(OH_Drawing_Pen* cPen, OH_Drawing_PenLineJoinStyle cJoin)
{
    Pen::JoinStyle join = CJoinCastToJoin(cJoin);
    CastToPen(cPen)->SetJoinStyle(join);
}
