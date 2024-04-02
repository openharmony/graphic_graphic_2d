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

#include <surface_utils.h>
#include <cinttypes>
#include "securec.h"
#include "buffer_log.h"

namespace OHOS {
using namespace HiviewDFX;
static SurfaceUtils* instance = nullptr;
static std::mutex mutextinstance_;

SurfaceUtils* SurfaceUtils::GetInstance()
{
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lockGuard(mutextinstance_);
        if (instance == nullptr) {
            instance = new SurfaceUtils();
        }
    }

    return instance;
}

SurfaceUtils::~SurfaceUtils()
{
    instance = nullptr;
    surfaceCache_.clear();
    nativeWindowCache_.clear();
}

sptr<Surface> SurfaceUtils::GetSurface(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (surfaceCache_.count(uniqueId) == 0) {
        BLOGE("Cannot find surface by uniqueId %" PRIu64 ".", uniqueId);
        return nullptr;
    }
    return surfaceCache_[uniqueId];
}

SurfaceError SurfaceUtils::Add(uint64_t uniqueId, const sptr<Surface> &surface)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (surface == nullptr) {
        BLOGE(" surface is nullptr.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (surfaceCache_.count(uniqueId) == 0) {
        surfaceCache_[uniqueId] = surface;
        return GSERROR_OK;
    }
    BLOGD("the surface by uniqueId %" PRIu64 " already existed", uniqueId);
    return GSERROR_OK;
}

SurfaceError SurfaceUtils::Remove(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (surfaceCache_.count(uniqueId) == 0) {
        BLOGD("Delete failed without surface by uniqueId %" PRIu64, uniqueId);
        return GSERROR_INVALID_OPERATING;
    }
    surfaceCache_.erase(uniqueId);
    return GSERROR_OK;
}

std::array<float, 16> SurfaceUtils::MatrixProduct(const std::array<float, 16>& lMat, const std::array<float, 16>& rMat)
{
    // Product matrix 4 * 4 = 16
    return std::array<float, 16> {lMat[0] * rMat[0] + lMat[4] * rMat[1] + lMat[8] * rMat[2] + lMat[12] * rMat[3],
                                  lMat[1] * rMat[0] + lMat[5] * rMat[1] + lMat[9] * rMat[2] + lMat[13] * rMat[3],
                                  lMat[2] * rMat[0] + lMat[6] * rMat[1] + lMat[10] * rMat[2] + lMat[14] * rMat[3],
                                  lMat[3] * rMat[0] + lMat[7] * rMat[1] + lMat[11] * rMat[2] + lMat[15] * rMat[3],

                                  lMat[0] * rMat[4] + lMat[4] * rMat[5] + lMat[8] * rMat[6] + lMat[12] * rMat[7],
                                  lMat[1] * rMat[4] + lMat[5] * rMat[5] + lMat[9] * rMat[6] + lMat[13] * rMat[7],
                                  lMat[2] * rMat[4] + lMat[6] * rMat[5] + lMat[10] * rMat[6] + lMat[14] * rMat[7],
                                  lMat[3] * rMat[4] + lMat[7] * rMat[5] + lMat[11] * rMat[6] + lMat[15] * rMat[7],

                                  lMat[0] * rMat[8] + lMat[4] * rMat[9] + lMat[8] * rMat[10] + lMat[12] * rMat[11],
                                  lMat[1] * rMat[8] + lMat[5] * rMat[9] + lMat[9] * rMat[10] + lMat[13] * rMat[11],
                                  lMat[2] * rMat[8] + lMat[6] * rMat[9] + lMat[10] * rMat[10] + lMat[14] * rMat[11],
                                  lMat[3] * rMat[8] + lMat[7] * rMat[9] + lMat[11] * rMat[10] + lMat[15] * rMat[11],

                                  lMat[0] * rMat[12] + lMat[4] * rMat[13] + lMat[8] * rMat[14] + lMat[12] * rMat[15],
                                  lMat[1] * rMat[12] + lMat[5] * rMat[13] + lMat[9] * rMat[14] + lMat[13] * rMat[15],
                                  lMat[2] * rMat[12] + lMat[6] * rMat[13] + lMat[10] * rMat[14] + lMat[14] * rMat[15],
                                  lMat[3] * rMat[12] + lMat[7] * rMat[13] + lMat[11] * rMat[14] + lMat[15] * rMat[15]};
}

