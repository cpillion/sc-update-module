import React from 'react';
import Communicator from 'communicator';
import scUpdate from './sc-update-client';

export default class UIToolbar extends React.Component {
  constructor() {
    super();
    this.scUpdate = new scUpdate('http://localhost:5000');
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
        alert("Error: Cannot write attributes to a body instance!")
    }
    else {
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
    let nodeIds = [8, 9, 10, 11]; // Screws nodeIds
    let color = new Communicator.Color.green();
    let colorMap = new Map();
    for (let nodeId of nodeIds) {
      colorMap.set(nodeId, color);
    }
    this.props.viewer.model.setNodesColors(colorMap);
    this.scUpdate.updateColors(colorMap);
    this.scUpdate.sendToLibSc();
  }

  async makeMeshes() {
    let hwv = this.props.viewer;
    let cubeMeshData = this.createCubeMeshData(20);
    const cubeMeshId = await hwv.model.createMesh(cubeMeshData);
    let meshInstanceData = new Communicator.MeshInstanceData(cubeMeshId);
    const nodeId = await hwv.model.createMeshInstance(meshInstanceData);
    scUpdate.updateMeshs(
      nodeId,
      hwv.model.getNodeParent(nodeId),
      hwv.model.getNodeMeshData(nodeId)
    );
    scUpdate.sendToLibSc();
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

  render() {
    return (
      <>
        <button
          id="home-button"
          onClick={() => {
            this.props.viewer.view
              .resetCamera(750)
              .then(() => this.props.viewer.view.fitWorld(750));
          }}
        >
          Home
        </button>
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
      </>
    );
  }
}
