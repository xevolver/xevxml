<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" encoding="UTF-8" />
  
  <xsl:template match="/">
    <xsl:apply-templates />
  </xsl:template>
  
  <!-- remove SgPragmaDeclaration -->
  <xsl:template match="SgPragmaDeclaration">
  </xsl:template>
  
  <xsl:template match="*">
    <xsl:copy>
      <xsl:copy-of select="@*" />
      <xsl:apply-templates />
    </xsl:copy>
  </xsl:template>
<!--      <xsl:when test="count(child::PreprocessingInfo)=1"> -->
<!-- NG     <xsl:when test="preceding::SgPragma/DIRECTIVE[@name='loop']/CLAUSE/@name='interchange'"> -->
<!-- OK     <xsl:when test="preceding::SgPragma/DIRECTIVE/@name='loop'"> -->
<!-- preceding-sibling::*[1]   -->
  
  <xsl:template match="SgFortranDo">
    <xsl:choose>
     <xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop']/CLAUSE[@name='interchange']/ARG/@value='1'">
	<xsl:element name="SgFortranDo">
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" />
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp" />
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgIntVal" />
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgNullExpression" />
	  <xsl:element name="SgBasicBlock">
	    <xsl:copy-of select="SgBasicBlock/@*" />
	    <xsl:copy>
	      <xsl:copy-of select="@*" />
	      <xsl:copy-of select="./SgAssignOp" />
	      <xsl:copy-of select="./SgIntVal" />
	      <xsl:copy-of select="./SgNullExpression" />
	      <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock" />
	    </xsl:copy>
	  </xsl:element>
	</xsl:element>
      </xsl:when>
      
     <xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop']/CLAUSE[@name='interchange']/ARG/@value='2'">
	<xsl:element name="SgFortranDo">
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/@*" />
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/SgAssignOp" />
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/SgIntVal" />
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/SgNullExpression" />
	  <xsl:element name="SgBasicBlock">
	    <xsl:copy-of select="SgBasicBlock/@*" />
	<xsl:element name="SgFortranDo">
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" />
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp" />
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgIntVal" />
	  <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgNullExpression" />
	  <xsl:element name="SgBasicBlock">
	    <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock/@*" />
	    
	    <xsl:copy>
	      <xsl:copy-of select="@*" />
	      <xsl:copy-of select="./SgAssignOp" />
	      <xsl:copy-of select="./SgIntVal" />
	      <xsl:copy-of select="./SgNullExpression" />
	      <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/SgBasicBlock" />
	    </xsl:copy>
	  </xsl:element>
	</xsl:element>
	</xsl:element>
	</xsl:element>
      </xsl:when>
      
      <xsl:otherwise>
	<xsl:copy>
	  <xsl:copy-of select="@*" />
	  <xsl:apply-templates />
	</xsl:copy>
	
      </xsl:otherwise>
    </xsl:choose>
    
  </xsl:template>
  
</xsl:stylesheet>
