import React from 'react';
import DefaultLayout from '../components/DefaultLayout';
import PropTypes from 'prop-types';

const menuItems = [
  {content: 'Dashboard', to: '/'},
  {content: 'Settings', to: '/settings'},
  {content: 'About', to: '/about'}
]

export default function Page({activeItem, children}) {
  return (
    <DefaultLayout items={menuItems} activeItem={activeItem}>
      {children}
    </DefaultLayout>
  )
}

Page.propTypes = {
  activeItem: PropTypes.string
}
