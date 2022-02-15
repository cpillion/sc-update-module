#pragma once

#include "sc_store.h"

#include <memory>
#include <vector>

#ifdef SC_UTILS_TEXT_DLL
#    ifdef sc_utils_text_EXPORTS
#        ifdef _MSC_VER
#            define SC_API __declspec(dllexport)
#        else
#            define SC_API __attribute__((visibility("default")))
#        endif
#    else
#        ifdef _MSC_VER
#            define SC_API __declspec(dllimport)
#        else
#            define SC_API
#        endif
#    endif
#else
#    define SC_API
#endif

namespace SC { namespace Store { namespace Utils { namespace Text {

//! Bits used to denote font styling.
struct FontStyle {
    enum Bits : uint8_t {
        None   = 0,
        Bold   = 1 << 0,
        Italic = 1 << 1,
    };
};

//! Configures generic quality.
enum Quality {
    Low,
    Medium,
    High,
};

enum class TextAlignment : uint32_t {
    TopLeft,      //!< Insertion point is the top left corner of the text string.
    CenterLeft,   //!< Insertion point is the center of the left side of the text string.
    BottomLeft,   //!< Insertion point is the bottom left corner of the text string.
    TopCenter,    //!< Insertion point is the center of the top side of the text string.
    Center,       //!< Insertion point is the vertical and horizontal center of the text string.
    BottomCenter, //!< Insertion point is the center of the bottom side of the text string.
    TopRight,     //!< Insertion point is the top right corner of the text string.
    CenterRight,  //!< Insertion point is the center of the right side of the text string.
    BottomRight   //!< Insertion point is the bottom right corner of the text string.
};

//! Denotes whether or not text insertion inserts faceted text and/or outlined text.
struct TextRepresentation {
    enum Bits : uint8_t {
        Faces = 1 << 0,
        Lines = 1 << 1,
        Full  = Faces | Lines,
    };
};

//! This function should be called after use of this library is finished.
//! After this function is called, functions in this library should not be called.
SC_API void Shutdown();

//! Sets the font directory to search for fonts in.
//! @param font_directory The file path of the search directory to set.
SC_API void SetFontDirectory(char const* font_directory);

//! Supplies a list of fonts to be used as fallbacks for when a character can not be found in an
//! active font.
//! @param fallback_font_names A list of UTF8 encoded strings that correspond to the fallback fonts
//! being used.
SC_API void SetFallbackFonts(std::vector<char const*> const& fallback_font_names);

//! Closes any open font resources used by this library.
//! Useful for deleting any temporary font files.
//! This is implicitly called by Shutdown().
SC_API void ReleaseFontResources();

//! Inserts text into the stream cache as mesh geometry.
//! @return The mesh key of the generated text geometry.
//! @param model The stream cache model to insert the text into.
//! @param alignment Controls the alignment of the inserted text relative to its inserted position.
//! @param transform Transforms the generated mesh points by this matrix.
//! @param utf8_text The text to insert. Text is expected to be in UTF-8 encoding.
//! @param font_name The name of the font to generate the text with.
//! @param font_style The style of the font to generate the text with.
//! @param font_quality The quality of the font to generate the text with.
//! @param representation_bits Controls whether or not the text is inserted as faces and/or lines.
SC_API MeshKey InsertText(
    Model&                   model,
    TextAlignment            alignment,
    Matrix3d const&          transform,
    char const*              utf8_text,
    char const*              font_name,
    FontStyle::Bits          font_style,
    Quality                  font_quality,
    TextRepresentation::Bits representation_bits);

}}}} // namespace SC::Store::Utils::Text
