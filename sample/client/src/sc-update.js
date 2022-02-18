import io from "socket.io-client";

export default class scUpdate {

    constructor(libScServerEndpoint) {
        this.scChanges = {attributes: [], camera: null};
        this.socket = io(libScServerEndpoint, { transports: ["websocket"] });

        // Socket.io error info for debugging
        this.socket.on("connect_error", (err) => {
            console.log(`connect_error due to ${err.message}`);
        });
    }

    updateAttributes(nodeId, nodeAttributes) {
        //Need to add check if nodeId already has attribute updates associated
        let nodeInfo = {
            "nodeId" : nodeId,
        }
        let updateInfo = Object.assign(nodeInfo, nodeAttributes)
        this.scChanges.attributes.push(updateInfo);
    }

    sendToLibSc() {
        this.socket.emit('sc_update_to_author', JSON.stringify(this.scChanges));
        // Add method to clear tracked updates once they are successfully published. 
    }

}