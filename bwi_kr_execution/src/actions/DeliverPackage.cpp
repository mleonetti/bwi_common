#include "DeliverPackage.h"

#include "ActionFactory.h"

#include "CallGUI.h"

#include "bwi_kr_execution/AspFluent.h"

#include "bwi_kr_execution/CurrentStateQuery.h"
#include <bwi_kr_execution/UpdateFluents.h>

#include <ros/ros.h>

#include <string>
#include <iostream>

using namespace std;

namespace bwi_krexec {

DeliverPackage::DeliverPackage() : 
            current_room(),
            delivery_room(),
            package(),
            done(false),
            failed(false){
            }

  
void DeliverPackage::run() {

  ros::NodeHandle n;

  vector<string> options;
  options.push_back("Delivered");

  CallGUI deliverPackage("deliverPackage", CallGUI::CHOICE_QUESTION,  "I have a package for you. Press 'Delivered' when retrieved.", 60.0, options);
  deliverPackage.run();

  int response = deliverPackage.getResponseIndex();

  if (response >= 0) {
    CallGUI thank("thank", CallGUI::DISPLAY,  "Thank You!");
    thank.run();
  }
  else{
    failed = true;
  }

  ros::ServiceClient krClient = n.serviceClient<bwi_kr_execution::UpdateFluents> ( "update_fluents" );
  krClient.waitForExistence();

  bwi_kr_execution::UpdateFluents uf;
  bwi_kr_execution::AspFluent fluent;
  fluent.timeStep = 0;
  fluent.variables.push_back(current_room);
  fluent.variables.push_back(delivery_room);
  fluent.variables.push_back(package);

  fluent.name = ((response == 0) ? "delivered" : "-delivered");

  uf.request.fluents.push_back(fluent);
  krClient.call(uf);

  done = true;

}  
  
actasp::Action* DeliverPackage::cloneAndInit(const actasp::AspFluent& fluent) const {
  DeliverPackage *newAction = new DeliverPackage();
  newAction->current_room = fluent.getParameters().at(0);
  newAction->delivery_room = fluent.getParameters().at(1);
  newAction->package = fluent.getParameters().at(2);
  
  return newAction;
}

std::vector<std::string> DeliverPackage::getParameters() const {
  vector<string> param;
  param.push_back(current_room);
  param.push_back(delivery_room);
  param.push_back(package);
  return param;
}


ActionFactory DeliverPackageFactory(new DeliverPackage());
  
}
