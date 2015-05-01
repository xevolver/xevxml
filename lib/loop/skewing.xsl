<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevLoopSkewing_Replace">
		<xsl:param name="old" />
		<xsl:param name="index" />
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="xevLoopSkewing_Replace">
				<xsl:with-param name="old" select="$old" />
				<xsl:with-param name="index" select="$index" />
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgVarRefExp" mode="xevLoopSkewing_Replace">
		<xsl:param name="old" />
		<xsl:param name="index" />
		<xsl:choose>
			<xsl:when test="@name = $old">
				<SgSubtractOp>
					<xsl:element name="SgVarRefExp">
						<xsl:attribute name="name">
					  <xsl:value-of select="$old" /><xsl:text>_sk</xsl:text>
					</xsl:attribute>
					</xsl:element>
					<xsl:element name="SgVarRefExp">
						<xsl:attribute name="name">
					  <xsl:value-of select="$index" />
					</xsl:attribute>
					</xsl:element>
				</SgSubtractOp>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="xevLoopSkewing_Replace">
						<xsl:with-param name="old" select="old" />
						<xsl:with-param name="index" select="$index" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<xsl:template match="SgFortranDo" mode="xevLoopSkewing">
		<xsl:param name="index" />
		<SgFortranDo style="0" end="1" slabel="">
			<SgAssignOp>
				<xsl:element name="SgVarRefExp">
					<xsl:attribute name="name">
					  <xsl:value-of select="SgAssignOp/SgVarRefExp/@name" />
					  <xsl:text>_sk</xsl:text>
					</xsl:attribute>
				</xsl:element>
				<!-- <SgVarRefExp name="j_new" lvalue="1" /> -->
				<SgAddOp>
					<xsl:element name="SgVarRefExp">
						<xsl:attribute name="name">
						  <xsl:value-of select="$index" />
					  </xsl:attribute>
					</xsl:element>
					<!-- <SgVarRefExp name="i" /> -->
					<xsl:copy-of select="SgAssignOp/*[2]" />
					<!-- <SgIntVal value="1" string="1" /> -->
				</SgAddOp>
			</SgAssignOp>
			<SgAddOp>
				<xsl:element name="SgVarRefExp">
					<xsl:attribute name="name">
						  <xsl:value-of select="$index" />
					  </xsl:attribute>
				</xsl:element>
				<!-- <SgVarRefExp name="i" /> -->
				<xsl:copy-of select="./*[2]" />
				<!-- <SgVarRefExp name="n" /> -->
			</SgAddOp>
			<SgNullExpression />
			<xsl:apply-templates select="SgBasicBlock"
				mode="xevLoopSkewing_Replace">
				<xsl:with-param name="old" select="SgAssignOp/SgVarRefExp/@name" />
				<xsl:with-param name="index" select="$index" />
			</xsl:apply-templates>
		</SgFortranDo>
	</xsl:template>
</xsl:stylesheet>
