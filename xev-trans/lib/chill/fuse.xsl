<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="../libXev.xsl" />

	<xsl:template match="*" mode="chillFuse">
		<xsl:apply-templates select="." mode="xevLoopFusion">
		</xsl:apply-templates>
	</xsl:template>

</xsl:stylesheet>