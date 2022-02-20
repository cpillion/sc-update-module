import React from 'react';
import Communicator from 'communicator';

export default class UIToolbar extends React.Component {
  constructor(props) {
    super(props);
    this.scUpdate = this.props.scUpdate;
    this.colorInput = undefined;
  }

  addNodeProperties() {
    let hwv = this.props.viewer;
    let selectionResults = hwv.selectionManager.getResults();
    if (selectionResults.length === 0) {
      alert('Please first select the nodes you would like to add properties for');
    }
    // Start with one selection. TODO: add support from multiple selections
    let nodeId = selectionResults[0].getNodeId();
    if (hwv.model.getNodeType(nodeId) === Communicator.NodeType.BodyInstance) {
      alert('Error: Cannot write attributes to a body instance!');
    } else {
      // Todo: Create modal, form, etc to collect this data. Using prompt for now.
      let propertyName = prompt('Enter the property name: ', 'Material');
      let propertyValue = prompt(`Enter the value for the ${propertyName} property: `, '');
      nodeId = selectionResults[0].getNodeId();
      // Use SC Update functions to send this data to the client - will need selected nodeID as well
      this.scUpdate.updateAttributes(nodeId, { [propertyName]: propertyValue });
      this.scUpdate.sendToLibSc();
    }
  }

  setDefaultView() {
    let camera = this.props.viewer.view.getCamera();
    this.scUpdate.updateDefaultCameraView(camera.toJson());
    this.scUpdate.sendToLibSc();
  }

  updateModelColors() {
    let hwv = this.props.viewer;
    let selectionResults = hwv.selectionManager.getResults();
    if (selectionResults.length === 0) {
      alert('Please first select the nodes you would like to change color');
    }
    let nodeIds = selectionResults.map((selectionItem) => selectionItem.getNodeId());
    let rgbColor = this.hexToRgb(this.colorInput);
    let color = new Communicator.Color(rgbColor.r, rgbColor.g, rgbColor.b);
    let colorMap = new Map();
    let scInstanceIdsMap = new Map();
    for (let nodeId of nodeIds) {
      colorMap.set(nodeId, color);
      //if (hwv.model.getNodeType(nodeId) )
      scInstanceIdsMap.set(nodeId, hwv.model.getSCInstanceKey(nodeId));
    }
    this.props.viewer.model.setNodesColors(colorMap);
    this.scUpdate.updateColors(colorMap, scInstanceIdsMap);
    this.scUpdate.sendToLibSc();
  }

  async makeMeshes() {
    let hwv = this.props.viewer;
    let cubeMeshData = this.createCubeMeshData(20);
    const cubeMeshId = await hwv.model.createMesh(cubeMeshData);
    let meshInstanceData = new Communicator.MeshInstanceData(cubeMeshId);
    const nodeId = await hwv.model.createMeshInstance(meshInstanceData);
    this.scUpdate.updateMeshs(
      nodeId,
      hwv.model.getNodeParent(nodeId),
      hwv.model.getNodeMeshData(nodeId)
    );
    this.scUpdate.sendToLibSc();
  }

  createCubeMeshData(size) {
    const cubeMeshData = new Communicator.MeshData();
    cubeMeshData.setFaceWinding(Communicator.FaceWinding.Clockwise);
    const p1 = new Communicator.Point3(-size, size, size);
    const p2 = new Communicator.Point3(size, size, size);
    const p3 = new Communicator.Point3(-size, -size, size);
    const p4 = new Communicator.Point3(size, -size, size);
    const p5 = new Communicator.Point3(size, size, -size);
    const p6 = new Communicator.Point3(-size, size, -size);
    const p7 = new Communicator.Point3(-size, -size, -size);
    const p8 = new Communicator.Point3(size, -size, -size);
    const points = [
      p1,
      p2,
      p3,
      p2,
      p4,
      p3,
      p5,
      p6,
      p7,
      p5,
      p7,
      p8,
      p6,
      p5,
      p2,
      p6,
      p2,
      p1,
      p7,
      p4,
      p8,
      p7,
      p3,
      p4,
      p6,
      p1,
      p7,
      p1,
      p3,
      p7,
      p2,
      p5,
      p8,
      p2,
      p8,
      p4,
    ];
    const normals = [
      new Communicator.Point3(0, 0, 1),
      new Communicator.Point3(0, 0, -1),
      new Communicator.Point3(0, 1, 0),
      new Communicator.Point3(0, -1, 0),
      new Communicator.Point3(-1, 0, 0),
      new Communicator.Point3(1, 0, 0),
    ];
    const verticexData = [];
    points.forEach((p) => {
      verticexData.push(p.x);
      verticexData.push(p.y);
      verticexData.push(p.z);
    });
    const normalData = [];
    normals.forEach((p) => {
      for (let i = 0; i < 6; ++i) {
        normalData.push(p.x);
        normalData.push(p.y);
        normalData.push(p.z);
      }
    });
    cubeMeshData.addFaces(verticexData, normalData);
    return cubeMeshData;
  }

  hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
      r: parseInt(result[1], 16),
      g: parseInt(result[2], 16),
      b: parseInt(result[3], 16)
    } : null;
  }

  render() {
    return (
      <div id="uiToolbar" className='uiToolbar'>
        <button
          id="add-property-button"
          onClick={() => {
            this.addNodeProperties();
          }}
        >
          Add Node Property
        </button>
        <button
          id="update-view-button"
          onClick={() => {
            this.setDefaultView();
          }}
        >
          Update Default View
        </button>
        <button
          id="update-colors-button"
          onClick={() => {
            this.updateModelColors();
          }}
        >
          Set and Author Assigned Colors
        </button>
        <button
          id="update-colors-button"
          onClick={() => {
            this.makeMeshes();
          }}
        >
          Insert and Author Sample Mesh
        </button>
          <input
            type="color"
            id="colorpicker"
            name="colorpicker"
            value="#f6b73c"
            onChange={(e) => {
              this.colorInput = e.target.value;
            }}
          />
      </div>
    );
  }
}
