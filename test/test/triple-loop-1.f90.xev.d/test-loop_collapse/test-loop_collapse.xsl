<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					test-loop_collapse.xsl found xev loop_tag
				</xsl:comment>

				<!-- <xsl:variable name="step1"> -->
				<xsl:comment>
					test-loop_collapse.xsl start step1
				</xsl:comment>
				<xsl:apply-templates select="." mode="loop_collapse">
					<xsl:with-param name="firstLoop" select="'k'" />
					<xsl:with-param name="secondLoop" select="'i'" />
				</xsl:apply-templates>
				<!-- </xsl:variable> -->

				<!-- <xsl:apply-templates select="exslt:node-set($step1)" /> -->

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
	