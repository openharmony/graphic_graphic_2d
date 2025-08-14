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

#include "fontcollection_fuzzer.h"

#include <fstream>
#include <sstream>
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace Rosen {
std::vector<uint8_t> GetFileData(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    std::string fileData = fileStream.str();
    return std::vector<uint8_t>(fileData.begin(), fileData.end());
}

void FontCollectionFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);

    std::shared_ptr fc = FontCollection::Create();
    if (fc == nullptr) {
        return;
    }
    Drawing::Typeface::RegisterCallBackFunc([](std::shared_ptr<Drawing::Typeface>) { return true; });
    std::string familyName = "familyname";
    size_t datalen = fdp.ConsumeIntegral<uint16_t>();
    std::unique_ptr fontData = std::make_unique<uint8_t[]>(datalen);
    fdp.ConsumeData(fontData.get(), datalen);
    fc->DisableFallback();
    fc->DisableSystemFont();
    fc->GetFontMgr();
    fc->LoadFont(familyName, fontData.get(), datalen);
    fc->UnloadFont(familyName);
    fc->LoadSymbolFont(familyName, fontData.get(), datalen);
    fc->LoadThemeFont(familyName, fontData.get(), datalen);
    fc->ClearThemeFont();
    fc->ClearCaches();
    FontCollection::RegisterUnloadFontStartCallback(nullptr);
    FontCollection::RegisterUnloadFontFinishCallback(nullptr);
    FontCollection::RegisterLoadFontStartCallback(nullptr);
    FontCollection::RegisterLoadFontFinishCallback(nullptr);
    auto callback = [](const FontCollection* fc, const std::string& family) {};
    FontCollection::RegisterLoadFontStartCallback(callback);
    FontCollection::RegisterUnloadFontStartCallback(callback);
    FontCollection::RegisterLoadFontFinishCallback(callback);
    FontCollection::RegisterUnloadFontFinishCallback(callback);
    const char* cjkFile = "/system/fonts/NotoSansCJK-Regular.ttc";
    std::vector<uint8_t> file = GetFileData(cjkFile);
    fc->LoadFont(familyName, file.data(), file.size());
    fc->UnloadFont(familyName);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::FontCollectionFuzzTest(data, size);
    return 0;
}
