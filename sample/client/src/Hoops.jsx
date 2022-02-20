import React from 'react';
import ViewerManager from './ViewerManager';
import UIToolbar from './UIToolbar';
import scUpdate from './sc-update-client';
import Communicator from 'communicator';

class Hoops extends React.Component {
  constructor() {
    super();
    this.state = { hwvInstantiated: false, libScStdOut: [] };
    this._viewer = undefined;
    this._hwvManager = new ViewerManager();
    this.scUpdate = new scUpdate('http://localhost:5000');
    this.scUpdate.getSocketControl().on('libscstdout', (outputStream) => {
      this.setState((state, props) => {
        return { 
            libScStdOut: [...state.libScStdOut, `${outputStream}\n`] 
        };
      });
    });
  }

  // Once the element is in the browser DOM, initialize the viewer and set up sockets
  componentDidMount() {
    this._hwvManager.createViewer('canvas', null, 'microengine').then((viewer) => {
      this._viewer = viewer;
      // Once the viewer is instantiated, we can set the state to true to have the React update the DOM
      this.setState({
        hwvInstantiated: true,
      });

      // Storing the callback in its own function to avoid registering a bound callback
      // (more difficult to unregister that in HC)
      this._viewer.setCallbacks({
        modelStructureReady: () => this._viewer.fitWorld(),
        selectionArray: () => console.log('Selection Changed'),
        sceneReady: () => {
          this._viewer.view.setBackgroundColor(
            new Communicator.Color.black(),
            new Communicator.Color.white()
          );
        this._viewer.selectionManager.setNodeSelectionColor(new Communicator.Color.green())
        // this._viewer.selectionManager.setHighlightNodeSelection(false);
        // this._viewer.selectionManager.setHighlightFaceElementSelection(false);
        // this._viewer.selectionManager.setHighlightLineElementSelection(false);
        // this._viewer.selectionManager.setHighlightPointSelection(false);
        },
      });

      this._viewer.start();
      window.addEventListener('resize', () => this._viewer.resizeCanvas());
    });
  }

  componentWillUnmount() {}

  render() {
    let viewerElement;
    // We must render a div for the viewer to be placed in. This component must mount before instantiating the viewer.
    // Once the component has mounted and the viewer is instantiated, we can render other components that rely on it.
    if (!this.state.hwvInstantiated) {
      viewerElement = <div></div>;
    } else {
      // Now that the viewer is instantiated, we can render any other components that depend on it (like UI for the viewer or part info)
      let terminalOutput = [];

      if (this.state.libScStdOut.length > 0) {
        for (let stdoutline of this.state.libScStdOut) {
          terminalOutput.push(<text>{stdoutline}</text>);
        }
      }

      viewerElement = (
        <div id="viewerTools">
          <UIToolbar viewer={this._viewer} scUpdate={this.scUpdate} />
          {/* <InformationPane viewer={this._viewer}/> */}
          <div id="terminal-output" className="terminal">
            {terminalOutput}
          </div>
        </div>
      );
    }

    return (
      <div id="scUpdateContainer">
        <div id="hoops-container">
          <div id="canvas" />
        </div>
        {viewerElement}
      </div>
    );
  }
}

export default Hoops;
