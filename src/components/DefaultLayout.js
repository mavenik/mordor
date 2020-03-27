import React, {useState} from 'react';
import {Grid, Segment} from 'semantic-ui-react';
import NavMenu from './NavMenu';
import PageHeader from './PageHeader';

export default function DefaultLayout(props) {
  return (
    <>
      <Grid>
        <Grid.Row centered color="black" textAlign="center" style={{marginTop: '0.7em'}}>
          <Grid.Column width={16}>
            <PageHeader headerColor="white"/>
          </Grid.Column>
        </Grid.Row>
        <Grid.Row>
          <Grid.Column width={4}>
            <NavMenu items={props.items}/>
          </Grid.Column>
          <Grid.Column width={12}>
            {props.children}
          </Grid.Column>
        </Grid.Row>
      </Grid>
    </>
  )
}
