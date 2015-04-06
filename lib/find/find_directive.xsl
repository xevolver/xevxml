<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="find_directive">
		<xsl:param name="directiveName" />
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/preceding-sibling::*[1]/SgPragma/@pragma = $directiveName">

				<xsl:apply-templates select="." mode="xev_move_hook" />

			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="find_directive">
						<xsl:with-param name="directiveName" select="$directiveName" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>

		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>
