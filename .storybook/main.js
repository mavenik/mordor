module.exports = {
  stories: ['../src/components/**/*.stories.js'],
  addons: [
    '@storybook/addon-actions', 
    '@storybook/addon-links',
    '@storybook/addon-viewport/register'
  ],
  webpackFinal: async config => {
    // do mutation to the config

    return config;
  },
};
