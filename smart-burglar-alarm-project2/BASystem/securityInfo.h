#ifndef securityInfo_H
#define securityInfo_H
class securityInfo
{
protected:
  unsigned long contactNum;
public:
  securityInfo(unsigned long);
  void setContactDetails(int[], int);
  long getContactNumber();
  int validateLength(int);
  int validateContactNumber(int[]);
  void changeContactDetails();
};
#endif
