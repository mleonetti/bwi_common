#include "ReceivePackage.h"

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

ReceivePackage::ReceivePackage() : 
            current_room(),
            package(),
            done(false),
            failed(false){
            }

  
void ReceivePackage::run() {

  ros::NodeHandle n;

  vector<string> options;
  options.push_back("Loaded");

  CallGUI receivePackage("receivePackage", CallGUI::CHOICE_QUESTION,  "Please place the package on me. Press 'Loaded' when the package is in place.", 60.0, options);
  receivePackage.run();

  int response = receivePackage.getResponseIndex();

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
  fluent.variables.push_back(package);

  fluent.name = ((response == 0) ? "received" : "-received");

  uf.request.fluents.push_back(fluent);
  krClient.call(uf);

  done = true;

}  
  
actasp::Action* ReceivePackage::cloneAndInit(const actasp::AspFluent& fluent) const {
  ReceivePackage *newAction = new ReceivePackage();
  newAction->current_room = fluent.getParameters().at(0);
  newAction->package = fluent.getParameters().at(1);
  
  return newAction;
}

std::vector<std::string> ReceivePackage::getParameters() const {
  vector<string> param;
  param.push_back(current_room);
  param.push_back(package);
  return param;
}


ActionFactory ReceivePackageFactory(new ReceivePackage());
  
}
