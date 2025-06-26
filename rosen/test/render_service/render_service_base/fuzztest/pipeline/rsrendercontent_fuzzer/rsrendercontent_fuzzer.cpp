#include "rsrendercontent_fuzzer.h"

#include <stddef.h>
#include <stdint.h>
#include "pipeline/rs_render_content.h"

namespace OHOS::Rosen {

namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool GetRenderProperties(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    
    RSRenderContent content;
    content.GetRenderProperties();
    content.GetMutableRenderProperties();

    return true;
}

bool DrawPropertyDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSRenderContent content;
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas canvas(drawingCanvas.get());
    uint8_t slotNum = GetData<uint8_t>();

    content->DrawPropertyDrawable(RSPropertyDrawableSlot (slotNum), canvas);

    return true;
}
} // namespace OHOS::Rosen

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::GetRenderProperties(data, size);
    OHOS::Rosen::DrawPropertyDrawable(data, size);
    return 0;
}