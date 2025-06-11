/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cj_matrix.h"
#include "cj_drawing_log.h"
#include "utils/point.h"

static constexpr uint32_t POLY_POINT_COUNT_MAX = 4;

namespace OHOS {
namespace CJDrawing {
using namespace Rosen::Drawing;

int64_t CjMatrix::CreateCjMatrix(int64_t id)
{
    std::shared_ptr<Matrix> matrix = nullptr;
    if (id == 0) {
        matrix = std::make_shared<Matrix>();
    } else {
        auto cjMatrix = FFIData::GetData<CjMatrix>(id);
        if (cjMatrix == nullptr || cjMatrix->GetMatrix() == nullptr) {
            matrix = std::make_shared<Matrix>();
        } else {
            matrix = std::make_shared<Matrix>(*(cjMatrix->GetMatrix()));
        }
    }
    if (matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::CreateCjMatrix get matrix failed");
        return 0;
    }
    auto instance = FFIData::Create<CjMatrix>(matrix);
    if (instance == nullptr) {
        ROSEN_LOGE("CjMatrix::CreateCjMatrix create FFIData failed");
        return 0;
    }
    return instance->GetID();
}

int32_t CjMatrix::SetRotation(scalar degree, scalar px, scalar py)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetRotation matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->Rotate(degree, px, py);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::SetScale(scalar sx, scalar sy, scalar px, scalar py)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetScale matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->Scale(sx, sy, px, py);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::SetTranslation(scalar dx, scalar dy)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetTranslation matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->Translate(dx, dy);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::SetSkew(scalar kx, scalar ky, scalar px, scalar py)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetSkew matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->SetSkew(kx, ky, px, py);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::SetConcat(CjMatrix& matrix1, CjMatrix& matrix2)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetConcat matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    std::shared_ptr<Matrix> nativeMatrix1 = matrix1.GetMatrix();
    std::shared_ptr<Matrix> nativeMatrix2 = matrix2.GetMatrix();
    if (nativeMatrix1 == nullptr || nativeMatrix2 == nullptr) {
        ROSEN_LOGE("CjMatrix::SetConcat param matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::OK);
    }
    m_matrix->SetConcat(*nativeMatrix1, *nativeMatrix2);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::SetSinCos(scalar sinValue, scalar cosValue, scalar px, scalar py)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetSinCos matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->SetSinCos(sinValue, cosValue, px, py);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::SetMatrix(const CArrFloat& values)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetMatrix matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    if (values.size != Matrix::MATRIX_SIZE) {
        ROSEN_LOGE("CjMatrix::SetMatrix count of array is not nine");
        return ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED);
    }
    m_matrix->SetMatrix(values.head[ARGC_ZERO], values.head[ARGC_ONE], values.head[ARGC_TWO], values.head[ARGC_THREE],
                        values.head[ARGC_FOUR], values.head[ARGC_FIVE], values.head[ARGC_SIX], values.head[ARGC_SEVEN],
                        values.head[ARGC_EIGHT]);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::SetMatrixByMatrix(CjMatrix& matrix)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetMatrixByMatrix matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    auto nativeMatrix = matrix.GetMatrix();
    if (nativeMatrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetMatrixByMatrix param matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::OK);
    }
    *m_matrix = *nativeMatrix;
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::PostRotate(scalar degree, scalar px, scalar py)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::PostRotate matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->PostRotate(degree, px, py);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::PostScale(scalar sx, scalar sy, scalar px, scalar py)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::PostScale matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->PostScale(sx, sy, px, py);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::PostTranslate(scalar dx, scalar dy)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::PostTranslate matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->PostTranslate(dx, dy);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::PostSkew(scalar kx, scalar ky, scalar px, scalar py)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::PostSkew matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->PostSkew(kx, ky, px, py);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::PostConcat(CjMatrix& matrix)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::PostConcat matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    if (matrix.GetMatrix() == nullptr) {
        ROSEN_LOGE("CjMatrix::PostConcat param matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::OK);
    }
    m_matrix->PostConcat(*matrix.GetMatrix());
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::PreRotate(scalar degree, scalar px, scalar py)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::PreRotate matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->PreRotate(degree, px, py);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::PreScale(scalar sx, scalar sy, scalar px, scalar py)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::PreScale matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->PreScale(sx, sy, px, py);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::PreTranslate(scalar dx, scalar dy)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::PreTranslate matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->PreTranslate(dx, dy);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::PreSkew(scalar kx, scalar ky, scalar px, scalar py)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::PreSkew matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->PreSkew(kx, ky, px, py);
    return ParserErr(CjDrawingErrorCode::OK);
}

int32_t CjMatrix::PreConcat(CjMatrix& matrix)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::PreConcat matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    if (matrix.GetMatrix() == nullptr) {
        ROSEN_LOGE("CjMatrix::PreConcat param matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::OK);
    }
    m_matrix->PreConcat(*matrix.GetMatrix());
    return ParserErr(CjDrawingErrorCode::OK);
}

RetDataBool CjMatrix::Invert(CjMatrix& matrix)
{
    RetDataBool ret = {.code = ParserErr(CjDrawingErrorCode::OK), .data = false};
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::Invert matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
        return ret;
    }
    if (matrix.GetMatrix() == nullptr) {
        ROSEN_LOGE("CjMatrix::Invert param matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED);
        return ret;
    }
    ret.data = m_matrix->Invert(*matrix.GetMatrix());
    return ret;
}

int32_t CjMatrix::Reset()
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::Reset matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    m_matrix->Reset();
    return ParserErr(CjDrawingErrorCode::OK);
}

