import React from "react";

export default class UIToolbar extends React.Component {


    addNodeProperties() {
        let hwv = this.props.viewer;
        let selectionResults = hwv.selectionManager.getResults();
        if (selectionResults.length === 0) {
            alert("Please first select the nodes you would like to add properties for");
        }
        else {
            // Todo: Create modal, form, etc to collect this data. Using prompt for now. 
            let propertyName = prompt("Enter the property name: ", "i.e. Material");
            let propertyValue = prompt(`Enter the value for the ${propertyName} property: `, "");
            
            // Use SC Update functions to send this data to the client - will need selected nodeID as well
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