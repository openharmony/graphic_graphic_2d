/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

#include "accesstoken_kit.h"
#ifdef SUPPORT_ACCESS_TOKEN
#include "nativetoken_kit.h"
#include "token_setproc.h"
#endif
#include "iconsumer_surface.h"
#include "pixel_map.h"
#include "png.h"
#include "surface.h"

#include "draw/canvas.h"
#include "draw/color.h"
#include "draw/path.h"
#include "render_context/render_context.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"
#include "utils/point.h"
#include "utils/rect.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

namespace {
constexpr int BITMAP_DEPTH = 8;
constexpr uint32_t VIRTUAL_SCREEN_WIDTH = 800;
constexpr uint32_t VIRTUAL_SCREEN_HEIGHT = 600;
constexpr uint32_t CAPTURE_TIMEOUT_MS = 5000;
constexpr uint32_t POLL_INTERVAL_US = 10000; // 10ms
constexpr uint32_t POLL_INTERVAL_MS = POLL_INTERVAL_US / 1000;
constexpr uint32_t RENDER_WAIT_SECONDS = 2;
constexpr char DEFAULT_OUTPUT[] = "/data/local/tmp/display_capture.png";
constexpr int DECIMAL_BASE = 10;
constexpr int EXIT_OK = 0;
constexpr int EXIT_INVALID_ARGS = 1;
constexpr int EXIT_SETUP_FAILED = 2;
constexpr int EXIT_CAPTURE_FAILED = 3;

#ifdef RS_ENABLE_GPU
shared_ptr<RenderContext> g_renderContext = nullptr;
#endif

struct DemoOptions {
    Media::PixelFormat format = Media::PixelFormat::RGBA_8888;
    string output = DEFAULT_OUTPUT;
    bool useDma = false;
};

struct FormatEntry {
    const char* name;
    Media::PixelFormat format;
};

// Media::PixelFormat values 0-9 have stable names; other values fall back to FORMAT(n).
const FormatEntry FORMAT_TABLE[] = {
    { "UNKNOWN", Media::PixelFormat::UNKNOWN },
    { "ARGB_8888", Media::PixelFormat::ARGB_8888 },
    { "RGB_565", Media::PixelFormat::RGB_565 },
    { "RGBA_8888", Media::PixelFormat::RGBA_8888 },
    { "BGRA_8888", Media::PixelFormat::BGRA_8888 },
    { "RGB_888", Media::PixelFormat::RGB_888 },
    { "ALPHA_8", Media::PixelFormat::ALPHA_8 },
    { "RGBA_F16", Media::PixelFormat::RGBA_F16 },
    { "NV21", Media::PixelFormat::NV21 },
    { "NV12", Media::PixelFormat::NV12 },
};

string FormatName(Media::PixelFormat format)
{
    for (auto& entry : FORMAT_TABLE) {
        if (entry.format == format) {
            return entry.name;
        }
    }
    return "FORMAT(" + to_string(static_cast<uint32_t>(format)) + ")";
}

string AllocatorName(Media::AllocatorType type)
{
    switch (type) {
        case Media::AllocatorType::DEFAULT:
            return "DEFAULT";
        case Media::AllocatorType::HEAP_ALLOC:
            return "HEAP_ALLOC";
        case Media::AllocatorType::SHARE_MEM_ALLOC:
            return "SHARE_MEM_ALLOC";
        case Media::AllocatorType::CUSTOM_ALLOC:
            return "CUSTOM_ALLOC";
        case Media::AllocatorType::DMA_ALLOC:
            return "DMA_ALLOC";
        default:
            return "ALLOCATOR(" + to_string(static_cast<int32_t>(type)) + ")";
    }
}

string CaptureErrorName(CaptureError error)
{
    switch (error) {
        case CaptureError::CAPTURE_OK:
            return "CAPTURE_OK";
        case CaptureError::CAPTURE_NO_PERMISSION:
            return "CAPTURE_NO_PERMISSION";
        case CaptureError::CAPTURE_NO_NODE:
            return "CAPTURE_NO_NODE";
        case CaptureError::CAPTURE_CONFIG_WRONG:
            return "CAPTURE_CONFIG_WRONG";
        case CaptureError::CAPTURE_PIXELMAP_NULL:
            return "CAPTURE_PIXELMAP_NULL";
        case CaptureError::CAPTURE_PIXELMAP_COPY_ERROR:
            return "CAPTURE_PIXELMAP_COPY_ERROR";
        case CaptureError::CAPTURE_NULL_FAIL:
            return "CAPTURE_NULL_FAIL";
        case CaptureError::HDR_SET_FAIL:
            return "HDR_SET_FAIL";
        case CaptureError::CAPTURE_RENDER_FAIL:
            return "CAPTURE_RENDER_FAIL";
        case CaptureError::AUTO_NOT_SUPPORT:
            return "AUTO_NOT_SUPPORT";
        case CaptureError::COLOR_SPACE_NOT_SUPPORT:
            return "COLOR_SPACE_NOT_SUPPORT";
        case CaptureError::DYNAMIC_RANGE_NOT_SUPPORT:
            return "DYNAMIC_RANGE_NOT_SUPPORT";
        case CaptureError::CAPTURE_NO_SECURE_PERMISSION:
            return "CAPTURE_NO_SECURE_PERMISSION";
        case CaptureError::CAPTURE_FAIL_SPECIAL_LAYER:
            return "CAPTURE_FAIL_SPECIAL_LAYER";
        case CaptureError::CAPTURE_UNSUPPORTED_FORMAT:
            return "CAPTURE_UNSUPPORTED_FORMAT";
        default:
            return "CAPTURE_ERROR(" + to_string(static_cast<uint32_t>(error)) + ")";
    }
}

bool ParseFormat(const string& text, Media::PixelFormat& format)
{
    for (auto& entry : FORMAT_TABLE) {
        if (text == entry.name) {
            format = entry.format;
            return true;
        }
    }
    char* end = nullptr;
    unsigned long value = strtoul(text.c_str(), &end, DECIMAL_BASE);
    if (end == nullptr || *end != '\0' || value > static_cast<unsigned long>(Media::PixelFormat::NV12)) {
        return false;
    }
    format = static_cast<Media::PixelFormat>(value);
    return true;
}

void PrintUsage(const char* name)
{
    cout << "Usage: " << name << " [options]" << endl;
    cout << "Options:" << endl;
    cout << "  --format <name|value>  capture pixel format (default RGBA_8888):" << endl;
    cout << "                         UNKNOWN ARGB_8888 RGB_565 RGBA_8888 BGRA_8888 RGB_888"
            " ALPHA_8 RGBA_F16 NV21 NV12"
         << endl;
    cout << "  --output <file>        output base file; any extension in it is dropped and both" << endl;
    cout << "                         files are named with the actual format, e.g."
            " xxx_BGRA_8888.png plus"
         << endl;
    cout << "                         raw copy xxx_BGRA_8888.raw (default base " << DEFAULT_OUTPUT << ")" << endl;
    cout << "  --dma                  capture with dma buffer (default off)" << endl;
    cout << "  --help                 print this help" << endl;
    cout << "Note: RS outputs RGBA_8888/BGRA_8888 directly; other requested formats fall back" << endl;
    cout << "      to RGBA_8888 and the callback reports CAPTURE_UNSUPPORTED_FORMAT." << endl;
    cout << "Note: --dma capture always outputs RGBA_8888; the requested --format is ignored" << endl;
    cout << "      and no CAPTURE_UNSUPPORTED_FORMAT is reported." << endl;
}

bool ParseArgs(int argc, char* argv[], DemoOptions& options, bool& helpRequested)
{
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
            return false;
        }
        if (arg == "--dma") {
            options.useDma = true;
            continue;
        }
        if (i + 1 >= argc) {
            cout << "missing value for " << arg << endl;
            return false;
        }
        string value = argv[++i];
        if (arg == "--format") {
            if (!ParseFormat(value, options.format)) {
                cout << "invalid format: " << value << endl;
                return false;
            }
        } else if (arg == "--output") {
            if (value.empty()) {
                cout << "invalid output: " << value << endl;
                return false;
            }
            options.output = value;
        } else {
            cout << "unknown option: " << arg << endl;
            return false;
        }
    }
    return true;
}

