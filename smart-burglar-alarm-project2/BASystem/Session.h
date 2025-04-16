#ifndef Session_H
#define Session_H

class Session
{
  private:
  bool idleState;
  int serviceNumber;

  public:
  void checkIdleState(int conButton);
  int enterSession(int * idleConstantpointer,int * conButtonpointer);
  int manageServiceOptions(int * idleConstantpointer,int * conButtonpointer);
  int manageSession(int serviceNumber);
  void Arm();
  void Disarm();

};




#endif