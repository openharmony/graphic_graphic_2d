/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drawing_path_iterator.h"

#include "draw/path_iterator.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

constexpr uint32_t MAX_PAIRS_PATHVERB = 4;

class CPathIterator {
public:
    explicit CPathIterator(const Path& path)
    {
        pathIterator_ = new PathIterator(path);
    }

    ~CPathIterator()
    {
        if (pathIterator_) {
            delete pathIterator_;
        }
    }

    PathVerb Next(Point* points, uint32_t count, uint32_t offset)
    {
        PathVerb verb = getReturnVerb(verb_);
        verb_ = PathVerb::UNINIT;
        if (points != nullptr && count >= offset + MAX_PAIRS_PATHVERB) {
            for (uint32_t i = 0; i < MAX_PAIRS_PATHVERB; i++) {
                points[offset + i].SetX(points_[i].GetX());
                points[offset + i].SetY(points_[i].GetY());
            }
        }
        return verb;
    }

    bool HasNext()
    {
        if (verb_ == PathVerb::UNINIT) {
            verb_ = nextInternal();
        }
        return verb_ != PathVerb::DONE;
    }

    PathVerb Peek() const
    {
        if (isDone_) {
            return PathVerb::DONE;
        }
        return pathIterator_->Peek();
    }

private:
    PathVerb nextInternal()
    {
        if (isDone_) {
            return PathVerb::DONE;
        }
        PathVerb verb = pathIterator_->Next(points_);
        if (verb == PathVerb::CONIC) {
            float weight = pathIterator_->ConicWeight();
            points_[MAX_PAIRS_PATHVERB - 1].SetX(weight);
        } else if (verb == PathVerb::DONE) {
            isDone_ = true;
        }
        return verb;
    }

    PathVerb getReturnVerb(const PathVerb& cachedVerb)
    {
        switch (cachedVerb) {
            case PathVerb::MOVE: return PathVerb::MOVE;
            case PathVerb::LINE: return PathVerb::LINE;
            case PathVerb::QUAD: return PathVerb::QUAD;
            case PathVerb::CONIC: return PathVerb::CONIC;
            case PathVerb::CUBIC: return PathVerb::CUBIC;
            case PathVerb::CLOSE: return PathVerb::CLOSE;
            case PathVerb::DONE: return PathVerb::DONE;
            default: return nextInternal();
        }
    }

    PathIterator* pathIterator_ = nullptr;
    PathVerb verb_ = PathVerb::UNINIT;
    bool isDone_ = false;
    Point points_[MAX_PAIRS_PATHVERB] = {{0, 0}};
};

static Point* CastToPoint(OH_Drawing_Point2D* cPoint)
{
    return reinterpret_cast<Point*>(cPoint);
}

static const Path* CastToPath(const OH_Drawing_Path* cPath)
{
    return reinterpret_cast<const Path*>(cPath);
}

static const CPathIterator* CastToCPathIterator(const OH_Drawing_PathIterator* cPathIterator)
{
    return reinterpret_cast<const CPathIterator*>(cPathIterator);
}

static CPathIterator* CastToCPathIterator(OH_Drawing_PathIterator* cPathIterator)
{
    return reinterpret_cast<CPathIterator*>(cPathIterator);
}

OH_Drawing_ErrorCode OH_Drawing_PathIteratorCreate(const OH_Drawing_Path* cPath,
    OH_Drawing_PathIterator** cPathIterator)
{
    const Path* path = CastToPath(cPath);
    if (path == nullptr || cPathIterator == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    CPathIterator* pathIterator = new CPathIterator(*path);
    *cPathIterator = reinterpret_cast<OH_Drawing_PathIterator*>(pathIterator);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_PathIteratorDestroy(OH_Drawing_PathIterator* pathIterator)
{
    if (pathIterator == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    delete reinterpret_cast<CPathIterator*>(pathIterator);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_PathIteratorHasNext(const OH_Drawing_PathIterator* cPathIterator, bool* hasNext)
{
    const CPathIterator* pathIterator = CastToCPathIterator(cPathIterator);
    if (pathIterator == nullptr || hasNext == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    CPathIterator* mutPathIterator = const_cast<CPathIterator*>(pathIterator);
    *hasNext = mutPathIterator->HasNext();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_PathIteratorNext(OH_Drawing_PathIterator* cPathIterator,
    OH_Drawing_Point2D* cPoints, uint32_t count, uint32_t offset, OH_Drawing_PathIteratorVerb* verb)
{
    CPathIterator* pathIterator = CastToCPathIterator(cPathIterator);
    Point* points = CastToPoint(cPoints);
    if (pathIterator == nullptr || points == nullptr || verb == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    if (count < offset || count - offset < MAX_PAIRS_PATHVERB) {
        return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
    }
    *verb = static_cast<OH_Drawing_PathIteratorVerb>(pathIterator->Next(points, count, offset));
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_PathIteratorPeek(const OH_Drawing_PathIterator* cPathIterator,
    OH_Drawing_PathIteratorVerb* verb)
{
    const CPathIterator* pathIterator = CastToCPathIterator(cPathIterator);
    if (pathIterator == nullptr || verb == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    *verb = static_cast<OH_Drawing_PathIteratorVerb>(pathIterator->Peek());
    return OH_DRAWING_SUCCESS;
}