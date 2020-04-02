import React, {useState, useContext} from 'react';
import {
  Button,
  Form,
  Accordion,
  Icon,
  Grid,
  Input,
  Dropdown
} from 'semantic-ui-react';
import {SpaceContext} from '../context/SpaceContext';
import {uuidv4} from '../uuid';

export default function AddSpaceForm(){
  const [formVisible, setFormVisible] = useState(false);
  const [spaceName, setSpaceName] = useState('');
  const [iconName, setIconName] = useState('');
  const {addSpace} = useContext(SpaceContext);

  const toggleForm = (e, titleProps) => {
    e.preventDefault();
    setFormVisible(!formVisible);
  }

  const submitForm = (e) => {
    e.preventDefault();
    var space = {};
    space[uuidv4()] = {name: spaceName, icon: iconName};
    addSpace(space);
    setSpaceName('');
    setIconName('');
    setFormVisible(false);
  }

  const handleDropdown = (data) => {
    console.log(data);
  }

  const dropdownOptions = [
    {icon: "food", content: "Kitchen"},
    {icon: "tree", content: "Garden"},
    {icon: "home", content: "Living Area"},
    {icon: "bath", content: "Bathroom"},
    {icon: "bed", content: "Bedroom"},
  ].map(option => {
    return {...option, key: option.icon, value: option.icon};
  })

  return (
    <>
    <Grid>
      <Grid.Column width={8}>
    <Accordion>
      <Accordion.Title active={formVisible} onClick={toggleForm}>
        <Icon name='dropdown'/>
        Add new space
      </Accordion.Title>
      <Accordion.Content active={formVisible}>
        <Form>
        <Grid>
          <Grid.Row>
          <Grid.Column width={12}>
          <Form.Field fluid="true">
            <Input name="name" 
              placeholder="Space Name" 
              value={spaceName}
              onChange={(e,nameProps) => setSpaceName(nameProps.value)}
            />
          </Form.Field>
          </Grid.Column>
          <Grid.Column width={4} textAlign="center" verticalAlign="middle">
            <Form.Dropdown name="icon" text="Icon&nbsp;&nbsp;&nbsp;" 
              labeled button className="icon"
              icon={iconName || 'dropdown'}
              value={iconName}
              onChange={(e, dropdownProps) => setIconName(dropdownProps.value)}
              options={dropdownOptions}
            />
          </Grid.Column>
          </Grid.Row>
          <Grid.Row>
          <Grid.Column>
          <Form.Button fluid 
            type='submit' 
            onClick={submitForm}
            disabled={spaceName === '' || iconName === ''}
          >
            Add Space
          </Form.Button>
          </Grid.Column>
          </Grid.Row>
        </Grid>
        </Form>
      </Accordion.Content>
    </Accordion>
      </Grid.Column>
    </Grid>
    </>
  )
}
