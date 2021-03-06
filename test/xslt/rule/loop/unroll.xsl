<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="*" mode="loop_unroll">
    <xsl:param name="max" />
    <xsl:param name="var" />

    <xsl:choose>
      <xsl:when test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $var">
        <xsl:apply-templates select="." mode="unroll_target_loop">
          <xsl:with-param name="max" select="$max" />
          <xsl:with-param name="var" select="$var" />
        </xsl:apply-templates>
      </xsl:when>
      <xsl:otherwise>
        <xsl:copy>
          <xsl:copy-of select="@*" />
          <xsl:apply-templates mode="loop_unroll">
            <xsl:with-param name="max" select="$max" />
            <xsl:with-param name="var" select="$var" />
          </xsl:apply-templates>
        </xsl:copy>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

  <xsl:template match="SgFortranDo" mode="unroll_target_loop">
    <xsl:param name="max" />
    <xsl:param name="var" />
    <xsl:copy>
      <xsl:copy-of select="@*" />
      <!-- begin -->
      <xsl:copy-of select="./*[1]" />
      <!-- end -->
      <xsl:copy-of select="./*[2]" />
      <!-- step -->
      <xsl:element name="SgIntVal">
        <xsl:attribute name="value">
          <xsl:value-of select="$max" />
        </xsl:attribute>
      </xsl:element>

      <xsl:apply-templates select="./SgBasicBlock" mode="loop_unroll">
        <xsl:with-param name="max" select="$max" />
        <xsl:with-param name="var" select="$var" />
      </xsl:apply-templates>

    </xsl:copy>
  </xsl:template>

  <!-- TODO: ExprStatement -> BasicBlock -->
  <xsl:template match="SgExprStatement[last()]" mode="loop_unroll">
    <xsl:param name="max" /> <!-- STEP数 -->
    <xsl:param name="var" /> <!-- 置き換える変数 -->

    <!-- 自分を出力する -->
    <xsl:apply-templates select="." />


    <!-- コピーする【SgExprStatement】行を設定 -->
    <!-- <xsl:param name="copy_stm" select="../SgExprStatement" /> OK for libxml -->
    <xsl:variable name="copy_stm" select="../SgExprStatement" />
    <!-- <xsl:variable name="copy_stm" select="." /> -->

    <!-- <xsl:apply-templates select="$copy_stm" /> -->

    <!-- コピーする【SgExprStatement】行を設定 -->
    <xsl:for-each select="(//*)[position() &lt; $max]">
      <xsl:apply-templates select="$copy_stm" mode="loop_unroll_body">
        <xsl:with-param name="max" select="$max" />
        <xsl:with-param name="var" select="$var" />
        <xsl:with-param name="cnt" select="position()" />
      </xsl:apply-templates>
    </xsl:for-each>

  </xsl:template>


  <xsl:template match="SgVarRefExp" mode="loop_unroll_body">
    <xsl:param name="max" />
    <xsl:param name="var" />
    <xsl:param name="cnt" />

    <xsl:if test="./@name=$var">
      <SgAddOp>
        <xsl:copy-of select="." />
        <SgIntVal>
          <xsl:attribute name="value">
            <xsl:value-of select="$cnt" />
          </xsl:attribute>
        </SgIntVal>
      </SgAddOp>
    </xsl:if>

    <xsl:if test="./@name!=$var">
      <xsl:copy-of select="." />
    </xsl:if>
  </xsl:template>

  <xsl:template match="*" mode="loop_unroll_body">
    <xsl:param name="max" />
    <xsl:param name="var" />
    <xsl:param name="cnt" />
    <xsl:copy>
      <xsl:copy-of select="@*" />
      <xsl:apply-templates mode="loop_unroll_body">
        <xsl:with-param name="max" select="$max" />
        <xsl:with-param name="var" select="$var" />
        <xsl:with-param name="cnt" select="$cnt" />
      </xsl:apply-templates>
    </xsl:copy>
  </xsl:template>


  <!-- SgPragmaDeclaration removal -->
  <xsl:template match="SgPragmaDeclaration" mode="loop_unroll_Epilog">
  </xsl:template>
  <!-- PreprocessingInfo removal -->
  <xsl:template match="PreprocessingInfo" mode="loop_unroll_Epilog">
  </xsl:template>

  <!-- keep unchanged -->
  <xsl:template match="*" mode="loop_unroll_Epilog">
    <xsl:copy>
      <xsl:copy-of select="@*" />
      <xsl:apply-templates mode="loop_unroll_Epilog" />
    </xsl:copy>
  </xsl:template>
  <!--+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->

  <!-- remove PreprocessingInfo -->
  <xsl:template match="PreprocessingInfo" mode="loop_unroll">
  </xsl:template>
  <!-- <xsl:template match="SgPragmaDeclaration[1]" mode="loop_unroll"> </xsl:template> -->

</xsl:stylesheet>
