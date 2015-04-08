<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="../loop/loop.xsl" />

	<xsl:template match="*" mode="xevVarReplace">
		<xsl:param name="varName" />
		<xsl:choose>
			<xsl:when test="self::SgVarRefExp">
				<xsl:element name="SgVarRefExp">
					<xsl:attribute name="name">
					  <xsl:value-of select="$varName" />
					</xsl:attribute>
				</xsl:element>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="xevVarReplace">
						<xsl:with-param name="varName" select="$varName" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>
