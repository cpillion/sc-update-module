import io from 'socket.io-client';

export default class scUpdate {
  constructor(libScServerEndpoint) {
    this.scChanges = {};
    this.socket = io(libScServerEndpoint, { transports: ['websocket'] });

    // Socket.io error info for debugging
    this.socket.on('connect_error', (err) => {
      console.log(`connect_error due to ${err.message}`);
    });
  }

  updateNodeName(nodeId, nodeName) {
    //Need to add check if nodeId already has attribute updates associated
    let nodeInfo = {
      nodeId: nodeId,
      nodeName: nodeName
    };
    if (!this.scChanges.hasOwnProperty('nodeNames')) {
      this.scChanges.nodeNames = [];
    }
    this.scChanges.nodeNames.push(nodeInfo);
  }

  updateAttributes(nodeId, nodeAttributes) {
    //Need to add check if nodeId already has attribute updates associated
    let nodeInfo = {
      nodeId: nodeId,
    };
    let updateInfo = Object.assign(nodeInfo, nodeAttributes);
    if (!this.scChanges.hasOwnProperty('attributes')) {
      this.scChanges.attributes = [];
    }
    this.scChanges.attributes.push(updateInfo);
  }

  updateDefaultCameraView(camera) {
    this.scChanges.defaultCamera = camera;
  }

  updateColors(colorMap, scInstanceIdsMap) {
    if (!this.scChanges.hasOwnProperty('colors')) {
      this.scChanges.colors = [];
    }
    colorMap.forEach((value, key) => {
      let nodeInfo = {
        nodeId: key,
        color: value,
        scInstanceId: scInstanceIdsMap.get(key)[1] // ignore inclusion id, take instance id
      };
      this.scChanges.colors.push(nodeInfo);
    });
  }

  updateMeshes(nodeId, parentNodeId, meshData) {
    // Need to parse meshdatacopy and put into JSON.
    let meshDataTemplate = {
      nodeId: nodeId,
      parentNodeId: parentNodeId,
      localTransform: undefined,
      faces: [],
      lines: [],
      points: [],
      winding: meshData.winding,
      isTwoSided: meshData.isTwoSided,
      isManifold: meshData.isManifold,
    }

    let elementTypes = ["faces", "lines", "points"];

    for (let elementType of elementTypes) {
      let elementGroup = meshData[elementType];
      if (elementGroup.vertexCount === 0) continue;
      let egIterator = elementGroup.iterate();
      let meshElementData = {
        position: [],
        normal: [],
        rgba: [],
        uv: [],
      };
      while (!egIterator.done()) {
        let vertex = egIterator.next();
        meshElementData.position.push(...vertex.position);
        if (elementGroup.hasNormals) meshElementData.normal.push(...vertex.normal);
        if (elementGroup.hasRGBAs) meshElementData.rgba.push(...vertex.rgba);
        if (elementGroup.hasUVs) meshElementData.uv.push(...vertex.uv);
      }
      
      meshDataTemplate[elementType].push(meshElementData);
    }
    if (!this.scChanges.hasOwnProperty('meshes')) {
      this.scChanges.meshes = [];
    }
    this.scChanges.meshes.push(meshDataTemplate);
  }
  
  getSocketControl() {
    return this.socket;
  }

  getScChanges() {
    return this.scChanges;
  }

  sendToLibSc() {
    console.log(JSON.stringify(this.scChanges));
    this.socket.emit('sc_update_to_author', JSON.stringify(this.scChanges));
    // Add method to clear tracked updates once they are successfully published.
  }
}
