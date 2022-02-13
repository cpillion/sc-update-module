import Communicator from 'communicator';

export default class ViewerManager {

    // constructor() {

    // }

    /** @param {string} name */
    _getParameterByName(name) {
        name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
        var regexS = "[\\?&]" + name + "=([^&#]*)";
        var regex = new RegExp(regexS);
        var results = regex.exec(window.location.search);
        return results === null ? null : decodeURIComponent(results[1]);
    }

    _getStreamingMode() {
        var streamingMode = this._getParameterByName("streamingMode");
        // eslint-disable-next-line default-case
        switch (streamingMode) {
            case "interactive":
                return Communicator.StreamingMode.Interactive;
            case "all":
                return Communicator.StreamingMode.All;
            case "ondemand":
                return Communicator.StreamingMode.OnDemand;
        }

        return Communicator.StreamingMode.Interactive;
    }

    _getRenderingLocationString() {
        var val = this._getParameterByName("viewer");
        return val === "ssr" ? "ssr" : "csr";
    }

    _getRendererType() {
        var val = this._getRenderingLocationString();
        return val === "ssr" ? Communicator.RendererType.Server : Communicator.RendererType.Client;
    }

    _getLayout() {
        return this._getParameterByName("layout");
    }

    _getModel() {
        var modelName =
            this._getParameterByName("model") || this._getParameterByName("instance");
        return modelName;
    }

    _getMemoryLimit() {
        var memoryLimit = this._getParameterByName("memoryLimit");
        return memoryLimit === null ? 0 : parseInt(memoryLimit);
    }

    _getProxy() {
        var proxy = this._getParameterByName("proxy");
        return proxy !== null;
    }

    /** URL parameter "connect". Valid values are "broker" or (default)"direct" */
    _getConnectType() {
        var connect = this._getParameterByName("connect");
        return connect === "broker" ? "broker" : "direct";
    }

    /** @param {string} endpoint */
    _rewrite(endpoint) {
        var regex = /([ws]+):\/\/(.*):([0-9]+)/;
        var matches = regex.exec(endpoint);
        if (matches === null) {
            return endpoint;
        }

        var protocol = matches[1];
        var host = matches[2];
        var port = matches[3];

        return protocol + "://" + host + "/" + protocol + "proxy/" + port;
    }

    /** @param {Communicator.WebViewerConfig} config */
    _applyExtraProperties(config) {
        var debug = this._getParameterByName("debug");
        if (debug !== null && parseInt(debug, 10) !== 0) {
            config._markImplicitNodesOutOfHierarchy = false;
        }

        var defaultMetallicFactor = this._getParameterByName("metallicFactor");
        if (defaultMetallicFactor !== null)
            config.defaultMetallicFactor = parseFloat(defaultMetallicFactor);

        var defaultRoughnessFactor = this._getParameterByName("roughnessFactor");
        if (defaultRoughnessFactor !== null) {
            config.defaultRoughnessFactor = parseFloat(defaultRoughnessFactor);
        }

        return config;
    }

    /** @param {string} scsFile */
    _createScsViewer(divId, scsModelname) {
        /** @type {Communicator.WebViewerConfig} */
        var config = {
            containerId: divId
        };

        if (scsModelname) {
            config.endpointUri = `models/${scsModelname}.scs`;
        } else {
            config.empty = true;
        }

        this._applyExtraProperties(config);
        var viewer = new Communicator.WebViewer(config);

        return Promise.resolve(viewer);
    }

    /** @param {string} stylesheetUrl */
    _addStylesheet(stylesheetUrl) {
        var link = document.createElement("link");
        link.setAttribute("rel", "stylesheet");
        link.setAttribute("type", "text/css");
        link.setAttribute("href", stylesheetUrl);
        document.getElementsByTagName("head")[0].appendChild(link);
    }

    //screenConfiguration: Communicator.ScreenConfiguration.Desktop,

    _checkforMobile() {
        var layout = this._getLayout();
        if (layout === "mobile") {
            this._addStylesheet("css/Mobile.css");
            this.screenConfiguration = Communicator.ScreenConfiguration.Mobile;
        } else {
            this.screenConfiguration = Communicator.ScreenConfiguration.Desktop;
        }
    }

    createViewer(divId, scMode, modelname) {
        this._checkforMobile();

        // Todo: Add logic to check for different SC workflow types
        return this._createScsViewer(divId, modelname);
    }


}