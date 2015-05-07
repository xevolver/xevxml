<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="../../../lib/libCHiLL.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />


	<xsl:template match="*" mode="xevInitHook">
		<xsl:apply-templates select="." mode="xevFindDirective">
			<xsl:with-param name="directiveName" select="'xev loop_tag'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevMoveHook">
		<xsl:apply-templates select="." mode="xevGoToLoop">
			<xsl:with-param name="loopName" select="'k'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevTransformationHook">
		<xsl:apply-templates select="." mode="chillPermute">
		<xsl:with-param name="firstLoop" select="'3'" />
		<xsl:with-param name="secondLoop" select="'1'" />
		<xsl:with-param name="thirdLoop" select="'2'" />
	</xsl:apply-templates>
	</xsl:template>


</xsl:stylesheet>
	