#pragma once
#define _SC_STORE_H

#include <cstdint>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "communicator_version.h"

namespace TC {
namespace Store {
class CacheImpl;
class ModelImpl;
} // namespace Store
namespace IO {
class MemoryFilesystem;
class AbstractFile;
} // namespace IO
} // namespace TC

namespace SC { namespace Store {

/*! The logger class used by the SC Store. It is an abstract base class that users must derive to
 * capture log messages. */
class Logger {
public:
    virtual ~Logger() {}

    /*!
     * Implement this function to receive log messages.
     * @param utf8_message A log message.
     */
    virtual void Message(char const* utf8_message) const = 0;
};

/*! Exceptions thrown by `SC::Store` code will be of this class. */
class Exception: public std::runtime_error {
public:
    Exception(std::string const& message)
    : std::runtime_error(message)
    {}
};

/*! A hasher class for any of the `SC::Store::*Key` classes. */
template<typename T>
struct KeyHasher;

template<>
struct KeyHasher<uint32_t> {
    inline uint64_t
    operator()(uint32_t key) const
    {
        uint64_t hash = (uint64_t)key;
        hash          = (hash ^ 61) ^ (hash >> 16);
        hash          = hash + (hash << 3);
        hash          = hash ^ (hash >> 4);
        hash          = hash * 0x27d4eb2d;
        hash          = hash ^ (hash >> 15);
        return hash;
    }
};

template<typename T>
struct KeyHasher: public KeyHasher<uint32_t> {
    inline uint64_t
    operator()(const T& key) const
    {
        return KeyHasher<uint32_t>()(key._key);
    }
};

/*! A hasher class for any of the `SC::Store::*Id` classes. */
template<typename T>
struct IdHasher {
    inline uint64_t
    operator()(const T& id) const
    {
        KeyHasher<decltype(id.first)>  first_hasher;
        KeyHasher<decltype(id.second)> second_hasher;
        return first_hasher(id.first) ^ second_hasher(id.second);
    }
};

/*! A key used to uniquely identify a model in a given cache. */
class ModelKey {
public:
    ModelKey()
    : _key(0)
    {}
    constexpr explicit ModelKey(uint32_t key)
    : _key(key)
    {}
    ModelKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(ModelKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify a mesh in a given model. */
class MeshKey {
public:
    MeshKey()
    : _key(0)
    {}
    explicit MeshKey(uint32_t key)
    : _key(key)
    {}
    MeshKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(MeshKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify a mesh instance in a model. */
class InstanceKey {
public:
    InstanceKey()
    : _key(0)
    {}
    explicit InstanceKey(uint32_t key)
    : _key(key)
    {}
    InstanceKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(InstanceKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify a polypolygon in a model. */
class PolyPolygonKey {
public:
    PolyPolygonKey()
    : _key(0)
    {}
    explicit PolyPolygonKey(uint32_t key)
    : _key(key)
    {}
    PolyPolygonKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(PolyPolygonKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify a clip region in a model.
@sa
<span class="code">\sa SC::Store::ClipSubRegion</span>
<span class="code">\sa Model::Insert(ClipSubRegions const & clip_subregions)</span>
*/
class ClipRegionKey {
public:
    ClipRegionKey()
    : _key(0)
    {}
    explicit ClipRegionKey(uint32_t key)
    : _key(key)
    {}
    ClipRegionKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(ClipRegionKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify an inclusion in a model. */
class InclusionKey {
public:
    InclusionKey()
    : _key(0)
    {}
    explicit InclusionKey(uint32_t key)
    : _key(key)
    {}
    InclusionKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(InclusionKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify a material in a given model. */
class MaterialKey {
public:
    MaterialKey()
    : _key(0)
    {}
    explicit MaterialKey(uint32_t key)
    : _key(key)
    {}
    MaterialKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(MaterialKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
    inline static MaterialKey
    Unspecified()
    {
        return MaterialKey(~(uint32_t)0);
    }
};

/* A key used to uniquely identify a line pattern in a given model. */
class LinePatternKey {
public:
    LinePatternKey()
    : _key(0)
    {}

    explicit LinePatternKey(uint32_t key)
    : _key(key)
    {}
    LinePatternKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(LinePatternKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
    inline static LinePatternKey
    Unspecified()
    {
        return LinePatternKey(~(uint32_t)0);
    }
};

/*! A key used to uniquely identify a matrix in a given model. */
class MatrixKey {
public:
    MatrixKey()
    : _key(0)
    {}
    explicit MatrixKey(uint32_t key)
    : _key(key)
    {}
    MatrixKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(MatrixKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify a view in a given model. */
class ViewKey {
public:
    ViewKey()
    : _key(0)
    {}
    explicit ViewKey(uint32_t key)
    : _key(key)
    {}
    ViewKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(ViewKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify a image in a given model. */
class ImageKey {
public:
    ImageKey()
    : _key(0)
    {}
    explicit ImageKey(uint32_t key)
    : _key(key)
    {}
    ImageKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(ImageKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify data in a given model. */
class DataKey {
public:
    DataKey()
    : _key(0)
    {}
    explicit DataKey(uint32_t key)
    : _key(key)
    {}
    DataKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(DataKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify a group in a given model. */
class GroupKey {
public:
    GroupKey()
    : _key(0)
    {}
    explicit GroupKey(uint32_t key)
    : _key(key)
    {}
    GroupKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(GroupKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

/*! A key used to uniquely identify data in a given model. */
class MaterialMapKey {
public:
    MaterialMapKey()
    : _key(0)
    {}
    explicit MaterialMapKey(uint32_t key)
    : _key(key)
    {}
    MaterialMapKey&
    operator=(uint32_t key)
    {
        _key = key;
        return *this;
    }
    bool
    operator==(MaterialMapKey const& that)
    {
        return (_key == that._key);
    }
    operator uint32_t() const
    {
        return _key;
    }
    uint32_t _key;
};

typedef std::pair<ModelKey, DataKey>
                             DataId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<DataId>  DataIds;  //!< A std::vector of DataId.
typedef std::vector<DataKey> DataKeys; //!< A std::vector of DataKey.

typedef std::pair<ModelKey, PolyPolygonKey>
                                    PolyPolygonId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<PolyPolygonId>  PolyPolygonIds;  //!< A std::vector of PolyPolygonId.
typedef std::vector<PolyPolygonKey> PolyPolygonKeys; //!< A std::vector of PolyPolygonKey

typedef std::pair<ModelKey, ClipRegionKey>
                                   ClipRegionId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<ClipRegionId>  ClipRegionIds;  //!< A std::vector of ClipRegionId.
typedef std::vector<ClipRegionKey> ClipRegionKeys; //!< A std::vector of ClipRegionKey

typedef std::pair<ModelKey, ImageKey>
                              ImageId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<ImageId>  ImageIds;  //!< A std::vector of ImageId.
typedef std::vector<ImageKey> ImageKeys; //!< A std::vector of ImageKey

typedef std::pair<ModelKey, MeshKey>
                             MeshId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<MeshId>  MeshIds;  //!< A std::vector of MeshId.
typedef std::vector<MeshKey> MeshKeys; //!< A std::vector of MeshKey

typedef std::pair<ModelKey, MaterialKey>
                                 MaterialId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<MaterialId>  MaterialIds;  //!< A std::vector of MaterialId.
typedef std::vector<MaterialKey> MaterialKeys; //!< A std::vector of MaterialKey

typedef std::pair<ModelKey, LinePatternKey>
                                    LinePatternId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<LinePatternId>  LinePatternIds;  //!< A std::vector of LinePatternId.
typedef std::vector<LinePatternKey> LinePatternKeys; //!< A std::vector of LinePatternKey

typedef std::pair<ModelKey, MatrixKey>
                               MatrixId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<MatrixId>  MatrixIds;  //!< A std::vector of MatrixId.
typedef std::vector<MatrixKey> MatrixKeys; //!< A std::vector of MatrixKey

typedef std::vector<InclusionKey> InclusionKeys; //!< A std::vector of InclusionKey
typedef std::pair<InclusionKey, InstanceKey>
                                 InstanceInc;  //!< This is an instance through a specific include.
typedef std::vector<InstanceInc> InstanceIncs; //!< A std::vector of InstanceInc.
typedef std::pair<ModelKey, InstanceKey>
                                 InstanceId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<InstanceId>  InstanceIds;  //!< A std::vector of InstanceId.
typedef std::vector<InstanceKey> InstanceKeys; //!< A std::vector of InstanceKey

typedef std::pair<InclusionKey, GroupKey>
                              GroupInc;  //!< This is an instance through a specific include.
typedef std::vector<GroupInc> GroupIncs; //!< A std::vector of GroupInc.
typedef std::pair<ModelKey, GroupKey>
                              GroupId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<GroupId>  GroupIds;  //!< A std::vector of MeshId.
typedef std::vector<GroupKey> GroupKeys; //!< A std::vector of GroupKey.

typedef std::pair<ModelKey, MaterialMapKey>
                                    MaterialMapId; //!< This is an ID that is completely unique throughout an entire cache.
typedef std::vector<MaterialMapId>  MaterialMapIds;  //!< A std::vector of MaterialMapId.
typedef std::vector<MaterialMapKey> MaterialMapKeys; //!< A std::vector of MaterialMapKey.

/*! An enumeration of usable compression stratagies. */
enum class CompressionStrategy { Fast, Default, Strong };

/*! The Visibility class holds Bits used to define an instance's visibility. */
class Visibility {
public:
    /*! An enumeration of bit flags that denote which parts of a geometry are visible. */
    enum Bits : uint32_t {
        None        = 0x00000000, //!< If set nothing will be visible.
        Face        = 0x00000001, //!< If set faces will be visible.
        Line        = 0x00000002, //!< If set lines will be visible.
        Point       = 0x00000004, //!< If set points will be visible.
        Unspecified = 0xFFFFFFFF, //!< Used internally for InstanceInc properties; do not use.
        Default     = (Face | Line | Point),
    };

    Visibility()
    : _value(Bits::Default)
    {}
    explicit Visibility(uint32_t value)
    : _value(value)
    {}
    Visibility&
    operator=(uint32_t value)
    {
        _value = value;
        return *this;
    }
    bool
    operator==(Visibility const& that)
    {
        return (_value == that._value);
    }
    operator uint32_t() const
    {
        return _value;
    }
    uint32_t _value;
};

/*! The Modifiers class holds Bits used to define instance modifiers. */
class Modifiers {
public:
    /*! An enumeration of bit flags that modify the drawing of an instance. */
    enum Bits : uint32_t {
        None = 0x00000000,
        /** If set, the instance will not be affected by cutting planes. */
        DoNotCut = 0x00000001,
        /** If set, the instance will not generate capping geometry. */
        DoNotCap = 0x00000002,
        /**
         * If set, the instance will be drawn before other instances
         * and will not be culled in order to reach the target frame rate.
         */
        AlwaysDraw = 0x00000004,
        /**
         * If set, the instance will be oriented such that the cardinal
         * axes of object space are aligned with the cardinal axes of
         * screen space.
         */
        ScreenOriented = 0x00000008,
        /**
         * If set, the instance will be drawn at the same size
         * regardless of camera settings.
         */
        SuppressCameraScale = 0x00000010,
        /** If set, the instance will not be affected by an explode operation. */
        DoNotExplode = 0x00000020,
        /** If set, the instance will not be selectable. */
        DoNotSelect = 0x00000040,
        /**
         * If set, the instance will be drawn in a space that extends
         * from X=-1 at the left side of the screen to X=1 at the right
         * side of the screen and from Y=-1 at the bottom of the screen
         * to Y=1 at the top of the screen.
         *
         * If the aspect ratio of the screen is not 1:1, the instance
         * will be scaled so that it appears as if the aspect ratio
         * were 1:1 (i.e., so that the instance will not appear stretched).
         */
        ScreenSpace = 0x00000100,
        /**
         * If set, the instance will be drawn in a space that extends
         * from X=-1 at the left side of the screen to X=1 at the right
         * side of the screen and from Y=-1 at the bottom of the screen
         * to Y=1 at the top of the screen.
         *
         * If the aspect ratio of the screen is not 1:1, the instance
         * will stretch in the direction of the longer side.
         */
        ScreenSpaceStretched = 0x00000200,
        /**
         * If set, the instance clip region will be outlined in the viewer
         */
        OutlineClipRegion = 0x00000400,

        /** If set, the instance will not be lit. */
        DoNotLight = 0x00000800,
        /** If set, the instance will ignore scene-level visibility. */
        OverrideSceneVisibility = 0x00001000,
        /** If set, the instance will not be outlined when highlighted. */
        DoNotOutlineHighlight = 0x00002000,
        /** If set, the instance will not be included in bounding calculations. */
        ExcludeBounding = 0x00004000,
        /** If set, the instance will not use the mesh's per-vertex colors. */
        DoNotUseVertexColors = 0x00008000,
        /**
         * If set, the instance will be streamed first and not unloaded
         * by memory limiting.
         */
        AlwaysStream = 0x00010000,

        Default = (None),
    };

    Modifiers()
    : _value(Bits::Default)
    {}
    explicit Modifiers(uint32_t value)
    : _value(value)
    {}
    Modifiers&
    operator=(uint32_t value)
    {
        _value = value;
        return *this;
    }
    bool
    operator==(Modifiers const& that)
    {
        return (_value == that._value);
    }
    operator uint32_t() const
    {
        return _value;
    }
    uint32_t _value;
};

class LinePattern {
public:
    void
    setLength(double const& a_length)
    {
        length = a_length;
    }
    void
    setValues(std::vector<uint8_t> const& a_values)
    {
        values = a_values;
    }
    void
    setValues(std::vector<uint8_t>&& a_values)
    {
        values = std::move(a_values);
    }

    double               length;
    std::vector<uint8_t> values;
};

/*! An RGBA color, where each channel is in normalized [0, 1] space. */
struct Color {
    Color() = default;
    Color(float red, float green, float blue, float alpha)
    : red(red)
    , green(green)
    , blue(blue)
    , alpha(alpha)
    {} //!< Constructor definition for a color. Takes four float values as parameters. This example
       //!< represents the color red: \n <div class="fragment">Color * red_color = new Color(1.0f,
       //!< 0.0f, 0.0f, 1.0f);</div>
    bool
    operator==(Color const& that) const
    {
        return (red == that.red && green == that.green && blue == that.blue && alpha == that.alpha);
    }
    bool
    operator!=(Color const& that) const
    {
        return !(operator==(that));
    }
    float red;   //!< Red component of this color object, from 0.0 to 1.0.
    float green; //!< Green component of this color object, from 0.0 to 1.0.
    float blue;  //!< Blue component of this color object, from 0.0 to 1.0.
    float alpha; //!< Alpha component of this color object, from 0.0 to 1.0. Used to define
                 //!< transparency level.
};

/*! An RGBA color, where each channel is in discrete [0, 255] space. */
struct RGBA32 {
    RGBA32() = default;
    RGBA32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
    {}
    bool
    operator==(RGBA32 const& that) const
    {
        return (r == that.r && g == that.g && b == that.b && a == that.a);
    }
    bool
    operator!=(RGBA32 const& that) const
    {
        return !(operator==(that));
    }
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

/*! An RGB color, where each channel is in discrete [0, 255] space. */
struct RGB24 {
    RGB24() = default;
    RGB24(uint8_t r, uint8_t g, uint8_t b)
    : r(r)
    , g(g)
    , b(b)
    {}
    bool
    operator==(RGB24 const& that) const
    {
        return (r == that.r && g == that.g && b == that.b);
    }
    bool
    operator!=(RGB24 const& that) const
    {
        return !(operator==(that));
    }
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

/*! A gray color, where the channel is in discrete [0, 255] space. */
struct Gray8 {
    Gray8() = default;
    explicit Gray8(uint8_t gray)
    : gray(gray)
    {}
    bool
    operator==(Gray8 const& that) const
    {
        return (gray == that.gray);
    }
    bool
    operator!=(Gray8 const& that) const
    {
        return !(operator==(that));
    }
    uint8_t gray;
};

/*! A gray color with an alpha , where each channel is in discrete [0, 255] space. */
struct GrayAlpha16 {
    GrayAlpha16() = default;
    GrayAlpha16(uint8_t gray, uint8_t alpha)
    : gray(gray)
    , alpha(alpha)
    {}
    bool
    operator==(GrayAlpha16 const& that) const
    {
        return (gray == that.gray && alpha == that.alpha);
    }
    bool
    operator!=(GrayAlpha16 const& that) const
    {
        return !(operator==(that));
    }
    uint8_t gray;
    uint8_t alpha;
};

/*!
 * Represents a single precision 3d linear transformation.
 *
 * An identity matrix is specified as follows:
 *		float const identity[] = {
 *			1.0f, 0.0f, 0.0f,
 *			0.0f, 1.0f, 0.0f,
 *			0.0f, 0.0f, 1.0f,
 *			0.0f, 0.0f, 0.0f,
 *		};
 */
struct Matrix3d {
    float m[12];
    bool
    operator==(Matrix3d const& that) const
    {
        return m == that.m;
    }
    bool
    operator!=(Matrix3d const& that) const
    {
        return !(operator==(that));
    }

    void
    SetIdentity()
    {
        float const identity[] = {
            1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        };
        memcpy(m, identity, sizeof(m));
    }

    void
    SetTranslation(float x, float y, float z)
    {
        m[9]  = x;
        m[10] = y;
        m[11] = z;
    }

    void
    Translate(float x, float y, float z)
    {
        m[9] += x;
        m[10] += y;
        m[11] += z;
    }
};

/*!
 * Represents a double precision 3d linear transformation.
 *
 * An identity matrix is specified as follows:
 *		double const identity[] = {
 *			1.0, 0.0, 0.0,
 *			0.0, 1.0, 0.0,
 *			0.0, 0.0, 1.0,
 *			0.0, 0.0, 0.0,
 *		};
 */
struct DMatrix3d {
    double m[12];
    bool
    operator==(DMatrix3d const& that) const
    {
        return m == that.m;
    }
    bool
    operator!=(DMatrix3d const& that) const
    {
        return !(operator==(that));
    }

    void
    SetIdentity()
    {
        double const identity[] = {
            1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        };
        memcpy(m, identity, sizeof(m));
    }

    void
    SetTranslation(double x, double y, double z)
    {
        m[9]  = x;
        m[10] = y;
        m[11] = z;
    }

    void
    Translate(double x, double y, double z)
    {
        m[9] += x;
        m[10] += y;
        m[11] += z;
    }

    static DMatrix3d
    fromMatrix3d(Matrix3d const& matrix)
    {
        DMatrix3d result;

        for (int i = 0; i < 12; i++)
            result.m[i] = static_cast<double>(matrix.m[i]);

        return result;
    }
};

/*! A single precision point in 3-space. */
struct Point {
    Point() = default;
    Point(float x, float y, float z)
    : x(x)
    , y(y)
    , z(z)
    {}
    bool
    operator==(Point const& that) const
    {
        return (x == that.x && y == that.y && z == that.z);
    }
    bool
    operator!=(Point const& that) const
    {
        return !(operator==(that));
    }
    float x;
    float y;
    float z;
};

/*! A double precision point in 3-space. */
struct DPoint {
    DPoint() = default;
    DPoint(double x, double y, double z)
    : x(x)
    , y(y)
    , z(z)
    {}
    bool
    operator==(DPoint const& that) const
    {
        return (x == that.x && y == that.y && z == that.z);
    }
    bool
    operator!=(DPoint const& that) const
    {
        return !(operator==(that));
    }
    double x;
    double y;
    double z;

    Point
    toPoint()
    {
        Point ret;
        ret.x = static_cast<float>(x);
        ret.y = static_cast<float>(y);
        ret.z = static_cast<float>(z);
        return ret;
    }

    static DPoint
    fromPoint(Point const& point)
    {
        DPoint ret;
        ret.x = static_cast<double>(point.x);
        ret.y = static_cast<double>(point.y);
        ret.z = static_cast<double>(point.z);
        return ret;
    }
};

/*! A single precision vector in 3-space. */
struct Vector {
    Vector() = default;
    Vector(float x, float y, float z)
    : x(x)
    , y(y)
    , z(z)
    {}
    bool
    operator==(Vector const& that) const
    {
        return (x == that.x && y == that.y && z == that.z);
    }
    bool
    operator!=(Vector const& that) const
    {
        return !(operator==(that));
    }
    float x;
    float y;
    float z;
};

/*! A double precision vector in 3-space. */
struct DVector {
    DVector() = default;
    DVector(double x, double y, double z)
    : x(x)
    , y(y)
    , z(z)
    {}
    bool
    operator==(DVector const& that) const
    {
        return (x == that.x && y == that.y && z == that.z);
    }
    bool
    operator!=(DVector const& that) const
    {
        return !(operator==(that));
    }
    double x;
    double y;
    double z;
};

/*! A single precision normalized vector in 3-space. It is the producer's burden to ensure that the
 * normal is normalized. */
struct Normal {
    Normal() = default;
    Normal(float x, float y, float z)
    : x(x)
    , y(y)
    , z(z)
    {}
    bool
    operator==(Normal const& that) const
    {
        return (x == that.x && y == that.y && z == that.z);
    }
    bool
    operator!=(Normal const& that) const
    {
        return !(operator==(that));
    }
    float x;
    float y;
    float z;
};

/*! A structure that contains UV parameter information. */
struct UV {
    UV() = default;
    UV(float u, float v)
    : u(u)
    , v(v)
    {}
    bool
    operator==(UV const& that) const
    {
        return (u == that.u && v == that.v);
    }
    bool
    operator!=(UV const& that) const
    {
        return !(operator==(that));
    }
    float u;
    float v;
};

/*! <p>The BaseMesh class is used to define a Mesh as its base class.</p>

    <p> It contains indexable properties common to all meshes.</p>

    <p>Definable properties are points, normals, UV parameters, and RGBA32 colors along with their
   corresponding counts. Each of these attributes is given by a user-supplied array pointer that is
   NOT owned by the base mesh. The pointers must remain valid until the mesh is inserted into the
   database.</p>

    <p>These properties are not directly used to represent geometry. Instead, they are indexed by
   mesh elements in order to define actual geometry.</p>

    <p>The BaseMesh is movable but not copyable.*/
class BaseMesh {
public:
    ~BaseMesh() = default;

    /*! BaseMesh is inherited by the Mesh class. This constructor shouldn't be called directly. */
    BaseMesh()
    : points(nullptr)
    , normals(nullptr)
    , uvs(nullptr)
    , rgba32s(nullptr)
    , point_count(0)
    , normal_count(0)
    , uv_count(0)
    , rgba32_count(0)
    {}

    // No copying
    BaseMesh(const BaseMesh& other) = delete;
    BaseMesh& operator=(const BaseMesh& other) = delete;

    // Default move is appropriate
    BaseMesh(BaseMesh&& other) = default;
    BaseMesh& operator=(BaseMesh&& other) = default;

    Point const* points; //!< @brief An array of points. \details Each point is constructed with a
                         //!< three-element float array. For example: \n <div class="fragment"><div
                         //!< class="line">	SC::Store::Point points[] = { \n { 0.0f, 0.0f, 1.0f },
                         //!< \n { 1.0f, 0.0f, 1.0f }, \n };</div></div>
    Normal const* normals; //!< @brief An array of normals. \details Each normal is constructed with
                           //!< a three-element float array. For example: \n <div
                           //!< class="fragment"><div class="line">	SC::Store::Normal normals[] = {
                           //!< \n { 0.0f, -1.0f, 0.0f }, \n { 0.0f, 0.0f, 1.0f }, \n };</div></div>
    UV const*
        uvs; //!< @brief An array of UV points. \details Each UV is constructed with a two-element
             //!< float array. For example: \n <div class="fragment"><div class="line">
             //!< SC::Store::UV uvs[] = { \n { 0.0f, -1.0f }, \n { 0.0f, 0.0f }, \n };</div></div>
    RGBA32 const*
        rgba32s; //!< @brief An array of RGBA32 color values. \details Each RGBA32 object is
                 //!< constructed with a four-element array of 8-bit integers (0&ndash;255). For
                 //!< example: \n <div class="fragment"><div class="line">	SC::Store::RGBA32 rgba[]
                 //!< =  {\n RGBA32(255, 0, 0, 255), \n RGBA32(255, 0, 0, 128),\n };</div></div>

    uint32_t
        point_count; //!< @brief The number of Point elements in the \ref points array. \details A
                     //!< \ref points array with the elements <span class="code">{ 0.0f, 0.0f, 1.0f
                     //!< }, { 1.0f, 0.0f, 1.0f }</span> would have a point_count of 2.
    uint32_t
        normal_count;  //!< @brief The number of Normal elements in the \ref normals array. \details
                       //!< A \ref normals array with the elements <span class="code">{ 0.0f, -1.0f,
                       //!< 0.0f }, { 0.0f, 0.0f, 1.0f }</span> would have a normal_count of 2.
    uint32_t uv_count; //!< @brief The number of UV elements in the \ref uvs array. \details A \ref
                       //!< uvs array with the elements <span class="code">{ 0.0f, 0.0f }, { 1.0f,
                       //!< 0.0f }</span> would have a uv_count of 2.
    uint32_t rgba32_count; //!< @brief The number of RGBA32 elements in the \ref rgba32s array.
                           //!< \details An \ref rgba32s array with the elements <span
                           //!< class="code">{ RGBA32(255, 0, 0, 255), RGBA32(255, 0, 0, 128)
                           //!< }</span> would have an rgba32_count of 2.
};

/*! <p>The MeshElement class is used to define a Mesh.</p>

    <p>These elements define a mesh's faces, polylines and points using indices that refer into its
   `BaseMesh` pointers.</p>

    <p>The encoding scheme for the mesh element's indices are given by:</p>

    <ul>
        <li>For face elements - A sequence of grouped indices from the base mesh. Every sequence of
   three index groups defines a triangle in the mesh. A grouped index is a single point, vertex
   normal, UV, and/or RGBA32 index from the base mesh, denoting a point and its local properties.
   For each property that is defined (i.e., non-null) in the base mesh, the grouped index must
   contain an index into that property. The order in which the indices appear are given by the
   following ordering: point, normal, UV, and then RGBA32. Only complete triangles are allowed in
   the encoding.</li> <li>For polyline elements - A sequence of point indices from the base mesh.
   The order in which the points are denoted describe the polygonal line in the mesh. Using
   (~(uint32_t)0) in a polyline index list is a special token used to break a polyline into a new
   polyline.</li> <li>For point elements - A sequence of point indices from the base mesh. Each
   index defines a geometry point in the mesh.</li>
    </ul>

    <p>Note: A point element is not to be confused with a base mesh's point array. The base mesh
   point array is solely used to index into in order to define actual mesh element geometry.</p> */
class MeshElement {
public:
    MeshElement()
    : indices()
    {}

    MeshElement(MeshElement const&) = default;

    MeshElement(MeshElement&& that)
    : indices(std::move(that.indices))
    {}

    MeshElement& operator=(MeshElement const&) = default;

    MeshElement&
    operator=(MeshElement&& that)
    {
        indices = std::move(that.indices);
        return *this;
    }

    bool
    operator==(MeshElement const& that) const
    {
        return (indices == that.indices);
    }
    bool
    operator!=(MeshElement const& that) const
    {
        return !(operator==(that));
    }

    /*! <p> These indices must match the flags provided to the Mesh.
        By default Faces use normals, UVs and rgba32s and lines and points use rgba32s. </p>

        <p> If normals only exist 'PNPNPN' defines a triangle else 'PPP' for points only or
        'PUPUPU' for UVs or 'PNUPNU' for normals and UVs or 'PRPR' for points and rgba32s or
        'PNRPNR' etc. </p>

        <p> A polyline or points is drawn on base mesh points 'P' or 'PR'.</p>
    */
    std::vector<uint32_t> indices;
};

/*! <p>The Mesh class is used to define a MeshKey.</p>

    <p>It contains any number of face elements, polyline elements, and point elements.</p>

    <p>It may specify a MaterialKey for its faces, polylines and points.
     These MaterialKeys are the default material used for an instance that
     does not provide specific instance-level materials.</p>

    <p>A mesh element index may be given an optional MaterialKey by providing a MaterialMapKey.
     This MaterialMapKey is the default for any instance of this mesh.</p>

    <p>A Mesh is movable but not copyable.*/
class Mesh: public BaseMesh {
public:
    enum Bits : uint32_t {

        None = 0x00000000,

        //! Set a winding if known.
        ClockwiseWinding        = 0x00000001,
        CounterClockwiseWinding = 0x00000002,

        TwoSided = 0x00000004, //!< If set, the mesh will not be back-face culled.

        FaceNormals = 0x00000010, //!< Set if normals are present in the BaseMesh.
        FaceUVs     = 0x00000020, //!< Set if UVs are present in the BaseMesh.
        FaceRGBA32s = 0x00000040, //!< Set if RGBA32s are present in the BaseMesh.

        LineNormals = 0x00000100, //!< Set if normals are present in the BaseMesh.
        LineUVs     = 0x00000200, //!< Set if UVs are present in the BaseMesh.
        LineRGBA32s = 0x00000400, //!< Set if RGBA32s are present in the BaseMesh.

        PointNormals = 0x00001000, //!< Set if normals are present in the BaseMesh.
        PointUVs     = 0x00002000, //!< Set if UVs are present in the BaseMesh.
        PointRGBA32s = 0x00004000, //!< Set if PointRGBA32s are present in the BaseMesh.

        Manifold = 0x00010000, //!< Set if mesh faces define a manifold body.  This allows cutting
                               //!< plane capping for the mesh.

        RandomizedPoints = 0x00020000, //!< Set if mesh points are randomized.  This allows vertex
                                       //!< decimation for the mesh.

        Default =
            (CounterClockwiseWinding | FaceNormals | FaceUVs | FaceRGBA32s | LineRGBA32s
             | PointRGBA32s),
    };

    ~Mesh() = default;
    Mesh()  = default;

    // No copying
    Mesh(const Mesh& other) = delete;
    Mesh& operator=(const Mesh& other) = delete;

    // Default move is appropriate
    Mesh(Mesh&& other) = default;
    Mesh& operator=(Mesh&& other) = default;

    bool
    operator==(Mesh const& that) const
    {
        return (
            face_elements == that.face_elements && polyline_elements == that.polyline_elements
            && point_elements == that.point_elements
            && face_elements_bits == that.face_elements_bits
            && polyline_elements_bits == that.polyline_elements_bits
            && point_elements_bits == that.point_elements_bits
            && mesh_face_material == that.mesh_face_material
            && mesh_line_material == that.mesh_line_material
            && mesh_point_material == that.mesh_point_material
            && mesh_face_element_material_map == that.mesh_face_element_material_map
            && mesh_line_element_material_map == that.mesh_line_element_material_map
            && mesh_point_element_material_map == that.mesh_point_element_material_map
            && flags == that.flags);
    }
    bool
    operator!=(Mesh const& that) const
    {
        return !(operator==(that));
    }

    std::vector<MeshElement> face_elements;     //!< These define the mesh's faces.
    std::vector<MeshElement> polyline_elements; //!< These define the mesh's lines.
    std::vector<MeshElement> point_elements;    //!< These define the mesh's points.

    /*!
     * The default bit value is 0.
     *	The provided values will cycle for all mesh elements.
     *	If the vector of bits is length 1, then the single value will apply for all mesh elements of
     *the type. If more bits are provided than mesh elements it will throw an exception.
     */
    std::vector<uint8_t>
        face_elements_bits; //!< Optional bits for filtering selections on the mesh element.
    std::vector<uint8_t>
        polyline_elements_bits; //!< Optional bits for filtering selections on the mesh element.
    std::vector<uint8_t>
        point_elements_bits; //!< Optional bits for filtering selections on the mesh element.

    MaterialKey    mesh_face_material;  //!< Optional material that applies to the Mesh faces.
    MaterialKey    mesh_line_material;  //!< Optional material that applies to the Mesh lines.
    MaterialKey    mesh_point_material; //!< Optional material that applies to the Mesh points.
    MaterialMapKey mesh_face_element_material_map; //!< Optional material map that applies to faces
                                                   //!< in the MeshElement at the matching indices.
    MaterialMapKey mesh_line_element_material_map; //!< Optional material map that applies to lines
                                                   //!< in the MeshElement at the matching indices.
    MaterialMapKey
        mesh_point_element_material_map; //!< Optional material map that applies to points in the
                                         //!< MeshElement at the matching indices.

    Bits flags = Bits::Default; //!< Bits that specify this mesh's details.
};

/*! The Camera class is used to define a camera for a View.
    It specifies the position, target, up vector, field dimensions, and projection type.

    @n The field of view of the camera is comprised of two numbers, field_width and field_height.
   These define the minimum area around the target that will be visible in the output window. The
   camera field, along with the distance between the camera position and the camera target,
    determine - in photographic terms - what kind of lens the camera is using. If the field is
   larger than the distance from the camera position to the target, then we have the equivalent of a
   wide-angle lens. If the distance between the camera position and target is much larger than the
   camera field, then we have the equivalent of a telephoto lens. Changing the size of the field (if
   the camera position and target remain fixed) is the same as zooming the lens. The ratio of the
   width to the height of the field is called the aspect ratio.
*/
class Camera {
    uint32_t pad;

public:
    /*! An enumeration that indicates a camera's projection type. */
    enum Projection : uint32_t {
        Invalid = 0, //!< This means the Camera Projection has not been set, and is effectively like
                     //!< having no Camera at all.
        Perspective, //!< Objects diminish in size the further they are from the camera, just as in
                     //!< a typical photograph.
        Orthographic, //!< Objects remain the same size, regardless of their distance from the
                      //!< camera.
        Stretched, //!< With a stretched projection, the scene stretches to fit the output window.
                   //!< The aspect ratio will not be preserved and the output may appear stretched
                   //!< or distorted.
    };

    Camera()
    {
        memset(this, 0, sizeof(*this));
    }

    Camera(
        Projection     projection,
        DPoint const&  position,
        DPoint const&  target,
        DVector const& up_vector,
        double         field_width,
        double         field_height)
    : pad(0)
    , projection(projection)
    , position(position)
    , target(target)
    , up_vector(up_vector)
    , field_width(field_width)
    , field_height(field_height)
    {
        (void)pad;
    }

    bool
    operator==(Camera const& that) const
    {
        return (
            projection == that.projection && position == that.position && target == that.target
            && up_vector == that.up_vector && field_width == that.field_width
            && field_height == that.field_height);
    }
    bool
    operator!=(Camera const& that) const
    {
        return !(operator==(that));
    }

    Projection projection;   //!< Enum that specifies the projection type.
    DPoint     position;     //!< The location of the camera.
    DPoint     target;       //!< Sets the location where the camera is pointing.
    DVector    up_vector;    //!< Defines the vertical up vector of the camera.
    double     field_width;  //!< Sets the width of the camera field in world space units.
    double     field_height; //!< Sets the height of the camera field in world space units.
};

//! The ImageFormat enum class is used to define an ImageKey from the byte data of an image file.
enum class ImageFormat : uint32_t {
    PNG,
    JPEG,
    BMP, //!< The 1 bit and RLE formats are not supported.
};

//! The Texture class is used to define a Material.
class Texture {
public:
    enum Tiling : uint32_t {
        Repeat =
            0, //!< Repeat the texture image when UV coordinates go outside the [0.0, 1.0] range.
        Clamp, //!< Textures don't repeat. Any UV coordinates outside the [0.0, 1.0] range are
               //!< clamped to 0.0 or 1.0, whichever is closer.
        Trim   //!< The texture will get mapped normally for parameters in the range [0,1], but
               //!< parameters outside that range will act as if the texture at that location is
               //!< transparent.
    };

    enum Interpolation : uint32_t {
        InterpolationOn =
            0, //!< Enable interpolation of texel colors (also known as linear filtering).
        InterpolationOff, //!< Disable interpolation. The texture image will appear pixelated when
                          //!< enlarged.
    };

    enum MipMapping : uint32_t {
        MipMappingOn = 0, //!< Turn on mipmapping to improve image quality at the expense of
                          //!< slightly more memory usage. Depending on the platform, mipmapping may
                          //!< take effect only for textures with dimensions that are powers of two.
        MipMappingOff,    //!< Disable mipmapping. Textures may appear noisy when reduced in size.
    };

    enum Parameterization : uint32_t {
        UV = 0, //!< The texture will be mapped according to UV coordinates specified in the mesh
                //!< data.
    };

    enum Modifiers : uint32_t {
        None = 0,
        Decal, //!< When set, any transparent areas of the texture will be the same color as the
               //!< underlying diffuse color of the object.
    };

    //! This method generates an integer value representing the desired settings for tiling,
    //! interpolation, mipmapping, parameterization, and modifiers.
    static uint32_t
    PackFlags(
        Tiling           tiling,
        Interpolation    interpolation,
        MipMapping       mipmapping,
        Parameterization parameterization,
        Modifiers        modifiers)
    {
        uint32_t flags = tiling;
        flags |= (interpolation << 4);
        flags |= (mipmapping << 8);
        flags |= (parameterization << 12);
        flags |= (modifiers << 16);
        return flags;
    }

    Texture()
    : image_key()
    , matrix()
    , flags(0)
    {}

    Texture(ImageKey image_key, MatrixKey matrix = MatrixKey(), uint32_t flags = 0)
    : image_key(image_key)
    , matrix(matrix)
    , flags(flags)
    {}
    bool
    operator==(Texture const& that) const
    {
        return (image_key == that.image_key && matrix == that.matrix && flags == that.flags);
    }
    bool
    operator!=(Texture const& that) const
    {
        return !(operator==(that));
    }

    ImageKey
              image_key; //!< A key used to uniquely identify an image in this model for this texture.
    MatrixKey matrix;    //!< The matrix to use for transforming the texture coordinates.
    uint32_t  flags; //!< A value representing the settings for tiling, interpolation, mipmapping,
                     //!< parameterization, and modifiers. \sa PackFlags
};

//! The MaterialProperties class is used to define a Material or specify the defaults for a View.
class MaterialProperties {
public:
    MaterialProperties()
    : environment()
    , mirror(-1)
    , specular(-1)
    , gloss(-1)
    {}
    MaterialProperties(float mirror, float specular, float gloss)
    : mirror(mirror)
    , specular(specular)
    , gloss(gloss)
    {}

    bool
    operator==(MaterialProperties const& that) const
    {
        return (
            environment == that.environment && mirror == that.mirror && specular == that.specular
            && gloss == that.gloss);
    }
    bool
    operator!=(MaterialProperties const& that) const
    {
        return !(operator==(that));
    }

    bool
    empty() const
    {
        return environment.empty() && mirror == -1 && specular == -1 && gloss == -1;
    }

    ImageKeys environment; //!< Valid counts are {0,1,6} ImageKeys. None if 0 or 1 [spheremap] or 6
                           //!< cubemap [negative_z positive_z negative_x positive_x negative_y
                           //!< positive_y]

    float mirror;   //!< Clamp value with 0 minimum and 1 maximum; value -1 is unset.
    float specular; //!< Clamp value with 0 minimum and 1 maximum; value -1 is unset.
    float gloss;    //!< Clamp value with 0 minimum and 1 maximum; value -1 is unset.
};

//! The Material class is used to define a MaterialKey.
class Material {
public:
    Material()
    : diffuse_textures()
    , diffuse_color(Color(1, 1, 1, 1))
    , properties()
    , line_pattern_key(LinePatternKey::Unspecified())
    {}

    explicit Material(Color const& diffuse_color)
    : diffuse_textures()
    , diffuse_color(diffuse_color)
    , properties()
    , line_pattern_key(LinePatternKey::Unspecified())
    {}

    bool
    operator==(Material const& that) const
    {
        return (
            diffuse_textures == that.diffuse_textures && diffuse_color == that.diffuse_color
            && properties == that.properties && line_pattern_key == that.line_pattern_key);
    }
    bool
    operator!=(Material const& that) const
    {
        return !(operator==(that));
    }

    std::vector<Texture> diffuse_textures; //!< This supports 0 or 1 or 2 diffuse textures.
    Color                diffuse_color;

    MaterialProperties properties;

    LinePatternKey line_pattern_key;
};

//! The PBRMaterial class is used to define a MaterialKey.
class PBRMaterial {
public:
    enum ChannelMapping : uint32_t {
        Red = 0,
        Green,
        Blue,
        Alpha,
    };

    enum AlphaMode : uint32_t { Blend, Mask, Opaque };

    PBRMaterial()
    : base_color_map()
    , normal_map()
    , emissive_map()
    , metalness_map()
    , roughness_map()
    , occlusion_map()
    , metalness_map_channel(ChannelMapping::Red)
    , roughness_map_channel(ChannelMapping::Red)
    , occlusion_map_channel(ChannelMapping::Red)
    , base_color_factor(Color(1, 1, 1, 1))
    , emissive_factor(Color(0, 0, 0, 1))
    , normal_factor(1)
    , metalness_factor(1)
    , roughness_factor(1)
    , occlusion_factor(1)
    , alpha_cutoff(1)
    , alpha_mode(AlphaMode::Opaque)
    {}

    bool
    operator==(PBRMaterial const& that) const
    {
        return (
            base_color_map == that.base_color_map && normal_map == that.normal_map
            && emissive_map == that.emissive_map && metalness_map == that.metalness_map
            && roughness_map == that.roughness_map && occlusion_map == that.occlusion_map
            && metalness_map_channel == that.metalness_map_channel
            && roughness_map_channel == that.roughness_map_channel
            && occlusion_map_channel == that.occlusion_map_channel
            && base_color_factor == that.base_color_factor
            && emissive_factor == that.emissive_factor && normal_factor == that.normal_factor
            && metalness_factor == that.metalness_factor
            && roughness_factor == that.roughness_factor
            && occlusion_factor == that.occlusion_factor && alpha_cutoff == that.alpha_cutoff
            && alpha_mode == that.alpha_mode);
    }
    bool
    operator!=(PBRMaterial const& that) const
    {
        return !(operator==(that));
    }

    Texture base_color_map; //!< Stored in sRGB color space.
    Texture normal_map;
    Texture emissive_map; // !< Stored in sRGB color space.
    Texture metalness_map;
    Texture roughness_map;
    Texture occlusion_map;

    ChannelMapping metalness_map_channel;
    ChannelMapping roughness_map_channel;
    ChannelMapping occlusion_map_channel;

    Color     base_color_factor; //!< Stored in linear color space.
    Color     emissive_factor;   //!< Stored in linear color space.
    float     normal_factor;
    float     metalness_factor;
    float     roughness_factor;
    float     occlusion_factor;
    float     alpha_cutoff;
    AlphaMode alpha_mode;
};

//! The MaterialMapElement class is used to define a MaterialMapKey with a sparse material map.
class MaterialMapElement {
public:
    MaterialMapElement()
    : index(0)
    , length(0)
    , material(0)
    {}

    MaterialMapElement(uint32_t index, uint32_t length, MaterialKey material)
    : index(index)
    , length(length)
    , material(material)
    {}

    bool
    operator==(MaterialMapElement const& that) const
    {
        return (index == that.index && length == that.length && material == that.material);
    }
    bool
    operator!=(MaterialMapElement const& that) const
    {
        return !(operator==(that));
    }

    uint32_t    index;
    uint32_t    length;
    MaterialKey material;
};

typedef std::vector<MaterialMapElement>
    MaterialMapElements; //!< A std::vector of MaterialMapElement

//! The ClipSubRegion class is used to define a ClipRegionKey.
class ClipSubRegion {
public:
    enum Operation : uint32_t {
        Keep,   //!< The geometry inside the clip region is drawn. Everything outside of it is
                //!< clipped.
        Remove, //!< The geometry outside the clip region is drawn. Everything inside of it is
                //!< clipped.
    };

    enum Space : uint32_t {
        World,  //!< The clip region is specified in world coordinates.
        Window, //!< The clip region is specified in window coordinates. -1 to 1
    };

    ClipSubRegion()
    : polypolygons()
    , matrix(MatrixKey())
    , operation(Keep)
    , space(World)
    {}

    ClipSubRegion(MatrixKey matrix, Operation operation, Space space)
    : polypolygons()
    , matrix(matrix)
    , operation(operation)
    , space(space)
    {}

    bool
    operator==(ClipSubRegion const& that) const
    {
        return (
            polypolygons == that.polypolygons && matrix == that.matrix
            && operation == that.operation && space == that.space);
    }
    bool
    operator!=(ClipSubRegion const& that) const
    {
        return !(operator==(that));
    }

    PolyPolygonKeys polypolygons; //!< The polypolygons that define this <span
                                  //!< class="code">ClipSubRegion</span>.
    MatrixKey matrix;             //!< The transformation matrix for this <span
                                  //!< class="code">ClipSubRegion</span> (optional).
    Operation
        operation; //!< The <span class="code">\ref ClipSubRegion::Operation</span> to use; defaults
                   //!< to <span class="code">\ref ClipSubRegion::Operation "Keep"</span>.
    Space space; //!< The <span class="code">\ref ClipSubRegion::Space</span> coordinate type to use
                 //!< for defining this <span class="code">ClipSubRegion</span>; defaults to <span
                 //!< class="code">\ref ClipSubRegion::Space "World"</span> space.
};

typedef std::vector<ClipSubRegion> ClipSubRegions; //!< A std::vector of ClipSubRegion

//! The VectorCulling class is used to define vector culling on an instance.
class VectorCulling {
public:
    enum Space : uint32_t {
        None,   //!< The vector is not specified.
        World,  //!< The vector is specified in world coordinates.
        Object, //!< The vector is specified in object coordinates.
    };

    VectorCulling()
    : vector(Vector(0.0f, 0.0f, 0.0f))
    , tolerance(0.0f)
    , space(None)
    {}

    VectorCulling(Vector const& vector, float tolerance, Space space)
    : vector(vector)
    , tolerance(tolerance)
    , space(space)
    {}

    Vector vector;
    float  tolerance;
    Space  space;
};

/*!
 * The MemoryDevice class is an in-memory virtual file used for sandboxing file IO.
 * These objects can be obtained from a MemoryDevice object by calling
 * `MemoryDevice::OpenFileForReading`.
 */
class MemoryDeviceFile {
public:
    /*! Creates an invalid memory device file. */
    MemoryDeviceFile();
    ~MemoryDeviceFile();

    /*! Move construction. The source file will become invalid. */
    MemoryDeviceFile(MemoryDeviceFile&& source) = default;

    /*! Move assignment. The source file will become invalid. */
    MemoryDeviceFile& operator=(MemoryDeviceFile&& source) = default;

    /*! Determines if this is a valid file object.
     * @return true if the file is valid, or false if it not. A moved-from file object will be
     * invalid.
     */
    bool
    IsValid() const
    {
        return _impl != nullptr;
    }

    /*! Query the size of the file.
     * @return The size, in bytes, of the given file. */
    size_t Size() const;

    /*! Read bytes from the file
     * @return The number of bytes read. */
    size_t Read(uint8_t* buffer, size_t num_bytes);

    /*! Resets the file such that the next Read operation will start at the beginning. */
    void SeekToBeginning();

private:
    MemoryDeviceFile(std::unique_ptr<TC::IO::AbstractFile>);

    std::unique_ptr<TC::IO::AbstractFile> _impl;
    friend class MemoryDevice;
};

/*!
 * The MemoryDevice class is an in-memory virtual file device used to sandbox file IO.
 * These objects can be obtained by Database::GetMemoryDevice.
 */
class MemoryDevice {
public:
    /*!
     * Queries if a path exists within this device.
     * @returns True if the path exists and false otherwise.
     */
    bool Exists(char const* utf8_path);

    /*!
     * Queries if a path is a directory within this device.
     * @returns True if the path is a directory and false otherwise.
     */
    bool IsDirectory(char const* utf8_path);

    /*!
     * Creates all the directories in this device required for the input directory path to exist.
     * @returns True if the leaf directory was successfully created and false otherwise.
     */
    bool CreateDirectories(char const* utf8_directory_path);

    /*!
     * Removes a file or directory in this device. If it is a directory, all of its children are
     * also removed.
     * @returns True if the directory was successfully removed and false otherwise.
     */
    bool RemoveAll(char const* utf8_path);

    /*!
     * Opens the file specified by `path` for read operations. A valid MemoryDeviceFile
     * will be returned which allows reading the file data.
     *
     * If the path is not a valid file, an Exception will be thrown.
     *
     * @returns A valid file object if the given path is an actual file.
     */
    MemoryDeviceFile OpenFileForReading(char const* utf8_path);

    /*!
     * Transforms the given path into an absolute version of the same underlying path.
     * To find the current working directory for the MemoryDevice, call this function
     * with an empty string for the path.
     *
     * @returns An aboslute version of the given path.
     */
    char const* ToAbsolutePath(char const* utf8_path);

private:
    MemoryDevice()                    = default;
    MemoryDevice(MemoryDevice const&) = delete;
    MemoryDevice& operator=(const MemoryDevice&) = delete;
    MemoryDevice(MemoryDevice&&)                 = delete;
    MemoryDevice& operator=(MemoryDevice&&) = delete;

    MemoryDevice(std::unique_ptr<TC::IO::MemoryFilesystem> impl);
    std::unique_ptr<TC::IO::MemoryFilesystem> _impl;
    friend class Database;
    friend class ::TC::IO::MemoryFilesystem;
};

/*! <p>The interface into a model. This class allows the definition of meshes, colors, matrices,
instances, and other model data. It also provides means of modifying existing definitions. Querying
of its name, model key, default camera, keys and many other aspects. </p>

<p> A model is closed when its destructor or cache.Close(model) is called.</p>

<p> In general operations on invalid keys or otherwise invalid usage will throw exceptions.
    Using try catch blocks around your code is advised. <p>

<p> A model has a default View, but unless a self include is created nothing will be visible when it
is streamed. A model without a self include can be seen as an pure container for data, but unless it
is included in some other model it is not directly viewable. </p>

<p> Consider a model containing a single instance of a sphere.  If it was self included 4 times with
different matrices then 4 spheres would be visible when the model is streamed and viewed. If it was
included in a second model once, a single sphere would be visible in the second model.</p>

*/

class Model {
public:
    ~Model();
    Model(Model&& that);

    /*!
     * Insert a new definition into the model.
     * @param mesh The Mesh.
     * @return A MeshKey.
     */
    MeshKey Insert(Mesh const& mesh);

    /*!
     * Insert a new definition into the model.  This is a mesh selector with different mesh levels.
     * @param meshes The MeshKeys.  The index 0 is the highest representation and the Nth index is
     * the lowest representation.
     * @return A MeshKey.
     */
    MeshKey Insert(MeshKeys const& meshes);

    /*!
     * Insert a new definition into the model.
     * @param color The Color.
     * @return A MaterialKey.
     */
    MaterialKey Insert(Color const& color);

    /*!
     * Insert a new definition into the model.
     * @param material The Material.
     * @return A MaterialKey.
     */
    MaterialKey Insert(Material const& material);

    //! @cond HIDDEN
    /*!
     * Insert a new definition into the model.
     * @param pbr_material The PBRMaterial.
     * @return A MaterialKey.
     */
    MaterialKey Insert(PBRMaterial const& pbr_material);
    //! @endcond

    /*!
     * Insert a new definition into the model.
     * @param line_pattern The Line Pattern.
     * @return A LinePatternKey.
     */
    LinePatternKey Insert(LinePattern const& line_pattern);

    /*!
     * Insert a new definition into the model.
     * @param matrix The Matrix3d.
     * @return A MatrixKey.
     */
    MatrixKey Insert(Matrix3d const& matrix);

    /*!
     * Insert a new definition into the model.
     * @param matrix The DMatrix3d.
     * @return A MatrixKey.
     */
    MatrixKey Insert(DMatrix3d const& matrix);

    /*!
     * Insert a new definition into the model.
     * @param byte_count The byte count.
     * @param byte_data The image data of length byte_count.
     * @param format The ImageFormat of the byte_data.
     * @return An ImageKey.
     */
    ImageKey Insert(uint32_t byte_count, uint8_t const* byte_data, ImageFormat format);

    /*!
     * Insert a new definition into the model.
     * @param width The image width.
     * @param width The image height.
     * @param data The RGBA32 image data of length width * height.
     * @return An ImageKey.
     */
    ImageKey Insert(uint32_t width, uint32_t height, RGBA32 const* data);

    /*!
     * Insert a new definition into the model.
     * @param width The image width.
     * @param width The image height.
     * @param data The RGB24 image data of length width * height.
     * @return An ImageKey.
     */
    ImageKey Insert(uint32_t width, uint32_t height, RGB24 const* data);

    /*!
     * Insert a new definition into the model.
     * @param width The image width.
     * @param width The image height.
     * @param data The Gray8 image data of length width * height.
     * @return An ImageKey.
     */
    ImageKey Insert(uint32_t width, uint32_t height, Gray8 const* data);

    /*!
     * Insert a new definition into the model.
     * @param width The image width.
     * @param width The image height.
     * @param data The GrayAlpha16 image data of length width * height.
     * @return An ImageKey.
     */
    ImageKey Insert(uint32_t width, uint32_t height, GrayAlpha16 const* data);

    /*!
     * Insert a new definition into the model.
     * @param byte_count The byte count.
     * @param byte_data The bytes of length byte_count.
     * @param priority The priority. Controls streaming order: 0 not sent by default - ~0 sent by
     * request only and excluded from .scs; otherwise, highest first.
     * @return A DataKey.
     */
    DataKey Insert(uint32_t byte_count, uint8_t const* byte_data, uint32_t priority = 0);

    /*!
     * Insert a new definition into the model.
     * @param instance_incs The InstanceIncs to insert.
     * @param group_incs The GroupIncs to insert.
     * @return A GroupKey.
     */
    GroupKey Insert(InstanceIncs const& instance_incs, GroupIncs const& group_incs);

    /*!
     * Insert a new definition into the model.
     * @param material_keys The MaterialKeys to insert.  These define a fully enumerated material
     * map.
     * @return A MaterialMapKey.
     */
    MaterialMapKey Insert(MaterialKeys const& material_keys); // fully enumerated material map

    /*!
     * Insert a new definition into the model.
     * @param material_map_elements The MaterialMapElements to insert.  These define a sparse
     * material map.
     * @return A MaterialMapKey.
     */
    MaterialMapKey Insert(MaterialMapElements const& material_map_elements);

    /*!
     * Insert a new definition into the model.
     * @param loop_count The loop count.
     * @param loop_counts The counts for loop_count loops.
     * @param points The points.
     * @return A PolyPolygonKey.
     */
    PolyPolygonKey Insert(uint32_t loop_count, uint32_t const* loop_counts, Point const* points);

    /*!
     * Insert a new definition into the model.
     * @param key clip_subregions ClipSubRegions to insert.
     * @return A ClipRegionKey.
     */
    ClipRegionKey Insert(ClipSubRegions const& clip_subregions);

    /*!
     * Look up matching definition or insert if not found.
     * Only definitions created using FindOrInsert are matchable.
     * Helps create a more compact model if unique keying is not needed.
     * @param color The Color to match or insert.
     * @return A MaterialKey.
     */
    MaterialKey FindOrInsert(Color const& color);

    /*!
     * Look up matching definition or insert if not found.
     * Only definitions created using FindOrInsert are matchable.
     * Helps create a more compact model if unique keying is not needed.
     * @param material The Material to match or insert.
     * @return A MaterialKey.
     */
    MaterialKey FindOrInsert(Material const& material);

    //! @cond HIDDEN
    /*!
     * Look up matching definition or insert if not found.
     * Only definitions created using FindOrInsert are matchable.
     * Helps create a more compact model if unique keying is not needed.
     * @param pbr_material The PBRMaterial to match or insert.
     * @return A MaterialKey.
     */
    MaterialKey FindOrInsert(PBRMaterial const& pbr_material);
    //! @endcond

    /*!
     * Look up matching definition or insert if not found.
     * Only definitions created using FindOrInsert are matchable.
     * Helps create a more compact model if unique keying is not needed.
     * @param matrix The Matrix3d to match or insert.
     * @return A MatrixKey.
     */
    MatrixKey FindOrInsert(Matrix3d const& matrix);

    /*!
     * Look up matching definition or insert if not found.
     * Only definitions created using FindOrInsert are matchable.
     * Helps create a more compact model if unique keying is not needed.
     * @param matrix The DMatrix3d to match or insert.
     * @return A MatrixKey.
     */
    MatrixKey FindOrInsert(DMatrix3d const& matrix);

    /*!
     * Look up matching definition or insert if not found.
     * Only definitions created using FindOrInsert are matchable.
     * Helps create a more compact model if unique keying is not needed.
     * @param material_keys The MaterialKeys to match or insert.  These define a fully enumerated
     * material map.
     * @return A MaterialMapKey.
     */
    MaterialMapKey FindOrInsert(MaterialKeys const& material_keys);

    /*!
     * Look up matching definition or insert if not found.
     * Only definitions created using FindOrInsert are matchable.
     * Helps create a more compact model if unique keying is not needed.
     * @param material_map_elements The MaterialMapElements to match or insert.  These define a
     * sparse material map.
     * @return A MaterialMapKey.
     */
    MaterialMapKey FindOrInsert(MaterialMapElements const& material_map_elements);

    /*!
     * Look up matching definition or insert if not found.
     * Only definitions created using FindOrInsert are matchable.
     * Helps create a more compact model if unique keying is not needed.
     * @param loop_count The loop count.
     * @param loop_counts The counts for loop_count loops.
     * @param points The points.
     * @return A PolyPolygonKey.
     */
    PolyPolygonKey
    FindOrInsert(uint32_t loop_count, uint32_t const* loop_counts, Point const* points);

    /*!
     * Look up matching definition or insert if not found.
     * Only definitions created using FindOrInsert are matchable.
     * Helps create a more compact model if unique keying is not needed.
     * @param key clip_subregions ClipSubRegions to match or insert.
     * @return A ClipRegionKey.
     */
    ClipRegionKey FindOrInsert(ClipSubRegions const& clip_subregions);

    /*!
     * Replace the underlying definition for key.
     * @param key The MeshKey to modify.
     * @param meshes The replacement Mesh.
     */
    void Set(MeshKey key, Mesh const& mesh);

    /*!
     * Replace the underlying definition for key.
     * @param key The MeshKey to modify.
     * @param meshes The replacement MeshKeys.  The index 0 is the highest representation and the
     * Nth index is the lowest representation.
     */
    void Set(MeshKey key, MeshKeys const& meshes);

    /*!
     * Replace the underlying definition for key.
     * @param key The MaterialKey to modify.
     * @param color The replacement Color.
     */
    void Set(MaterialKey key, Color const& color);

    /*!
     * Replace the underlying definition for key.
     * @param key The MaterialKey to modify.
     * @param material The replacement Material.
     */
    void Set(MaterialKey key, Material const& material);

    //! @cond HIDDEN
    /*!
     * Replace the underlying definition for key.
     * @param key The MaterialKey to modify.
     * @param pbr_material The replacement PBRMaterial.
     */
    void Set(MaterialKey key, PBRMaterial const& pbr_material);
    //! @endcond

    /*!
     * Replace the underlying definition for key.
     * @param key The MatrixKey to modify.
     * @param matrix The replacement Matrix3d.
     */
    void Set(MatrixKey key, Matrix3d const& matrix);

    /*!
     * Replace the underlying definition for key.
     * @param key The MatrixKey to modify.
     * @param matrix The replacement DMatrix3d.
     */
    void Set(MatrixKey key, DMatrix3d const& matrix);

    /*!
     * Replace the underlying definition for key.
     * @param key The ImageKey to modify.
     * @param byte_count The replacement byte count.
     * @param byte_data The replacement image data of length byte_count.
     * @param format The replacement ImageFormat of the byte_data.
     */
    void Set(ImageKey key, uint32_t byte_count, uint8_t const* byte_data, ImageFormat format);

    /*! Replace the underlying definition for key.
     * @param key The ImageKey to modify.
     * @param width The replacement image width.
     * @param width The replacement image height.
     * @param data The replacement RGBA32 image data of length width * height.
     */
    void Set(ImageKey key, uint32_t width, uint32_t height, RGBA32 const* data);

    /*!
     * Replace the underlying definition for key.
     * @param key The ImageKey to modify.
     * @param width The replacement image width.
     * @param width The replacement image height.
     * @param data The replacement RGB24 image data of length width * height.
     */
    void Set(ImageKey key, uint32_t width, uint32_t height, RGB24 const* data);

    /*!
     * Replace the underlying definition for key.
     * @param key The ImageKey to modify.
     * @param width The replacement image width.
     * @param width The replacement image height.
     * @param data The replacement Gray8 image data of length width * height.
     */
    void Set(ImageKey key, uint32_t width, uint32_t height, Gray8 const* data);

    /*!
     * Replace the underlying definition for key.
     * @param key The ImageKey to modify.
     * @param width The replacement image width.
     * @param width The replacement image height.
     * @param data The replacement GrayAlpha16 image data of length width * height.
     */
    void Set(ImageKey key, uint32_t width, uint32_t height, GrayAlpha16 const* data);

    /*!
     * Replace the underlying definition for key.  The original priority is retained.
     * @param key The DataKey to modify.
     * @param byte_count The replacement byte count.
     * @param byte_data The replacement bytes of length byte_count.
     */
    void Set(DataKey key, uint32_t byte_count, uint8_t const* byte_data);

    /*!
     * Replace the underlying definition for key.
     * @param key The DataKey to modify.
     * @param byte_count The replacement byte count.
     * @param byte_data The replacement bytes of length byte_count.
     * @param priority The replacement priority. Controls streaming order: 0 not sent by default -
     * ~0 sent by request only and excluded from .scs; otherwise, highest first.
     */
    void Set(DataKey key, uint32_t byte_count, uint8_t const* byte_data, uint32_t priority);

    /*!
     * Replace the underlying definition for key.
     * @param key The GroupKey to modify.
     * @param instance_incs The replacement InstanceIncs.
     * @param group_incs The replacement GroupIncs.
     */
    void Set(GroupKey key, InstanceIncs const& instance_incs, GroupIncs const& group_incs);

    /*!
     * Replace the underlying definition for key.
     * @param key The MaterialMapKey to modify.
     * @param material_keys The replacement MaterialKeys.  These define a fully enumerated material
     * map.
     */
    void Set(MaterialMapKey key, MaterialKeys const& material_keys);

    /*!
     * Replace the underlying definition for key.
     * @param key The MaterialMapKey to modify.
     * @param material_map_elements The replacement MaterialMapElements.  These define a sparse
     * material map.
     */
    void Set(MaterialMapKey key, MaterialMapElements const& material_map_elements);

    /*!
     * Replace the underlying definition for key.
     * @param key The PolyPolygonKey to modify.
     * @param loop_count The replacement loop count.
     * @param loop_counts The replacement counts for loop_count loops.
     * @param points The replacement points.
     */
    void
    Set(PolyPolygonKey key, uint32_t loop_count, uint32_t const* loop_counts, Point const* points);

    /*!
     * Replace the underlying definition for key.
     * @param key The ClipRegionKey to modify.
     * @param clip_subregions The replacement ClipSubRegions.
     */
    void Set(ClipRegionKey key, ClipSubRegions const& clip_subregions);

    /*!
     * Creates an Instance of a mesh.
     * @param mesh The MeshKey to instance.
     * @param matrix The transform matrix for the instance. The default constructed MatrixKey is an
     * identity matrix.
     * @param face_material The MaterialKey for faces.  If MaterialKey::Unspecified() is used then
     * the mesh's setting will be used.
     * @param line_material The MaterialKey for lines.  If MaterialKey::Unspecified() is used then
     * the mesh's setting will be used.
     * @param point_material The MaterialKey for points.  If MaterialKey::Unspecified() is used then
     * the mesh's setting will be used.
     * @param face_material_map The MaterialMapKey for faces.  If the default constructed
     * MaterialMapKey is used the mesh's setting will be used.
     * @param line_material_map The MaterialMapKey for lines.  If the default constructed
     * MaterialMapKey is used the mesh's setting will be used.
     * @param point_material_map The MaterialMapKey for points.  If the default constructed
     * MaterialMapKey is used the mesh's setting will be used.
     * @param visibility The Visibility.  The default constructed Visibility is all on.
     * @param modifiers The Modifiers.  The default constructed Modifiers is none.
     * @param clip_region The ClipRegionKey.  The default constructed ClipRegionKey is none.
     * @param vector_culling The VectorCulling setting.  The default constructed VectorCulling is
     * none.
     * @return An InstanceKey.
     */
    InstanceKey Instance(
        MeshKey              mesh,
        MatrixKey            matrix             = MatrixKey(),
        MaterialKey          face_material      = MaterialKey::Unspecified(),
        MaterialKey          line_material      = MaterialKey::Unspecified(),
        MaterialKey          point_material     = MaterialKey::Unspecified(),
        MaterialMapKey       face_material_map  = MaterialMapKey(),
        MaterialMapKey       line_material_map  = MaterialMapKey(),
        MaterialMapKey       point_material_map = MaterialMapKey(),
        Visibility           visibility         = Visibility(),
        Modifiers            modifiers          = Modifiers(),
        ClipRegionKey        clip_region        = ClipRegionKey(),
        VectorCulling const& vector_culling     = VectorCulling());

    /*!
     * Include a model in this model's views.  May be a self inclusion.
     * Inclusions present in the included model are not included, just the included model's data.
     * @param model The Model to include.
     * @param matrix The transform matrix for the inclusion the default constructed MatrixKey is an
     * identity matrix.
     * @return An InclusionKey.
     */
    InclusionKey Include(Model const& model, MatrixKey matrix = MatrixKey());

    /*!
     * Replace the mesh for an Instance.
     * @param key The InstanceKey to modify.
     * @param mesh The replacement MeshKey.
     */
    void Set(InstanceKey key, MeshKey mesh);

    /*!
     * Replace the matrix for an Instance.
     * @param key The InstanceKey to modify.
     * @param matrix The replacement MatrixKey.
     */
    void Set(InstanceKey key, MatrixKey matrix);

    /*!
     * Replace the vector culling settings for an Instance.
     * @param key The InstanceKey to modify.
     * @param vector_culling The replacement VectorCulling setting.
     */
    void Set(InstanceKey key, VectorCulling const& vector_culling);

    /*!
     * Replace the materials for an Instance.
     * @param key The InstanceKey to modify.
     * @param face_material The MaterialKey for faces.  If MaterialKey::Unspecified() is used then
     * the mesh's setting will be used.
     * @param line_material The MaterialKey for lines.  If MaterialKey::Unspecified() is used then
     * the mesh's setting will be used.
     * @param point_material The MaterialKey for points.  If MaterialKey::Unspecified() is used then
     * the mesh's setting will be used.
     */
    void
    Set(InstanceKey key,
        MaterialKey face_material,
        MaterialKey line_material,
        MaterialKey point_material);

    /*!
     * Replace the material maps for an Instance.
     * @param key The InstanceKey to modify.
     * @param face_material_map The MaterialMapKey for faces.  If the default constructed
     * MaterialMapKey is used the mesh's setting will be used.
     * @param line_material_map The MaterialMapKey for lines.  If the default constructed
     * MaterialMapKey is used the mesh's setting will be used.
     * @param point_material_map The MaterialMapKey for points.  If the default constructed
     * MaterialMapKey is used the mesh's setting will be used.
     */
    void
    Set(InstanceKey    key,
        MaterialMapKey face_material_map,
        MaterialMapKey line_material_map,
        MaterialMapKey point_material_map);

    /*!
     * Replace the visibility for an Instance.
     * @param key The InstanceKey to modify.
     * @param visibility The replacement Visibility.
     */
    void Set(InstanceKey key, Visibility visibility);

    /*!
     * Replace the modifiers for an Instance.
     * @param key The InstanceKey to modify.
     * @param modifiers The replacement Modifiers.
     */
    void Set(InstanceKey key, Modifiers modifiers);

    /*!
     * Replace the clip region for an Instance.
     * @param key The InstanceKey to modify.
     * @param clip_region The replacement ClipRegionKey.
     */
    void Set(InstanceKey key, ClipRegionKey clip_region);

    /*!
     * Override the materials for a specific InstanceInc.
     * @param inc The InstanceInc to override.
     * @param face_material The MaterialKey for faces.  If MaterialKey::Unspecified() is used then
     * the mesh's setting will be used.
     * @param line_material The MaterialKey for lines.  If MaterialKey::Unspecified() is used then
     * the mesh's setting will be used.
     * @param point_material The MaterialKey for points.  If MaterialKey::Unspecified() is used then
     * the mesh's setting will be used.
     */
    void
    Set(InstanceInc const& inc,
        MaterialKey        face_material,
        MaterialKey        line_material,
        MaterialKey        point_material);

    /*!
     * Override the material maps for a specific InstanceInc.
     * @param inc The InstanceInc to override.
     * @param face_material_map The MaterialMapKey for faces.  If the default constructed
     * MaterialMapKey is used the mesh's setting will be used.
     * @param line_material_map The MaterialMapKey for lines.  If the default constructed
     * MaterialMapKey is used the mesh's setting will be used.
     * @param point_material_map The MaterialMapKey for points.  If the default constructed
     * MaterialMapKey is used the mesh's setting will be used.
     */
    void
    Set(InstanceInc const& inc,
        MaterialMapKey     face_material_map,
        MaterialMapKey     line_material_map,
        MaterialMapKey     point_material_map);

    /*!
     * Override the visibility for a specific InstanceInc.
     * @param inc The InstanceInc to override.
     * @param visibility The override Visibility.
     */
    void Set(InstanceInc const& inc, Visibility visibility);

    /*!
     * Add modifiers for a specific InstanceInc.
     * @param inc The InstanceInc to add modifiers to.
     * @param modifiers_to_add The additional Modifiers.
     */
    void Set(InstanceInc const& inc, Modifiers modifiers_to_add);

    /*!
     * Override clip region for a specific InstanceInc.
     * @param inc The InstanceInc to override.
     * @param clip_region The override ClipRegionKey.
     */
    void Set(InstanceInc const& inc, ClipRegionKey clip_region);

    /*!
     * Remove an existing Inclusion from a model.
     * @param inclusion_key The InclusionKey to remove.
     */
    void Delete(InclusionKey inclusion_key);

    /*!
     * Remove an existing Instance from a model.
     * @param instance_key The InstanceKey to remove.
     */
    void Delete(InstanceKey instance_key);

    /*!
     * Remove an existing View from a model.
     * @param view_key The ViewKey to remove.
     */
    void Delete(ViewKey view_key);

    /*!
     * Query the current ModelKey.  ModelKeys are not stable values from one open model to another.
     * @return A ModelKey.
     */
    ModelKey GetKey() const;

    /*!
     * Query the name used by a model.
     * The name is not sufficient to uniquely identify a model; use a path instead.
     * Returned pointer is only valid until the next call.
     * @return A pointer to a 'C' string with the UTF8 model name.
     */
    const char* GetName() const;

    /*!
     * Query the file system path used by a model.  Returned pointer is only valid until the next
     * call.
     * @return A pointer to a 'C' string with the UTF8 file system path.
     */
    const char* GetPath() const;

    /*!
     * Query the InstanceIncs defined in a model.
     * @param instance_incs A write back vector to return defined InstanceIncs.
     */
    void GetInstanceIncs(InstanceIncs& instance_incs) const;

    /*!
     * Query the MeshIds defined in a model.
     * @param mesh_ids A write back vector to return defined MeshIds.
     */
    void GetMeshIds(MeshIds& mesh_ids) const;

    /*!
     * Query the MaterialIds defined in a model.  Ids are not stable values from one open model to
     * another.
     * @param material_ids A write back vector to return defined MaterialIds.
     */
    void GetMaterialIds(MaterialIds& material_ids) const;

    /*!
     * Query the MatrixIds defined in a model.  Ids are not stable values from one open model to
     * another.
     * @param matrix_ids A write back vector to return defined MatrixIds.
     */
    void GetMatrixIds(MatrixIds& matrix_ids) const;

    /*!
     * Query the InstanceIds defined in a model.  Ids are not stable values from one open model to
     * another.
     * @param instance_ids A write back vector to return defined InstanceIds.
     */
    void GetInstanceIds(InstanceIds& instance_ids) const;

    /*!
     * Query the ImageIds defined in a model.  Ids are not stable values from one open model to
     * another.
     * @param image_ids A write back vector to return defined ImageIds.
     */
    void GetImageIds(ImageIds& image_ids) const;

    /*!
     * Query the DataIds defined in a model.
     * @param data_ids A write back vector to return defined DataIds.
     */
    void GetDataIds(DataIds& data_ids) const;

    /*!
     * Query the GroupIds defined in a model.  Ids are not stable values from one open model to
     * another.
     * @param group_ids A write back vector to return defined GroupIds.
     */
    void GetGroupIds(GroupIds& group_ids) const;

    /*!
     * Query the MaterialMapIds defined in a model.  Ids are not stable values from one open model
     * to another.
     * @param material_map_ids A write back vector to return defined MaterialMapIds.
     */
    void GetMaterialMapIds(MaterialMapIds& material_map_ids) const;

    /*!
     * Query the PolyPolygonIds defined in a model.  Ids are not stable values from one open model
     * to another.
     * @param polypolygon_ids A write back vector to return defined PolyPolygonIds.
     */
    void GetPolyPolygonIds(PolyPolygonIds& polypolygon_ids) const;

    /*!
     * Query the ClipRegionIds defined in a model.  Ids are not stable values from one open model to
     * another.
     * @param clip_region_ids A write back vector to return defined ClipRegionIds.
     */
    void GetClipRegionIds(ClipRegionIds& clip_region_ids) const;

    /*!
     * Query the MeshKeys defined in a model.
     * @param mesh_keys A write back vector to return defined MeshKeys.
     */
    void GetMeshKeys(MeshKeys& mesh_keys) const;

    /*!
     * Query the MaterialKeys defined in a model.
     * @param material_keys A write back vector to return defined MaterialKeys.
     */
    void GetMaterialKeys(MaterialKeys& material_keys) const;

    /*!
     * Query the MatrixKeys defined in a model.
     * @param matrix_keys A write back vector to return defined MatrixKeys.
     */
    void GetMatrixKeys(MatrixKeys& matrix_keys) const;

    /*!
     * Query the InstanceKeys defined in a model.
     * @param instance_keys A write back vector to return defined InstanceKeys.
     */
    void GetInstanceKeys(InstanceKeys& instance_keys) const;

    /*!
     * Query the ImageKeys defined in a model.
     * @param image_keys A write back vector to return defined ImageKeys.
     */
    void GetImageKeys(ImageKeys& image_keys) const;

    /*!
     * Query the DataKeys defined in a model.
     * @param data_keys A write back vector to return defined DataKeys.
     */
    void GetDataKeys(DataKeys& data_keys) const;

    /*!
     * Query the GroupKeys defined in a model.
     * @param group_keys A write back vector to return defined GroupKeys.
     */
    void GetGroupKeys(GroupKeys& group_keys) const;

    /*!
     * Query the MaterialMapKeys defined in a model.
     * @param material_map_keys A write back vector to return defined MaterialMapKeys.
     */
    void GetMaterialMapKeys(MaterialMapKeys& material_map_keys) const;

    /*!
     * Query the PolyPolygonKeys defined in a model.
     * @param polypolygon_keys A write back vector to return defined PolyPolygonKeys.
     */
    void GetPolyPolygonKeys(PolyPolygonKeys& polypolygon_keys) const;

    /*!
     * Query the ClipRegionKeys defined in a model.
     * @param clip_region_keys A write back vector to return defined ClipRegionKeys.
     */
    void GetClipRegionKeys(ClipRegionKeys& clip_region_keys) const;

    /*!
     * Query the data associated with a DataKey.
     * @param key The data to read
     * @param byte_data The write back vector to return the associated data.
     */
    void Get(DataKey key, std::vector<uint8_t>& byte_data) const;

    /*!
     * Set a views camera.
     * @param camera The camera for the view.
     * @param view The view to set the properties on.  If unspecified the default view is used.
     */
    void Set(Camera const& camera, ViewKey view = ViewKey());

    /*!
     * Query a views camera.
     * @param camera A write back value to return the Camera.  Only valid if true is returned.
     * @param view The view to query the properties on.  If unspecified the default view is used.
     * @return True if Camera has been set.
     */
    bool Get(Camera& camera, ViewKey view = ViewKey());

    /*!
     * Set a views default material properties - these apply to all materials that do not have their
     * own properties.
     * @param material_properties The default properties for the view.
     * @param view The view to set the properties on.  If unspecified the default view is used.
     */
    void Set(MaterialProperties const& material_properties, ViewKey view = ViewKey());

    /*!
     * Query a views default material properties.
     * @param material_properties A write back value to return the MaterialProperties.  Only valid
     * if true is returned.
     * @param view The view to query the properties on.  If unspecified the default view is used.
     * @return True if MaterialProperties have been set.
     */
    bool Get(MaterialProperties& material_properties, ViewKey view = ViewKey());

    /*!
     * Set a new initial View.  Only needed if more than one View exists.
     *	@param view The initial ViewKey.
     */
    void Set(ViewKey view);

    /*!
     * Query an optional initial View.
     * *@param view A write back value containing the initial ViewKey.  Only valid if true is
     * returned.
     * *@return True if an optional View has been set.
     */
    bool Get(ViewKey& view);

    /*!
     * Create a new View.
     * @return A new ViewKey.
     */
    ViewKey View();

    /*!
     * Prepares this model for streaming.  This should be done last when all data has been authored.
     * *@param compression_strategy The strategy use to compress large meshes and images. Affects
     * speed and output size
     */
    void PrepareStream(CompressionStrategy compression_strategy = CompressionStrategy::Default);

    /*!
     * Compute the bounding for this model.  This requires PrepareStream() has been called.
     * If the stream has not been prepared an exception may be thrown.
     */
    void ComputeBounding(Point& min, Point& max);

    /*!
     * Returns the measurement unit of the root node in the model if any. Returns 0 if no
     * measurement unit is set.
     * @return The measurement unit of the root node.
     */
    double GetRootMeasurementUnit();

    /*!
     * Generates an .scs file.  Returned pointer is only valid until the next call.
     * @param utf8_scs_file_path Specifies the full path and file name of the .scs file to generate
     *include the .scs extension. If the utf8_scs_file_path is nullptr or empty a .scs file will be
     *generated in the model directory with a default name.
     * @return A pointer to a 'C' string UTF8 file system path of the generated .scs file.
     */
    const char* GenerateSCSFile(const char* utf8_scs_file_path = nullptr);

    /*!
     * Generates an .scz file.  Returned pointer is only valid until the next call.  Using
     *"include_derived = false" is not compatible with "include_editable = false" the resulting file
     *is unusable and the call will throw.  A bundled .scz can be streamed without decompression but
     *will create a larger file and may only be opened read-only.  A file created with
     *"include_editable = false" can only be opened read-only.
     * @param utf8_scz_file_path Specifies the full path and file name of the .scz file to generate
     *include the .scz extension. If the utf8_scz_file_path is nullptr or empty a .scz file will be
     *generated in the model directory with a default name.
     * @param include_derived If true the .scz file will include the files needed from streaming.
     * @param include_editable If true the .scz file will include the files needed for future
     *editing.
     * @param create_bundled If true the .scz file will be bundled rather than compressed.
     * @return A pointer to a 'C' string UTF8 file system path of the generated .scz file.
     */
    const char* GenerateSCZFile(
        const char* utf8_scz_file_path = nullptr,
        bool        include_derived    = true,
        bool        include_editable   = true,
        bool        create_bundled     = false);

private:
    friend class TC::Store::CacheImpl;
    friend class TC::Store::ModelImpl;

    Model();
    Model(Model const& that) = delete;
    Model operator=(Model const& that) = delete;

    void Init(void* opaque_abstract_device);

    std::shared_ptr<TC::Store::ModelImpl> _impl;
};

/*! <p>This class is used to create and open existing models. Creation is done by simply
    opening a non-existent model. The cache can query known model names and whether or
    not a model exists in the system.</p>

    <p>A cache is closed when its destructor is called or Database::Close(cache) is called.</p>

    <p> In general invalid usage will throw exceptions.  Using try catch blocks around your code is
   advised. </p>
*/
class Cache {

public:
    ~Cache();
    Cache(Cache&& that);

    /*!
     * Opens a Model object.  New Models can only be created writable.
     * @param utf8_model_path An absolute or partial path of a model directory.
     * @param read_only If true the Model will be read only.
     * @return A Model object.
     */
    Model Open(const char* utf8_model_path, bool read_only = false);

    /*!
     * Closes a Model object. The model object will be invalid after this call.  This has the same
     * effect as deleting the model object.
     * @param model The Model to close.
     */
    void Close(Model& model);

    /*!
     * Sets the paths that this Cache will use to search for models provided with a partial path.
     * The provided path string will be parsed and broken on ';'. The '*' character is not allowed.
     * Existing search path will be reset. An example path: "e:/sc_test;e:/sc_data".
     * @param utf8_search_path A path to search for models.
     */
    void SetSearchPath(const char* utf8_search_path);

    /*!
     * Appends more paths that this Cache will use to search for models provided with a partial
     * path. The provided path string will be parsed and broken on ';'. The '*' character is not
     * allowed. An example path: "e:/sc_test;e:/sc_data".
     * @param utf8_search_path A path to search for models.
     */
    void AppendSearchPath(const char* utf8_search_path);

    /*!
     * Sets a path that this Cache will use as a temporary work space.
     * This is a good idea if .scz files are in use.
     * If the Cache exits normally the workspace will be left clean.
     * If this is not set then the directory containing a .scz file will be used and must be
     * writable.
     * @param utf8_directory_path A path to use for a temporary work space.
     */
    void SetWorkspacePath(const char* utf8_directory_path);

    /*!
     * Query if the specified model is a model known to the Cache.  The returned pointer is only
     * valid until the next call. If the utf8_model_path is not an absolute path the search path
     * will be scanned. The utf8_model_path will not be expanded by the current working directory.
     * @param utf8_model_path The path or partial path of the model to query.
     * @return The 'C' string UTF8 file system path if found or nullptr if not found.
     */
    const char* Exists(const char* utf8_model_path);

    /*!
     * Query the models known to the Cache.  This will scan the search path for valid models.
     * The returned pointer is only valid until the next call.
     * @param out_count A write back value containing the number of models found.
     * @return An array of count length of 'C' string UTF8 file system paths of the known models.
     */
    const char* const* GetModelPaths(size_t& out_count);

    /*!
     * Remove any temporary files created by the cache. This includes decompressed files and
     * directories created when opening a compressed SCZ file.
     */
    void RemoveTemporaryFiles();

private:
    friend class Database;

    Cache()                  = delete;
    Cache(Cache const& that) = delete;
    Cache operator=(Cache const& that) = delete;

    Cache(Logger const& logger, void* opaque_abstract_device);

    std::unique_ptr<TC::Store::CacheImpl> _impl;
};

/*! <p> This class is used to create a Cache object or decompress a .scz file. </p>
    <p> In general invalid usage will throw exceptions.  Using try catch blocks around your code is
   advised. </p>
*/
class Database {
public:
    /*!
     * Sets the license.  A valid license must be provided or an exception will be thrown.
     * @param encoded_license A valid encoded license string.
     */
    static void SetLicense(const char* encoded_license);

    /*!
     * Opens a Cache object.
     * @param logger A Logger object for status and error reporting.
     * @param memory_device An optional MemoryDevice used to sandbox IO.
     */
    static Cache Open(Logger const& logger, MemoryDevice* memory_device = nullptr);

    /*!
     * Closes a Cache object.  The Cache object will be invalid after this call.  This has the same
     * effect as deleting the cache object.
     * @param cache A Cache object.
     */
    static void Close(Cache& cache);

    /*!
     * Query the version of the SC::Store
     * @return The SC::Store version.
     */
    static uint32_t Version();

    /*!
     * Utility to decompress a .scz file.
     * @param utf8_path_to_scz_file An absolute path to a .scz file to decompress.
     * @param utf8_output_model_path An absolute path to output the uncompressed model.
     * @param logger A Logger object for status and error reporting.
     * @param memory_device An optional MemoryDevice used to sandbox IO.
     * @return True if the file decompressed.
     */
    static bool DecompressSCZ(
        char const*   utf8_path_to_scz_file,
        char const*   utf8_output_model_path,
        Logger const& logger,
        MemoryDevice* memory_device = nullptr);

    /*!
     * Utility to query information about a .scz file. Returned pointer is only valid until the next
     * call. The returned string will contain "Compressed" if it is a compressed .scz or "Invalid"
     * if the file is not a scz. A bundled .scz will contain "Bundled" with "Editable" and
     * "Streamable" depending on it's contents.
     * @param utf8_path_to_scz_file An absolute path to a .scz file to query.
     * @param logger A Logger object for status and error reporting.
     * @param memory_device An optional MemoryDevice used to sandbox IO.
     * @return A pointer to a 'C' string with informations about the .scz file.
     */
    static char const* QuerySCZ(
        char const*   utf8_path_to_scz_file,
        Logger const& logger,
        MemoryDevice* memory_device = nullptr);

    /*!
     * Creates an in-memory virtual file device used to sandbox file IO.
     *
     * If a device is successfully created (this function returns true),
     * `DestroyMemoryDevice` must later be called to free its memory.
     *
     * @param memory_device_name The name of the memory device to create.
     * @return True if the device is newly created or false if it already existed.
     */
    static bool CreateMemoryDevice(char const* memory_device_name);

    /*!
     * Destroys an in-memory virtual file device created by `CreateMemoryDevice`.
     * @param memory_device_name The name of the memory device to destroy.
     * @return True if the device existed or false if no such device existed to prior to this call.
     */
    static bool DestroyMemoryDevice(char const* memory_device_name);

    /*!
     * Returns the memory device created for the input name.
     * If no device exists for that name then null is returned.
     *
     * Pointers returned by this function become dangling after any
     * call to `DestroyMemoryDevice` for the given input name. A
     * dangling pointer must never be used again. (It should not be
     * dereferenced or passed to any of our APIs.) Doing otherwise
     * has the same risks of using any dangling pointer in C++.
     *
     * @return The memory device if it exists or null if it does not.
     */
    static MemoryDevice* GetMemoryDevice(char const* memory_device_name);

    /*!
     * Returns the names of existing memory devices.
     * The returned name pointers are only valid until after
     * `CreateMemoryDevice` or `DestroyMemoryDevice` are called.
     *
     * @param memory_device_name_buffer The buffer to be populated with memory device names.
     * @param capacity The max capacity of the input buffer.
     * @return The number of devices in existence.
     */
    static size_t GetMemoryDeviceNames(char const** memory_device_name_buffer, size_t capacity);

private:
    Database() = delete;
};

}} // namespace SC::Store