bool WriteToPng(
    const string& fileName, uint32_t width, uint32_t height, uint32_t stride, const uint8_t* data, bool bgrInput)
{
    if (data == nullptr) {
        cout << "pixel data is null" << endl;
        return false;
    }
    png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (pngStruct == nullptr) {
        cout << "png_create_write_struct error, nullptr!" << endl;
        return false;
    }
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == nullptr) {
        cout << "png_create_info_struct error, nullptr!" << endl;
        png_destroy_write_struct(&pngStruct, nullptr);
        return false;
    }
    FILE* fp = fopen(fileName.c_str(), "wb");
    if (fp == nullptr) {
        cout << "open file error: " << fileName << endl;
        png_destroy_write_struct(&pngStruct, &pngInfo);
        return false;
    }
    png_init_io(pngStruct, fp);
    png_set_IHDR(pngStruct, pngInfo, width, height, BITMAP_DEPTH, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    if (bgrInput) {
        // PixelMap data is BGRA-ordered; libpng converts to RGBA before writing rows.
        png_set_bgr(pngStruct);
    }
    png_set_packing(pngStruct);
    png_write_info(pngStruct, pngInfo);
    for (uint32_t i = 0; i < height; i++) {
        png_write_row(pngStruct, data + (i * stride));
    }
    png_write_end(pngStruct, pngInfo);
    png_destroy_write_struct(&pngStruct, &pngInfo);
    if (fclose(fp) != 0) {
        cout << "close file failed: " << fileName << endl;
        return false;
    }
    return true;
}

