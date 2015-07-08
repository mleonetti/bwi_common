#include "bwi_kr_execution/ExecutePlanAction.h"
#include "bwi_kr_execution/CurrentStateQuery.h"
#include <actasp/AspFluent.h>
#include "actasp/AnswerSet.h"
#include "bwi_msgs/QuestionDialog.h"
#include <actionlib/client/simple_action_client.h>
#include <ros/ros.h>
#include <string>
#include <iostream>

typedef actionlib::SimpleActionClient<bwi_kr_execution::ExecutePlanAction> Client;

using namespace std;

 
struct IsFluentAt {
 
 bool operator()(const bwi_kr_execution::AspFluent& fluent) {
   return fluent.name == "at";
 }
 
};
 

int main(int argc, char**argv) {
  ros::init(argc, argv, "deliver_package");
  ros::NodeHandle n;
  
  ros::NodeHandle privateNode("~");
  string current_location;
  string package_location;
  string delivery_location;
  string package;
  privateNode.param<string>("pickup",package_location,"");
  privateNode.param<string>("delivery",delivery_location,"");
  //privateNode.param<string>("package",package,"p1");
  package = "p1";


  //current state query
  ros::ServiceClient currentClient = n.serviceClient<bwi_kr_execution::CurrentStateQuery> ( "current_state_query" );
  currentClient.waitForExistence();

  bwi_kr_execution::CurrentStateQuery csq;
  currentClient.call(csq);

  vector<bwi_kr_execution::AspFluent>::const_iterator atIt = 
                  find_if(csq.response.answer.fluents.begin(), csq.response.answer.fluents.end(), IsFluentAt());

  bool error = false;
                    
  if (atIt == csq.response.answer.fluents.end()) {
    ROS_ERROR("ReceivePackage: fluent \"at\" missing ");
    error = true;
  }
  else { 
    current_location = atIt->variables[0];
  }

  // cout << "Current Location is: " << current_location << endl;

  if (package_location == "") {
    package_location = current_location;
  }

  // cout << "Package Location is: " << package_location << endl;

  // Building Fluent Rule
  ros::ServiceClient gui_client = n.serviceClient<bwi_msgs::QuestionDialog>("question_dialog"); 

  ros::Duration(0.5).sleep(); 
  bwi_msgs::QuestionDialog srv;

  vector<std::string> options;
  options.push_back("Yes");
  options.push_back("No");

  srv.request.type = 1; 
  srv.request.message = "Do you need me to pick up the package from another location?"; 
  srv.request.options = options;
  srv.request.timeout = bwi_msgs::QuestionDialogRequest::NO_TIMEOUT; 
  gui_client.waitForExistence();
  gui_client.call(srv);

  if (srv.response.index == 0) {
    srv.request.type = 2;
    srv.request.message = "What is the package location? [ex. l3_414b or l3_418]"; 
    gui_client.call(srv);
    package_location = srv.response.text;
  }

  srv.request.type = 2;
  srv.request.message = "What is the delivery location? [ex. l3_414b or l3_418]"; 
  gui_client.call(srv);
  delivery_location = srv.response.text;
  
  Client client("action_executor/execute_plan", true);
  client.waitForServer();
  
  bwi_kr_execution::ExecutePlanGoal goal;
  
  bwi_kr_execution::AspRule rule;
  bwi_kr_execution::AspFluent fluent;
  fluent.name = "not delivered";
  
  fluent.variables.push_back(package_location);
  fluent.variables.push_back(delivery_location);
  fluent.variables.push_back(package);
 
  rule.body.push_back(fluent);
  goal.aspGoal.push_back(rule);
  
  ROS_INFO("sending goal");
  client.sendGoal(goal);
  
  ros::Rate wait_rate(10);
  while(ros::ok() && !client.getState().isDone()) {
    wait_rate.sleep();
  }


  if (!client.getState().isDone()) {
    ROS_INFO("Canceling goal");
    client.cancelGoal();
    //and wait for canceling confirmation
    for(int i = 0; !client.getState().isDone() && i<50; ++i)
      wait_rate.sleep();
  }
  if (client.getState() == actionlib::SimpleClientGoalState::ABORTED) {
    ROS_INFO("Aborted");
  }
  else if (client.getState() == actionlib::SimpleClientGoalState::PREEMPTED) {
    ROS_INFO("Preempted");
  }
  else if (client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
    ROS_INFO("Succeeded!");
  }
  else {
     ROS_INFO("Terminated");
  }
    
  return 0;
}
