import React from 'react';
import {Header, Item, Icon} from 'semantic-ui-react';
import {Link} from 'react-router-dom';

export default function PageHeader(props){
  return (
    <Item as={Link} to="/">
      <Header as="h2" textAlign="center">
        <Icon name="home" style={{color: props.headerColor}}/>
        <Header.Content style={{color: props.headerColor}}>Mordor</Header.Content>
      </Header>
    </Item>
  )
}
