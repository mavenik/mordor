import React from 'react';
import ReactDOM from 'react-dom';
import {Header} from 'semantic-ui-react';
import './App.css';

if (module.hot) {
    module.hot.accept();
}

function App(){
  return (
    <Header as='h1'>Mordor</Header>
  );
}

ReactDOM.render(<App/>, document.getElementById('root'));
