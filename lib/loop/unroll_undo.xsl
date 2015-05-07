<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevLoopUnrollUndo">
		<xsl:param name="factor" />

		<xsl:choose>
			<xsl:when test="self::SgFortranDo">
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:copy-of select="./*[1]" />
					<xsl:copy-of select="./*[2]" />
					<xsl:element name="SgNullExpression" />
					<xsl:element name="SgBasicBlock">
						<xsl:copy-of select="SgFortranDo/SgBasicBlock/@*" />
						<xsl:apply-templates
							select="self::SgFortranDo/SgBasicBlock/SgExprStatement[1]" />
					</xsl:element>
					<xsl:copy-of select="PreprocessingInfo" />
				</xsl:copy>

			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="xevLoopUnrollUndo">
						<xsl:with-param name="factor" select="$factor" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>

	</xsl:template>

	<xsl:template match="SgPragmaDeclaration" mode="loop_unroll_undo">
	</xsl:template>
	<xsl:template match="PreprocessingInfo" mode="loop_unroll_undo">
	</xsl:template>

</xsl:stylesheet>
