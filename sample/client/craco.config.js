module.exports = {
    webpack: {
      configure: (webpackConfig, { env, paths }) => {
        webpackConfig.externals = {
            communicator: 'Communicator'
        }
        return webpackConfig;
      }
    }
  }