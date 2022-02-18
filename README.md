# sc-update-module
Sample framework that hooks into HOOPS Communicator and allows for client-side updates to stream cache models


## Project setup

This project is intended to be a simple framework for udpating your Stream Cache files from within the WebViewer. A sample application is provided to help test and demonstrate the capability. The application is broken up into a client and server modules. 

1. Install node dependencies for client, server, and sample root. The client is set up using Create React App (CRA). The server uses Express and Socket.io. Both components can be run simultaneously via Concurrently. 
    - Navigate to sample/client, and run `npm install` (installs CRA and its dependencies, as well as socket.io)
    - Navigate to sample/server, and run `npm install` (installs express, socket.io, and their dependencies)
    - In the sample directory, one more time, `npm install` (installs concurrently)

2. The package.json file in the sample root directory has three scripts - `start`, `client`, and `server`. All can be run with `npm {scriptName}`.  To start both the client and server, run `npm start`

3. Create an outputs/modelCache and outputs/modelCache/xmlAssembly in your libsc directory, for model outputs to be written to (need to set this up in the repo so it is already there).  Libsc will write to these directories. An output directory must exist before writing the Cache (I currently have outputs ignored by git so they do not push to remote).


4. The client code can run out of the box, but we will need to build our libsc exectuable to be called by the server. You can use your own method to do this, but there are VS Code task.json and launch.json files to help build and debug your code in VSCode. Whatever you choose, you will need to link the approprate libsc libraries, and ensure that the libhps_core.dylib (or .dll or .so) is findable in your system path. See tasks.json for sample compile params. Notice that in launch.json, we are specifiying the LD_LIBRARY_PATH (assuming Mac for now).


## Sample Use Cases
 - Add attributes to a model from the viewer
 - Aggregate models into one SC
 - Update default camera view on model load
 - Add/edit additional meshes
 - Reorganize nodes in an assembly tree
