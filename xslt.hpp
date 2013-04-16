#ifndef ___XSLT_H___
#define ___XSLT_H___

#include "xevxml.hpp"

extern void XsltTransform(std::stringstream& istr, 
			  std::stringstream& ostr, 
			  std::string xsltfn="example.xsl"); 

#endif
