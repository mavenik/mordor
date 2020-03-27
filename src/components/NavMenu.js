import React, {useState} from 'react';
import {Menu} from 'semantic-ui-react';
import {Link} from 'react-router-dom';

export default function NavMenu(props) {
  const [activeItem, setActiveItem] = useState(null)
  const handleItemClick = (event, { name }) => setActiveItem(name)

  return (
    <Menu fluid vertical tabular>
      {props.items.map( item => <Menu.Item 
        {...item}
        name={item.content.toLowerCase().replace(/\s+/g, '-')}
        key={item.content.toLowerCase().replace(/\s+/g, '-')}
        onClick={handleItemClick} as={Link}
        active={activeItem === item.content.toLowerCase().replace(/\s+/g, '-')}
      />)}
    </Menu>
  )
}
