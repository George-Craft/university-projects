#ifndef Interface_H
#define Interface_H
class Interface
{
  private:
    int serviceNumber; // dont need??
  public:
    int if_available();
    void loop_until_one(int);
    void sendMatlabAorB(int); //11 OR 12
    void sendMatlabCorD(int); //61 OR 62
    void sendMatlabEorF(int); //63 OR 64
    void sendMatlabG(int); //66
    void sendMatlabXorY(int);
    void sendChar(char);
    void sendInt(int);
    void flush();
    int readMATLAB();
};
#endif