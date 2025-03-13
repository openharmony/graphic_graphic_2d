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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "draw/canvas.h"
#include "impl_interface/document_impl.h"
#include "utils/file_w_stream.h"
#include "utils/sharing_serial_context.h"
#include "utils/serial_procs.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Document {
public:
    Document() noexcept;
    explicit Document(std::shared_ptr<DocumentImpl> documentImpl) noexcept;
    virtual ~Document() {}
    template <typename T>
    T* GetImpl() const
    {
        return documentImplPtr_->DowncastingTo<T>();
    }

    /*
     * @brief  Writes into a file format that is similar to Picture::Serialize
     */
    static std::shared_ptr<Document> MakeMultiPictureDocument(FileWStream* fileStream,
        SerialProcs* procs, std::unique_ptr<SharingSerialContext>& serialContext);

    /*
     * @brief  begin a new page for the document, returning the canvas that will draw
     *         into the page.
     */
    std::shared_ptr<Canvas> BeginPage(float width, float height);
    void EndPage();
    void Close();

private:
    std::shared_ptr<DocumentImpl> documentImplPtr_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif