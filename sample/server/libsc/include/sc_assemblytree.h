#pragma once

#include "sc_store.h"
#include <map>
#include <string>

namespace SC { namespace Store {

/*! NodeId type */
using NodeId = uint32_t;

/*! LayerId type */
using LayerId = uint32_t;

/*! GenericTypeKey type */
using GenericTypeKey = uint32_t;

/*! UserDataIndex type */
using UserDataIndex = uint64_t; // Note: This is unsigned because deserialization in JS becomes
                                // complicated otherwise (despite it being signed in HPS).

using FileType = uint32_t;

/*! Body type */
enum BodyType : uint8_t {
    BodyTypeNotSet = 0, //!< Not defined
    BRep           = 1, //!< Body comes from a brep
    Tessellation   = 2, //!< Body comes from a tessellated model
    Wireframe      = 3, //!< Body comes from a wireframed model
    PointCloud     = 4, //!< Body comes from a point cloud
};

/*! PMI type */
enum PMIType : uint32_t {
    TypeUnknown          = 0,  //!< Unknown value
    Text                 = 1,  //!< Plain text
    Dimension            = 2,  //!< Dimension
    Arrow                = 3,  //!< Arrow
    Balloon              = 4,  //!< Balloon
    CircleCenter         = 5,  //!< Center of circle
    Coordinate           = 6,  //!< Coordinate
    Datum                = 7,  //!< Datum
    Fastener             = 8,  //!< Fastener
    Gdt                  = 9,  //!< GD&T
    Locator              = 10, //!< Locator
    MeasurementPoint     = 11, //!< Point
    Roughness            = 12, //!< Roughness
    Welding              = 13, //!< Welding
    Table                = 14, //!< Table
    Other                = 15, //!< Other
    GeometricalTolerance = 16, //!< Geometrical tolerance
};

/*! PMI subtype */
enum PMISubType : uint32_t {
    SubTypeUnknown = 0, // Unknown value

    DatumIdent  = 1, //!< Datum subtype
    DatumTarget = 2, //!< Datum subtype

    DimensionDistance          = 1,  //!< Dimension distance subtype
    DimensionDistanceOffset    = 2,  //!< Dimension distance subtype
    DimensionDistanceCumulate  = 3,  //!< Dimension distance subtype
    DimensionChamfer           = 4,  //!< Dimension chamfer subtype
    DimensionSlope             = 5,  //!< Dimension slope subtype
    DimensionOrdinate          = 6,  //!< Dimension ordinate subtype
    DimensionRadius            = 7,  //!< Dimension radius subtype
    DimensionRadiusTangent     = 8,  //!< Dimension radius subtype
    DimensionRadiusCylinder    = 9,  //!< Dimension radius subtype
    DimensionRadiusEdge        = 10, //!< Dimension radius subtype
    DimensionDiameter          = 11, //!< Dimension diameter subtype
    DimensionDiameterTangent   = 12, //!< Dimension diameter subtype
    DimensionDiameterCylinder  = 13, //!< Dimension diameter subtype
    DimensionDiameterEdge      = 14, //!< Dimension diameter subtype
    DimensionDiameterCone      = 15, //!< Dimension diameter subtype
    DimensionLength            = 16, //!< Dimension length subtype
    DimensionLengthCurvilinear = 17, //!< Dimension length subtype
    DimensionLengthCircular    = 18, //!< Dimension length subtype
    DimensionAngle             = 19, //!< Dimension angle subtype

    GdtFcf = 1, //!< GD&T Feature control frame subtype

    WeldingLine = 1, //!< Line welding subtype
    WeldingSpot = 2, //!< Spot welding subtype

    OtherSymbolUser         = 1, //!< User symbol, other subtype
    OtherSymbolUtility      = 2, //!< Utility symbol, other subtype
    OtherSymbolCustom       = 3, //!< Custom symbol, other subtype
    OtherGeometricReference = 4, //!< Geometric reference, other subtype
    OtherRegion             = 5, //!< Region symbol, other subtype
};

/*! PMI reference to topology (brep) */
enum PMITopoRef : uint8_t {
    Context        = 1,  //!< Self-containing set of topological entities
    Item           = 2,  //!< Abstract root type for any topological entity (body or single item)
    MultipleVertex = 3,  //!< Vertex whose position is the average of all edges' extremity positions
                         //!< to whom it belongs
    UniqueVertex   = 4,  //!< Vertex with one set of coordinates (absolute position)
    WireEdge       = 5,  //!< Edge belonging to a wire body
    Edge           = 6,  //!< Edge belonging to a BrepData body
    CoEdge         = 7,  //!< Usage of an edge in a loop
    Loop           = 8,  //!< Array of co-edges that delimit a face
    Face           = 9,  //!< Topological face delimiting a shell
    Shell          = 10, //!< Topological shell (open or closed)
    Connex         = 11, //!< Topological region
    Body           = 12, //!< Abstract root type for any topological body
    SingleWireBody = 13, //!< Single wire body
    BrepData       = 14, //!< Main entry to non-wire topology
                         //				= 15,
                         //				= 16,
    WireBody = 17,       //!< Main entry to wire topology
};

/*! Bits assignable to Mesh's face_elements_bits / polyline_elements_bits / point_elements_bits */
enum SelectionBits : uint8_t {
    SelectionBitsFaceHasMeasurementData = 1 << 0,
    SelectionBitsFacePlanar             = 1 << 1,
    SelectionBitsEdgeHasMeasurementData = 1 << 2,
};

//! Identifies a reference to an element on a brep
struct RefOnTopoItem {
    SC::Store::InstanceInc meshInstanceKey; //!< Instance key of the body
    PMITopoRef             type;            //!< Could be face, edge...
    uint32_t               index;           //!< Index of the face, edge...

    RefOnTopoItem(SC::Store::InstanceInc const& meshInstanceKey, PMITopoRef type, uint32_t index)
    : meshInstanceKey(meshInstanceKey)
    , type(type)
    , index(index)
    {}
};

//! Plane
struct Plane {
    DVector normal;
    double  d;

    Plane()
    : normal(0.0, 1.0, 0.0)
    , d(0.0)
    {}

    Plane(DPoint const& point, DVector const& normal)
    : normal(normal)
    {
        d = -(point.x * normal.x + point.y * normal.y + point.z * normal.z);
    }
};

//! BBox
struct BBox {
    double xmin, xmax, ymin, ymax, zmin, zmax;

    BBox()
    : xmin(0.0)
    , xmax(0.0)
    , ymin(0.0)
    , ymax(0.0)
    , zmin(0.0)
    , zmax(0.0)
    {}

    BBox(double _xmin, double _xmax, double _ymin, double _ymax, double _zmin, double _zmax)
    {
        xmin = _xmin;
        xmax = _xmax;
        ymin = _ymin;
        ymax = _ymax;
        zmin = _zmin;
        zmax = _zmax;
    }
};

//! Material information comes from the HOOPS Exchange definition. Please refer to the Exchange
//! documentation to get more information:
//! http://docs.techsoft3d.com/exchange/latest/build/struct_a3_d_graph_material_data.html
struct MaterialData {
    uint32_t ambientRGBIndex; //!< Ambient color index in color table (0 first color in the table, 3
                              //!< second color, 6 third color...)
    uint32_t diffuseRGBIndex; //!< Diffuse color index in color table (0 first color in the table, 3
                              //!< second color, 6 third color...)
    uint32_t emissiveRGBIndex; //!< Emissive color index in color table (0 first color in the table,
                               //!< 3 second color, 6 third color...)
    uint32_t specularRGBIndex; //!< Specular color index in color table (0 first color in the table,
                               //!< 3 second color, 6 third color...)
    double ambientAlpha;       //!< Ambient alpha value (valid values between 0.0 and 1.0)
    double diffuseAlpha;       //!< Diffuse alpha value (valid values between 0.0 and 1.0)
    double emissiveAlpha;      //!< Emissive alpha value (valid values between 0.0 and 1.0)
    double specularAlpha;      //!< Specular alpha value (valid values between 0.0 and 1.0)
    double shininess;          //!< Shininess (valid values between 0.0 and 1.0)

    MaterialData()
    : ambientRGBIndex(0)
    , diffuseRGBIndex(0)
    , emissiveRGBIndex(0)
    , specularRGBIndex(0)
    , ambientAlpha(1.0)
    , diffuseAlpha(1.0)
    , emissiveAlpha(1.0)
    , specularAlpha(1.0)
    , shininess(0.0)
    {}
};

//! Style information comes from the HOOPS Exchange definition. Please refer to the Exchange
//! documentation for more information:
//! http://docs.techsoft3d.com/exchange/latest/build/struct_a3_d_graph_style_data.html
struct StyleData {
    double width;      //!< Line width in mm
    bool   isMaterial; //!< A value of true indicates the rgbColorOrMaterialIndex member (next)
                     //!< references a material in the global arrays. A value of false indicates it
                     //!< references a color
    uint32_t rgbColorOrMaterialIndex; //!< Index in the global array as indicated with the
                                      //!< isMaterial member (above)
    uint8_t m_ucTransparency; //!< From 0 (transparent) to 255 (opaque). Default value is 255

    StyleData()
    : width(1.0)
    , isMaterial(false)
    , rgbColorOrMaterialIndex(0)
    , m_ucTransparency(255)
    {}
};

struct LayerData {
    LayerId     layerId;
    std::string layerName;

    LayerData()
    : layerId(~(LayerId)0u)
    {}
};

struct LayerFilterItemData {
    bool                 isInclusive;
    std::vector<LayerId> layers;
};

struct EntityFilterItemData {
    bool                isInclusive;
    std::vector<NodeId> nodes;
};

struct FilterData {
    bool                 isDisplayFilter;
    std::string          filterName;
    bool                 isActive;
    LayerFilterItemData  layerFilterItemData;
    EntityFilterItemData entityItemData;
};

enum class RelationshipType : uint32_t {
    ContainedInSpatialStructure = 0,
    Aggregates,
    VoidsElement,
    FillsElement,
    SpaceBoundary
};

struct RelationshipElement {
    std::string uid;
    std::string name;
    bool        connectedStatus;
};

struct RelationshipData {
    RelationshipType                 type;
    RelationshipElement              relating;
    std::vector<RelationshipElement> related;
};

//! AssemblyTree API
class AssemblyTree {
    class Impl;
    Impl* _pImpl;

    AssemblyTree(AssemblyTree const&) = delete;
    void operator=(AssemblyTree const&) = delete;

public:
    //! Major version number
    static const uint32_t MAJOR_VERSION = COMMUNICATOR_VERSION_MAJOR;

    //! Minor version number
    static const uint32_t MINOR_VERSION = COMMUNICATOR_VERSION_MINOR;

    //! Patch version number
    static const uint32_t PATCH_VERSION = COMMUNICATOR_VERSION_PATCH;

    //! Type of attributes to set on nodes
    enum AttributeType : uint8_t {
        AttributeTypeUndefined = 0,
        AttributeTypeInt,
        AttributeTypeFloat,
        AttributeTypeTime,
        AttributeTypeString,
        AttributeTypeIgnored
    };

    //! Constructor
    AssemblyTree(Logger const& logger, MemoryDevice* memory_device = nullptr);

    //! Constructor
    AssemblyTree(AssemblyTree&& other);

    //! Destructor
    ~AssemblyTree();

    void operator=(AssemblyTree&& other);

    /*! <p>Store the full path and name of the file that has been converted.</p>
    @param name Name of the node.
    @return none.*/
    void SetOriginalFileName(const char* name);

    /*! <p>Store the type of the file that has been converted.</p>
    @param Type of the file that has been converted.
    @return none.*/
    void SetOriginalFileType(FileType type);

    /*! <p>Creates a root node for the assembly.</p>
    @param outIDRoot (out) a node ID to access the created node.
    @return true if all went right, false otherwise.*/
    bool CreateAssemblyTreeRoot(NodeId& outIDRoot);

    /*! <p>Creates a child node linked with its parent.</p>
    @param parentNodeID (in) ID of the parent node.
    @param outIDChild (out) a node ID to access the created node.
    @param isAConfigurationNode (in) optional parameter telling if the node is a CAD configuration
    node.
    @return true if all went right, false otherwise.*/
    bool CreateChild(
        NodeId  parentNodeID,
        NodeId& outIDChild,
        bool    isAConfigurationNode = false,
        bool    isADefaultNode       = false);

    /*! <p>Name a node.</p>
    @param nodeID Node ID.
    @param name Name of the node.
    @return true if all went right, false otherwise.*/
    bool SetNodeName(NodeId nodeID, const char* name);

    /*! <p>Set the local transform of the node.</p>
    @param nodeID Node ID.
    @param localTransform Matrix representing the local transform of the node.
    @return true if all went right, false otherwise.*/
    bool SetNodeLocalTransform(NodeId nodeID, Matrix3d const& localTransform);

    /*! <p>Set the local transform of the node.</p>
    @param nodeID Node ID.
    @param localTransform Matrix representing the local transform of the node.
    @return true if all went right, false otherwise.*/
    bool SetNodeLocalTransform(NodeId nodeID, DMatrix3d const& localTransform);

    /*! <p>Set the material of the node.</p>
    @param nodeID Node ID.
    @param material Material to set on the node.
    @return true if all went right, false otherwise.*/
    bool SetNodeMaterial(NodeId nodeID, SC::Store::Material const& material);

    /*! <p>Set the material of the node.</p>
    @param nodeID Node ID.
    @param styleIndex Index of the display style in the global style array. Comes from the HOOPS
    Exchange definition. Please refer to the Exchange documentation for more information:
    http://docs.techsoft3d.com/exchange/latest/build/struct_a3_d_graphics_data.html
    @param behavior Behavior comes from the HOOPS Exchange definition. Please refer to the Exchange
    documentation for more information:
    http://docs.techsoft3d.com/exchange/latest/build/group__a3d__graphicsbits.html
    @return true if all went right, false otherwise.*/
    bool SetNodeMaterial(NodeId nodeID, uint32_t styleIndex, uint16_t behavior);

    /*! <p>Set if the node is visible or not.</p>
    @param nodeID Node ID.
    @param visible true if visible, false if not.
    @return true if all went right, false otherwise.*/
    bool SetNodeVisibility(NodeId nodeID, bool visible);

    /*! <p>Set if the node has been set as "removed". It reflects the fact that some objects in a
    CAD system could be set as removed, meaning they're still here but not interacting in any way
    with the existing scene. Corresponds to the removed boolean available in the HOOPS Exchange API
    (http://docs.techsoft3d.com/exchange/latest/build/struct_a3_d_misc_cascaded_attributes_data.html)</p>
    @param nodeID Node ID.
    @param removed true if "removed", false if not.
    @return true if all went right, false otherwise.*/
    bool SetNodeWasRemoved(NodeId nodeID, bool removed);

    /*! <p>Set node unit.</p>
    @param nodeID Node ID.
    @param unit multiplier of millimeters (for example inches will be 25.4).
    @return true if all went right, false otherwise.*/
    bool SetNodeUnit(NodeId nodeID, double unit);

    /*! <p>Set node file path.</p>
    @param nodeID Node ID.
    @param filePath CAD file file path that node came from.
    @return true if all went right, false otherwise.*/
    bool SetNodeFilePath(NodeId nodeID, const char* filePath);

    /*! <p>Adds an attribute to the node.</p>
    @param nodeID Node ID.
    @param attributeName Attribute name.
    @param attributeType Attribute type.
    @param attributeValue Attribute value.
    @return true if all went right, false otherwise.*/
    bool AddAttribute(
        NodeId        nodeID,
        const char*   attributeName,
        AttributeType attributeType,
        const char*   attributeValue);

    /*! <p>Adds an attribute to the node.</p>
    @param nodeID Node ID.
    @param attributeSetName Attribute set name.
    @param attributeName Attribute name.
    @param attributeType Attribute type.
    @param attributeValue Attribute value.
    @return true if all went right, false otherwise.*/
    bool AddAttribute(
        NodeId        nodeID,
        const char*   attributeSetName,
        const char*   attributeName,
        AttributeType attributeType,
        const char*   attributeValue);

    /*! <p>Set a Generic ID to the node.</p>
    @param nodeID Node ID.
    @param id generic id.
    @return true if all went right, false otherwise.*/
    bool SetGenericId(NodeId nodeID, const char* id);

    /*! <p>Set a Generic Type to the node.</p>
    @param nodeID Node ID.
    @param type generic type.
    @return true if all went right, false otherwise.*/
    bool SetGenericType(NodeId nodeID, const char* type);

    /*! <p>Adds user data to the node.</p>
    @param nodeID Node ID.
    @param userDataIndex The index of the user data.
    @param dataBegin The start pointer of the user data bytes.
    @param dataEnd The end pointer of the user data bytes.
    @return true if all went right, false otherwise.*/
    bool AddUserData(
        NodeId nodeID, UserDataIndex index, const uint8_t* dataBegin, const uint8_t* dataEnd);

    /*! <p>Adds an attribute to a face of the node.</p>
    @param bodyNodeID Body Node ID.
    @param faceIndex Face Index.
    @param attributeName Attribute name.
    @param attributeType Attribute type.
    @param attributeValue Attribute value.
    @return true if all went right, false otherwise.*/
    bool AddFaceAttribute(
        NodeId                      bodyNodeID,
        uint32_t                    faceIndex,
        const char*                 attributeName,
        AssemblyTree::AttributeType attributeType,
        const char*                 attributeValue);

    /*! <p>Adds an attribute to a face of the node.</p>
    @param bodyNodeID Body Node ID.
    @param faceIndex Face Index.
    @param attributeSetName Attribute set name.
    @param attributeName Attribute name.
    @param attributeType Attribute type.
    @param attributeValue Attribute value.
    @return true if all went right, false otherwise.*/
    bool AddFaceAttribute(
        NodeId                      bodyNodeID,
        uint32_t                    faceIndex,
        const char*                 attributeSetName,
        const char*                 attributeName,
        AssemblyTree::AttributeType attributeType,
        const char*                 attributeValue);

    /*! <p>Adds an attribute to an edge of the node.</p>
    @param bodyNodeID Body Node ID.
    @param edgeIndex Edge Index.
    @param attributeName Attribute name.
    @param attributeType Attribute type.
    @param attributeValue Attribute value.
    @return true if all went right, false otherwise.*/
    bool AddEdgeAttribute(
        NodeId                      bodyNodeID,
        uint32_t                    edgeIndex,
        const char*                 attributeName,
        AssemblyTree::AttributeType attributeType,
        const char*                 attributeValue);

    /*! <p>Adds an attribute to an edge of the node.</p>
    @param bodyNodeID Body Node ID.
    @param edgeIndex Edge Index.
    @param attributeSetName Attribute set name.
    @param attributeName Attribute name.
    @param attributeType Attribute type.
    @param attributeValue Attribute value.
    @return true if all went right, false otherwise.*/
    bool AddEdgeAttribute(
        NodeId                      bodyNodeID,
        uint32_t                    edgeIndex,
        const char*                 attributeSetName,
        const char*                 attributeName,
        AssemblyTree::AttributeType attributeType,
        const char*                 attributeValue);

    /*! <p>Adds an attribute to a point of the node.</p>
    @param bodyNodeID Body Node ID.
    @param pointIndex Point Index.
    @param attributeName Attribute name.
    @param attributeType Attribute type.
    @param attributeValue Attribute value.
    @return true if all went right, false otherwise.*/
    bool AddPointAttribute(
        NodeId                      bodyNodeID,
        uint32_t                    pointIndex,
        const char*                 attributeName,
        AssemblyTree::AttributeType attributeType,
        const char*                 attributeValue);

    /*! <p>Adds an attribute to a point of the node.</p>
    @param bodyNodeID Body Node ID.
    @param pointIndex Point Index.
    @param attributeSetName Attribute set name.
    @param attributeName Attribute name.
    @param attributeType Attribute type.
    @param attributeValue Attribute value.
    @return true if all went right, false otherwise.*/
    bool AddPointAttribute(
        NodeId                      bodyNodeID,
        uint32_t                    pointIndex,
        const char*                 attributeSetName,
        const char*                 attributeName,
        AssemblyTree::AttributeType attributeType,
        const char*                 attributeValue);

    /*! <p>Sets physical properties (volume, area, gravity center) to the node.</p>
    @param nodeID Node ID.
    @param gravityCenter gravity center in local coordinates.
    @param surfaceArea surface area, in squared current unit.
    @param volume volume, in cubed current unit.
    @return true if all went right, false otherwise.*/
    bool SetPhysicalProperties(
        NodeId nodeID, SC::Store::DVector gravityCenter, double surfaceArea, double volume);

    /*! <p>Sets BoundingBox to the node.</p>
    @param nodeID Node ID.
    @param BBox in current unit
    @return true if all went right, false otherwise.*/
    bool SetBoundingBox(NodeId nodeID, SC::Store::BBox bbox);

    /*! <p>Sets ExchangeId to the node.</p>
    @param nodeID Node ID.
    @param exchangeId
    @return true if all went right, false otherwise.*/
    bool SetNodeExchangeId(NodeId nodeID, const char* exchangeId);

    /*! <p>Add Filter to the node.</p>
    @param nodeID Node ID.
    @param filterData data of the filter
    @return true if all went right, false otherwise.*/
    bool AddFilterToNode(NodeId nodeID, const FilterData& filterData);

    /*! <p>Sets LayerId to the node.</p>
    @param nodeID Node ID.
    @param layerId
    @return true if all went right, false otherwise.*/
    bool SetNodeLayerId(NodeId nodeID, LayerId layerId);

    /*! <p>Add Layer to the layer list of the node.</p>
    @param nodeID Node ID.
    @param layerId
    @param layerName
    @return true if all went right, false otherwise.*/
    bool AddLayerToNode(NodeId nodeID, LayerId layerId, const char* layerName);

    /*! <p>Add Relationship to the relationship list of the node.</p>
    @param nodeID Node ID.
    @param relationshipData data of the relationship
    @return true if all went right, false otherwise.*/
    bool AddRelationshipToNode(NodeId nodeID, const RelationshipData& relationshipData);

    /*! <p>Creates a body instance on a assembly node.</p>
    @param nodeID Assembly node ID.
    @param outBodyInstanceNodeID (out) a node ID to access the created body instance.
    @return true if all went right, false otherwise.*/
    bool CreateAndAddBodyInstance(NodeId nodeID, NodeId& outBodyInstanceNodeID);

    /*! <p>Set mesh instance key on a body instance node.</p>
    @param nodeID Body instance node ID.
    @param meshInstanceKey Instance key of the mesh.
    @return true if all went right, false otherwise.*/
    bool SetBodyInstanceMeshInstanceKey(NodeId nodeID, InstanceInc const& meshInstanceKey);

    /*! <p>Creates an part node.</p>
    @param none.
    @return a node ID to access the created part node.*/
    NodeId CreatePart();

    /*! <p>Set a part on an assembly node.</p>
    @param assemblyNodeID ID of the assembly node.
    @param partNodeID ID of the part node.
    @return true if all went right, false otherwise.*/
    bool SetPart(NodeId assemblyNodeID, NodeId partNodeID);

    /*! <p>Set a link to an external model file (the model will then be included during
    BuildMasterAssemblyModel() call).</p>
    @param assemblyNodeID ID of the assembly node.
    @param name Name of the model.
    @return true if all went right, false otherwise.*/
    bool SetExternalModel(NodeId assemblyNodeID, const char* name);

    /*! <p>Creates a body on a part.</p>
    @param partNodeID (in) ID of the part node.
    @param outBodyNodeID (out) a node ID to access the created body.
    @param bodyType (in) optional parameter to specify which type of body it is (brep, tess,
    wireframe, points).
    @return true if all went right, false otherwise.*/
    bool
    CreateAndAddBody(NodeId partNodeID, NodeId& outBodyNodeID, BodyType bodyType = BodyTypeNotSet);

    /*! <p>Method to set Line measurement information on a specified body.</p>
    @param bodyNodeID Body node ID.
    @param edgeIndex Edge index.
    @param length Length of line.
    @return true if all went right, false otherwise.*/
    bool SetLineEdgeMeasurementData(NodeId bodyNodeID, uint32_t edgeIndex, float length);
    /*! <p>Method to set Circle measurement information on a specified body.</p>
    @param bodyNodeID Body node ID.
    @param edgeIndex Edge index.
    @param radius Radius of circle.
    @param origin Center of circle.
    @param normal Normal (orientation) of circle
    @return true if all went right, false otherwise.*/
    bool SetCircleEdgeMeasurementData(
        NodeId        bodyNodeID,
        uint32_t      edgeIndex,
        float         radius,
        Point const&  origin,
        Vector const& normal);
    /*! <p>Method to set Measurement information on a specified body of edge that is
    non-standard.</p>
    @param bodyNodeID Body node ID.
    @param edgeIndex Edge index.
    @param length Length of edge.
    @return true if all went right, false otherwise.*/
    bool SetOtherEdgeMeasurementData(NodeId bodyNodeID, uint32_t edgeIndex, float length);

    /*! <p>Method to set Cylindrical face measurement information on a specified body.</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @param radius Radius of Cylinder.
    @param origin Center point of Cylinder.
    @param normal Orientation of Cylinder
    @return true if all went right, false otherwise.*/
    bool SetCylinderFaceMeasurementData(
        NodeId        bodyNodeID,
        uint32_t      faceIndex,
        float         radius,
        Point const&  origin,
        Vector const& normal);
    /*! <p>Method to set planar face measurement information on a specified body.</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @param origin Origin of plane.
    @param normal Plane normal.
    @return true if all went right, false otherwise.*/
    bool SetPlaneFaceMeasurementData(
        NodeId bodyNodeID, uint32_t faceIndex, Point const& origin, Vector const& normal);
    /*! <p>Method to set cone measurement information on a specified body.</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @param radius Radius of Cone.
    @param origin Center point of Cone.
    @param normal Orientation of Cone.
    @param halfAngle Half Angle of cone.
    @return true if all went right, false otherwise.*/
    bool SetConeFaceMeasurementData(
        NodeId        bodyNodeID,
        uint32_t      faceIndex,
        float         radius,
        Point const&  origin,
        Vector const& normal,
        float         halfAngle);
    /*! <p>Method to set spherical face measurement information on a specified body.</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @param radius Radius of sphere.
    @param origin Center point of sphere.
    @param normal Normal of sphere.
    @return true if all went right, false otherwise.*/
    bool SetSphereFaceMeasurementData(
        NodeId        bodyNodeID,
        uint32_t      faceIndex,
        float         radius,
        Point const&  origin,
        Vector const& normal);
    /*! <p>Method to set torus measurement information on a specified body.</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @param majorRadius Major Radius of Torus.
    @param minorRadius Minor Radius of Torus.
    @param origin Center point of Torus.
    @param normal Orientation of Torus.
    @return true if all went right, false otherwise.*/
    bool SetTorusFaceMeasurementData(
        NodeId        bodyNodeID,
        uint32_t      faceIndex,
        float         majorRadius,
        float         minorRadius,
        Point const&  origin,
        Vector const& normal);
    /*! <p>Method to set faceIndex and bodyNodeID on a blend01 (more parameters for additional data
    will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetBlend01FaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on a blend02 (more parameters for additional data
    will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetBlend02FaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on a blend03 (more parameters for additional data
    will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetBlend03FaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on a NURBS body (more parameters for additional
    data will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetNurbsFaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on a cylinder body (more parameters for additional
    data will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetCylindricalFaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on an Offset body (more parameters for additional
    data will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetOffsetFaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on a Pipe body (more parameters for additional
    data will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetPipeFaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on a Ruled body (more parameters for additional
    data will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetRuledFaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on a Revolution body (more parameters for
    additional data will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetRevolutionFaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on an Extrusion body (more parameters for
    additional data will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetExtrusionFaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on a Curves body (more parameters for additional
    data will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetFromCurvesFaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);
    /*! <p>Method to set faceIndex and bodyNodeID on a Transform body (more parameters for
    additional data will be available in a future release).</p>
    @param bodyNodeID Body node ID.
    @param faceIndex Face index.
    @return true if all went right, false otherwise.*/
    bool SetTransformFaceMeasurementData(NodeId bodyNodeID, uint32_t faceIndex);

    /*! <p>Adds a CAD view on the node.</p>
    @param assemblyNodeID Node id to attach the created view to
    @param viewName Name of the view
    @param camera Camera that will be set when the view gets activated, pass nullptr if none to set
    @param pmiToShow Instance incs of the PMIs to show for the view
    @param nodesToShow Node IDs of the elements to force visibility on
    @param nodesToHide Node IDs of the elements to force visibility off
    @param nodesIDToLocalTransform Map of node ID and matrix pair, defining specific local transform
    to apply
    @param cuttingPlanes Cutting planes to set when the view gets activated
    @param frameMeshInstanceInc (could be null) Mesh instance inc of the rectangular frame (mostly
    found on capture views)
    @param frameInitiallyVisible Tells if the rectangular frame is initially visible or not
    @param isAnnotationView Tells if the CAD view is an annotation view or not
    @param outCADViewID (out) An ID to access the created CAD View
    @return true if all went right, false otherwise.*/
    bool AddCADView(
        NodeId                                       assemblyNodeID,
        const char*                                  viewName,
        SC::Store::Camera const*                     camera,
        SC::Store::InstanceIncs const&               pmiToShow,
        std::vector<NodeId> const&                   nodesToShow,
        std::vector<NodeId> const&                   nodesToHide,
        std::map<NodeId, SC::Store::Matrix3d> const& nodesIDToLocalTransform,
        std::vector<SC::Store::Plane> const&         cuttingPlanes,
        SC::Store::InstanceInc const*                frameMeshInstanceInc,
        bool                                         frameInitiallyVisible,
        bool                                         isAnnotationView,
        NodeId&                                      outCADViewID);

    /*! <p>Adds a CAD view on the node.</p>
    @param assemblyNodeID Node id to attach the created view to
    @param viewName Name of the view
    @param camera Camera that will be set when the view gets activated, pass nullptr if none to set
    @param pmiToShow Instance incs of the PMIs to show for the view
    @param nodesToShow Node IDs of the elements to force visibility on
    @param nodesToHide Node IDs of the elements to force visibility off
    @param nodesIDToLocalTransform Map of node ID and matrix pair, defining specific local transform
    to apply
    @param cuttingPlanes Cutting planes to set when the view gets activated
    @param frameMeshInstanceInc (could be null) Mesh instance inc of the rectangular frame (mostly
    found on capture views)
    @param frameInitiallyVisible Tells if the rectangular frame is initially visible or not
    @param isAnnotationView Tells if the CAD view is an annotation view or not
    @param outCADViewID (out) An ID to access the created CAD View
    @return true if all went right, false otherwise.*/
    bool AddCADView(
        NodeId                                        assemblyNodeID,
        const char*                                   viewName,
        SC::Store::Camera const*                      camera,
        SC::Store::InstanceIncs const&                pmiToShow,
        std::vector<NodeId> const&                    nodesToShow,
        std::vector<NodeId> const&                    nodesToHide,
        std::map<NodeId, SC::Store::DMatrix3d> const& nodesIDToLocalTransform,
        std::vector<SC::Store::Plane> const&          cuttingPlanes,
        SC::Store::InstanceInc const*                 frameMeshInstanceInc,
        bool                                          frameInitiallyVisible,
        bool                                          isAnnotationView,
        NodeId&                                       outCADViewID);

    /*! <p>Set a CAD view Flags.</p>
    @param CADViewNideID Node id of the CADView to set flags (must already exist)
    @param bIsCameraSet The View as a Camera, default: true
    @param bIsPMIFilteringSet The View handle PMI visibility, default: true
    @param bIsGeomFilteringSet The View handle Geometry visibility, default: true (not handled for
    now)
    @param bIsCrosssectionSet The View has a crosssection set, default: true (not handled for now)
    @param bIsExplosionSet The View has an explosion set, default: true (not handled for now)
    @param bIsCombineState This is a Combine State, default: false (not handled for now)
    @return true if all went right, false otherwise.*/
    bool SetCADViewFlags(
        NodeId CADViewNodeID,
        bool   bIsCameraSet        = true,
        bool   bIsPMIFilteringSet  = true,
        bool   bIsGeomFilteringSet = true,
        bool   bIsCrosssectionSet  = true,
        bool   bIsExplosionSet     = true,
        bool   bIsCombineState     = false);

    /*! <p>Adds a PMI instance with a single mesh instance.</p>
    @param assemblyNodeID Node id to attach the created PMI to
    @param name Name of the PMI
    @param meshInstanceInc PMI graphic representation mesh instance inc
    @param initiallyVisible Tells if the PMI is initially visible or not
    @param pmiType see PMIType enum (Datum, Gdt, Dimension...)
    @param pmiSubType see PMISubType enum (DatumTarget, GdtFcf, DimensionDistance...)
    @param refOnTopoItems see RefOnTopoItem. It defines the PMI links to a body element, like a face
    or an edge
    @param outPMIID (out) An ID to access the created PMI
    @return true if all went right, false otherwise.*/
    bool AddPMI(
        NodeId                            assemblyNodeID,
        const char*                       name,
        SC::Store::InstanceInc const&     meshInstanceInc,
        bool                              initiallyVisible,
        PMIType                           pmiType,
        PMISubType                        pmiSubType,
        std::vector<RefOnTopoItem> const& refOnTopoItems,
        NodeId&                           outPMIID);

    /*! <p>Adds a PMI instance with one or more mesh instances.</p>
    @param assemblyNodeID Node id to attach the created PMI to
    @param name Name of the PMI
    @param meshInstanceIncs PMI graphic representation mesh instance incs
    @param initiallyVisible Tells if the PMI is initially visible or not
    @param pmiType see PMIType enum (Datum, Gdt, Dimension...)
    @param pmiSubType see PMISubType enum (DatumTarget, GdtFcf, DimensionDistance...)
    @param refOnTopoItems see RefOnTopoItem. It defines the PMI links to a body element, like a face
    or an edge
    @param outPMIID (out) An ID to access the created PMI
    @return true if all went right, false otherwise.*/
    bool AddPMI(
        NodeId                                     assemblyNodeID,
        const char*                                name,
        std::vector<SC::Store::InstanceInc> const& meshInstanceIncs,
        bool                                       initiallyVisible,
        PMIType                                    pmiType,
        PMISubType                                 pmiSubType,
        std::vector<RefOnTopoItem> const&          refOnTopoItems,
        NodeId&                                    outPMIID);

    /*! <p>Register the global color, material and style tables.</p>
    @param colors R,G,B one double for each.
    @param materials The materials table comes from the HOOPS Exchange definition. Please refer to
    Exchange documentation for more information:
    http://docs.techsoft3d.com/exchange/latest/build/struct_a3_d_graph_material_data.html
    @param styles The styles table comes from the HOOPS Exchange definition. Please refer to
    Exchange documentation for more information:
    http://docs.techsoft3d.com/exchange/latest/build/struct_a3_d_graph_style_data.html
    @return true if all went right, false otherwise.*/
    bool SetColorMaterialAndStyleTable(
        std::vector<double> const&       colors,
        std::vector<MaterialData> const& materials,
        std::vector<StyleData> const&    styles);

    /*! <p>Set the Front and Up vector for the AssemblyTree</p>
    @param front vector
    @param up vector
    @return true if all went right, false otherwise.*/
    bool SetViewAxes(DVector const& front, DVector const& up);

    /*! <p>Set the default camera for the AssemblyTree</p>
    @param camera Camera to be set as default
    @return true if all went right, false otherwise.*/
    bool SetDefaultCamera(Camera const& camera);

    /*! <p>Serialize data into a given model.</p>
    @param model Model to serialize into.
    @return true if all went right, false otherwise.*/
    bool SerializeToModel(Model& model);

    /*! <p>Serialize data into a XML file.</p>
    @param outputXMLFilePathUTF8 File path of the xml file to output into.
    @param shatteredSearchDir Directory path of the shattered models written to XML.
    @return true if all went right, false otherwise.*/
    bool
    SerializeToXML(const char* outputXMLFilePathUTF8, const char* shatteredSearchDir = nullptr);

    /*! <p>De-serialize data from a XML file and create the assembly tree.</p>
    @param inputXMLFilePathUTF8 File path of the xml file to read from.
    @return true if all went right, false otherwise.*/
    bool DeserializeFromXML(const char* inputXMLFilePathUTF8);

    /*! <p>Will gather shattered part model files and create the master assembly model.</p>
    @param modelsPathUTF8 A path containing the models to assemble.
    @param assemblyModelPathUTF8 Full path of the assembly model to build.
    @param create_scz Controls if the created master will be a .scz file.
    @param compress_scz Controls if the created .scz will be compressed.
    @return true if all went right, false otherwise.*/
    bool BuildMasterAssemblyModel(
        const char* modelsPathUTF8,
        const char* assemblyModelPathUTF8,
        const char* workingDirectoryUTF8,
        bool        create_scz,
        bool        compress_scz = true);

    /*! <p>Creates a drawing sheet. You can find more information in the HOOPS Exchange
    documentation: http://docs.techsoft3d.com/exchange/latest/build/group__a3d__drawing__sheet.html
    </p>
    @param parentNodeID (in) ID of the parent node.
    @param outIDSheet (out) a node ID to access the created drawing sheet.
    @return true if all went right, false otherwise.*/
    bool CreateAndAddDrawingSheet(NodeId parentNodeID, NodeId& outIDDrawingSheet);

    /*! <p>Creates a drawing view. You can find more information in the HOOPS Exchange
    documentation: http://docs.techsoft3d.com/exchange/latest/build/group__a3d__drawing__view.html
    </p>
    @param sheetNodeID (in) ID of the sheet that will hold the view.
    @param outIDDrawing (out) a node ID to access the created drawing view.
    @return true if all went right, false otherwise.*/
    bool CreateAndAddDrawingView(NodeId drawingSheetNodeID, NodeId& outIDDrawingView);

    class Walker {
    public:
        virtual ~Walker() {}
        virtual void
        Node(NodeId nodeID, const char* model_name, DMatrix3d const& matrix, double unit) const = 0;
    };

    /*! <p> Walks the assembly tree created from a XML file.</p>
    @param Walker An overloaded class that is callback as the assembly tree is walked.
    @return true if all went right, false otherwise.*/
    bool WalkMasterAssemblyModel(Walker const& walker);
};
}} // namespace SC::Store
