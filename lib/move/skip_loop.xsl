<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevSkipToNthLoop">
		<xsl:param name="loopName" />
		<xsl:param name="N" />
		<xsl:variable name="currentNode" select="." /> <!-- for debug -->
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $loopName">
				<xsl:choose>
					<xsl:when test="$N = 1">
						<xsl:apply-templates select="."
							mode="xevTransformationHook" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:apply-templates mode="xevSkipToNthLoop">
							<xsl:with-param name="loopName" select="$loopName" />
							<xsl:with-param name="N" select="$N - '1'" />
						</xsl:apply-templates>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates mode="xevSkipToNthLoop">
					<xsl:with-param name="loopName" select="$loopName" />
					<xsl:with-param name="N" select="$N" />
				</xsl:apply-templates>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>
