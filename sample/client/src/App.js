import React from 'react';
import logo from './ts3dlogo.svg';
import './App.css';
import logoname from './ts3dlogoname.png';

import Hoops from './Hoops';

function App() {
  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <img src={logoname} className="App-logo-name" alt="logoname" />
        {/* <a
          className="App-link"
          href="https://docs.techsoft3d.com/communicator/latest/build/overview-technical-overview.html"
          target="_blank"
          rel="noopener noreferrer"
        >
          HOOPS Communicator Documentation
        </a> */}
      </header>
      <Hoops modelname="_MOTO_X"/>
    </div>
  );
}

export default App;
