<?xml version="1.0" encoding="UTF-8"?>

<!-- CHiLL compatible translation library -->

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="SgFortranDo" mode="chill_unroll">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />
		<xsl:comment>
			libCHiLL.xsl chill_unroll
		</xsl:comment>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<!-- start value -->
			<xsl:copy-of select="./*[1]" />
			<!-- end value -->
			<xsl:copy-of select="./*[2]" />
			<!-- stride value -->
			<xsl:element name="SgIntVal">
				<xsl:attribute name="value">
							<xsl:value-of select="$factor" />
						</xsl:attribute>
			</xsl:element>

			<xsl:apply-templates select="./SgBasicBlock"
				mode="xevLoopUnroll">
				<xsl:with-param name="factor" select="$factor" />
				<xsl:with-param name="loopName" select="$loopName" />
			</xsl:apply-templates>

		</xsl:copy>
	</xsl:template>



</xsl:stylesheet>