bool WriteRawFile(const string& fileName, const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        cout << "pixel data is null" << endl;
        return false;
    }
    FILE* fp = fopen(fileName.c_str(), "wb");
    if (fp == nullptr) {
        cout << "open file error: " << fileName << endl;
        return false;
    }
    bool ok = (fwrite(data, 1, size, fp) == size);
    if (fclose(fp) != 0) {
        ok = false;
    }
    return ok;
}

struct SaveResult {
    string pngFile;
    string rawFile;
    bool ok = false;
};

string InsertFormatSuffix(const string& path, const string& formatName, const string& extension)
{
    // Any extension in the base name is dropped so each artifact always gets its own
    // extension (.raw/.png); otherwise both files share one name and the png overwrites the raw.
    string base = path;
    auto slash = path.find_last_of('/');
    auto dot = path.find_last_of('.');
    if ((dot != string::npos) && (slash == string::npos || dot > slash)) {
        base = path.substr(0, dot);
    }
    return base + "_" + formatName + extension;
}

SaveResult SavePixelMap(Media::PixelMap& pixelMap, const string& path)
{
    SaveResult result;
    auto format = pixelMap.GetPixelFormat();
    uint32_t width = static_cast<uint32_t>(pixelMap.GetWidth());
    uint32_t height = static_cast<uint32_t>(pixelMap.GetHeight());
    uint32_t rowBytes = static_cast<uint32_t>(pixelMap.GetRowBytes());
    string formatName = FormatName(format);
    result.pngFile = InsertFormatSuffix(path, formatName, ".png");
    result.rawFile = InsertFormatSuffix(path, formatName, ".raw");
    // The raw file keeps the original memory layout of the pixelmap so testers can verify the
    // byte order directly (e.g. xxd); the PNG is always a converted standard image.
    bool rawOk = WriteRawFile(result.rawFile, pixelMap.GetPixels(),
        static_cast<size_t>(pixelMap.GetRowBytes()) * static_cast<size_t>(pixelMap.GetHeight()));
    bool pngOk = false;
    bool pngEncodable = (format == Media::PixelFormat::BGRA_8888 || format == Media::PixelFormat::RGBA_8888);
    if (pngEncodable) {
        pngOk = WriteToPng(
            result.pngFile, width, height, rowBytes, pixelMap.GetPixels(), format == Media::PixelFormat::BGRA_8888);
    } else {
        cout << "format " << formatName << " has no PNG encoder, raw data only" << endl;
    }
    result.ok = rawOk && (!pngEncodable || pngOk);
    return result;
}

