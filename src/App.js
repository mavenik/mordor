import React, {useEffect} from 'react';
import ReactDOM from 'react-dom';
import {
  BrowserRouter as Router,
 Switch,
 Route} from 'react-router-dom';
import {Header} from 'semantic-ui-react';
import Dashboard from './pages/Dashboard';
import About from './pages/About';
import Settings from './pages/Settings';
import {w3cwebsocket as W3CWebSocket} from 'websocket';

import './App.css';

if (module.hot) {
    module.hot.accept();
}

function App(){
  const socket = new W3CWebSocket("ws://192.168.1.200/socket");
  useEffect(() => {
    socket.onopen = () => {console.log('Socket Connected!')}

    socket.onmessage = (message) => {console.log(message)}
  });
  return (
    <Router>
      <Switch>
        <Route exact path='/'>
          <Dashboard />
        </Route>
      </Switch>
      <Switch>
        <Route exact path='/settings'>
          <Settings />
        </Route>
      </Switch>
      <Switch>
        <Route exact path='/about'>
          <About />
        </Route>
      </Switch>
    </Router>
  );
}

ReactDOM.render(<App/>, document.getElementById('root'));