void SurfaceUtils::ComputeTransformByMatrix(GraphicTransformType& transform,
    std::array<float, TRANSFORM_MATRIX_ELE_COUNT> *transformMatrix)
{
    const std::array<float, TRANSFORM_MATRIX_ELE_COUNT> rotate90 = {0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    const std::array<float, TRANSFORM_MATRIX_ELE_COUNT> rotate180 = {-1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    const std::array<float, TRANSFORM_MATRIX_ELE_COUNT> rotate270 = {0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    const std::array<float, TRANSFORM_MATRIX_ELE_COUNT> flipH = {-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    const std::array<float, TRANSFORM_MATRIX_ELE_COUNT> flipV = {1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    switch (transform) {
        case GraphicTransformType::GRAPHIC_ROTATE_NONE:
            break;
        case GraphicTransformType::GRAPHIC_ROTATE_90:
            *transformMatrix = MatrixProduct(*transformMatrix, rotate90);
            break;
        case GraphicTransformType::GRAPHIC_ROTATE_180:
            *transformMatrix = MatrixProduct(*transformMatrix, rotate180);
            break;
        case GraphicTransformType::GRAPHIC_ROTATE_270:
            *transformMatrix = MatrixProduct(*transformMatrix, rotate270);
            break;
        case GraphicTransformType::GRAPHIC_FLIP_H:
            *transformMatrix = MatrixProduct(*transformMatrix, flipH);
            break;
        case GraphicTransformType::GRAPHIC_FLIP_V:
            *transformMatrix = MatrixProduct(*transformMatrix, flipV);
            break;
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT90:
            *transformMatrix = MatrixProduct(flipH, rotate90);
            break;
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT90:
            *transformMatrix = MatrixProduct(flipV, rotate90);
            break;
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT180:
            *transformMatrix = MatrixProduct(flipH, rotate180);
            break;
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT180:
            *transformMatrix = MatrixProduct(flipV, rotate180);
            break;
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT270:
            *transformMatrix = MatrixProduct(flipH, rotate270);
            break;
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT270:
            *transformMatrix = MatrixProduct(flipV, rotate270);
            break;
        default:
            break;
    }
}

void SurfaceUtils::ComputeTransformMatrix(float matrix[16],
    sptr<SurfaceBuffer>& buffer, GraphicTransformType& transform, Rect& crop)
{
    float tx = 0.f;
    float ty = 0.f;
    float sx = 1.f;
    float sy = 1.f;
    std::array<float, TRANSFORM_MATRIX_ELE_COUNT> transformMatrix = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    ComputeTransformByMatrix(transform, &transformMatrix);

    float bufferWidth = buffer->GetWidth();
    float bufferHeight = buffer->GetHeight();
    bool changeFlag = false;
    if (crop.w < bufferWidth && bufferWidth != 0) {
        tx = (float(crop.x) / bufferWidth);
        sx = (float(crop.w) / bufferWidth);
        changeFlag = true;
    }
    if (crop.h < bufferHeight && bufferHeight != 0) {
        ty = (float(bufferHeight - crop.y) / bufferHeight);
        sy = (float(crop.h) / bufferHeight);
        changeFlag = true;
    }
    if (changeFlag) {
        static const std::array<float, 16> cropMatrix = {sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, 1, 0, tx, ty, 0, 1};
        transformMatrix = MatrixProduct(cropMatrix, transformMatrix);
    }

    auto ret = memcpy_s(matrix, sizeof(transformMatrix),
                        transformMatrix.data(), sizeof(transformMatrix));
    if (ret != EOK) {
        BLOGE("ComputeTransformMatrix: transformMatrix memcpy_s failed");
    }
}

void* SurfaceUtils::GetNativeWindow(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (nativeWindowCache_.count(uniqueId) == 0) {
        BLOGE("Cannot find nativeWindow by uniqueId %" PRIu64 ".", uniqueId);
        return nullptr;
    }
    return nativeWindowCache_[uniqueId];
}

SurfaceError SurfaceUtils::AddNativeWindow(uint64_t uniqueId, void *nativeWidow)
{
    if (nativeWidow == nullptr) {
        BLOGE("nativeWidow is nullptr.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (nativeWindowCache_.count(uniqueId) == 0) {
        nativeWindowCache_[uniqueId] = nativeWidow;
        return GSERROR_OK;
    }
    BLOGD("the nativeWidow by uniqueId %" PRIu64 " already existed", uniqueId);
    return GSERROR_OK;
}

SurfaceError SurfaceUtils::RemoveNativeWindow(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    nativeWindowCache_.erase(uniqueId);
    return GSERROR_OK;
}
} // namespace OHOS
