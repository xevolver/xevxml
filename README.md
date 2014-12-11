Xevolver XML Framework -- XevXml
=================================

XevXml is a framework for user-defined code transformations based on
XML. XevXml provides interconversion between an abstract syntax tree
(AST) of a C/Fortran code and an XML document. The XML document is
exposed to users. Hence, the users can use any XML tools for
user-defined AST transformations, and then generate a transformed
version of the C/Fortran code.

![Xevolver Logo](./Xevolver_logo.png "Xevolver")

Description
------------
  Xevolver XML (XevXml) is one of software products developed by the
Xevolver project supported by JST "Post-Peta" CREST.  The purpose of
this project is to help migration of legacy HPC applications to
new-generation systems by improving their performance portabilities
across system generations.  Since the top priority is usually given to
performance, an HPC application is often optimized and specialized for
a particular HPC system.  As a result, the performance is not portable
to other systems.  To make matters worse, such system-aware,
system-specific code optimizations are likely to be scattered over the
application code. This is one main reason why HPC application
migration is so painful. It is not affordable to reoptimize the whole
code whenever a new system becomes available.

XevXml is developed for XML-based AST transformations as an easy way
to provide user-defined code transformations.  The current
implementation of XevXml is built on top of the ROSE compiler
framework. XevXml converts a ROSE's AST to an XML document, and
exposes it to programmers. So the programmers can use any XML-related
technologies and tools to transform the AST. Then, the transformed AST
is given back to the ROSE compiler framework so that the AST is
unparsed to generate a transformed application code.  Instead of
directly modifying an application code, programmers can define their
own code transformations to optimize the code for each system.
System-specific optimizations are represented as XML translation
rules, which can be defined separately from an application code.  This
leads to separation between application requirements and system
requirements, expecting a lower migration cost of HPC applications to
new systems.

Requirements
------------

* ROSE compiler infrastructure -- http://rosecompiler.org/
* Apache Xerces C++ 3.1.1 -- http://xerces.apache.org/
* Apache Xalan C++ 1.0 -- http://xml.apache.org/xalan-c/

Usage
-----

 XevXml provides two basic commands, `src2xml` and `xml2src`. The
former converts a C/Fortran code into an XML document, which is output
to the standard output. The latter reads an XML document from the
standard input and unparses it to generate a C/Fortran code. For
example, equivalent transformation of `sample.c` to `sample-equiv.c`
is as follows.

```
    % src2xml sample.c | xml2src sample-equiv.c
```

Here, `src2xml` generates an XML document that contains necessary
information for rebuilding the AST. Each XML element corresponds one
AST node of Sage III AST classes that are used in the ROSE compiler
framework. An XML element may have some attributes. For example, an
AST node of a function declaration, `SgFunctionDefinition`, needs the
function name at rebuilding.

If a translation rule is written in an XSLT file, `test.xsl`, the rule
is applied to an AST by

```
    % src2xml sample.c | xsltexec test.xsl| xml2src sample-equiv.c
```

To append other information to XML documents, a developer can
customize two internal C++ classes, `XevSageVisitor` and
`XevXmlVisitor`. The former class traverses an AST of Sage III classes
used in ROSE, and translates it to an XML AST. The latter traverses an
XML AST to rebuild ROSE's AST.

Whenever `XevSageVisitor` visits an AST node, it writes an XML element
whose name is the same as the class name of an AST node,
e.g. `SgVarRefExp`. When writing XML attributes of each XML element,
`attribSgXXXX` method is invoked. Similarily, when wiring the child
nodes of each XML element, `inodeSgXXXX` method is invoked.Therefore,
by overloading those methods, a developer can customize XML documents
generated by the `XevSageVisitor` class.

Whenever `XevXmlVisitor` visits an XML element, `visigSgXXXX` method
is invoked. Therefore, by overloading such a method, a developer can
customize the rebuliding process of an AST.


License
-------
This project is released under the BSD 2-clause license

Copyright (C) 2010-2014 Hiroyuki TAKIZAWA. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:


 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