void InitNativeTokenInfo()
{
#ifdef SUPPORT_ACCESS_TOKEN
    // TAKE_SURFACE_CAPTURE/CREATE_VIRTUAL_SCREEN require ohos.permission.CAPTURE_SCREEN and
    // CREATE_DISPLAY_NODE requires a native(system) caller, so a native token must be registered
    // before touching RS interfaces.
    uint64_t tokenId;
    const char* perms[1];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "rs_display_capture_format_demo",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
#else
    cout << "SUPPORT_ACCESS_TOKEN off, capture may be denied by RS" << endl;
#endif
}

void InitRenderContext()
{
#ifdef RS_ENABLE_GPU
    g_renderContext = RenderContext::Create();
    if (g_renderContext != nullptr) {
        g_renderContext->Init();
    }
#endif
}

bool DrawSurfaceContent(const shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t width, uint32_t height)
{
    auto rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        cout << "extract rssurface failed" << endl;
        return false;
    }
#ifdef RS_ENABLE_GPU
    if (g_renderContext != nullptr) {
        rsSurface->SetRenderContext(g_renderContext);
    }
#endif
    auto framePtr = rsSurface->RequestFrame(width, height);
    if (framePtr == nullptr) {
        cout << "request frame failed" << endl;
        return false;
    }
    auto canvas = framePtr->GetCanvas();
    if (canvas == nullptr) {
        cout << "get canvas failed" << endl;
        return false;
    }
    canvas->Clear(Drawing::Color::COLOR_WHITE);
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(0xFFE23333);
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(0, 0, width / 4, height));
    brush.SetColor(0xFF3366E2);
    canvas->DrawRect(Drawing::Rect(width / 4, 0, width / 2, height / 2));
    canvas->DetachBrush();
    Drawing::Path path;
    path.MoveTo(width / 2, height / 2);
    path.CubicTo(width * 3 / 4, height / 2, width / 2, height * 3 / 4, width * 3 / 4, height * 3 / 4);
    brush.SetColor(0xFFF2C744);
    canvas->AttachBrush(brush);
    canvas->DrawPath(path);
    canvas->DetachBrush();
    brush.SetColor(0x8000AA66);
    canvas->AttachBrush(brush);
    canvas->DrawCircle(Drawing::Point(width * 3 / 4, height / 4), height / 5);
    canvas->DetachBrush();
    framePtr->SetDamageRegion(0, 0, width, height);
    rsSurface->FlushFrame(framePtr);
    return true;
}

shared_ptr<RSSurfaceNode> CreateContentSurface(const shared_ptr<RSUIContext>& uiContext, const string& name, float left,
    float top, uint32_t width, uint32_t height)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = name;
    config.isSync = true;
    auto surfaceNode = RSSurfaceNode::Create(config, true, uiContext);
    if (surfaceNode == nullptr) {
        cout << "create surface node failed: " << name << endl;
        return nullptr;
    }
    surfaceNode->SetBounds(left, top, width, height);
    surfaceNode->SetFrame(left, top, width, height);
    if (!DrawSurfaceContent(surfaceNode, width, height)) {
        return nullptr;
    }
    return surfaceNode;
}

struct CaptureSource {
    ScreenId virtualScreenId = INVALID_SCREEN_ID;
    shared_ptr<RSUIDirector> uiDirector;
    shared_ptr<RSUIContext> uiContext;
    shared_ptr<RSRenderInterface> renderInterface;
    shared_ptr<RSDisplayNode> displayNode;
};

