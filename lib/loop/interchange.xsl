<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="SgFortranDo" mode="xevLoopInterchange">
		<xsl:element name="SgFortranDo">
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" />
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/*[1]" />
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/*[2]" />
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/*[3]" />
			<xsl:element name="SgBasicBlock">
				<xsl:copy-of select="SgBasicBlock/@*" />
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:copy-of select="./*[1]" />
					<xsl:copy-of select="./*[2]" />
					<xsl:copy-of select="./*[3]" />
					<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock" />
				</xsl:copy>
			</xsl:element>
		</xsl:element>
	</xsl:template>

</xsl:stylesheet>
