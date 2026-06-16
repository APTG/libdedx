// Minimal modern-C++ RAII helpers around the libdedx C API.
//
// Their sole purpose is to demonstrate that the public headers — now wrapped
// in `extern "C"` linkage guards — can be consumed directly from a C++
// translation unit with no consumer-side `extern "C"` wrapping. The helpers
// are intentionally tiny and header-only; libdedx itself stays a pure C
// library and these are *not* an official C++ binding.
#ifndef DEDX_EXAMPLES_CPP_RAII_HPP
#define DEDX_EXAMPLES_CPP_RAII_HPP

#include <cstdlib>
#include <dedx.h>
#include <memory>
#include <stdexcept>
#include <string>

namespace dedx {

/// Exception carrying a libdedx error code plus the library's own message.
class error : public std::runtime_error {
  public:
    error(int code, const std::string &context) : std::runtime_error(build_message(code, context)), code_(code) {
    }

    [[nodiscard]] int code() const noexcept {
        return code_;
    }

  private:
    static std::string build_message(int code, const std::string &context) {
        char buf[256] = {};
        dedx_get_error_code(buf, code);
        return context + ": " + buf + " (code " + std::to_string(code) + ")";
    }

    int code_;
};

/// Throw dedx::error unless `err` is DEDX_OK.
inline void check(int err, const char *context) {
    if (err != DEDX_OK) {
        throw error(err, context);
    }
}

namespace detail {

struct workspace_deleter {
    void operator()(dedx_workspace *ws) const noexcept {
        int err = DEDX_OK;
        dedx_free_workspace(ws, &err);
    }
};

struct config_deleter {
    // dedx_free_config frees the struct's internal arrays *and* free()s the
    // struct itself, so make_config() below allocates with the C allocator.
    void operator()(dedx_config *cfg) const noexcept {
        int err = DEDX_OK;
        dedx_free_config(cfg, &err);
    }
};

} // namespace detail

using workspace = std::unique_ptr<dedx_workspace, detail::workspace_deleter>;
using config = std::unique_ptr<dedx_config, detail::config_deleter>;

/// Allocate a workspace; throws dedx::error on failure.
[[nodiscard]] inline workspace make_workspace(unsigned int count) {
    int err = DEDX_OK;
    dedx_workspace *ws = dedx_allocate_workspace(count, &err);
    check(err, "dedx_allocate_workspace");
    return workspace{ws};
}

/// Allocate a zero-initialised config. The deleter routes cleanup through
/// dedx_free_config (which calls free()), so the C allocator is used here to
/// keep the alloc/free pair consistent.
[[nodiscard]] inline config make_config() {
    auto *cfg = static_cast<dedx_config *>(std::calloc(1, sizeof(dedx_config)));
    if (cfg == nullptr) {
        throw error(DEDX_ERR_NO_MEMORY, "make_config");
    }
    return config{cfg};
}

} // namespace dedx

#endif // DEDX_EXAMPLES_CPP_RAII_HPP
