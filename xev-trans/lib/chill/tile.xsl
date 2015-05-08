<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="../libXev.xsl" />

	<xsl:template match="*" mode="chillTile">
		<xsl:param name="size1" />
		<xsl:param name="size2" />
		<xsl:apply-templates select="." mode="xevLoopTile">
			<xsl:with-param name="size1" select="$size1" />
			<xsl:with-param name="size2" select="$size2" />
		</xsl:apply-templates>
	</xsl:template>

</xsl:stylesheet>
