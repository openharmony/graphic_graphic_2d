#ifndef RENDER_SERVICE_PARSE_RENDER_INT_H
#define RENDER_SERVICE_PARSE_RENDER_INT_H

#include <charconv>
#include <cstdint>
#include <string>

namespace OHOS {
namespace Rosen {
inline bool ParseRenderInt32(const std::string &text, int32_t &out)
{
    if (text.empty()) {
        return false;
    }
    const char *first = text.data();
    const char *last = first + text.size();
    auto [ptr, ec] = std::from_chars(first, last, out);
    return ec == std::errc{} && ptr == last;
}
} // namespace Rosen
} // namespace OHOS
#endif
