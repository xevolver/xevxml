<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevGoToVar">
		<xsl:param name="varName" />
		<xsl:choose>
			<xsl:when test="self::SgVarRefExp/@name = $varName">
				<xsl:apply-templates select="."
					mode="xevTransformationHook" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="xevGoToVar">
						<xsl:with-param name="varName" select="$varName" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>
