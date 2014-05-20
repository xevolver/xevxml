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

	<xsl:template match="SgPragmaDeclaration">
		<xsl:choose>
			<xsl:when
				test="SgPragma/DIRECTIVE[@name='loop']/CLAUSE[@name='interchange']/ARG/@value='1'">
				<xsl:comment>
					this is interchange 1
				</xsl:comment>
				<xsl:apply-templates select="following-sibling::*[1]/SgFortranDo"
					mode="interchage" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:comment>
					removed SgPragmaDeclaration
				</xsl:comment>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<xsl:template match="SgFortranDo" mode="interchange">
		<xsl:comment>
			template SgFortranDo
		</xsl:comment>
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
