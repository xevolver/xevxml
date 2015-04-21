<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevGoToLoop">
		<xsl:param name="loopName" />
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $loopName">
				<xsl:apply-templates select="."
					mode="xevTransformationHook" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="xevGoToLoop">
						<xsl:with-param name="loopName" select="$loopName" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>
