<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">

				<xsl:apply-templates select="." mode="move_loop">
					<xsl:with-param name="loopName" select="'j'" />
				</xsl:apply-templates>

			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates />
				</xsl:copy>
			</xsl:otherwise>

		</xsl:choose>
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="xev_transformation_hook">
		<xsl:apply-templates select="."
			mode="loop_interchange_rule" />
	</xsl:template>

</xsl:stylesheet>
	