import React, {useEffect, useState, useReducer, createContext} from 'react';
import Page from './Page';
import {
  Message,
  Dimmer,
  Loader,
  Segment,
  Image,
  Button
} from 'semantic-ui-react';
import AddSpaceForm from '../components/AddSpaceForm';
import API from '../api';
import {SpaceContext} from '../context/SpaceContext';

export default function Dashboard() {
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  const spaceAdder = (spaceConfig, newSpace) => {
    return {...spaceConfig, ...newSpace};
  }
  const [spaceConfig, addSpace] = useReducer(spaceAdder,{});

  async function getSpaceConfig() {
    setLoading(true);
    setError(null);
    API.get('/status.json')
      .then(response => {
        addSpace(response.data.status);
        setLoading(false);
      })
      .catch(error => {
        setError(error);
        setLoading(false);
      });
  }
  useEffect(()=>{
    getSpaceConfig();
  }, []);

  var errorMessage;
  if(error)
  {
    errorMessage = (
        <Message negative>
          <Message.Header>Could not get space configuration</Message.Header>
          <p>Server response was {error.response? error.response.status : ''}: {error.response? error.response.data : error.message}</p>
        </Message>
    )
    }

    var pageContent;
    if(loading)
    {
      pageContent = (
        <Segment>
          <Dimmer active inverted>
            <Loader size="medium">Loading space configuration</Loader>
          </Dimmer>
          <Image src="https://react.semantic-ui.com/images/wireframe/paragraph.png"/>
        </Segment>
      )
    }
    else
    {
      // Check if spaces have not been initialized yet
      if(
        Object.keys(spaceConfig).length == 0)
      {
        pageContent = (
          <>
          <Message>
            <Message.Header>No spaces found</Message.Header>
            <p>Add spaces to manage your home</p>
          </Message>
          </>
        )
      }
      else
      {
        pageContent = <>
          <p>{JSON.stringify(spaceConfig)}</p>
        </>
      }
      pageContent = (
        <>
          {pageContent}
          <SpaceContext.Provider value={{addSpace}}>
            <AddSpaceForm />
            </SpaceContext.Provider>
        </>
      )
    }

    return (
      <Page activeItem='dashboard'>
        {errorMessage}
        {pageContent}
      </Page>
    )
  }
