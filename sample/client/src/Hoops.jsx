import React from "react";
import ViewerManager from "./ViewerManager";
import UIToolbar from "./UIToolbar";
import InformationPane from "./InformationPane"

class Hoops extends React.Component {

    constructor() {
        super();
        this.state = {hwvInstantiated: false};
        this._viewer = undefined;
        this._hwvManager = new ViewerManager();
    }

    // Once the element is in the browser DOM, initialize the viewer and set up sockets
    componentDidMount() {

        this._hwvManager.createViewer("canvas", null, "microengine")
        .then( (viewer) => {
            this._viewer = viewer;
            // Once the viewer is instantiated, we can set the state to true to have the React update the DOM
            this.setState({
                hwvInstantiated: true
            });

            // Storing the callback in its own function to avoid registering a bound callback 
            // (more difficult to unregister that in HC)
            this._viewer.setCallbacks({
                modelStructureReady: () => this._viewer.fitWorld(),
                selectionArray: () => console.log("Selection Changed"),
            })

            this._viewer.start();
            window.addEventListener("resize", () => this._viewer.resizeCanvas());

        });
    }

    componentWillUnmount() {

    }


    render () {
        let viewerElement;
        // We must render a div for the viewer to be placed in. This component must mount before instantiating the viewer.
        // Once the component has mounted and the viewer is instantiated, we can render other components that rely on it.
        if (!this.state.hwvInstantiated) {
            viewerElement = <div></div>;
        }

        else {
            // Now that the viewer is instantiated, we can render any other components that depend on it (like UI for the viewer or part info)
            viewerElement = (
                <div id="viewerTools">
                    <UIToolbar viewer={this._viewer}/>
                    <InformationPane viewer={this._viewer}/>
                </div>
            )

        }

        return (
            <div id="hoops-container">
                <div id="canvas"/>
                {viewerElement}
            </div>
        );
    }
}

export default Hoops;