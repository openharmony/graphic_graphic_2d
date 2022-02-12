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
#define CASE_MAP(S, T) case (S): cCap = (T); break
        CASE_MAP(Pen::CapStyle::FLAT_CAP, LINE_FLAT_CAP);
        CASE_MAP(Pen::CapStyle::SQUARE_CAP, LINE_SQUARE_CAP);
        CASE_MAP(Pen::CapStyle::ROUND_CAP, LINE_ROUND_CAP);
#undef CASE_MAP
    }
    return cCap;
}

static Pen::CapStyle CCapCastToCap(OH_Drawing_PenLineCapStyle cCap)
{
    Pen::CapStyle cap = Pen::CapStyle::FLAT_CAP;
    switch (cCap) {
#define CASE_MAP(S, T) case (S): cap = (T); break
        CASE_MAP(LINE_FLAT_CAP, Pen::CapStyle::FLAT_CAP);
        CASE_MAP(LINE_SQUARE_CAP, Pen::CapStyle::SQUARE_CAP);
        CASE_MAP(LINE_ROUND_CAP, Pen::CapStyle::ROUND_CAP);
#undef CASE_MAP
    }
    return cap;
}

static OH_Drawing_PenLineJoinStyle JoinCastToCJoin(Pen::JoinStyle join)
{
    OH_Drawing_PenLineJoinStyle cJoin = LINE_MITER_JOIN;
    switch (join) {
#define CASE_MAP(S, T) case (S): cJoin = (T); break
        CASE_MAP(Pen::JoinStyle::MITER_JOIN, LINE_MITER_JOIN);
        CASE_MAP(Pen::JoinStyle::ROUND_JOIN, LINE_ROUND_JOIN);
        CASE_MAP(Pen::JoinStyle::BEVEL_JOIN, LINE_BEVEL_JOIN);
#undef CASE_MAP
    }
    return cJoin;
}

static Pen::JoinStyle CJoinCastToJoin(OH_Drawing_PenLineJoinStyle cJoin)
{
    Pen::JoinStyle join = Pen::JoinStyle::MITER_JOIN;
    switch (cJoin) {
#define CASE_MAP(S, T) case (S): join = (T); break
        CASE_MAP(LINE_MITER_JOIN, Pen::JoinStyle::MITER_JOIN);
        CASE_MAP(LINE_ROUND_JOIN, Pen::JoinStyle::ROUND_JOIN);
        CASE_MAP(LINE_BEVEL_JOIN, Pen::JoinStyle::BEVEL_JOIN);
#undef CASE_MAP
    }
    return join;
}

OH_Drawing_Pen* OH_Drawing_PenCreate()
{
    return (OH_Drawing_Pen*)new Pen;
}

void OH_Drawing_PenDestory(OH_Drawing_Pen* cPen)
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

OH_Drawing_Color OH_Drawing_PenGetColor(const OH_Drawing_Pen* cPen)
{
    return CastToPen(*cPen).GetColor().CastToColorQuad();
}

void OH_Drawing_PenSetColor(OH_Drawing_Pen* cPen, OH_Drawing_Color c)
{
    CastToPen(cPen)->SetColor(c);
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
