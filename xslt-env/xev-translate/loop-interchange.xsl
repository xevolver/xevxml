<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="/">
		<xsl:apply-templates />
	</xsl:template>

	<xsl:template match="*">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates />
		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgFortranDo">
		<xsl:variable name="doNum" select="count(./SgBasicBlock/SgFortranDo)" />

		<xsl:if test="count(child::PreprocessingInfo)=1">
			<xsl:element name="SgFortranDo">
				<xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" />
				<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp" />
				<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgIntVal" />
				<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgNullExpression" />
				<xsl:element name="SgBasicBlock">
					<xsl:copy-of select="SgBasicBlock/@*" />
					<xsl:copy>
						<xsl:copy-of select="@*" />
						<xsl:copy-of select="./SgAssignOp" />
						<xsl:copy-of select="./SgIntVal" />
						<xsl:copy-of select="./SgNullExpression" />
						<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock" />
					</xsl:copy>
				</xsl:element>
			</xsl:element>
		</xsl:if>

		<xsl:if test="count(child::PreprocessingInfo)!=1">
			<xsl:copy>
				<xsl:copy-of select="@*" />
				<xsl:apply-templates />
			</xsl:copy>

		</xsl:if>

	</xsl:template>

</xsl:stylesheet>
