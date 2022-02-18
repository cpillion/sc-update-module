import React from "react";
import scUpdate from "./sc-update";

export default class UIToolbar extends React.Component {

    constructor() {
        super();
        this.scUpdate = new scUpdate("http://localhost:5000");
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
            this.scUpdate.updateAttributes(nodeId, {[propertyName]:propertyValue});
            this.scUpdate.sendToLibSc();
        }
    }


    render() {
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