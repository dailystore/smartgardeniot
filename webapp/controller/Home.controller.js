sap.ui.define([
	"sap/smartGarden/controller/BaseController",
	'sap/m/MessageToast',
	'sap/ui/model/json/JSONModel'
], function(BaseController, MessageToast, JSONModel) {
	"use strict";

	return BaseController.extend("sap.smartGarden.controller.Home", {
		onInit: function() {
			var oRouter = this.getRouter(),
				oModel = new JSONModel({
					treeOverview: {
						tem: 31,
						humi: 85,
						status: "watering"
					},
					block1: {
						header: "Garden 1",
						subHeader: "Tem/hu: 32.C - 80%.",
						footer: "500m2"
					},
					block2: {
						header: "Garden 2",
						subHeader: "Tem/hu: 33.C - 84%.",
						footer: "500m2"
					},
					mainLiquid: {
						header: "liquid volume",
						subHeader: "main water",
						unit: "l",
						footer: "watering",
						icon: "sap-icon://line-charts",
						volume: 1730,
						status: "watering",
						value: 1720,
						scale: "M",
						valueColor: "Good",
						indicator: "Up"
					}
				});
			this.setModel(oModel, "gardenOverview");
			oRouter.getRoute("appHome").attachMatched(this._onRouteMatched, this);
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
			// this.getRouter().getTargets().display("gardenOverview", {
			// 	fromTarget: "Home"
			// });
			// MessageToast.show("The GenericTile is pressed.");
			if (evt && evt.getSource() && evt.getSource().getAriaLabel()) {
				var gardenName = evt.getSource().getAriaLabel();
				this.handleNavToGardenDetail(gardenName);
			}
		},

		handleHomePress: function(evt) {
			this.goHome();
		},

		handleNavToGardenDetail: function(evt) {
			this.getRouter().navTo("gardenDetail", {
				gardenName: evt
			});
		}

	});

});