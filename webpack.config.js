/* webpack.config.js */
const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const webpack= require('webpack');

module.exports = env => {
  var apiHost = (!!env && env.production) ? "''" : "'http://192.168.1.200'";
  return {
    // Tell webpack to begin building its 
    // dependency graph from this file.
    entry: path.join(__dirname, 'src', 'App.js'),
    // And to place the output in the `build` directory
    output: {
      path: path.join(__dirname, 'build'),
      filename: 'bundle.js'
    },
    module: {
      rules: [
        {
          test: /\.jsx?$/,
          /* We'll leave npm packages as is and not 
           *            parse them with Babel since most of them 
           *                       are already pre-transpiled anyway. */
          exclude: /node_modules/,
          loader: 'babel-loader',
          options: {
            presets: [
              ['@babel/env', {targets: {esmodules: true}}]
            ]
          } // options
        },
        {
          test: /\.css$/,
          use: ['style-loader', 'css-loader']
        }
      ]
    },
    plugins: [
      new HtmlWebpackPlugin({
        template: path.join(__dirname, 'public', 'index.html')
      }),
      new webpack.DefinePlugin({__API_HOST__: apiHost})
    ]
  }
}
