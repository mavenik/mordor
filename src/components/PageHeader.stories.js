import React from 'react';
import {storiesOf} from '@storybook/react';
import {MemoryRouter} from 'react-router-dom';
import PageHeader from './PageHeader';

storiesOf('PageHeader', module)
  .addDecorator((story) => <MemoryRouter>{story()}</MemoryRouter>)
  .add('default', () => <PageHeader/>);
