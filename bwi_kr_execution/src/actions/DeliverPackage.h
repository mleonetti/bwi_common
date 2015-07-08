
#ifndef bwi_krexec_DeliverPackage_h__guard
#define bwi_krexec_DeliverPackage_h__guard

#include "actasp/Action.h"

#include <ros/ros.h>

#include <string>

namespace bwi_krexec {

class DeliverPackage : public actasp::Action{
public:
  DeliverPackage();

  int paramNumber() const {return 3;}
  
  std::string getName() const {return "deliverpackage";}
  
  void run();
  
  bool hasFinished() const {return done;}
  
  bool hasFailed() const {return failed;}
  
  actasp::Action *cloneAndInit(const actasp::AspFluent & fluent) const;
  
  virtual actasp::Action *clone() const {return new DeliverPackage(*this);}
  
private:
  
 std::vector<std::string> getParameters() const;
 std::string current_room;
 std::string delivery_room;
 std::string package;
 bool done,failed;
 
};

}
 
#endif
 
