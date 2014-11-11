/**
 * \license This project is released under the BSD 2-clause license
 *
 * Copyright (C) 2010-2013 Hiroyuki TAKIZAWA. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *    
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef ___XEV_XMLUTILS_H___
#define ___XEV_XMLUTILS_H___
#include <xevxml.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>


namespace XevXML {
// --- XML utility functions ---
extern void XmlInitialize(void);
extern void XmlFinalize(void);
extern std::string XmlStr2Entity( std::string);
extern std::string XmlEntity2Str( std::string);
extern bool XmlWriteToString( xercesc::DOMNode* node, std::stringstream& str );

//extern bool XmlGetAttributeValue( xercesc::DOMNode* node, const char* name, std::string* val);

template <typename Tp_>
bool XmlGetAttributeValue( xercesc::DOMNode* node, const char* name, Tp_* val)
{
  xercesc::DOMNamedNodeMap* amap = node->getAttributes();
  xercesc::DOMNode*         att  = 0;
  XMLCh* xbuf;
  char* cbuf;
  std::stringstream str;
  Tp_ tmp;

  if(amap) {
    xbuf = xercesc::XMLString::transcode(name);
    att=amap->getNamedItem(xbuf);
    xercesc::XMLString::release(&xbuf);
  }
  if(att && val) {
    cbuf = xercesc::XMLString::transcode(att->getNodeValue());
    str << cbuf;
    xercesc::XMLString::release(&cbuf);
    str >> tmp;
    *val = tmp;
    return true;
  }
  return false;
}

template <>
bool XmlGetAttributeValue( xercesc::DOMNode* node, const char* name, std::string* val);

template <typename Tp_>
bool XmlGetAttributeValue( xercesc::DOMNode* node, std::string& name, Tp_* val)
{
  return XmlGetAttributeValue(node,name.c_str(),val);
}

std::string XmlGetNodePosition(xercesc::DOMNode* node);
}

#endif
