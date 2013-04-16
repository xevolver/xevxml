#ifndef ___XEVXML_H___
#define ___XEVXML_H___
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <rose.h>


#define ABORT()			 {				\
    std::cerr << "ERROR @ " << __func__ << " :";		\
    std::cerr << __FILE__ ;					\
    std::cerr << "(" << __LINE__ << "): " << std::endl;		\
    abort();							\
  }

extern void XmlInitialize(void);
extern void XmlFinalize(void);

#endif
