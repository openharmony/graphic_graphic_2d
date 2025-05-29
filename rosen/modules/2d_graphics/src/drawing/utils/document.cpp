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

#include "utils/document.h"

#include "impl_factory.h"
#include "static_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

Document::Document(std::shared_ptr<DocumentImpl> documentImpl) noexcept : documentImplPtr_(documentImpl) {}

std::shared_ptr<Canvas> Document::BeginPage(float width, float height)
{
    if (documentImplPtr_ == nullptr) {
        return nullptr;
    }
    return documentImplPtr_->BeginPage(width, height);
}

void Document::EndPage()
{
    if (documentImplPtr_ == nullptr) {
        return;
    }
    documentImplPtr_->EndPage();
}

std::shared_ptr<Document> Document::MakeMultiPictureDocument(FileWStream* fileStream,
    SerialProcs* procs, std::unique_ptr<SharingSerialContext>& serialContext)
{
    return StaticFactory::MakeMultiPictureDocument(fileStream, procs, serialContext);
}

void Document::Close()
{
    if (documentImplPtr_ == nullptr) {
        return;
    }
    documentImplPtr_->Close();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
