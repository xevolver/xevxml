<?xml version="1.0" encoding="UTF-8"?>

<!-- CHiLL compatible translation library -->

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	

	<xsl:template match="*" mode="chillUnrollJam">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />
		<xsl:apply-templates select="." mode="xevLoopUnroll">
			<xsl:with-param name="factor" />
			<xsl:with-param name="loopName" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="old_chillUnrollJam">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="chillUnrollJam">
				<xsl:with-param name="factor" />
				<xsl:with-param name="loopName" />
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>



	<xsl:template match="SgFortranDo" mode="old_chillUnrollJam">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />
		<xsl:apply-templates select="." mode="unroll_target_loop">
			<xsl:with-param name="factor" select="$factor" />
			<xsl:with-param name="loopName" select="$loopName" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="old_chillUnrollJam">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />

		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:copy-of select="./*[1]" />
			<xsl:copy-of select="./*[2]" />
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
