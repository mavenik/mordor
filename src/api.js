/**
 * Base instance for REST API calls
 */
import axios from 'axios';

export default axios.create({
  baseURL: __API_HOST__+'/api'
});
