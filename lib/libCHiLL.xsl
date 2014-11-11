<?xml version="1.0" encoding="UTF-8"?>

<!-- CHiLL compatible translation library -->

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="libXev.xsl" />

	<xsl:template match="SgFortranDo" mode="chill_unroll_jam">
		<xsl:param name="max" />
		<xsl:param name="var" />
		<xsl:comment>
			libCHiLL.xsl chill_unroll_jam
		</xsl:comment>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<!-- 変数 初期値 -->
			<xsl:copy-of select="./*[1]" />
			<!-- 最終値 -->
			<xsl:copy-of select="./*[2]" />
			<!-- 刻み幅 -->
			<xsl:element name="SgIntVal">
				<xsl:attribute name="value">
							<xsl:value-of select="$max" />
						</xsl:attribute>
			</xsl:element>

			<xsl:apply-templates select="./SgBasicBlock"
				mode="loop_unroll">
				<xsl:with-param name="max" select="$max" />
				<xsl:with-param name="var" select="$var" />
			</xsl:apply-templates>

		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="chill_unroll">
		<xsl:param name="max" />
		<xsl:param name="var" />
		<xsl:comment>
			libCHiLL.xsl chill_unroll
		</xsl:comment>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<!-- 変数 初期値 -->
			<xsl:copy-of select="./*[1]" />
			<!-- 最終値 -->
			<xsl:copy-of select="./*[2]" />
			<!-- 刻み幅 -->
			<xsl:element name="SgIntVal">
				<xsl:attribute name="value">
							<xsl:value-of select="$max" />
						</xsl:attribute>
			</xsl:element>

			<xsl:apply-templates select="./SgBasicBlock"
				mode="loop_unroll">
				<xsl:with-param name="max" select="$max" />
				<xsl:with-param name="var" select="$var" />
			</xsl:apply-templates>

		</xsl:copy>
	</xsl:template>


</xsl:stylesheet>