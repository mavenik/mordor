import React, {useState} from 'react';
import {
  Button,
  Form,
  Transition
} from 'semantic-ui-react';

export default function AddSpaceForm(){
  const [formVisible, setFormVisible] = useState(false);

  const showForm = e => {
    e.preventDefault();
    setFormVisible(true);
  }

  const submitForm = e => {
    e.preventDefault();
  }

  return (
    <Form>
      <Transition visible={formVisible}>
        <Form.Field inline>
          <label>Name</label>
          <input placeholder="Space Name" />
        </Form.Field>
      </Transition>
      <Transition visible={!formVisible}>
            <Button primary onClick={showForm}>Add Space</Button>
      </Transition>
    </Form>
          )
          }
