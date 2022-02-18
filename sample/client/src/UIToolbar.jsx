import React from "react";
import io from "socket.io-client";

export default class UIToolbar extends React.Component {

    constructor() {
        super();
        this.socket = io("http://localhost:5000", { transports: ["websocket"] });
    }

    addNodeProperties() {
        let hwv = this.props.viewer;
        let selectionResults = hwv.selectionManager.getResults();
        // Start with one selection. TODO: add support from multiple selections
        let nodeId;
        if (selectionResults.length === 0) {
            alert("Please first select the nodes you would like to add properties for");
        }
        else {
            // Todo: Create modal, form, etc to collect this data. Using prompt for now. 
            let propertyName = prompt("Enter the property name: ", "Material");
            let propertyValue = prompt(`Enter the value for the ${propertyName} property: `, "");
            nodeId = selectionResults[0].getNodeId;
            // Use SC Update functions to send this data to the client - will need selected nodeID as well
            let libSCdata = {
                attributes: [{
                    "nodeId": nodeId,
                    [propertyName]: propertyValue
                }]
            }
            this.socket.emit('sc_update_to_author', JSON.stringify(libSCdata));
        }
    }


    render() {
        this.socket.on("connect_error", (err) => {
            console.log(`connect_error due to ${err.message}`);
          });
        return(
            <>
                <button id="home-button" onClick={() => {
                    this.props.viewer.view.resetCamera(750)
                    .then( () => this.props.viewer.view.fitWorld(750) )
                }
                }>Home</button>
                <button id="add-property-button" onClick={() => {
                    this.addNodeProperties();
                }
                }>Add Node Property</button>
            </>
        )
    }

}