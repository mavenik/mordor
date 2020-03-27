import React from 'react';
import {MemoryRouter} from 'react-router-dom';
import {items} from './NavMenu.stories';
import {storiesOf} from '@storybook/react';
import DefaultLayout from './DefaultLayout';

storiesOf('DefaultLayout', module)
  .addDecorator((story) => <MemoryRouter>{story()}</MemoryRouter>)
  .add('default', () => <DefaultLayout items={items} activeItem="about">
    Application content
  </DefaultLayout>
    )
