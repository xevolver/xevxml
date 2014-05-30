<xsl:template match="SgFortranDo" mode="nt_opt"><xsl:param name="start" /><xsl:param name="end" />
 <xsl:copy><xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" /> <!-- SgFortranDo -->
  <xsl:element name="SgAssignOp">
   <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp[1]" />
   <xsl:element name="SgIntVal"><xsl:attribute name="value"><xsl:value-of select="$start" />
   </xsl:attribute></xsl:element></xsl:element>
  <xsl:element name="SgVarRefExp"><xsl:attribute name="name"><xsl:value-of select="$end" />
   </xsl:attribute></xsl:element>
  <xsl:copy-of select="SgNullExpression" />
  <xsl:element name="SgBasicBlock">
   <xsl:element name="SgFortranDo">
    <xsl:copy-of select="@*" /> <!-- SgFortranDo -->
    <xsl:element name="SgAssignOp">
     <xsl:element name="SgVarRefExp"><xsl:attribute name="name">
      <xsl:value-of select="SgAssignOp/SgVarRefExp[1]/@name" /></xsl:attribute></xsl:element>
     <xsl:element name="SgVarRefExp"><xsl:attribute name="name">
       <xsl:value-of select="SgAssignOp/SgVarRefExp[2]/@name" /></xsl:attribute></xsl:element>
    </xsl:element>
    <xsl:element name="SgVarRefExp"><xsl:attribute name="name">
      <xsl:value-of select="SgVarRefExp/@name" /></xsl:attribute></xsl:element>
    <xsl:element name="SgNullExpression"></xsl:element>
    <xsl:element name="SgBasicBlock">
     <xsl:call-template name="if-filter">
      <xsl:with-param name="checkIndex" select="find the index to filter" />
      <xsl:with-param name="arrayStart" select="find the array of start indices" />
      <xsl:with-param name="arrayEnd" select="find the array of end indices" />
      <xsl:with-param name="arrayIndex" select="find the index array index name" />
     </xsl:call-template>
     <xsl:apply-templates select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgExprStatement" />
    </xsl:element></xsl:element></xsl:element></xsl:copy></xsl:template>
