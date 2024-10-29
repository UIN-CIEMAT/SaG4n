#ifndef SAG4NSTEPPINGACTION_HH
#define SAG4NSTEPPINGACTION_HH 1


#include "G4UserSteppingAction.hh"
#include "SaG4nEventAction.hh"



class SaG4nSteppingAction : public G4UserSteppingAction
{
public:

  SaG4nSteppingAction();
  ~SaG4nSteppingAction();

  void UserSteppingAction(const G4Step* aStep);
    
private:

  SaG4nEventAction* theEventAction;  
};



#endif
