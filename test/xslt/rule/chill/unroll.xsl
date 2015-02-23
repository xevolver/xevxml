<?xml version="1.0" encoding="UTF-8"?>

<!-- CHiLL compatible translation library -->

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="SgFortranDo" mode="chill_unroll_trans">
    <xsl:param name="max" />
    <xsl:param name="var" />
    <xsl:comment>
      libCHiLL.xsl chill_unroll
    </xsl:comment>
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

  <xsl:template match="*" mode="chill_unroll">
    <xsl:param name="max" />
    <xsl:param name="var" />
    <xsl:choose>
      <xsl:when test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $var">
        <xsl:apply-templates select="." mode="chill_unroll_trans">
          <xsl:with-param name="max" select="$max" />
          <xsl:with-param name="var" select="$var" />
        </xsl:apply-templates>
      </xsl:when>
      <xsl:otherwise>
        <xsl:copy>
          <xsl:copy-of select="@*" />
          <xsl:apply-templates mode="chill_unroll" >
            <xsl:with-param name="max" select="$max" />
            <xsl:with-param name="var" select="$var" />
          </xsl:apply-templates>
        </xsl:copy>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
