import io from 'socket.io-client';

export default class scUpdate {
  constructor(libScServerEndpoint) {
    this.scChanges = { attributes: [], defaultCamera: null, colors: [], meshs: []};
    this.socket = io(libScServerEndpoint, { transports: ['websocket'] });

    // Socket.io error info for debugging
    this.socket.on('connect_error', (err) => {
      console.log(`connect_error due to ${err.message}`);
    });
  }

  updateAttributes(nodeId, nodeAttributes) {
    //Need to add check if nodeId already has attribute updates associated
    let nodeInfo = {
      nodeId: nodeId,
    };
    let updateInfo = Object.assign(nodeInfo, nodeAttributes);
    this.scChanges.attributes.push(updateInfo);
  }

  updateDefaultCameraView(camera) {
    this.scChanges.defaultCamera = camera;
  }

  // Will need to update this for using something like a color map in HC
  updateColors(colorMap) {
    colorMap.forEach((value, key) => {
      let nodeInfo = {
        nodeId: key,
        color: value,
      };
      this.scChanges.colors.push(nodeInfo);
    });
  }

  updateMeshes(nodeId, parentNodeId, meshData) {
      // Need to parse meshdatacopy and put into JSON. BLAH!
      let nodeInfo = {
        nodeId: nodeId,
        parentNodeId: parentNodeId,
        mesh: {
          faces: meshData.faces,
          isMainfold: meshData.isMainfold,
          isTwoSided: meshData.isTwoSided,
          points: meshData.points,
          lines: meshData.lines,
          winding: meshData.winding
        }
      }
      this.scChanges.meshs.push(nodeInfo)
  }

  sendToLibSc() {
    let jsonSendText = JSON.stringify(this.scChanges);
    console.log(jsonSendText);
    this.socket.emit('sc_update_to_author', jsonSendText);
    // Add method to clear tracked updates once they are successfully published.
  }
}
