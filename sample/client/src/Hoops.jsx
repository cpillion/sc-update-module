import React from 'react';
import ViewerManager from './ViewerManager';
import UIToolbar from './UIToolbar';
import scUpdate from './sc-update-client';
import Communicator from 'communicator';

class Hoops extends React.Component {
  constructor(props) {
    super(props);
    this.state = { hwvInstantiated: false, libScStdOut: [] };
    this._viewer = undefined;
    this._modelname = this.props.modelname;
    this._hwvManager = new ViewerManager();
    this.scUpdate = new scUpdate('http://localhost:5000', this._modelname);
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
    this._hwvManager.createViewer('canvas', null, this._modelname).then((viewer) => {
      this._viewer = viewer;
      // Once the viewer is instantiated, we can set the state to true to have the React update the DOM
      this.setState({
        hwvInstantiated: true,
      });

      // Storing the callback in its own function to avoid registering a bound callback
      // (more difficult to unregister that in HC)
      this._viewer.setCallbacks({
        modelStructureReady: () => this._viewer.fitWorld(),
        sceneReady: () => {
          this._viewer.view.setBackgroundColor(
            new Communicator.Color.black(),
            new Communicator.Color(18, 116, 162)
          );
        this._viewer.selectionManager.setNodeSelectionColor(new Communicator.Color.green())
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
      let terminalOutput = [<p key="defaultKey">The libsc server is ready to capture updates...</p>];

      if (this.state.libScStdOut.length > 0) {
        for (let [index, stdoutline] of this.state.libScStdOut.entries()) {
          terminalOutput.push(<p key={index}>{stdoutline}</p>);
        }
      }

      viewerElement = (
        <div id="viewerTools">
          <UIToolbar viewer={this._viewer} scUpdate={this.scUpdate} />
          {/* <InformationPane viewer={this._viewer}/> */}
          <pre id="json-output" className="json-terminal">
            <p>JSON ScUpdate Object
            <br/>
            {JSON.stringify(this.scUpdate.getScChanges(), undefined, 2)}
            </p>
          </pre>
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
