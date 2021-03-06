<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="*" mode="xevInitHook">
		<xsl:apply-templates select="." />
	</xsl:template>

	<xsl:template match="SgFortranDo">
		<xsl:choose> <!-- skip first loop -->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					xev loop_tag loop_fusion skip
				</xsl:comment>
			</xsl:when>
			<xsl:when test="preceding-sibling::*[2]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					xev loop_tag loop_fusion
				</xsl:comment>
				<xsl:apply-templates select="." mode="xevLoopFusion">
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

</xsl:stylesheet>
	