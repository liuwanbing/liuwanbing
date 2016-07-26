#include "typeDef.h"
#include "InterFaceFactory.h"

#ifdef SER_WIN
SERAPI
#endif
IMainGameFactory*  GetMainGameFactory(void);