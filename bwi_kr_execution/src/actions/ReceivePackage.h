
#ifndef bwi_krexec_ReceivePackage_h__guard
#define bwi_krexec_ReceivePackage_h__guard

#include "actasp/Action.h"

#include <ros/ros.h>

#include <string>

namespace bwi_krexec {

class ReceivePackage : public actasp::Action{
public:
  ReceivePackage();

  int paramNumber() const {return 2;}
  
  std::string getName() const {return "receivepackage";}
  
  void run();
  
  bool hasFinished() const {return done;}
  
  bool hasFailed() const {return failed;}
  
  actasp::Action *cloneAndInit(const actasp::AspFluent & fluent) const;
  
  virtual actasp::Action *clone() const {return new ReceivePackage(*this);}
  
private:
  
 std::vector<std::string> getParameters() const;
 std::string current_room;
 std::string package;
 bool done,failed;
 
};

}
 
#endif
 
