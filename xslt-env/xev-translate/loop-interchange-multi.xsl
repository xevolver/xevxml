<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">
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

	<xsl:template
		match="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='loop']/CLAUSE[@name='interchange']/ARG">
		<xsl:comment>
			This is interchange
		</xsl:comment>
		<xsl:variable name="interchange_param" select="number(@value)" />
		<xsl:comment>
			interchange_param
			<xsl:value-of select="interchange_param"></xsl:value-of>
		</xsl:comment>
		<xsl:variable name="target_loop"
			select="ancestor::SgBasicBlock/SgFortranDo[1]" />

		<xsl:apply-templates select="exslt:node-set($target_loop)"
			mode="interchange" />

	</xsl:template>

	<xsl:template match="SgFortranDo" mode="interchange">
		<xsl:choose>
			<xsl:when
				test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop']/CLAUSE[@name='interchange']/ARG/@value='1'">
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
