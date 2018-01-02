sap.ui.define([
	"sap/ui/core/UIComponent"
], function (UIComponent) {
	"use strict";

	return UIComponent.extend("sap.smartGarden.Component", {

		metadata: {
			manifest: "json",
			includes: [
                "style/style.css"
            ]
		},

		init: function () {
			// call the init function of the parent
			UIComponent.prototype.init.apply(this, arguments);

			// create the views based on the url/hash
			this.getRouter().initialize();
		}

	});

});