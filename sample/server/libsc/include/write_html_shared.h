#pragma once

#include "sc_store.h"

#ifdef SC_UTILS_DLL
#    ifdef sc_utils_EXPORTS
#        ifdef _MSC_VER
#            define SC_HTML_API __declspec(dllexport)
#        else
#            define SC_HTML_API __attribute__((visibility("default")))
#        endif
#    else
#        ifdef _MSC_VER
#            define SC_HTML_API __declspec(dllimport)
#        else
#            define SC_HTML_API
#        endif
#    endif
#else
#    define SC_HTML_API
#endif

namespace SC { namespace Store { namespace Utils {

enum class WriteHtmlStatus {
    Success,
    ErrorTemplateNotFound,
    ErrorBadTemplate,
    ErrorWriteFailure,
    ErrorReadFailure,
};

SC_HTML_API WriteHtmlStatus WriteHtml(
    const char*   utf8_scs_file_name,
    const char*   utf8_html_template_path,
    const char*   utf8_output_path,
    MemoryDevice* dev = nullptr);

}}} // namespace SC::Store::Utils
