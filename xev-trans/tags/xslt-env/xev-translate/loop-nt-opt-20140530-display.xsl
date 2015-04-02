







<!-- with label: 17lines -->

<xsl:template match="*" mode="nt_opt">
 <xsl:choose>
  <xsl:when test="self::SgFortranDo/SgAssignOp/SgVarRefExp[@name='I']">
   DO <xsl:value-of select="self::SgFortranDo/@nlabel" /> I=1,inum
    !$acc loop seq
    DO L=lstart,lend
     IF (I.ge.IS(L) .and. I.le.IT(L)) EXIT
    END DO
    <xsl:apply-templates select="SgBasicBlock" />
  </xsl:when>
  <xsl:otherwise>
   <xsl:copy>
    <xsl:copy-of select="@*" />
    <xsl:apply-templates mode="nt_opt" />
   </xsl:copy>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>










<!-- no label: 19lines -->

<xsl:template match="*" mode="nt_opt">
 <xsl:choose>
  <xsl:when test="self::SgFortranDo/SgAssignOp/SgVarRefExp[@name='I']">
   DO I=1,inum
   !$acc loop seq
   DO L=lstart,lend
   IF (I.ge.IS(L) .and. I.le.IT(L)) EXIT
   END DO
   <xsl:apply-templates select="SgBasicBlock" />
   END DO
  </xsl:when>
  <xsl:otherwise>
   <xsl:copy>
    <xsl:copy-of select="@*" />
    <xsl:apply-templates mode="nt_opt" />
   </xsl:copy>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>










<xsl:template match="SgFortranDo" mode="nt_opt">
 <xsl:param name="start" />
 <xsl:param name="end" />
 <xsl:copy>
  <xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" /> <!-- SgFortranDo -->
  <xsl:element name="SgAssignOp">
   <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp[1]" />
   <xsl:element name="SgIntVal">
    <xsl:attribute name="value">
     <xsl:value-of select="$start" />
    </xsl:attribute>
   </xsl:element>
  </xsl:element>
  <xsl:element name="SgVarRefExp">
   <xsl:attribute name="name">
    <xsl:value-of select="$end" />
   </xsl:attribute>
  </xsl:element>
  <xsl:copy-of select="SgNullExpression" />
  <xsl:element name="SgBasicBlock">
   <xsl:element name="SgFortranDo">
    <xsl:copy-of select="@*" /> <!-- SgFortranDo -->
    <xsl:element name="SgAssignOp">
     <xsl:element name="SgVarRefExp">
      <xsl:attribute name="name">
       <xsl:value-of select="SgAssignOp/SgVarRefExp[1]/@name" />
      </xsl:attribute>
     </xsl:element>
     <xsl:element name="SgVarRefExp">
      <xsl:attribute name="name">
       <xsl:value-of select="SgAssignOp/SgVarRefExp[2]/@name" />
      </xsl:attribute>
     </xsl:element>
    </xsl:element>
    <xsl:element name="SgVarRefExp">
     <xsl:attribute name="name">
      <xsl:value-of select="SgVarRefExp/@name" />
     </xsl:attribute>
    </xsl:element>
    <xsl:element name="SgNullExpression"></xsl:element>
    <xsl:element name="SgBasicBlock">
     <xsl:call-template name="if-filter">
      <xsl:with-param name="checkIndex" select="path to the index to filter" />
      <xsl:with-param name="arrayStart" select="path to the array of start indices" />
      <xsl:with-param name="arrayEnd" select="path to the array of end indices" />
      <xsl:with-param name="arrayIndex" select="path to the index name of the array of start and end" />
     </xsl:call-template>
     <xsl:apply-templates select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgExprStatement" />
    </xsl:element>
   </xsl:element>
  </xsl:element>
 </xsl:copy>
</xsl:template>


<xsl:template match="SgFortranDo_20140529" mode="nt_opt">
 <xsl:param name="start" />
 <xsl:param name="end" />
 <xsl:copy>
  <xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" /> <!-- SgFortranDo -->
  <xsl:element name="SgAssignOp">
   <xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp[1]" />
   <xsl:element name="SgIntVal">
    <xsl:attribute name="value">
     <xsl:value-of select="$start" />
    </xsl:attribute>
   </xsl:element>
  </xsl:element>
  <xsl:element name="SgVarRefExp">
   <xsl:attribute name="name">
    <xsl:value-of select="$end" />
   </xsl:attribute>
  </xsl:element>
  <xsl:copy-of select="SgNullExpression" />
  <xsl:element name="SgBasicBlock">
   <xsl:element name="SgFortranDo">
    <xsl:copy-of select="@*" /> <!-- SgFortranDo -->
    <xsl:element name="SgAssignOp">
     <xsl:element name="SgVarRefExp">
      <xsl:attribute name="name">
       <xsl:value-of select="SgAssignOp/SgVarRefExp[1]/@name" />
      </xsl:attribute>
     </xsl:element>
     <xsl:element name="SgVarRefExp">
      <xsl:attribute name="name">
       <xsl:value-of select="SgAssignOp/SgVarRefExp[2]/@name" />
      </xsl:attribute>
     </xsl:element>
    </xsl:element>
    <xsl:element name="SgVarRefExp">
     <xsl:attribute name="name">
      <xsl:value-of select="SgVarRefExp/@name" />
     </xsl:attribute>
    </xsl:element>
    <xsl:element name="SgNullExpression"></xsl:element>
    <xsl:element name="SgBasicBlock">
     <xsl:call-template name="if-filter">
      <xsl:with-param name="checkIndex" select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp[1]/@name" />
      <xsl:with-param name="arrayStart"
       select="SgBasicBlock/SgExprStatement[1]/SgAssignOp/SgFunctionCallExp/SgFunctionRefExp/@symbol" />
      <xsl:with-param name="arrayEnd"
       select="SgBasicBlock/SgExprStatement[2]/SgAssignOp/SgFunctionCallExp/SgFunctionRefExp/@symbol" />
      <xsl:with-param name="arrayIndex"
       select="SgBasicBlock/SgExprStatement[1]/SgAssignOp/SgFunctionCallExp/SgExprListExp/SgVarRefExp/@name" />
     </xsl:call-template>
     <xsl:apply-templates select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgExprStatement" />
    </xsl:element>
   </xsl:element>
  </xsl:element>
 </xsl:copy>
</xsl:template> 

    