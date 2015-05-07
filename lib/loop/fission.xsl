<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevLoopFission">
		<xsl:choose>
			<xsl:when test="self::SgFortranDo">
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:copy-of select="./*[1]" />
					<xsl:copy-of select="./*[2]" />
					<xsl:copy-of select="./*[3]" />
					<SgBasicBlock>
						<xsl:copy-of select="SgBasicBlock/*[1]" />
					</SgBasicBlock>
					<xsl:copy-of select="PreprocessingInfo" />
				</xsl:copy>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:copy-of select="./*[1]" />
					<xsl:copy-of select="./*[2]" />
					<xsl:copy-of select="./*[3]" />
					<SgBasicBlock>
						<xsl:copy-of select="SgBasicBlock/*[2]" />
					</SgBasicBlock>
				</xsl:copy>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="xevLoopFission">
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>
