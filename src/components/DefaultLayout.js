import React, {useState} from 'react';
import {Grid, Segment, Container} from 'semantic-ui-react';
import NavMenu from './NavMenu';
import PageHeader from './PageHeader';
import PropTypes from 'prop-types';

export default function DefaultLayout({items, activeItem, children}) {
  return (
    <>
      <Grid>
        <Grid.Row centered color="black" textAlign="center" style={{marginTop: '1em'}}>
          <Grid.Column width={16}>
            <PageHeader headerColor="white"/>
          </Grid.Column>
        </Grid.Row>
      </Grid>
      <Container style={{marginTop: '0.5em'}}>
            <Grid>
        <Grid.Row>
          <Grid.Column width={4}>
            <NavMenu items={items} activeItem={activeItem}/>
          </Grid.Column>
          <Grid.Column width={12}>
            {children}
          </Grid.Column>
        </Grid.Row>
      </Grid>
          </Container>
    </>
  )
}

DefaultLayout.propTypes = {
  items: PropTypes.arrayOf(PropTypes.shape({
    content: PropTypes.string.isRequired,
    to: PropTypes.string.isRequired
  })).isRequired,
  activeItem: PropTypes.string
}
