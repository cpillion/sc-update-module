#pragma once

#include "sc_store.h"

#include <memory>
#include <vector>

#ifdef SC_UTILS_DLL
#    ifdef sc_utils_EXPORTS
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

namespace SC { namespace Store { namespace Utils {

//! Denotes a line's end cap.
enum class LineCap {
    None,
    Round,
};

//! Denotes line thickness and end caps.
struct LineStroke {
    struct {
        LineCap begin;
        LineCap end;
    } cap;

    float thickness;
};

//! Configures tessellation quality.
struct TessellationConfig {
    float steps_per_circle = 30.0;
};

//! Generates the points for the path along an elliptical arc.
//! Ignores z-coordinates. Generated z-coordinates are set to zero.
//! The arguments and behavior are that of SVG elliptical arcs:
//! https://www.w3.org/TR/SVG/paths.html#PathDataEllipticalArcCommands
//! @return Returns true if the operation succeeded. Otherwise it returns false.
//! @param config Configures tessellation.
//! @param x_radius The radius of the ellipse along the x-axis
//! @param y_radius The radius of the ellipse along the y-axis
//! @param x_axis_rotation_radians Rotates the generated ellipse along the x-axis by this many
//! radians.
//! @param large_arc_flag Controls whether or not the generated arc is generated as a large arc.
//! @param sweep_flag If true, the generated arc is swept in the positive angle direction. Otherwise
//! it is swept in the negative direction.
//! @param invert_y Denotes whether or not the Y-axis is inverted.
//! @param begin_x_pos The x-coordinate of the arc's begin position
//! @param begin_y_pos The y-coordinate of the arc's begin position
//! @param end_x_pos The x-coordinate of the arc's end position
//! @param end_y_pos The y-coordinate of the arc's end position
//! @param out_points Where the generate arc's points get returned.
SC_API bool CreateEllipticalArc(
    TessellationConfig const& config,
    float                     x_radius,
    float                     y_radius,
    float                     x_axis_rotation_radians,
    bool                      large_arc_flag,
    bool                      sweep_flag,
    bool                      invert_y,
    float                     begin_x_pos,
    float                     begin_y_pos,
    float                     end_x_pos,
    float                     end_y_pos,
    std::vector<Point>&       out_points);

//! Generates the points and triangles for a thick line.
//! Ignores z-coordinates. Generated z-coordinates are set to zero.
//! @return Returns true if the operation succeeded. Otherwise it returns false.
//! @param config Configures tessellation.
//! @param stroke Configures stroke properties of the generated thick line.
//! @param path The path used to define the spine of the thick line.
//! @param out_points Generated points are returned here.
//! @param out_triangle_indices Generated triangle indices are returned here.
SC_API bool CreateThickLine(
    TessellationConfig const& config,
    LineStroke const&         stroke,
    std::vector<Point> const& path,
    std::vector<Point>&       out_points,
    std::vector<uint32_t>&    out_triangle_indices);

//! Triangulates a supplied polygonal face.
//! @return Returns true if successful. Returns false otherwise.
//! @param points The points used by the supplied face list for specifying face vertices.
//! @param face_list A list of indices that refer to the supplied points. These indices denote the
//! face to triangulate. An unclosed face list loop will automatically be closed by the
//! triangulator.
//! @param face_list_count The number of indices used in the supplied face list.
//! @param normal The normal of the face to triangulate.
//! @param out_points The vertices of the generated triangles get emitted here.
SC_API bool TriangulateFace(
    SC::Store::Point const*        points,
    int const*                     face_list,
    size_t                         face_list_count,
    SC::Store::Point const&        normal,
    std::vector<SC::Store::Point>& out_points);

//! Clip region will trim hatching pattern.
struct ClipRegion {
    std::vector<Point> loopPoints;
};

//! Defines how to handle hatching (gap between lines, orientation).
struct HatchingStyle {
    double startX;  //! X start position: 0 will start hatching at the left of the clip region.
    double startY;  //! Y start position: 0 will start hatching at the bottom of the clip region.
    double offsetX; //! X gap between hatches (after applying rotation).
    double offsetY; //! Y gap between hatches (after applying rotation).
    double angle;   //! Rotation angle (in radians) to apply on hatchings.
};

/*! <p>Creates a hatching zone.</p>
@param model (in) SC model file to generate hatching into.
@param localSpaceClipRegions (in) hatching limits in local space.
@param hatchingStyles (in) defines hatching style (orientation, gap between lines).
@param worldMatrix (in) world space matrix (will be applied on the localSpaceClipRegions).
@param material (in) line material to use.
@param visibility (in) tells if hatching will be visible or not.
@param instanceKey (out) stream cache instance key of the resulting hatching.
@return true if all went right, false otherwise.*/
SC_API bool CreateHatchingPattern(
    Model&                            model,
    std::vector<ClipRegion> const&    localSpaceClipRegions,
    std::vector<HatchingStyle> const& hatchingStyles,
    Matrix3d const&                   worldMatrix,
    MaterialKey                       material,
    bool                              visibility,
    InstanceKey&                      instanceKey);

/*! <p>Creates a textured quad.</p>
@param model (in) SC model file to generate quad into.
@param textureKey (in) SC key of the texture
@param xPos (in) left local position of the quad
@param yPos (in) bottom local position of the quad
@param width (in) width of the quad
@param height (in) height of the quad
@param worldMatrix (in) world space matrix
@param visibility (in) tells if the quad will be visible or not.
@param instanceKey (out) stream cache instance key of the resulting quad.
@return true if all went right, false otherwise.*/
SC_API bool CreateTexturedQuad(
    Model&          model,
    ImageKey        textureKey,
    float           xPos,
    float           yPos,
    float           width,
    float           height,
    Matrix3d const& worldMatrix,
    bool            visibility,
    InstanceKey&    instanceKey);

/*! <p>Creates point meshes for a set of points.</p>
@param model (in) SC model file to generate point meshes into.
@param points (in) Points to process
@param rgba32s (in) Colors to process maybe nullptr.  If provided must be per-point.
@param count (in) Number of points to process
@param mesh_keys (out) Resulting mesh keys.
@return true if all went right, false otherwise.*/
SC_API bool CreatePointMeshes(
    Model& model, Point const* points, RGBA32 const* rgba32s, size_t count, MeshKeys& mesh_keys);
}}} // namespace SC::Store::Utils
