<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="chill_permute">
		<xsl:param name="firstLoop" />
		<xsl:param name="secondLoop" />
		<xsl:param name="thirdLoop" />

		<xsl:element name="SgFortranDo">
			<xsl:attribute name="end">1</xsl:attribute>
			<xsl:apply-templates select="." mode="find_permute_loop">
				<xsl:with-param name="depth" select="$firstLoop" />
			</xsl:apply-templates>
			<xsl:element name="SgBasicBlock">
				<xsl:element name="SgFortranDo">
					<xsl:attribute name="end">1</xsl:attribute>
					<xsl:apply-templates select="." mode="find_permute_loop">
						<xsl:with-param name="depth" select="$secondLoop" />
					</xsl:apply-templates>
					<xsl:element name="SgBasicBlock">
						<xsl:element name="SgFortranDo">
							<xsl:attribute name="end">1</xsl:attribute>
							<xsl:apply-templates select="."
								mode="find_permute_loop">
								<xsl:with-param name="depth" select="$thirdLoop" />
							</xsl:apply-templates>
							<xsl:element name="SgBasicBlock">

								<xsl:apply-templates
									select="self::SgFortranDo/SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/SgBasicBlock" />

							</xsl:element>
						</xsl:element>
					</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:element>

	</xsl:template>

	<xsl:template match="*" mode="find_permute_loop">
		<xsl:param name="depth" />
		<xsl:choose>
			<xsl:when test="self::SgFortranDo">
				<xsl:choose>
					<xsl:when test="$depth = '1'">
						<xsl:copy-of select="./*[1]" />
						<xsl:copy-of select="./*[2]" />
						<xsl:copy-of select="./*[3]" />
					</xsl:when>
					<xsl:otherwise>
						<xsl:apply-templates mode="find_permute_loop">
							<xsl:with-param name="depth" select="$depth - 1" />
						</xsl:apply-templates>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates mode="find_permute_loop">
					<xsl:with-param name="depth" select="$depth" />
				</xsl:apply-templates>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>