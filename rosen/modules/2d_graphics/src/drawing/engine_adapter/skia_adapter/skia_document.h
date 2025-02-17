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

#ifndef SKIA_DOCUMENT_H
#define SKIA_DOCUMENT_H

#include "impl_interface/document_impl.h"

#include "include/core/SkDocument.h"

#include "utils/document.h"
#include "utils/file_w_stream.h"
#include "utils/sharing_serial_context.h"
#include "utils/serial_procs.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class SkiaDocument : public DocumentImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaDocument(sk_sp<SkDocument> skDocument) noexcept;
    ~SkiaDocument() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    static std::shared_ptr<Document> MakeMultiPictureDocument(FileWStream* fileStream,
        SerialProcs* procs, std::unique_ptr<SharingSerialContext>& serialContext);

    const sk_sp<SkDocument> GetSkiaDocument() const;
    void SetSkDocument(const sk_sp<SkDocument>& skDocument);

    std::shared_ptr<Canvas> BeginPage(float width, float height) override;
    void EndPage() override;
    void Close() override;

private:
    sk_sp<SkDocument> skDocument_;
};
} // Drawing
} // Rosen
} // OHOS
#endif