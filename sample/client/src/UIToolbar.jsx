import React from "react";

export default class UIToolbar extends React.Component {


    render() {
        return(
            <button id="home-button" onClick={() => {
                this.props.viewer.view.resetCamera(750)
                .then( () => this.props.viewer.view.fitWorld(750) )
            }
            }>Home</button>
        )
    }

}