// Creates a virtual screen plus a display node with drawn content. The producer surface
// must outlive the virtual screen, so csurface/psurface are returned to the caller.
bool SetupCaptureSource(sptr<IConsumerSurface>& csurface, sptr<Surface>& psurface, CaptureSource& source)
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    ScreenId defaultScreenId = rsInterfaces.GetDefaultScreenId();
    if (defaultScreenId == INVALID_SCREEN_ID) {
        cout << "get default screen id failed" << endl;
        return false;
    }
    sptr<IRemoteObject> connectToRender = rsInterfaces.GetConnectToRenderToken(defaultScreenId);
    source.uiDirector = RSUIDirector::Create(connectToRender);
    if (source.uiDirector == nullptr) {
        cout << "create RSUIDirector failed" << endl;
        return false;
    }
    source.uiContext = source.uiDirector->GetRSUIContext();
    source.renderInterface = source.uiContext->GetRSRenderInterface();
    if (source.renderInterface == nullptr) {
        cout << "get RSRenderInterface failed" << endl;
        return false;
    }
    csurface = IConsumerSurface::Create();
    if (csurface == nullptr) {
        cout << "create consumer surface failed" << endl;
        return false;
    }
    sptr<IBufferProducer> producer = csurface->GetProducer();
    psurface = Surface::CreateSurfaceAsProducer(producer);
    source.virtualScreenId = rsInterfaces.CreateVirtualScreen(
        "DisplayCaptureFormatDemo", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, psurface, INVALID_SCREEN_ID, -1, {});
    if (source.virtualScreenId == INVALID_SCREEN_ID) {
        cout << "create virtual screen failed" << endl;
        return false;
    }
    RSDisplayNodeConfig displayConfig = { source.virtualScreenId, DisplayMode::EXPAND, 0, true };
    source.displayNode = RSDisplayNode::Create(displayConfig, source.uiContext);
    if (source.displayNode == nullptr) {
        cout << "create display node failed" << endl;
        rsInterfaces.RemoveVirtualScreen(source.virtualScreenId);
        return false;
    }
    source.displayNode->SetBounds(0, 0, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
    source.displayNode->SetFrame(0, 0, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
    auto backgroundNode = CreateContentSurface(
        source.uiContext, "DemoBackgroundSurface", 0, 0, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
    auto overlayNode = CreateContentSurface(source.uiContext, "DemoOverlaySurface", VIRTUAL_SCREEN_WIDTH / 4,
        VIRTUAL_SCREEN_HEIGHT / 4, VIRTUAL_SCREEN_WIDTH / 2, VIRTUAL_SCREEN_HEIGHT / 2);
    if (backgroundNode == nullptr || overlayNode == nullptr) {
        rsInterfaces.RemoveVirtualScreen(source.virtualScreenId);
        return false;
    }
    source.displayNode->AddChild(backgroundNode, -1);
    source.displayNode->AddChild(overlayNode, -1);
    // Nodes are created with uiContext, so their commands go to the UIContext's transaction.
    source.uiDirector->SendMessages();
    sleep(RENDER_WAIT_SECONDS);
    return true;
}
} // namespace

class DemoSurfaceCaptureCallback : public SurfaceCaptureCallback {
public:
    explicit DemoSurfaceCaptureCallback(const string& outputFile) : outputFile_(outputFile) {}

    ~DemoSurfaceCaptureCallback() override = default;

    void OnSurfaceCapture(shared_ptr<Media::PixelMap> pixelMap) override
    {
        HandleCaptureResult(pixelMap, nullptr, CaptureError::CAPTURE_OK);
    }

    void OnSurfaceCaptureHDR(shared_ptr<Media::PixelMap> pixelMap, shared_ptr<Media::PixelMap> pixelMapHDR) override
    {
        HandleCaptureResult(pixelMap, pixelMapHDR, CaptureError::CAPTURE_OK);
    }

    void OnSurfaceCaptureWithErrorCode(shared_ptr<Media::PixelMap> pixelMap, shared_ptr<Media::PixelMap> pixelMapHDR,
        CaptureError captureErrorCode) override
    {
        HandleCaptureResult(pixelMap, pixelMapHDR, captureErrorCode);
    }

    bool IsDone() const
    {
        return done_.load();
    }

    bool IsSuccess() const
    {
        return success_.load();
    }

private:
    void HandleCaptureResult(const shared_ptr<Media::PixelMap>& pixelMap,
        const shared_ptr<Media::PixelMap>& pixelMapHDR, CaptureError errorCode)
    {
        // Runs on a binder thread of this process; the main thread polls IsDone().
        if (pixelMap == nullptr) {
            cout << "capture failed: nullptr pixelmap, error=" << CaptureErrorName(errorCode) << endl;
            done_ = true;
            return;
        }
        cout << "capture done: error=" << CaptureErrorName(errorCode)
             << ", actual pixel format=" << FormatName(pixelMap->GetPixelFormat())
             << ", allocator=" << AllocatorName(pixelMap->GetAllocatorType()) << ", size=" << pixelMap->GetWidth()
             << "x" << pixelMap->GetHeight() << ", rowBytes=" << pixelMap->GetRowBytes() << endl;
        if (pixelMapHDR != nullptr) {
            cout << "hdr pixelmap format=" << FormatName(pixelMapHDR->GetPixelFormat())
                 << ", size=" << pixelMapHDR->GetWidth() << "x" << pixelMapHDR->GetHeight() << endl;
        }
        SaveResult saved = SavePixelMap(*pixelMap, outputFile_);
        success_ = saved.ok;
        if (success_) {
            cout << "save success: png=" << saved.pngFile << ", raw=" << saved.rawFile << endl;
            if (pixelMap->GetPixelFormat() == Media::PixelFormat::RGBA_8888 ||
                pixelMap->GetPixelFormat() == Media::PixelFormat::BGRA_8888) {
                // The red block drawn at the top-left is 0xFFE23333 (R=0xE2, G=0x33, B=0x33, A=0xFF),
                // so the first 4 bytes of the raw file expose the memory byte order.
                cout << "raw byte order check: first 4 bytes are E2 33 33 FF for RGBA_8888,"
                     << " 33 33 E2 FF for BGRA_8888" << endl;
            }
        } else {
            cout << "save failed: png=" << saved.pngFile << ", raw=" << saved.rawFile << endl;
        }
        done_ = true;
    }

    string outputFile_;
    atomic<bool> done_ { false };
    atomic<bool> success_ { false };
};

int main(int argc, char* argv[])
{
    DemoOptions options;
    bool helpRequested = false;
    if (!ParseArgs(argc, argv, options, helpRequested)) {
        PrintUsage(argv[0]);
        return helpRequested ? EXIT_OK : EXIT_INVALID_ARGS;
    }
    cout << "display capture format demo start: format=" << FormatName(options.format)
         << ", dma=" << (options.useDma ? "on" : "off") << ", output=" << options.output << ", screen="
         << VIRTUAL_SCREEN_WIDTH << "x" << VIRTUAL_SCREEN_HEIGHT << endl;
    if (options.useDma && options.format != Media::PixelFormat::RGBA_8888) {
        cout << "hint: dma capture always outputs RGBA_8888, requested format " << FormatName(options.format)
             << " will be ignored" << endl;
    }

    InitNativeTokenInfo();
    InitRenderContext();

    CaptureSource source;
    sptr<IConsumerSurface> csurface;
    sptr<Surface> psurface;
    if (!SetupCaptureSource(csurface, psurface, source)) {
        return EXIT_SETUP_FAILED;
    }

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.pixelFormat = options.format;
    captureConfig.useDma = options.useDma;
    captureConfig.needErrorCode = true;
    auto callback = make_shared<DemoSurfaceCaptureCallback>(options.output);
    if (!source.renderInterface->TakeSurfaceCapture(source.displayNode, callback, captureConfig)) {
        cout << "TakeSurfaceCapture request failed" << endl;
        RSInterfaces::GetInstance().RemoveVirtualScreen(source.virtualScreenId);
        return EXIT_CAPTURE_FAILED;
    }
    uint32_t waitedMs = 0;
    while (!callback->IsDone() && waitedMs < CAPTURE_TIMEOUT_MS) {
        usleep(POLL_INTERVAL_US);
        waitedMs += POLL_INTERVAL_MS;
    }
    RSInterfaces::GetInstance().RemoveVirtualScreen(source.virtualScreenId);
    if (!callback->IsDone()) {
        cout << "timeout waiting capture callback, capture may be rejected (check CAPTURE_SCREEN permission)" << endl;
#ifdef RS_ENABLE_GPU
        g_renderContext.reset();
#endif
        return EXIT_CAPTURE_FAILED;
    }
    cout << "display capture format demo end, result=" << (callback->IsSuccess() ? "success" : "failed") << endl;
    // Explicitly destroy the RenderContext while the process is still in a healthy state
#ifdef RS_ENABLE_GPU
    g_renderContext.reset();
#endif
    return callback->IsSuccess() ? EXIT_OK : EXIT_CAPTURE_FAILED;
}
