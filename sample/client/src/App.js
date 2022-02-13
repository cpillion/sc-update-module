import React from 'react';
import logo from './logo.svg';
import './App.css';

import Hoops from './Hoops';

function App() {
  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <a
          className="App-link"
          href="https://docs.techsoft3d.com/communicator/latest/build/overview-technical-overview.html"
          target="_blank"
          rel="noopener noreferrer"
        >
          HOOPS Communicator Documentation
        </a>
      </header>
      <Hoops/>
    </div>
  );
}

export default App;
