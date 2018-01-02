sap.ui.define([
	"sap/ui/core/mvc/Controller",
	"sap/ui/core/routing/History"
], function (Controller, History) {
	"use strict";

	return Controller.extend("sap.smartGarden.controller.BaseController", {
		
		getModel: function(sModelName) {
            return this.getView().getModel(sModelName);
        },

        setModel: function(oModel, sModelName) {
            this.getView().setModel(oModel, sModelName);
        },
        
		getRouter : function () {
			return sap.ui.core.UIComponent.getRouterFor(this);
		},

		onNavBack: function (oEvent) {
			var oHistory, sPreviousHash;

			oHistory = History.getInstance();
			sPreviousHash = oHistory.getPreviousHash();

			if (sPreviousHash !== undefined) {
				window.history.go(-1);
			} else {
				this.getRouter().navTo("appHome", {}, true /*no history*/);
			}
		},
		
		goHome: function(oEvent){
			// this.getRouter().getTargets().display("Home", {
			// 	fromTarget: "Home"
			// });
			this.getRouter().navTo("Home", {});
		}

	});

});