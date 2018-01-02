sap.ui.define([
	"sap/smartGarden/controller/BaseController",
	'sap/m/MessageToast',
	'sap/ui/model/json/JSONModel'
], function(BaseController, MessageToast, JSONModel) {
	"use strict";

	return BaseController.extend("sap.smartGarden.controller.gardenDetail", {
		onInit: function() {
			var oRouter = this.getRouter(),
				oModel = new JSONModel({
					treeOverview: {
						tem: 31,
						humi: 85,
						status: "watering"
					},
					block1: {
						header: "Instant 1",
						subHeader: "Tem/hu: 32.C - 80%.",
						cover: "125m2"
					},
					block2: {
						header: "Instant 2",
						subHeader: "Tem/hu: 33.C - 84%.",
						cover: "125m2"
					},
					block3: {
						header: "Instant 3",
						subHeader: "Tem/hu: 32.C - 80%.",
						cover: "125m2"
					},
					block4: {
						header: "Instant 4",
						subHeader: "Tem/hu: 33.C - 84%.",
						cover: "125m2"
					}
				});
			this.setModel(oModel, "gardenDetail");
			oRouter.getRoute("gardenDetail").attachMatched(this._onRouteMatched, this);
		},
		
		_onRouteMatched: function(oEvent) {
            this.oArgs = oEvent.getParameter("arguments");
            this.oArgs.sRequest = this.oArgs.tileName;
            this.oArgs.option = this.oArgs["?option"] || {};
            // this.createChartMetadata(this);
            // this._thinking();
        },
        
		onDisplayNotFound: function(oEvent) {
			// display the "notFound" target without changing the hash
			this.getRouter().getTargets().display("notFound", {
				fromTarget: "home"
			});
		},

		press: function(evt) {
			this.getRouter().getTargets().display("gardenOverview", {
				fromTarget: "gardenDetail"
			});
			// MessageToast.show("The GenericTile is pressed.");
		},
		
		handleHomePress: function(evt){
			this.goHome();
		}

	});

});