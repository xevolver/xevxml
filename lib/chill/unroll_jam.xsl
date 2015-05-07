<?xml version="1.0" encoding="UTF-8"?>

<!-- CHiLL compatible translation library -->

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">


	<xsl:template match="*" mode="chill_unroll_jam">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="chill_unroll_jam">
				<xsl:with-param name="factor" />
				<xsl:with-param name="loopName" />
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>


	<xsl:template match="SgFortranDo" mode="chill_unroll_jam">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />

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
			<xsl:copy-of select="PreprocessingInfo" />
		</xsl:copy>
	</xsl:template>

</xsl:stylesheet>
