#ifndef transitionTime_H
#define transitionTime_H
class transitionTime
{
protected:
  int TransitionTime;
  int tLength;
public:
  transitionTime();
  void setTransitionTime(int[], int);
  int getTransitionTime();
  int getTransitionTimeLength();
  int validateTransitionTime(int enteredTime[],int timeLength);
  int validateLength(int);
  void changeTransitionTime();
};
#endif
