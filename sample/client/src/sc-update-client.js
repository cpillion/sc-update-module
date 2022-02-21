import io from 'socket.io-client';

export default class scUpdate {
  constructor(libScServerEndpoint) {
    this.scChanges = {};
    this.socket = io(libScServerEndpoint, { transports: ['websocket'] });
    // this.libScStdOut = "";

    // this.socket.on('libscstdout', (outputStream)=> {
    //   this.libScStdOut += outputStream;
    // })

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
  }
  
  getSocketControl() {
    return this.socket;
  }

  sendToLibSc() {
    this.socket.emit('sc_update_to_author', JSON.stringify(this.scChanges));
    // Add method to clear tracked updates once they are successfully published.
  }
}
