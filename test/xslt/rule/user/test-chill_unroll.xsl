<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:import href="../libCHiLL.xsl" />

  <xsl:output method="xml" encoding="UTF-8" />

  <xsl:template match="SgFortranDo">
    <xsl:choose>
      <xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">
        <xsl:comment>test-2.xsl xev loop_tag</xsl:comment>
        <xsl:apply-templates select="." mode="call_loop" />
      </xsl:when>

      <xsl:otherwise>
        <xsl:copy>
          <xsl:copy-of select="@*" />
          <xsl:apply-templates />
        </xsl:copy>
      </xsl:otherwise>

    </xsl:choose>
  </xsl:template>

  <xsl:template match="*" mode="call_loop">
    <xsl:apply-templates select="." mode="chill_unroll">
      <xsl:with-param name="max" select="4" />
      <xsl:with-param name="var" select="'i'" />
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="SgPragmaDeclaration" mode="call_loop">
  </xsl:template>
  <xsl:template match="PreprocessingInfo" mode="call_loop">
  </xsl:template>

</xsl:stylesheet>
