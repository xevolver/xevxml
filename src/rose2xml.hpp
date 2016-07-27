/**
 * @file     rose2xml.hpp
 * @brief    ROSE Sage III AST is converted to an XML document.
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

#ifndef ___ROSE2XML_HPP___
#define ___ROSE2XML_HPP___
#include "common.hpp"
#include <xevxml.hpp>
#include <xmlutils.hpp>

namespace XevXml {

#define XEV_PRAGMA_PREFIX "!$xev"
#define XEV_PRAGMA_PREFIX_A "*$xev"
#define XEV_PRAGMA_PREFIX_C "c$xev"
#define XEV_ENCODE "XEV_ENCODE"

/*
 * creates SgPramgaDeclaration elements from Fortran comments.
 */
extern void
writeFortranPragma(XevXml::XevSageVisitor* visitor, SgNode* node,
    PreprocessingInfo::RelativePositionType pos=PreprocessingInfo::before, SgFile* =NULL);

/*
 * writes Attached Preprocessing Info of each SgNode node to XML.
 */
extern AttachedPreprocessingInfoType*
writePreprocessingInfo(std::ostream& sstr_,SgNode* n);
} // XevXml
#endif
