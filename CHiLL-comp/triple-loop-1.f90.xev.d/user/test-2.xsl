<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="../../../lib/libCHiLL.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<xsl:when
				test="preceding-sibling::*[1]/SgPragmaDeclaration/@pragma = 'xev loop_tag'">
				<xsl:comment>
					<xsl:variable name="QName">
						test-2.xsl xev loop_tag
					</xsl:variable>
				</xsl:comment>
				<xsl:apply-templates select="." mode="chill_unroll_jam" />
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
	