RetDataBool CjMatrix::IsEqual(CjMatrix& matrix)
{
    RetDataBool ret = {.code = ParserErr(CjDrawingErrorCode::OK), .data = false};
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::IsEqual matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
        return ret;
    }
    if (matrix.GetMatrix() == nullptr) {
        ROSEN_LOGE("CjMatrix::IsEqual param matrix is nullptr");
        return ret;
    }
    ret.data = ((*m_matrix) == (*matrix.GetMatrix()));
    return ret;
}

RetDataBool CjMatrix::IsIdentity()
{
    RetDataBool ret = {.code = ParserErr(CjDrawingErrorCode::OK), .data = false};
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::IsIdentity matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
        return ret;
    }
    ret.data = m_matrix->IsIdentity();
    return ret;
}

RetDataBool CjMatrix::IsAffine()
{
    RetDataBool ret = {.code = ParserErr(CjDrawingErrorCode::OK), .data = false};
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::IsAffine matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
        return ret;
    }
    ret.data = m_matrix->IsAffine();
    return ret;
}

RetDataBool CjMatrix::RectStaysRect()
{
    RetDataBool ret = {.code = ParserErr(CjDrawingErrorCode::OK), .data = false};
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::RectStaysRect matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
        return ret;
    }
    ret.data = m_matrix->RectStaysRect();
    return ret;
}

RetDataScalar CjMatrix::GetValue(int64_t index)
{
    RetDataScalar ret = {.code = ParserErr(CjDrawingErrorCode::OK), .data = 0.0};
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::GetValue matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
        return ret;
    }
    if (index < 0 || index >= Matrix::MATRIX_SIZE) {
        ROSEN_LOGE("CjMatrix::GetValue index is out of range");
        ret.code = ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED);
        return ret;
    }
    ret.data = m_matrix->Get(index);
    return ret;
}

int32_t CjMatrix::GetAll(const CArrFloat& arr)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::GetAll matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    Matrix::Buffer matrData;
    m_matrix->GetAll(matrData);
    for (uint32_t i = 0; i < Matrix::MATRIX_SIZE; i++) {
        arr.head[i] = matrData[i];
    }
    return ParserErr(CjDrawingErrorCode::OK);
}

RetDataScalar CjMatrix::MapRadius(scalar radius)
{
    RetDataScalar ret = {.code = ParserErr(CjDrawingErrorCode::OK), .data = 0.0};
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::MapRadius matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
        return ret;
    }
    ret.data = m_matrix->MapRadius(radius);
    return ret;
}

int32_t CjMatrix::MapPoints(const CArrPoint& src, const CArrPoint& dst)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::MapPoints matrix is nullptr");
        return ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
    }
    std::vector<Point> dstPoints(src.size);
    std::vector<Point> srcPoints(src.size);
    ConvertToPointsArray(src, srcPoints.data(), src.size);
    m_matrix->MapPoints(dstPoints, srcPoints, src.size);
    WriteToCjPointsArray(dstPoints.data(), dst, src.size);
    return ParserErr(CjDrawingErrorCode::OK);
}

RetDataBool CjMatrix::MapRect(CRect& dst, const CRect& src)
{
    RetDataBool ret = {.code = ParserErr(CjDrawingErrorCode::OK), .data = false};
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::MapRect matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
        return ret;
    }
    Rect dstRect;
    Rect srcRect = Rect(src.left, src.top, src.right, src.bottom);
    ret.data = m_matrix->MapRect(dstRect, srcRect);
    dst.left = dstRect.GetLeft();
    dst.top = dstRect.GetTop();
    dst.right = dstRect.GetRight();
    dst.bottom = dstRect.GetBottom();
    return ret;
}

RetDataBool CjMatrix::SetRectToRect(const CRect& src, const CRect& dst, int32_t scaleToFit)
{
    RetDataBool ret = {.code = ParserErr(CjDrawingErrorCode::OK), .data = false};
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetRectToRect matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
        return ret;
    }
    Rect srcRect = Rect(src.left, src.top, src.right, src.bottom);
    Rect dstRect = Rect(dst.left, dst.top, dst.right, dst.bottom);
    ret.data = m_matrix->SetRectToRect(srcRect, dstRect, static_cast<ScaleToFit>(scaleToFit));
    return ret;
}

RetDataBool CjMatrix::SetPolyToPoly(const CArrPoint& src, const CArrPoint& dst, int64_t count)
{
    RetDataBool ret = {.code = ParserErr(CjDrawingErrorCode::OK), .data = false};
    if (m_matrix == nullptr) {
        ROSEN_LOGE("CjMatrix::SetPolyToPoly matrix is nullptr");
        ret.code = ParserErr(CjDrawingErrorCode::INTERNAL_ERROR);
        return ret;
    }
    if (count > POLY_POINT_COUNT_MAX || count < 0) {
        ROSEN_LOGE("CjMatrix::SetPolyToPoly invalid param");
        ret.code = ParserErr(CjDrawingErrorCode::ERROR_PARAM_VERIFICATION_FAILED);
        return ret;
    }
    Point srcPoints[POLY_POINT_COUNT_MAX];
    Point dstPoints[POLY_POINT_COUNT_MAX];
    ConvertToPointsArray(src, srcPoints, count);
    ConvertToPointsArray(dst, dstPoints, count);
    ret.data = m_matrix->SetPolyToPoly(srcPoints, dstPoints, count);
    return ret;
}

CjMatrix::~CjMatrix()
{
    m_matrix = nullptr;
}

std::shared_ptr<Matrix> CjMatrix::GetMatrix()
{
    return m_matrix;
}
}
}
