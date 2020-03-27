import React from 'react';
import {MemoryRouter} from 'react-router-dom';
import {storiesOf} from '@storybook/react';
import NavMenu from './NavMenu';

export const items = [
  {content: 'Home', to: '/home'},
  {content: 'About', to: '/about'}
]

storiesOf('NavMenu', module)
  .addDecorator((story) => <MemoryRouter>{story()}</MemoryRouter>)
  .add('default', () => <NavMenu items={items} />);
