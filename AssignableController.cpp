#include "AssignableController.h"

AssignableControllers AssignableControllers::instance;

void AssignableControllers::setup()
{
  // Default controls
  mControllers[DATA]  =new AssignableController(DATA,   0,0,159,154);
  mControllers[SLIDER]=new AssignableController(SLIDER, 0,0,147,147);
  mControllers[WHEEL1]=new AssignableController(WHEEL1, 0,0,147,146);
  mControllers[WHEEL2]=new AssignableController(WHEEL2, 0,0,147,1);
  mControllers[PEDALA]=new AssignableController(PEDALA, 0,0,151,64);
  mControllers[PEDALB]=new AssignableController(PEDALB, 0,0,151,67);
  mControllers[R1]    =new AssignableController(R1,     0,0,159,152);
  mControllers[R2]    =new AssignableController(R2,     0,0,159,153);
  mControllers[R3]    =new AssignableController(R3,     0,0,159,156);
  mControllers[R4]    =new AssignableController(R4,     0,0,159,157);
  mControllers[R5]    =new AssignableController(R5,     1,0,159,7);
  mControllers[R6]    =new AssignableController(R6,     2,0,159,7);
  mControllers[R7]    =new AssignableController(R7,     3,0,159,7);
  mControllers[R8]    =new AssignableController(R8,     4,0,159,7);
}
