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

#include "skia_vertices.h"

#include <unistd.h>
#include <securec.h>

#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
#include "src/core/SkVerticesPriv.h"

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaVertices::SkiaVertices() noexcept : skiaVertices_(nullptr) {}

SkiaVertices::SkiaVertices(sk_sp<SkVertices> vertices) noexcept : skiaVertices_(vertices) {}

const sk_sp<SkVertices> SkiaVertices::GetVertices() const
{
    return skiaVertices_;
}

bool SkiaVertices::MakeCopy(VertexMode mode,
    int vertexCount, const Point positions[], const Point texs[], const ColorQuad colors[],
    int indexCount, const uint16_t indices[])
{
    std::vector<SkPoint> skPts = {};
    if (positions != nullptr) {
        skPts.resize(vertexCount);
        if (memcpy_s(skPts.data(), skPts.size() * sizeof(SkPoint),
            positions, vertexCount * sizeof(Point)) != EOK) {
            return false;
        }
    }

    std::vector<SkPoint> skTexs = {};
    if (positions != nullptr) {
        skTexs.resize(vertexCount);
        if (memcpy_s(skTexs.data(), skTexs.size() * sizeof(SkPoint),
            texs, vertexCount * sizeof(Point)) != EOK) {
            return false;
        }
    }

    std::vector<SkColor> skColors = {};
    if (colors != nullptr) {
        skColors.resize(vertexCount);
        if (memcpy_s(skColors.data(), skColors.size() * sizeof(SkColor),
            colors, vertexCount * sizeof(ColorQuad)) != EOK) {
            return false;
        }
    }

    skiaVertices_ = SkVertices::MakeCopy(static_cast<SkVertices::VertexMode>(mode), vertexCount,
        skPts.empty() ? nullptr : skPts.data(),
        skTexs.empty() ? nullptr : skTexs.data(),
        skColors.empty() ? nullptr : skColors.data(),
        indexCount, indices);
    return true;
}

bool SkiaVertices::MakeCopy(VertexMode mode,
    int vertexCount, const Point positions[], const Point texs[], const ColorQuad colors[])
{
    std::vector<SkPoint> skPts = {};
    if (positions != nullptr) {
        skPts.resize(vertexCount);
        if (memcpy_s(skPts.data(), skPts.size() * sizeof(SkPoint),
            positions, vertexCount * sizeof(Point)) != EOK) {
            return false;
        }
    }

    std::vector<SkPoint> skTexs = {};
    if (positions != nullptr) {
        skTexs.resize(vertexCount);
        if (memcpy_s(skTexs.data(), skTexs.size() * sizeof(SkPoint),
            texs, vertexCount * sizeof(Point)) != EOK) {
            return false;
        }
    }

    std::vector<SkColor> skColors = {};
    if (colors != nullptr) {
        skColors.resize(vertexCount);
        if (memcpy_s(skColors.data(), skColors.size() * sizeof(SkColor),
            colors, vertexCount * sizeof(ColorQuad)) != EOK) {
            return false;
        }
    }

    skiaVertices_ = SkVertices::MakeCopy(static_cast<SkVertices::VertexMode>(mode), vertexCount,
        skPts.empty() ? nullptr : skPts.data(),
        skTexs.empty() ? nullptr : skTexs.data(),
        skColors.empty() ? nullptr : skColors.data());
    return true;
}

std::shared_ptr<Data> SkiaVertices::Serialize() const
{
    if (skiaVertices_ == nullptr) {
        LOGD("SkiaVertices::Serialize, SkVertices is nullptr!");
        return nullptr;
    }

    SkBinaryWriteBuffer writer;
    skiaVertices_->priv().encode(writer);
    size_t length = writer.bytesWritten();
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildUninitialized(length);
    writer.writeToMemory(data->WritableData());
    return data;
}

bool SkiaVertices::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        LOGD("SkiaVertices::Deserialize, data is invalid!");
        return false;
    }

    SkReadBuffer reader(data->GetData(), data->GetSize());
    skiaVertices_ = skiaVertices_->priv().Decode(reader);
    return skiaVertices_ != nullptr;
}

void SkiaVertices::SkiaBuilder::Init(VertexMode mode, int vertexCount, int indexCount, uint32_t flags)
{
    skiaVerticesBuilder_ = std::make_shared<SkVertices::Builder>(static_cast<SkVertices::VertexMode>(mode),
        vertexCount, indexCount, flags);
}

bool SkiaVertices::SkiaBuilder::IsValid()
{
    if (skiaVerticesBuilder_ == nullptr) {
        return false;
    }
    return skiaVerticesBuilder_->isValid();
}

Point* SkiaVertices::SkiaBuilder::Positions()
{
    if (skiaVerticesBuilder_ == nullptr) {
        return nullptr;
    }
    return reinterpret_cast<Point*>(skiaVerticesBuilder_->positions());
}

uint16_t* SkiaVertices::SkiaBuilder::Indices()
{
    if (skiaVerticesBuilder_ == nullptr) {
        return nullptr;
    }
    return skiaVerticesBuilder_->indices();
}

Point* SkiaVertices::SkiaBuilder::TexCoords()
{
    if (skiaVerticesBuilder_ == nullptr) {
        return nullptr;
    }
    return reinterpret_cast<Point*>(skiaVerticesBuilder_->texCoords());
}

ColorQuad* SkiaVertices::SkiaBuilder::Colors()
{
    if (skiaVerticesBuilder_ == nullptr) {
        return nullptr;
    }
    return reinterpret_cast<ColorQuad*>(skiaVerticesBuilder_->colors());
}

std::shared_ptr<VerticesImpl> SkiaVertices::SkiaBuilder::Detach()
{
    if (skiaVerticesBuilder_ == nullptr) {
        return nullptr;
    }

    return std::make_shared<SkiaVertices>(skiaVerticesBuilder_->detach());
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS