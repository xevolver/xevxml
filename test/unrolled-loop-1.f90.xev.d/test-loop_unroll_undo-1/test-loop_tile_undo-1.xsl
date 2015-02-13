<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					xev loop_tag loop_unroll_undo
				</xsl:comment>
				<xsl:apply-templates select="." mode="loop_unroll_undo">
					<xsl:with-param name="loopName1" select="'j'" />
					<xsl:with-param name="start1" select="'1'" />
					<xsl:with-param name="end1" select="'N'" />
					<xsl:with-param name="loopName2" select="'i'" />
					<xsl:with-param name="start2" select="'1'" />
					<xsl:with-param name="end2" select="'N'" />
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
	